// A test program for detection threshold measurements for probe-mediated exploration.
// Analog input/output using NI PCI 6229 and NIDAQmX
// ---------
// 1. Generate sine wave through the board (background operation)
// 2. Measure acceleration data through the board (background operation)
// 3. Measure force data through the board (background operation)
// 4. The I/O is NOT synchronized. 
// ---------
// Seungmoon Choi, 6/26/2005
// Modified: Ali Israr, 8/05/2005

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "shaker.h"


// global variables
TaskHandle  taskHandleInput, taskHandleOutput;	// task handles for the DAQ card
float64  *pdOutputData, *pdInputData;
struct sine_wave mySineWave;
struct ft_sensor myFTSensor;



// function declarations
void init_parameters();
void load_FT_sensor_calibration();
void init_DAQ_channels();
char get_user_input();
void compute_sine_wave();
void set_bias_FT_sensor();
void DAQ_operation();

void save_data_to_file();
void free_memory();
void clear_FT_sensor();


#define DAQmxErrChk(functionCall) { if( DAQmxFailed(error=(functionCall)) ) { goto Error; } }


// File variables for Calibration process
FILE * infile;
FILE * outfile;
char file_name[100];


void main(void)
{
	// Local Variable Declarations: 
	//char c;
	float64 dFrequency, dAmplitude;
	float64 dUpdaterate, dDuration;


    // initialize paramters for the DAQ board
	init_parameters();

	// load calibration data of the force sensor
	//load_FT_sensor_calibration();

	// intialize the DAQ board
	init_DAQ_channels();

	
	printf("\n>Press ENTER to start the Calibration process\n\n");
	getchar();

	infile = fopen("InputCalibrationData.txt","r");
		
	if (infile == NULL ) {
		printf(">Error! Cannot open the data file to read the data.\n");
		printf("Press ENTER to terminate this application \n\n");
		getchar();
		exit(0);
	}

	else {
		printf("\n\n>Input data file for the calibration process is successfully opened.\n\n");

	}

		// Run loop until data starts in the datafile.
	while (!(fgetc(infile)=='/')) 
		;
    
	// this is a main loop
	while (!(fgetc(infile) == EOF))
	{
		// get parameters for the sine wave output
		//c=get_user_input();

		fscanf(infile,"%Lf%Lf%Lf%Lf",&dFrequency, &dAmplitude, &dUpdaterate, &dDuration);

		// setting sinewave parameters
		mySineWave.A=dAmplitude;
		mySineWave.F=dFrequency;
		mySineWave.UR=dUpdaterate;
		mySineWave.SD=dDuration;

		printf("\nThe Data for %f Hz and %f Volts is being tested\n",mySineWave.F, mySineWave.A);
	
		// compute parameters for signal generation
		compute_sine_wave();
		
		// set F/T sensor bias
		// store an unloaded measurement; this removes the effect of tooling weight
		//set_bias_FT_sensor();

		// do data collection
		DAQ_operation();

		// save data to a file
		save_data_to_file();

		// free memory
		free_memory();
	
	}

	fclose(infile);

	
	// clear out the F/T sensor
	//clear_FT_sensor();

	// DAQ clear codes
	DAQmxClearTask(taskHandleInput);
	DAQmxClearTask(taskHandleOutput);

	printf("\n>Press ENTER to end the Calibration process\n\n");
	getchar();
}


///////////////////////////////////////////////////
// Set up I/O channels and initialize parameters
void init_parameters()
{

	// default wave form
	mySineWave.A=2;		// V	
	mySineWave.F=100;	// Hz
	mySineWave.UR=10000;	// Hz
	mySineWave.SD=1;		// sec	

}


////////////////////////////////////////////////////
// Load calibration matrix for the F/T sensor
//void load_FT_sensor_calibration()
//{
//	short sts;              // return value from functions
//
//	// initialize calibration file location
//	myFTSensor.pcCalFilePath = (char *) malloc(50*sizeof(char));
//	strcpy(myFTSensor.pcCalFilePath, FT_SENSOR_CAL_FILE);
//	
//	myFTSensor.iIndex = FT_SENSOR_INDEX;
//
//	// create Calibration struct
//	myFTSensor.pCal = createCalibration(myFTSensor.pcCalFilePath,myFTSensor.iIndex);
//	if ( myFTSensor.pCal==NULL) {
//		printf(">> Error! Specified calibration could not be loaded.\n");
//		exit(1);
//	}
//	
//	// Set force units.
//	// This step is optional; by default, the units are inherited from the calibration file.
//	sts=SetForceUnits(myFTSensor.pCal,"N");
//	switch (sts) {
//		case 0: 
//			break;	// successful completion
//		case 1: 
//			printf(">> Error! Invalid calibration struct.\n"); 
//			exit(1);
//		case 2: 
//			printf(">> Error! Invalid force units.\n"); 
//			exit(1);;
//		default: 
//			printf(">> Error! Unknown error in force sensor calibration data.\n"); 
//			exit(1);
//	}
//
//	// Set torque units.
//	// This step is optional; by default, the units are inherited from the calibration file.
//	sts=SetTorqueUnits(myFTSensor.pCal,"N-m");
//	switch (sts) {
//		case 0: 
//			break;	// successful completion
//		case 1: 
//			printf(">> Error! Invalid calibration struct.\n"); 
//			exit(1);
//		case 2: 
//			printf(">> Error! Invalid torque units.\n"); 
//			exit(1);;
//		default: 
//			printf(">> Error! Unknown error in force sensor calibration data.\n"); 
//			exit(1);
//	}
//}
//
//
//

//////////////////////////////////////////////////////////////////////////
// Print current parameter values and get an user input to update them
char get_user_input()
{
	char c;
	float temp;

	do {
		// print parameter values
		printf(">> Current Parameter Values:\n");
		printf("   - Shaker input amplitude = %4.2f     - Frequency = %5.0f\n", mySineWave.A, mySineWave.F);
		printf("   - Update rate = %7.0f               - Signal duration = %3.1f\n", mySineWave.UR, mySineWave.SD);
		
		// get user input
		puts("\n\n>> Instructions:");
		puts("  -- 'u': Update rate (Hz) ");
		puts("  -- 'd': Signal duration (sec)");
		puts("  -- 'a': Signal amplitude (V)");
		puts("  -- 'f': Signal frequency (Hz)");
		puts("  -- 'g': Go with previous settings");
		puts("  -- 'q': Quit");
		printf(">> What do you want? ");
	
		c=getchar();
		if ( c=='a' || c=='A') {
			printf("\n>> Input amplitude (V):");
			scanf("%f", &temp);
			mySineWave.A=(float64)temp;
		} else if ( c=='f' || c=='F') {
			printf("\n>> Input frequency (Hz):");
			scanf("%f", &temp);
			mySineWave.F=(float64)temp;
		} else if ( c=='u' || c=='U') {
			printf("\n>> Input update rate (Hz):");
			scanf("%f", &temp);
			mySineWave.UR=(float64)temp;
		} else if ( c=='d' || c=='D') {
			printf("\n>> Input signal duration (Sec):");
			scanf("%f", &temp);
			mySineWave.SD=(float64)temp;
		} 
		printf("\n");

	} while (c!='g' && c!='G' && c!='q' && c!='Q');


	return c;
}


//////////////////////////////////////////////////////
// Compute sine waveform to send to the DAQ board
void compute_sine_wave()
{
	int32 ulCount, i;
	
	ulCount = (int32) (mySineWave.UR*mySineWave.SD);  

	// allocate memory for output buffer
	pdOutputData = (float64 *) malloc(2*ulCount*sizeof(float64));
	if ( pdOutputData==NULL ) {
		printf(">> Error! Cannot allocate memory for float buffer of analog output.\n");
		exit(1);
	}
		
	// compute values of the sine waveform for shaker output
	mySineWave.del_t = (float64)1.0/mySineWave.UR;
	double win_t = mySineWave.UR/2.0; //window size, 10 ms window를 반 나누어 앞과 뒤에 씌움. 5 ms.
	for ( i=0 ; i<ulCount ; i++ ) {
		if(i<win_t/2.0) { //Window 씌우기
			pdOutputData[i]= mySineWave.A *0.5*(1-cos(2*PI*i/(win_t-1))) * sin( 2*PI*mySineWave.F*mySineWave.del_t*i );
		}
		else if(i>(ulCount-win_t/2.0)) {
			pdOutputData[i]= mySineWave.A *0.5*(1-cos(2*PI*(ulCount-i)/(win_t-1))) * sin( 2*PI*mySineWave.F*mySineWave.del_t*i );
		}
		else
		{
			pdOutputData[i] = mySineWave.A * sin( 2*PI*mySineWave.F*mySineWave.del_t*i );
		}
	}

	pdOutputData[ulCount-1]=0;

	printf(">> Output sine waveform is computed.\n");
}



////////////////////////////////////////////////
// Drive shaker and measure data
void DAQ_operation()
{
	// Local Variable Declarations: 
	int error=0;
	char errBuff[2048]={'\0'};

	int32   	written;
	int32       read;
	int32 ulCount;

	ulCount=mySineWave.UR*mySineWave.SD;

	/*********************************************/
	/*/ DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandleOutput,"",	mySineWave.UR, DAQmx_Val_Rising,DAQmx_Val_FiniteSamps, 
		mySineWave.UR*mySineWave.SD));

	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandleInput,"", mySineWave.UR ,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,
		mySineWave.UR*mySineWave.SD));

	// input buffer. 2 input channels.
	pdInputData = (float64 *) malloc(ulCount*sizeof(float64));

	/*********************************************/
	/*/ DAQmx Write Code
	/*********************************************/
	DAQmxErrChk (DAQmxWriteAnalogF64(taskHandleOutput,ulCount,0,10.0,DAQmx_Val_GroupByChannel,pdOutputData,&written,NULL));

	/*********************************************/
	/*/ DAQmx Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxStartTask(taskHandleInput)); //읽기 시작 후
	DAQmxErrChk (DAQmxStartTask(taskHandleOutput));//쓴다. delay 손상 막음

	/*********************************************/
	/*/ DAQmx Wait Code
	/*********************************************/
	DAQmxErrChk (DAQmxReadAnalogF64(taskHandleInput,ulCount,10.0,DAQmx_Val_GroupByChannel,pdInputData,ulCount,&read,NULL));

	DAQmxErrChk (DAQmxWaitUntilTaskDone(taskHandleInput,10.0)); 
	DAQmxErrChk (DAQmxWaitUntilTaskDone(taskHandleOutput,10.0)); 

	DAQmxStopTask(taskHandleInput);
	DAQmxStopTask(taskHandleOutput);

	printf("Acquired %d points\n",read);
	
	return;

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
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
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
	printf("End of program, press Enter key to quit\n");
	getchar();

}


//////////////////////////////////////////////////////
// Save collected data to a file
void save_data_to_file()
{
	// Local Variable Declarations: 
	int32 i, si, ei;
	int32 j;

	int32 ulCount;
	
	ulCount = (int32) (mySineWave.UR*mySineWave.SD);


	//char file_name[50];
	FILE *fid;
	float pfFT[6], pfSampleReading[7], *pfFTFileOutput;

	// save data to a file
	// rename a file name		
	sprintf(file_name,".\\DataFiles\\Data%0.0fHz%0.4fV%0.0fHz%0.2fsec.dat",mySineWave.F,mySineWave.A,mySineWave.UR,mySineWave.SD);
	
		
	// open a file to write data
	j=1;
	while ( j!=0 ) {
		fid=fopen(file_name, "w");
		if (fid==NULL ) {
			printf(">Error! Cannot open the data file.\n");
		} else
			j=0;
	}

	// write data to the file
	// output sine wave information
	fprintf(fid, "%f %f %f %f\n", mySineWave.A, mySineWave.F, 
		mySineWave.UR, mySineWave.SD);
// 조금 수정할 것

	// write amp output voltages;
	si=0;
	ei=ulCount;
	for (i=si ; i<ei ; i++ ) 
		fprintf(fid, "%f\t", pdOutputData[i]);	// in V
	
	//For FFT, zero padding
	for (i=ei; i<10000;i++)  
		fprintf(fid, "%f\t", 0.0);

	fprintf(fid, "\n");

	
		// write accelerometer Z output;
	si=0*ulCount;
	ei=1*ulCount;
	for (i=si ; i<ei ; i++ ) 
		fprintf(fid, "%f\t", pdInputData[i]*ACC_AMP_GAIN);	// in G //중력변환부분확인 2.0 10G/5V

	//For FFT, zero padding
	for (i=ei; i<10000;i++)  
		fprintf(fid, "%f\t", 0.0);

	fprintf(fid, "\n");

/*
	// write accelerometer X output;
	si=0*ulCount;
	ei=1*ulCount;
	for (i=si ; i<ei ; i++ ) 
		fprintf(fid, "%f\t", pdInputData[i]*ACC_AMP_GAIN);	// in G //중력변환부분확인 2.0 10G/5V
	fprintf(fid, "\n");

	
	
	// write accelerometer Y output;
	si=1*ulCount;
	ei=2*ulCount;
	for (i=si ; i<ei ; i++ ) 
		fprintf(fid, "%f\t", pdInputData[i]*ACC_AMP_GAIN);	// in G //중력변환부분확인 2.0 10G/5V
	fprintf(fid, "\n");*/


	// write F/T sensor output
	////pfFTFileOutput = (float *) malloc(sizeof(float)*6*ulCount);
	////for (i=0 ; i<ulCount; i++ ) {
	////	for (j=0; j<7 ; j++ )
	////		pfSampleReading[j] 
	////			= (float) pdInputData[i+j*ulCount];
	////	ConvertToFT(myFTSensor.pCal, pfSampleReading, pfFT);
	////	for (j=0; j<6 ; j++ )
	////		pfFTFileOutput[i+j*ulCount] = pfFT[j];
	////}

	////for (j=0 ; j<6 ; j++ ) {
	////	si=j*ulCount;
	////	ei=(j+1)*ulCount;
	////	for (i=si ; i<ei ; i++ )
	////		fprintf(fid, "%f\t", pfFTFileOutput[i]);
	////	fprintf(fid, "\n");
	////}

	fclose(fid);

	//free(pfFTFileOutput);

}

///////////////////////////////////////////
// Free memories
void free_memory()
{
	free(pdInputData);
	free(pdOutputData);
}


///////////////////////////////////////////////////////
// Clear data structre for the F/T sensor
//void clear_FT_sensor()
//{
//	// free memory allocated to Calibration structure
//	destroyCalibration(myFTSensor.pCal);
//	free(myFTSensor.pcCalFilePath);
//}



////////////////////////////////////////////////
// Set the bias of the F/T senseor
//void set_bias_FT_sensor()
//{
//	// Local Variable Declarations: 
//	int i,j;
//	int error=0;
//	char errBuff[2048]={'\0'};
//
//	int32       read;
//	int32 ulCount;
//
//	ulCount=10000;
//
//	/*********************************************/
//	/*/ DAQmx Configure Code
//	/*********************************************/
//	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandleInput,"", 10000 ,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,
//		ulCount));
//
//	// input buffer. 1 sec and 9 input channels.
//	pdInputData = (float64 *) malloc(ulCount*9*sizeof(float64));
//
//	/*********************************************/
//	/*/ DAQmx Start Code
//	/*********************************************/
//	DAQmxErrChk (DAQmxStartTask(taskHandleInput));
//
//	/*********************************************/
//	/*/ DAQmx Wait Code
//	/*********************************************/
//	DAQmxErrChk (DAQmxReadAnalogF64(taskHandleInput,ulCount,10.0,DAQmx_Val_GroupByChannel,pdInputData,ulCount*9,&read,NULL));
//	DAQmxErrChk (DAQmxWaitUntilTaskDone(taskHandleInput,10.0));
//	DAQmxStopTask(taskHandleInput);
//	
//
//	float64 temp;
//
//	for (i=0;i<7;i++) {
//		temp=0.0;
//		for (j=0;j<ulCount;j++) {
//			temp+=pdInputData[i*ulCount+j];
//		}
//		myFTSensor.pfFTBias[i]=temp/10000;
//	}
//
//	// Set bias
//	Bias(myFTSensor.pCal, myFTSensor.pfFTBias);
//
//	// free memory
//	free(pdInputData);
//
//	printf(">> Force sensor bias set.\n");
//
//	return;
//
//Error:
//	if( DAQmxFailed(error) )
//		DAQmxGetExtendedErrorInfo(errBuff,2048);
//	if( taskHandleInput!=0 ) {
//		/*********************************************/
//		/*/ DAQmx Stop Code
//		/*********************************************/
//		DAQmxStopTask(taskHandleInput);
//		DAQmxClearTask(taskHandleInput);
//	}
//
//	if( DAQmxFailed(error) )
//		printf("DAQmx Error: %s\n",errBuff);
//	printf("End of program, press Enter key to quit\n");
//	getchar();
//
//}
//
//
//
//// initialize DAQ channels
//// Note that each init functions takes 2-3 seconds to run.
//


void	init_DAQ_channels()
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

	return;

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
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
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
	printf("End of program, press Enter key to quit\n");
	getchar();
}






