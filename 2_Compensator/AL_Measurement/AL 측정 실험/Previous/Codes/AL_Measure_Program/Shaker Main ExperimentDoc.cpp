// Shaker Main ExperimentDoc.cpp : implementation of the CShakerMainExperimentDoc class
//

#include "stdafx.h"
#include "Shaker Main Experiment.h"

#include "Shaker Main ExperimentDoc.h"
#include "Shaker Main ExperimentView.h"

#include "MDialogStartExperiment.h"

#include "ft_sensor.h"

#include "shaker.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShakerMainExperimentDoc

template <typename T>
void swap(T *a, T *b)
{
	T tmp;
	tmp = *a; *a = *b; *b = tmp;
}


IMPLEMENT_DYNCREATE(CShakerMainExperimentDoc, CDocument)

BEGIN_MESSAGE_MAP(CShakerMainExperimentDoc, CDocument)
	//{{AFX_MSG_MAP(CShakerMainExperimentDoc)
	ON_COMMAND(ID_EXPERIMENT_STARTEXPERIMENT, OnExperimentStartexperiment)
	ON_UPDATE_COMMAND_UI(ID_EXPERIMENT_STARTEXPERIMENT, OnUpdateExperimentStartexperiment)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShakerMainExperimentDoc construction/destruction

CShakerMainExperimentDoc::CShakerMainExperimentDoc()
{
	// TODO: add one-time construction code here

}

CShakerMainExperimentDoc::~CShakerMainExperimentDoc()
{
		ClearUp();

	if( taskHandleInput!=0 ) {
		/*********************************************/
		/*/ DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandleInput);
		DAQmxClearTask(taskHandleInput);
	}
	if( taskHandleOutput!=0 ) {
		/*********************************************/
		/*/ DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandleOutput);
		DAQmxClearTask(taskHandleOutput);
	}
}

BOOL CShakerMainExperimentDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	// My initialization
	piFrequencies = NULL;		// Hz
	pfStepSizes = NULL;
	piOrderOfFrequencies = NULL;
	pfRoughThresholds = NULL;
	pDisplacement = NULL;
	strcpy(main_dir_path, ".\\Data");
	fInitialSignalIntensity = 15.0;

	bIsDataReady = false;
	bIsExperimentStarted = false;

	pOutputFileID = NULL;
	pfSlope = NULL;
	pfOffset = NULL;

	//return TRUE;

	pdOutputData = NULL;
	pdInputData = NULL;
	pdOutputBuffer = NULL;
	pdOutputBufferForNullStimulus = NULL;
	pdOutputBufferForTestStimulus = NULL;
	pdInputBuffer = NULL;
	pMeasurementDataOutputFileID=NULL;
	return TRUE;

}



/////////////////////////////////////////////////////////////////////////////
// CShakerMainExperimentDoc serialization

void CShakerMainExperimentDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CShakerMainExperimentDoc diagnostics

#ifdef _DEBUG
void CShakerMainExperimentDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CShakerMainExperimentDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CShakerMainExperimentDoc commands


//bool LoadFTSensorCalibration(struct ft_sensor *myFTSensor);
//bool SetBiasFTSensor(CShakerMainExperimentDoc *pDoc, struct ft_sensor *pMyFTSensor);
bool SaveMeasurementDataToFile(FILE *fid, CShakerMainExperimentDoc *pDoc,  struct sine_wave *pTestStimulus, double dPreviousStimulusIntensity);

void CShakerMainExperimentDoc::OnExperimentStartexperiment() 
{
	// TODO: Add your command handler code here

	bIsExperimentStarted = true;

	MDialogStartExperiment dlg;

	if ( dlg.DoModal() == IDCANCEL) 
		return;
	
	// Get subject code and experiment number from the dialog box
	sSubjectCode = dlg.m_sSubjectCode;
	iExperimentNumber = dlg.m_iExperimentNumber;

	// Read experiment data for this subject and experiment number
	if ( ReadExperimentData() == false ) {
		bIsExperimentStarted = false;
		ClearUp();
		return;	
	}

	// Read a calibration file for the shaker
	if ( ReadCalibrationFile() == false ) {
		bIsExperimentStarted = false;
		ClearUp();
		return;	
	}

	// Initialize the DAQ board
	if ( HarewareInitialize() == false ) {
		bIsExperimentStarted = false;
		ClearUp();
		return;	
	}

	//// Initialize the F/T sensor
	//struct ft_sensor myFTSensor;
	//if ( LoadFTSensorCalibration(&myFTSensor) == false ) {
	//	bIsExperimentStarted = false;
	//	ClearUp();
	//	return;	
	//}

	//// Set a bias to the F/T sensor
	//if ( SetBiasFTSensor(this, &myFTSensor) == false ) {
	//	bIsExperimentStarted = false;
	//	ClearUp();
	//	return;	
	//}

	// Set up the I/O mode of the DAQ card
	if ( SetIOModeOfDAQCard() == false ) {
		bIsExperimentStarted = false;
		ClearUp();
		return;	
	}

	// Confirm whether the condition is right
	CString str("This is condition ");
	CString str2;
	str2.Format("%d.\nPress the OK button to start", iCurrentCondition+1);
	str = str + str2;
	MessageBox(NULL, str, "Starting Experiment...", MB_OK | MB_ICONQUESTION);


	// Make or check out the output file
	if ( PrepareOutputFile() == false) {
		bIsExperimentStarted = false;
		ClearUp();
		return;	// error in making an output file
	}

	char file_name2[200];
	strcpy(file_name2, ".\\Data\\acc_measure_");
	strcat(file_name2, (LPCTSTR)sSubjectCode);
	strcat(file_name2, ".dat");
	facc = fopen(file_name2, "a");



	// Write down the current experiment information on the output file
	fprintf(pOutputFileID, "\n\n>> Condition %d\n", iCurrentCondition+1);
	fprintf(pOutputFileID, "\n\n>> Frequency = %d\n", piOrderOfFrequencies[iCurrentCondition]);

	int i;
	for (i=0 ; i<iNumberOfFrequencies ; i++ ) {
		if ( piOrderOfFrequencies[iCurrentCondition] == piFrequencies[i] ) {
			iCurrentConditionIndex = i ;	// index of the frequency for the current condition
			break;
		}
	}

	if ( i>= iNumberOfFrequencies ) {
		MessageBox(NULL, "Cannot find a frequency index for experiment.",
			"Error!!", MB_OK | MB_ICONERROR);
		bIsExperimentStarted = false;
		ClearUp();
		return;	// error in making an output file
	}


	// Set the flag
	bIsExperimentStarted = true;

	// Initiate experiment variables
	iCurrentNumberOfReversals = 0;
	iCurrentExperimentState = NUM_CORRECT_0;
	iCurrentTrialNumber = 1;
	iCurrentStaircaseDirection = GOING_DOWN;
	dCurrentStimulusIntensity = pfRoughThresholds[iCurrentConditionIndex]*pow(10.0, fInitialSignalIntensity/20.0) ;

	
	// Start experiment
	srand( (unsigned)time( NULL ) );

	// Make a null stimulus
	NullStimulus.A = ComputeVoltageFromDisplacement(0.0, piOrderOfFrequencies[iCurrentCondition]);
	if (NullStimulus.A > 5 || NullStimulus.A < -5) {
		//MessageBox(NULL, "Error in computing a voltage to the shaker.",
		//	"Error!!", MB_OK | MB_ICONERROR);
		//bIsExperimentStarted = false;
		//ClearUp();
		//return;
		NullStimulus.A = 5;
	}
	

	NullStimulus.F = 0.0;		// means DC
	NullStimulus.UR = UPDATE_RATE;		// Hz
	NullStimulus.SD = (float64)iStimulusDuration / 1000.0; // now in Sec.
	NullStimulus.del_t = 1.0 / 	NullStimulus.UR;


	// Allocate memory for output buffers
	pdOutputBufferForNullStimulus = (float64 *) malloc(ulCount*sizeof(float64));
	if ( pdOutputBufferForNullStimulus==NULL ) {
		MessageBox(NULL, "Cannot allocate memory for output buffers.",
			"Error!!", MB_OK | MB_ICONERROR);
		bIsExperimentStarted = false;
		ClearUp();
		return;
	}

	pdOutputBufferForTestStimulus = (float64 *) malloc(ulCount*sizeof(float64));
	if ( pdOutputBufferForTestStimulus==NULL ) {
		MessageBox(NULL, "Cannot allocate memory for output buffers.",
			"Error!!", MB_OK | MB_ICONERROR);
		bIsExperimentStarted = false;
		ClearUp();
		return;
	}

				
	// Allocate memories for analog input buffers
	pdInputBuffer = (float64 *) malloc(ulCount*2*sizeof(float64));
	if ( pdInputBuffer==NULL ) {
		MessageBox(NULL, "Cannot allocate memory for input buffers.",
			"Error!!", MB_OK | MB_ICONERROR);
		bIsExperimentStarted = false;
		ClearUp();
		return;
	}


	// Compute output buffers for the null stimuluos
	if ( ComputeDAQOutputForNullStimulus() == false) {
		bIsExperimentStarted = false;
		ClearUp();
		return;
	}



	// Send information of the current stimulus number;
	MSG msg;
	bool bHasResponseYet;
	POSITION pos;

	// Local Variable Declarations: 
	int error=0;
	char errBuff[2048]={'\0'};

	int32   	written;
	int32       read;
	
	
	double dPreviousStimulusIntensity;

	do {
		// Make a test stimulus
		TestStimulus.A = ComputeVoltageFromDisplacement(dCurrentStimulusIntensity, piOrderOfFrequencies[iCurrentCondition]);
		if (TestStimulus.A > 5 || TestStimulus.A < -5) {
			/*MessageBox(NULL, "Error in computing a voltage to the shaker.",
				"Error!!", MB_OK | MB_ICONERROR);
			bIsExperimentStarted = false;
			ClearUp();
			return;*/
			TestStimulus.A = 5;
		}

		TestStimulus.F = piOrderOfFrequencies[iCurrentCondition];
		TestStimulus.UR = UPDATE_RATE;		// Hz
		TestStimulus.SD = (float64)iStimulusDuration / 1000.0; // now in Sec.
		TestStimulus.del_t = 1.0 / 	TestStimulus.UR;

		// Compute output buffers for the test stimuluos
		if ( ComputeDAQOutputForTestStimulus() == false) {
			bIsExperimentStarted = false;
			ClearUp();
			return;
		}

		iRightAnswer = rand() % 3 + 1;	// either 1 or 2 or 3 (3AFC)

		// Send a stimulus
		for ( iCurrentStimulusNumber = 1; iCurrentStimulusNumber <= 3 ; iCurrentStimulusNumber++ ) {
			// Display the current interval information 

		   pos = GetFirstViewPosition();
		   while (pos != NULL)
		   {
			  CView* pView = GetNextView(pos);
			  pView->Invalidate();
			  pView->UpdateWindow();
		   }   
		
			Sleep(iInterStimulusInterval/2);	// Wait for the half of the ISI

			
			// Set stimulus accordingly
			if ( iCurrentStimulusNumber == iRightAnswer )
				pdOutputBuffer = pdOutputBufferForTestStimulus;
			else
				pdOutputBuffer = pdOutputBufferForNullStimulus;

			// Send the stimulus
			
			/*********************************************/
			/*/ DAQmx Write Code
			/*********************************************/
			DAQmxErrChk (DAQmxWriteAnalogF64(taskHandleOutput,ulCount,0,10.0,DAQmx_Val_GroupByChannel,pdOutputBuffer,&written,NULL));

			/*********************************************/
			/*/ DAQmx Start Code
			/*********************************************/
			DAQmxErrChk (DAQmxStartTask(taskHandleOutput));
			if ( iCurrentStimulusNumber == iRightAnswer ) 
				DAQmxErrChk (DAQmxStartTask(taskHandleInput));

			/*********************************************/
			/*/ DAQmx Wait Code
			/*********************************************/
			if ( iCurrentStimulusNumber == iRightAnswer ) //건드림 뒤쪽 ulcount*9
				DAQmxErrChk (DAQmxReadAnalogF64(taskHandleInput,ulCount,10.0,DAQmx_Val_GroupByChannel,pdInputBuffer,ulCount,&read,NULL));

			DAQmxErrChk (DAQmxWaitUntilTaskDone(taskHandleOutput,10.0));
			if ( iCurrentStimulusNumber == iRightAnswer ) 
				DAQmxErrChk (DAQmxWaitUntilTaskDone(taskHandleInput,10.0));

			if ( iCurrentStimulusNumber == iRightAnswer ) 
				DAQmxStopTask(taskHandleInput);
			DAQmxStopTask(taskHandleOutput);


			if ( iCurrentStimulusNumber<3 )
				Sleep(iInterStimulusInterval/2);	// Wait for the ISI
			
		}


		// Display a message asking an user response. 
		// Now iCurrentStimulusNumber=4, meaning that time to get a user response
		UpdateAllViews(NULL);

		// Get an user response
		bHasResponseYet = false;
		while ( bHasResponseYet == false ) {
			GetMessage(&msg, NULL, 0, 0);
			DispatchMessage(&msg);
			if ( msg.message != WM_KEYDOWN ) 
				continue;
					
			// Now a key is pressed.
			switch ( msg.wParam ) {
				case '1':
				case VK_NUMPAD1:
					iCurrentResponse = 1;
                    bHasResponseYet = true;
					break;
				case '2':
				case VK_NUMPAD2:
					iCurrentResponse = 2;
                    bHasResponseYet = true;
					break;
				case '3':
				case VK_NUMPAD3:
					iCurrentResponse = 3;
                    bHasResponseYet = true;
					break;
				default:
					break;
			}
		}

		// Check whether the user response is right
		if ( iRightAnswer == iCurrentResponse )
			bWasAnswerRight = true;
		else
			bWasAnswerRight = false;


		// Update the state of the experiment and the stimulus intensity accordingly.
		// This is when the step size is constant in log scale

		//1up-2down
		dPreviousStimulusIntensity = dCurrentStimulusIntensity;
		switch ( iCurrentExperimentState ) {
			case NUM_CORRECT_0:
				if ( bWasAnswerRight == true ) {
					// state transition
					iCurrentExperimentState = NUM_CORRECT_1;
					// staircase direction 
					if ( iCurrentStaircaseDirection == GOING_DOWN ) {
						iCurrentStaircaseDirection = GOING_DOWN;
						bIsReversalOccurred = false;
					} else {
						iCurrentStaircaseDirection = GOING_UP;
						bIsReversalOccurred = false;
					}
					// no intensity change
					dCurrentStimulusIntensity = dCurrentStimulusIntensity;
				} else {
					// state transition
					iCurrentExperimentState = NUM_CORRECT_0;
					// staircase direction 
					if ( iCurrentStaircaseDirection == GOING_DOWN ) {
						iCurrentStaircaseDirection = GOING_UP;
						bIsReversalOccurred = true;
						iCurrentNumberOfReversals ++;
					} else {
						iCurrentStaircaseDirection = GOING_UP;
						bIsReversalOccurred = false;
					}
					// increase intensity, when the step size is constant in log scale
					dCurrentStimulusIntensity *= 
						pow(10.0, pfStepSizes[iCurrentNumberOfReversals]/20.0);
				}
				break;

			case NUM_CORRECT_1:
				if ( bWasAnswerRight == true ) {
					// state transition
					iCurrentExperimentState = NUM_CORRECT_0;
					// staircase direction 
					if ( iCurrentStaircaseDirection == GOING_DOWN ) {
						iCurrentStaircaseDirection = GOING_DOWN;
						bIsReversalOccurred = false;
					} else {
						iCurrentStaircaseDirection = GOING_DOWN;
						bIsReversalOccurred = true;
						iCurrentNumberOfReversals ++;
					}
					// decrease intensity
					dCurrentStimulusIntensity *= 
						pow(10.0, -pfStepSizes[iCurrentNumberOfReversals]/20.0);
				} else {
					// state transition
					iCurrentExperimentState = NUM_CORRECT_0;
					// staircase direction 
					if ( iCurrentStaircaseDirection == GOING_DOWN ) {
						iCurrentStaircaseDirection = GOING_UP;
						bIsReversalOccurred = true;
						iCurrentNumberOfReversals ++;
					} else {
						iCurrentStaircaseDirection = GOING_UP;
						bIsReversalOccurred = false;
					}
					// increase intensity
					dCurrentStimulusIntensity *= 
						pow(10.0, pfStepSizes[iCurrentNumberOfReversals]/20.0);
				}
				break;
			default:
				break;
		}



		// If feedback is ON, give a feedback.
		if ( sFeedback == "ON" ) {
			iCurrentStimulusNumber++;	// now iCurrentStimulusNumber=5
		   pos = GetFirstViewPosition();
		   while (pos != NULL)
		   {
			  CView* pView = GetNextView(pos);
			  pView->Invalidate();
			  pView->UpdateWindow();
		   }   

			// wait a little
			Sleep(iStimulusDuration);
		}


		// write down data to a file
		fprintf(pOutputFileID, "%d\t%f\t", iCurrentTrialNumber, dPreviousStimulusIntensity);
		if ( bWasAnswerRight == true )
			fprintf(pOutputFileID, "RIGHT\t");
		else
			fprintf(pOutputFileID, "WRONG\t");
		if ( bIsReversalOccurred == true )
			fprintf(pOutputFileID, "YES\n");
		else
			fprintf(pOutputFileID, "NO\n");

		fprintf(pMeasurementDataOutputFileID, "micrometer\t%f\tVoltage\t%f\n", dPreviousStimulusIntensity, TestStimulus.A);
		// save measurement data to a file if a reversal occurred.

		
		if ( bIsReversalOccurred == true) 
			SaveMeasurementDataToFile(facc, this, &TestStimulus, dPreviousStimulusIntensity);
		

		// increase the current trial number
		iCurrentTrialNumber ++;

		if ( bIsReversalOccurred != true)
			Sleep(iStimulusDuration);	// Wait for the ISI
		
	} while ( iCurrentNumberOfReversals < iNumberOfReversals && iCurrentTrialNumber <= 500 );


	fclose(pOutputFileID);
	fclose(pMeasurementDataOutputFileID);
	fclose(facc);
	UpdateConfigurationFile();

	iCurrentStimulusNumber = 6;	// meaning that the experiment is done
   pos = GetFirstViewPosition();
   while (pos != NULL)
   {
	  CView* pView = GetNextView(pos);
	  pView->Invalidate();
	  pView->UpdateWindow();
   }   

	bIsExperimentStarted = false;


	// free memory allocated to Calibration structure
//	destroyCalibration(myFTSensor.pCal);
//	free(myFTSensor.pcCalFilePath);
	
	// DAQ clear codes
	DAQmxClearTask(taskHandleInput);
	DAQmxClearTask(taskHandleOutput);
	
	if( taskHandleInput!=0 ) {
		/*********************************************/
		/*/ DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandleInput);
		DAQmxClearTask(taskHandleInput);
	}
	if( taskHandleOutput!=0 ) {
		/*********************************************/
		/*/ DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandleOutput);
		DAQmxClearTask(taskHandleOutput);
	}

	ClearUp();
	exit(1);
	return;


Error:
	if( taskHandleInput!=0 ) {
		/*********************************************/
		/*/ DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandleInput);
		DAQmxClearTask(taskHandleInput);
	}
	if( taskHandleOutput!=0 ) {
		/*********************************************/
		/*/ DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandleOutput);
		DAQmxClearTask(taskHandleOutput);
	}
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		MessageBox(NULL, errBuff, "Error!!", MB_OK | MB_ICONERROR);
	}

	return;

}


//=========================== Confirmed
bool CShakerMainExperimentDoc::UpdateConfigurationFile()
{
	char temp[200];
	char file_name[200];
	time_t current_time;

	// open the config file
	strcpy(file_name, main_dir_path);
	strcat(file_name, "\\");
	strcat(file_name, "config_");
	strcat(file_name, (LPCTSTR)sSubjectCode);
	strcat(file_name, ".cfg");

	FILE * config_file_id = fopen(file_name, "a");

	if ( config_file_id==NULL ) {
		MessageBox(NULL, "The configuration file could not be opended in the append mode.",
			"Error!!", MB_OK | MB_ICONERROR);
		return false;
	}

	time( &current_time );

	if ( (iCurrentCondition+1) == iNumberOfFrequencies ) {
		fprintf(config_file_id, "Experiment was finished at %s.\n", ctime(&current_time));
		fprintf(config_file_id, "Next condition -- %d\t", iCurrentCondition+1 );
	} else {
		fprintf(config_file_id, "Condition %d was finished at %s.\n", iCurrentCondition, ctime(&current_time));
		fprintf(config_file_id, "Next condition -- %d\t", iCurrentCondition+1 );
	}

	fclose(config_file_id);
	return true;
}


//=========================== Confirmed
//////////////////////////////////////////////////////
// Save collected data to a file
bool SaveMeasurementDataToFile(FILE *fid, CShakerMainExperimentDoc *pDoc, struct sine_wave *pTestStimulus,double dPreviousStimulusIntensity)
{
	int32 i, si, ei;
//	int16 j;
	//float pfFT[6], pfSampleReading[7], *pfFTFileOutput;


	// write data of the current time step to the file
	// output sine wave information
	fprintf(fid, "%f\t%f\t%f\t%f", dPreviousStimulusIntensity, pTestStimulus->F, 
		pTestStimulus->UR, pTestStimulus->SD);

	// write accelerometer output;
	si=0*pDoc->ulCount;
	ei=1*pDoc->ulCount;
	for (i=si ; i<ei ; i++ ) 
		fprintf(fid, "%f\t", pDoc->pdInputBuffer[i]*ACC_AMP_GAIN);	// in G 100.0->2
	fprintf(fid, "\n");

	//// write F/T sensor output
	//pfFTFileOutput = (float *) malloc(sizeof(float)*6*pDoc->ulCount);
	//for (i=0 ; i<pDoc->ulCount; i++ ) {
	//	for (j=0; j<7 ; j++ )
	//		pfSampleReading[j] 
	//			= (float)pDoc->pdInputBuffer[i+j*pDoc->ulCount];
	//	ConvertToFT(pMyFTSensor->pCal, pfSampleReading, pfFT);
	//	for (j=0; j<6 ; j++ )
	//		pfFTFileOutput[i+j*pDoc->ulCount] = pfFT[j];
	//}

	//j=2;
	//si=j*pDoc->ulCount;
	//ei=(j+1)*pDoc->ulCount;
	//for (i=si ; i<ei ; i++ )
	//	fprintf(fid, "%f\t", pfFTFileOutput[i]);
	//fprintf(fid, "\n");

	//free(pfFTFileOutput);


	return true;
}





//===================== Confirmed
bool CShakerMainExperimentDoc::ComputeDAQOutputForNullStimulus()
{
	int32 i;
	
	
	// compute values of the sine waveform for shaker output
	for ( i=0 ; i<ulCount ; i++ )
		pdOutputBufferForNullStimulus[i] = NullStimulus.A ;

	return true;
}


//===================== Confirmed
bool CShakerMainExperimentDoc::ComputeDAQOutputForTestStimulus()
{
 	int32 i;
	

	double win_t = TestStimulus.UR/2.0; //window size, 10 ms window를 반 나누어 앞과 뒤에 씌움. 5 ms.


	// compute values of the sine waveform for shaker output
	for ( i=0 ; i<ulCount ; i++ ) {

		//pdOutputBufferForTestStimulus[i] = 0;
		if(i<win_t/2.0) { //Window 씌우기
			pdOutputBufferForTestStimulus[i]= TestStimulus.A *0.5*(1-cos(2*PI*i/(win_t-1))) * sin( 2*PI*TestStimulus.F*TestStimulus.del_t*i );
		}
		else if(i>(ulCount-win_t/2.0)) {
			pdOutputBufferForTestStimulus[i]= TestStimulus.A *0.5*(1-cos(2*PI*(ulCount-i)/(win_t-1))) * sin( 2*PI*TestStimulus.F*TestStimulus.del_t*i );
		}
		else
		{
			pdOutputBufferForTestStimulus[i] = TestStimulus.A * sin( 2*PI*TestStimulus.F*TestStimulus.del_t*i );
		}
	}

	return true;
}




//======================== Confirmed
float64 CShakerMainExperimentDoc::ComputeVoltageFromDisplacement(double dDisplacement, int iFrequency)
{
	//float64 volt;

	float64 voltage;

	//double disp_mm = dDisplacement/1000;

	//for (int j=0;j<iNumberOfAmplitudeLevels;j++) {
	//	if(disp_mm > pDisplacement[iCurrentConditionIndex*iNumberOfAmplitudeLevels + j]) { //기준치보다 높으면 무시
	//		if(j==(iNumberOfAmplitudeLevels-1)) //마지막 step인데도 작은게 안나오면 마지막값으로 처리
	//			volt = 2;
	//		else
	//			continue;
	//	}
	//	else { //기준치보다 낮거나 같으면 계산 시작
	//		if(j==0) { //처음에는 0 이므로 예외처리
	//			volt = disp_mm*0.1/pDisplacement[iCurrentConditionIndex*iNumberOfAmplitudeLevels + j];
	//			break;
	//		}
	//		else {
	//			volt = j*0.1+0.1*(disp_mm-pDisplacement[iCurrentConditionIndex*iNumberOfAmplitudeLevels + j-1])/(pDisplacement[iCurrentConditionIndex*iNumberOfAmplitudeLevels + j] - pDisplacement[iCurrentConditionIndex*iNumberOfAmplitudeLevels + j-1]); 
	//			break;
	//		}
	//	}
	//}

	voltage = (dDisplacement/1000.0 - pfOffset[iCurrentConditionIndex])/pfSlope[iCurrentConditionIndex];	// check this out later
	return voltage;
}



//====================== Confirmed
bool CShakerMainExperimentDoc::PrepareOutputFile()
{
	char temp[200];
	char file_name[200];
	int i;

	// open an output data file
	strcpy(file_name, main_dir_path);
	strcat(file_name, "\\");
	strcat(file_name, (LPCTSTR) sSubjectCode);
	strcat(file_name, "_");
	strcat(file_name, "Detection_Threshold.dat");

	pOutputFileID = fopen(file_name, "a");
	if ( pOutputFileID==NULL ) {
		MessageBox(NULL, "The output file for this experiment cannot be opened or created.",
			"Error!!", MB_OK | MB_ICONERROR);
		return false; 
	}

	if ( iCurrentCondition == 0 ) {
		// write down some header info if this is the first session of experiment
		fprintf(pOutputFileID, "[Threshold Data File for Detection Thresholds Measured with a Shaker]\n");
		fprintf(pOutputFileID, "[Subject Code]\n%s\n", (LPCTSTR) sSubjectCode);
		fprintf(pOutputFileID, "[Subject Name]\n%s\n", (LPCTSTR) sSubjectFirstName);	
		//fprintf(pOutputFileID, "[Subject Age]\n%d\n", iSubjectAge); 
		//fprintf(pOutputFileID, "[Subject Gender]\n%s\n", (LPCTSTR) sSubjectGender); 
		//fprintf(pOutputFileID, "[Subject Dominant Hand]\n%s\n", (LPCTSTR) sSubjectDominantHand); 

		fprintf(pOutputFileID, "[Experiment Number]\n%d\n", iExperimentNumber); 
		fprintf(pOutputFileID, "[Tested Frequencies (Hz)]\n");
		for ( i=0 ; i<iNumberOfFrequencies; i++ )
			fprintf(pOutputFileID, "%d\t", piFrequencies[i]);
		fprintf(pOutputFileID, "\n");
		fprintf(pOutputFileID, "[Number of Reversals]\n%d\n", iNumberOfReversals); 
		fprintf(pOutputFileID, "[Step Sizes (dB)]\n"); 
		for (i=0 ; i<iNumberOfReversals ; i++ )
			fprintf(pOutputFileID, "%f\t", pfStepSizes[i]);
		fprintf(pOutputFileID, "\n");
	}

	// open a measurement data file
	strcpy(file_name, main_dir_path);
	strcat(file_name, "\\");
	strcat(file_name, (LPCTSTR) sSubjectCode);
	strcat(file_name, "_");
	strcat(file_name, "Measurement.dat");

	pMeasurementDataOutputFileID = fopen(file_name, "a");
	if ( pMeasurementDataOutputFileID==NULL ) {
		MessageBox(NULL, "The output file for this experiment cannot be opened or created.",
			"Error!!", MB_OK | MB_ICONERROR);
		return false; 
	}
	if ( iCurrentCondition == 0 ) {
		fprintf(pMeasurementDataOutputFileID, "[Measurement Data File for Detection Thresholds Measured with a Shaker]\n");
		fprintf(pMeasurementDataOutputFileID, "[Subject Code]\n%s\n", (LPCTSTR) sSubjectCode);
		fprintf(pMeasurementDataOutputFileID, "[Subject Name]\n%s\n", (LPCTSTR) sSubjectFirstName);	
		//fprintf(pMeasurementDataOutputFileID, "[Subject Age]\n%d\n", iSubjectAge); 
		//fprintf(pMeasurementDataOutputFileID, "[Subject Gender]\n%s\n", (LPCTSTR) sSubjectGender); 
		//fprintf(pMeasurementDataOutputFileID, "[Subject Dominant Hand]\n%s\n", (LPCTSTR) sSubjectDominantHand); 

		fprintf(pMeasurementDataOutputFileID, "[Experiment Number]\n%d\n", iExperimentNumber); 
		fprintf(pMeasurementDataOutputFileID, "[Tested Frequencies (Hz)]\n");
		for ( i=0 ; i<iNumberOfFrequencies; i++ )
			fprintf(pMeasurementDataOutputFileID, "%d\t", piFrequencies[i]);
		fprintf(pMeasurementDataOutputFileID, "\n");
		fprintf(pMeasurementDataOutputFileID, "[Number of Reversals]\n%d\n", iNumberOfReversals); 
		fprintf(pMeasurementDataOutputFileID, "[Step Sizes (dB)]\n"); 
		for (i=0 ; i<iNumberOfReversals ; i++ )
			fprintf(pMeasurementDataOutputFileID, "%f\t", pfStepSizes[i]);
		fprintf(pMeasurementDataOutputFileID, "\n");

		fprintf(pMeasurementDataOutputFileID, "[Measurement Data]\n"); 
	}

	return true;
}




////========================= Confirmed
//bool LoadFTSensorCalibration(struct ft_sensor *pMyFTSensor)
//{
//	short sts;              // return value from functions
//
//	// initialize calibration file location
//	pMyFTSensor->pcCalFilePath = (char *) malloc(50*sizeof(char));
//	strcpy(pMyFTSensor->pcCalFilePath, FT_SENSOR_CAL_FILE);
//	
//	pMyFTSensor->iIndex = FT_SENSOR_INDEX;
//
//	// create Calibration struct
//	pMyFTSensor->pCal = createCalibration(pMyFTSensor->pcCalFilePath,pMyFTSensor->iIndex);
//	if ( pMyFTSensor->pCal==NULL) {
//		MessageBox(NULL, "The calibration file for the F/T sensor does not exist.",
//			"Error!!", MB_OK | MB_ICONERROR);
//		return false;
//	}
//	
//	// Set force units.
//	// This step is optional; by default, the units are inherited from the calibration file.
//	sts=SetForceUnits(pMyFTSensor->pCal,"N");
//	switch (sts) {
//		case 0: 
//			break;	// successful completion
//		case 1: 
//			MessageBox(NULL, "Invalid F/T sensor calibration struct.",
//				"Error!!", MB_OK | MB_ICONERROR);
//			return false;
//		case 2: 
//			MessageBox(NULL, "Invalid F/T sensor force units.",
//				"Error!!", MB_OK | MB_ICONERROR);
//			return false;
//		default: 
//			MessageBox(NULL, "Unknown error in force sensor calibration data.",
//				"Error!!", MB_OK | MB_ICONERROR);
//			return false;
//	}
//
//	// Set torque units.
//	// This step is optional; by default, the units are inherited from the calibration file.
//	sts=SetTorqueUnits(pMyFTSensor->pCal,"N-m");
//	switch (sts) {
//		case 0: 
//			break;	// successful completion
//		case 1: 
//			MessageBox(NULL, "Invalid F/T sensor calibration struct.",
//				"Error!!", MB_OK | MB_ICONERROR);
//			return false;
//		case 2: 
//			MessageBox(NULL, "Invalid F/T sensor torque units.",
//				"Error!!", MB_OK | MB_ICONERROR);
//			return false;
//		default: 
//			MessageBox(NULL, "Unknown error in force sensor calibration data.",
//				"Error!!", MB_OK | MB_ICONERROR);
//			return false;
//	}
//
//	return true;
//}
//
//
//
//
////========================= Confirmed
//bool SetBiasFTSensor(CShakerMainExperimentDoc *pDoc, struct ft_sensor *pMyFTSensor)
//{
//	// Local Variable Declarations: 
//	int i,j;
//	int error=0;
//	char errBuff[2048]={'\0'};
//
//	int32 read;
//	int32 ulCount;
//
//	ulCount=10000;
//
//	/*********************************************/
//	/*/ DAQmx Configure Code
//	/*********************************************/
//	DAQmxErrChk (DAQmxCfgSampClkTiming(pDoc->taskHandleInput,"", 10000 ,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,
//		ulCount));
//
//	// input buffer. 1 sec and 9 input channels.
//	pDoc->pdInputData = (float64 *) malloc(ulCount*9*sizeof(float64));
//
//	/*********************************************/
//	/*/ DAQmx Start Code
//	/*********************************************/
//	DAQmxErrChk (DAQmxStartTask(pDoc->taskHandleInput));
//
//	/*********************************************/
//	/*/ DAQmx Wait Code
//	/*********************************************/
//	DAQmxErrChk (DAQmxReadAnalogF64(pDoc->taskHandleInput,ulCount,10.0,DAQmx_Val_GroupByChannel,pDoc->pdInputData,ulCount*9,&read,NULL));
//	DAQmxErrChk (DAQmxWaitUntilTaskDone(pDoc->taskHandleInput,10.0));
//	DAQmxStopTask(pDoc->taskHandleInput);
//	
//
//	float64 temp;
//
//	for (i=0;i<7;i++) {
//		temp=0.0;
//		for (j=0;j<ulCount;j++) {
//			temp+=pDoc->pdInputData[i*ulCount+j];
//		}
//		pMyFTSensor->pfFTBias[i]=temp/10000;
//	}
//
//	// Set bias
//	Bias(pMyFTSensor->pCal, pMyFTSensor->pfFTBias);
//
//	// free memory
//	free(pDoc->pdInputData);
//
//	printf(">> Force sensor bias set.\n");
//
//	return true;
//
//Error:
//	if( pDoc->taskHandleInput!=0 ) {
//		/*********************************************/
//		/*/ DAQmx Stop Code
//		/*********************************************/
//		DAQmxStopTask(pDoc->taskHandleInput);
//		DAQmxClearTask(pDoc->taskHandleInput);
//	}
//
//	if( DAQmxFailed(error) ) {
//		DAQmxGetExtendedErrorInfo(errBuff,2048);
//		MessageBox(NULL, errBuff, "Error!!", MB_OK | MB_ICONERROR);
//		return false; 
//	}
//
//	return true;
//}
//

//========================= Confirmed
void CShakerMainExperimentDoc::ClearUp()
{
	if ( piFrequencies!= NULL )
		delete piFrequencies;
	if ( pfStepSizes != NULL )
		delete pfStepSizes;
	if ( piOrderOfFrequencies != NULL )
		delete piOrderOfFrequencies ;
	if ( pfRoughThresholds!= NULL )
		delete pfRoughThresholds;
	if ( pfSlope != NULL )
		delete pfSlope;
	if ( pfOffset != NULL )
		delete pfOffset;

	if ( pdInputData != NULL )
		free(pdInputBuffer);
	if ( pdOutputData != NULL )
		free(pdOutputBuffer);
	if ( pdInputBuffer != NULL )
		free(pdInputBuffer);
	if ( pdOutputBuffer != NULL )
		free(pdOutputBuffer);
	if ( pdOutputBufferForNullStimulus != NULL )
	{
		
		//free(pdOutputBufferForNullStimulus);
		pdOutputBufferForNullStimulus=NULL;
	}
	if ( pdOutputBufferForTestStimulus != NULL )
//		free(pdOutputBufferForTestStimulus);
	pdOutputBufferForTestStimulus=NULL;

	if ( pOutputFileID != NULL )
		fclose(pOutputFileID);
	if ( pMeasurementDataOutputFileID != NULL )
		fclose(pMeasurementDataOutputFileID);
	if (facc != NULL)
		fclose(facc);

}

//============================= Confirmed
bool CShakerMainExperimentDoc::ReadExperimentData()
{
	char temp[200];
	char file_name[200];
	int i, ret, num;
	FILE *config_file_id;

	// open the config file
	strcpy(file_name, main_dir_path);
	strcat(file_name, "\\");
	strcat(file_name, "config_");
	strcat(file_name, (LPCTSTR)sSubjectCode);
	strcat(file_name, ".cfg");

	config_file_id = fopen(file_name, "r");

	if ( config_file_id==NULL ) {
		MessageBox(NULL, "The configuration file for this experiment does not exist.",
			"Error!!", MB_OK | MB_ICONERROR);
		fclose(config_file_id);
		return false;
	}

	// read the data from the config file
	// Experiment Number
	do {
		ret = fscanf(config_file_id, "%s", temp);
	} while (ret != EOF && strcmp(temp, "[Number]") != 0 );
	if (ret == EOF) {
		MessageBox(NULL, "The experiment configuration file is corrupted.\nExperiment aborted.",
			"Error!!", MB_OK | MB_ICONERROR);
		fclose(config_file_id);
		return false;	
	}
	fscanf(config_file_id, "%d", &iExperimentNumber);

	
	// subject name
	do {
		ret = fscanf(config_file_id, "%s", temp);
	} while (ret != EOF && strcmp(temp, "[Name]") != 0 );
	if (ret == EOF) {
		MessageBox(NULL, "The experiment configuration file is corrupted.\nExperiment aborted.",
			"Error!!", MB_OK | MB_ICONERROR);
		fclose(config_file_id);
		return false;	
	}
	fscanf(config_file_id, "%s", &temp);
	sSubjectFirstName = temp;
	//fscanf(config_file_id, "%s", &temp);
	//sSubjectLastName = temp;

	//// subject age
	//do {
	//	ret = fscanf(config_file_id, "%s", temp);
	//} while (ret != EOF && strcmp(temp, "Age]") != 0 );
	//if (ret == EOF) {
	//	MessageBox(NULL, "The experiment configuration file is corrupted.\nExperiment aborted.",
	//		"Error!!", MB_OK | MB_ICONERROR);
	//	fclose(config_file_id);
	//	return false;	
	//}
	//fscanf(config_file_id, "%d", &iSubjectAge);

	//// subject gender
	//do {
	//	ret = fscanf(config_file_id, "%s", temp);
	//} while (ret != EOF && strcmp(temp, "Gender]") != 0 );
	//if (ret == EOF) {
	//	MessageBox(NULL, "The experiment configuration file is corrupted.\nExperiment aborted.",
	//		"Error!!", MB_OK | MB_ICONERROR);
	//	fclose(config_file_id);
	//	return false;	
	//}
	//fscanf(config_file_id, "%s", &temp);
	//sSubjectGender = temp;

	//// subject dominant hand
	//do {
	//	ret = fscanf(config_file_id, "%s", temp);
	//} while (ret != EOF && strcmp(temp, "Hand]") != 0 );
	//if (ret == EOF) {
	//	MessageBox(NULL, "The experiment configuration file is corrupted.\nExperiment aborted.",
	//		"Error!!", MB_OK | MB_ICONERROR);
	//	fclose(config_file_id);
	//	return false;	
	//}
	//fscanf(config_file_id, "%s", &temp);
	//sSubjectDominantHand = temp;

	// number of frequencies
	do {
		ret = fscanf(config_file_id, "%s", temp);
	} while (ret != EOF && strcmp(temp, "[Frequencies]") != 0 );
	if (ret == EOF) {
		MessageBox(NULL, "The experiment configuration file is corrupted.\nExperiment aborted.",
			"Error!!", MB_OK | MB_ICONERROR);
		fclose(config_file_id);
		return false;	
	}
	fscanf(config_file_id, "%d", &iNumberOfFrequencies);

	// frequencies
	piFrequencies = new int[iNumberOfFrequencies];
	do {
		ret = fscanf(config_file_id, "%s", temp);
	} while (ret != EOF && strcmp(temp, "[(Hz)]") != 0 );
	if (ret == EOF) {
		MessageBox(NULL, "The experiment configuration file is corrupted.\nExperiment aborted.",
			"Error!!", MB_OK | MB_ICONERROR);
		fclose(config_file_id);
		return false;	
	}
	for (i=0; i<iNumberOfFrequencies; i++ )
		fscanf(config_file_id, "%d", piFrequencies+i);

	// initial signal intensities
	pfRoughThresholds = new float[iNumberOfFrequencies];
	do {
		ret = fscanf(config_file_id, "%s", temp);
	} while (ret != EOF && strcmp(temp, "[(micrometer)]") != 0 );
	if (ret == EOF) {
		MessageBox(NULL, "The experiment configuration file is corrupted.\nExperiment aborted.",
			"Error!!", MB_OK | MB_ICONERROR);
		fclose(config_file_id);

		return false;	
	}
	for (i=0; i<iNumberOfFrequencies; i++ )
		fscanf(config_file_id, "%f", pfRoughThresholds+i);
	
	// inter-stimulus interval
	do {
		ret = fscanf(config_file_id, "%s", temp);
	} while (ret != EOF && strcmp(temp, "[ISI(ms)]") != 0 );
	if (ret == EOF) {
		MessageBox(NULL, "The experiment configuration file is corrupted.\nExperiment aborted.",
			"Error!!", MB_OK | MB_ICONERROR);
		fclose(config_file_id);
		return false;	
	}
	fscanf(config_file_id, "%d", &iInterStimulusInterval);

	// signal duration
	do {
		ret = fscanf(config_file_id, "%s", temp);
	} while (ret != EOF && strcmp(temp, "[Duration(ms)]") != 0 );
	if (ret == EOF) {
		MessageBox(NULL, "The experiment configuration file is corrupted.\nExperiment aborted.",
			"Error!!", MB_OK | MB_ICONERROR);
		fclose(config_file_id);
		return false;	
	}
	fscanf(config_file_id, "%d", &iStimulusDuration);

	// number of reversals
	do {
		ret = fscanf(config_file_id, "%s", temp);
	} while (ret != EOF && strcmp(temp, "[Reversals]") != 0 );
	if (ret == EOF) {
		MessageBox(NULL, "The experiment configuration file is corrupted.\nExperiment aborted.",
			"Error!!", MB_OK | MB_ICONERROR);
		fclose(config_file_id);
		return false;	
	}
	fscanf(config_file_id, "%d", &iNumberOfReversals);

	// step sizes
	pfStepSizes = new float[iNumberOfReversals];
	do {
		ret = fscanf(config_file_id, "%s", temp);
	} while (ret != EOF && strcmp(temp, "[Step(dB)]") != 0 );
	if (ret == EOF) {
		MessageBox(NULL, "The experiment configuration file is corrupted.\nExperiment aborted.",
			"Error!!", MB_OK | MB_ICONERROR);
		fclose(config_file_id);
		return false;	
	}
	for (i=0; i<iNumberOfReversals; i++ )
		fscanf(config_file_id, "%f", pfStepSizes+i);

	// feedback
	do {
		ret = fscanf(config_file_id, "%s", temp);
	} while (ret != EOF && strcmp(temp, "[Feedback]") != 0 );
	if (ret == EOF) {
		MessageBox(NULL, "The experiment configuration file is corrupted.\nExperiment aborted.",
			"Error!!", MB_OK | MB_ICONERROR);
		fclose(config_file_id);
		return false;	
	}
	fscanf(config_file_id, "%s", &temp);
	sFeedback = temp;

	// stimulus order
	piOrderOfFrequencies = new int[iNumberOfFrequencies];
	//for (i=0; i<iNumberOfFrequencies ; i++ ) 
	//	piOrderOfFrequencies[i] = piFrequencies[i];

	////randomize order
	//srand((unsigned)time(NULL));

	//for (int i = 0; i<10; i++){
	//	for(int j=0;j<iNumberOfFrequencies;j++) {
	//		swap<int>(&piOrderOfFrequencies[j],&piOrderOfFrequencies[rand()%iNumberOfFrequencies]);
	//	}
	//}

	do {
		ret = fscanf(config_file_id, "%s", temp);
	} while (ret != EOF && strcmp(temp, "(Hz)") != 0 );
	if (ret == EOF) {
		MessageBox(NULL, "The experiment configuration file is corrupted.\nExperiment aborted.",
			"Error!!", MB_OK | MB_ICONERROR);
		fclose(config_file_id);
		return false;	
	}
	for (i=0; i<iNumberOfFrequencies ; i++ ) {
		fscanf(config_file_id, "%d %d", &num, piOrderOfFrequencies+i);  
	}

	int ret_value;
	while (1) {
		do {
			ret_value=fscanf(config_file_id, "%s", temp);
			if ( ret_value==EOF)
				break;
		} while ( strcmp(temp, "--") != 0 );
		if ( ret_value==EOF )
			break;
		
		fscanf(config_file_id, "%d", &iCurrentCondition);
	}

	fclose(config_file_id);

	// set a flag meaning that ready for experiment
	bIsDataReady = true;

	return true;
}



//=========================== Confirmed
void CShakerMainExperimentDoc::OnUpdateExperimentStartexperiment(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here

   pCmdUI->Enable(!bIsExperimentStarted);	

}



//========================= Confirmed
bool CShakerMainExperimentDoc::ReadCalibrationFile()
{
	char temp[200];
	char file_name[200];
	int i, ret;
	FILE *calibration_file_id;

	// open the config file
	strcpy(file_name, main_dir_path);
	strcat(file_name, "\\shaker_calibration.shc");
	calibration_file_id = fopen(file_name, "r");

	if ( calibration_file_id==NULL ) {
		MessageBox(NULL, "The shaker calibration file does not exist.",
			"Error!!", MB_OK | MB_ICONERROR);
		fclose(calibration_file_id);
		return false;
	}


	// number of frequencies
	int num_frequencies_calibration;
	do {
		ret = fscanf(calibration_file_id, "%s", temp);
	} while (ret != EOF && strcmp(temp, "Frequencies]") != 0 );
	if (ret == EOF) {
		MessageBox(NULL, "The shaker calibration file is corrupted.\nExperiment aborted.",
			"Error!!", MB_OK | MB_ICONERROR);
		return false;	
		fclose(calibration_file_id);
	}
	fscanf(calibration_file_id, "%d", &num_frequencies_calibration);


	// calibration data
	do {
		ret = fscanf(calibration_file_id, "%s", temp);
	} while (ret != EOF && strcmp(temp, "Data]") != 0 );
	if (ret == EOF) {
		MessageBox(NULL, "The shaker calibration file is corrupted.\nExperiment aborted.",
			"Error!!", MB_OK | MB_ICONERROR);
		return false;	
		fclose(calibration_file_id);
	}

	if ( num_frequencies_calibration != iNumberOfFrequencies ) {
		MessageBox(NULL, "The shaker calibration file have a different number of frequencies\nExperiment aborted.",
			"Error!!", MB_OK | MB_ICONERROR);
		fclose(calibration_file_id);
		return false;	
	}

	// calibration data
	do {
		ret = fscanf(calibration_file_id, "%s", temp);
	} while (ret != EOF && strcmp(temp, "Offset") != 0 );
	if (ret == EOF) {
		MessageBox(NULL, "The shaker calibration file is corrupted.\nExperiment aborted.",
			"Error!!", MB_OK | MB_ICONERROR);
		fclose(calibration_file_id);
		return false;	
	}

	pfOffset = (float*)malloc(iNumberOfFrequencies * sizeof(float));
	pfSlope = (float*)malloc(iNumberOfFrequencies * sizeof(float));


	double tmp;
	for (int i=0 ; i<iNumberOfFrequencies ; i++ ) {
		fscanf(calibration_file_id,"%f %f %f",&tmp, &pfSlope[i], &pfOffset[i]);
	}


	fclose(calibration_file_id);
	return true;
}


//======================== Confirmed
bool CShakerMainExperimentDoc::HarewareInitialize()
{
	int error=0;
	char errBuff[2048]={'\0'};

	/*********************************************/
	/*/ DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandleOutput));
	DAQmxErrChk (DAQmxCreateAOVoltageChan(taskHandleOutput,"Dev1/ao0","",-5.0,5.0,DAQmx_Val_Volts,NULL));

	DAQmxErrChk (DAQmxCreateTask("",&taskHandleInput));
	DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandleInput,"Dev1/ai0","",DAQmx_Val_Diff,-5.0,5.0,DAQmx_Val_Volts,NULL));

	return true;

Error:
	if( taskHandleInput!=0 ) {
		/*********************************************/
		/*/ DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandleInput);
		DAQmxClearTask(taskHandleInput);
	}
	if( taskHandleOutput!=0 ) {
		/*********************************************/
		/*/ DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandleOutput);
		DAQmxClearTask(taskHandleOutput);
	}
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		MessageBox(NULL, errBuff, "Error!!", MB_OK | MB_ICONERROR);
		return false; 
	}

	return true;

}



//======================== Confirmed

bool CShakerMainExperimentDoc::SetIOModeOfDAQCard()
{
	// Local Variable Declarations: 
	int error=0;
	char errBuff[2048]={'\0'};


	ulCount=UPDATE_RATE  /* 10 KHz update rate */
	* (float)iStimulusDuration / 1000.0   /* now in sec */;

	/*********************************************/
	/*/ DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandleOutput,"",	UPDATE_RATE, DAQmx_Val_Rising,DAQmx_Val_FiniteSamps, 
		ulCount));

	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandleInput,"", UPDATE_RATE,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,
		ulCount));

	return true;


Error:
	if( taskHandleInput!=0 ) {
		/*********************************************/
		/*/ DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandleInput);
		DAQmxClearTask(taskHandleInput);
	}
	if( taskHandleOutput!=0 ) {
		/*********************************************/
		/*/ DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandleOutput);
		DAQmxClearTask(taskHandleOutput);
	}
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		MessageBox(NULL, errBuff, "Error!!", MB_OK | MB_ICONERROR);
		return false; 
	}

	return true;

}

