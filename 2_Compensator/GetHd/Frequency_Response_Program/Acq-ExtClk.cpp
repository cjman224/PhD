/*********************************************************************
*
* ANSI C Example program:
*    ContAcq-IntClk.c
*
* Example Category:
*    AI
*
* Description:
*    This example demonstrates how to acquire a continuous amount of
*    data using the DAQ device's internal clock.
*
* Instructions for Running:
*    1. Select the physical channel to correspond to where your
*       signal is input on the DAQ device.
*    2. Enter the minimum and maximum voltage range.
*    Note: For better accuracy try to match the input range to the
*          expected voltage level of the measured signal.
*    3. Set the rate of the acquisition. Also set the Samples per
*       Channel control. This will determine how many samples are
*       read each time the while loop iterates. This also determines
*       how many points are plotted on the graph each iteration.
*    Note: The rate should be at least twice as fast as the maximum
*          frequency component of the signal being acquired.
*
* Steps:
*    1. Create a task.
*    2. Create an analog input voltage channel.
*    3. Set the rate for the sample clock. Additionally, define the
*       sample mode to be continuous.
*    4. Call the Start function to start the acquistion.
*    5. Read the data in a loop until the stop button is pressed or
*       an error occurs.
*    6. Call the Clear Task function to clear the task.
*    7. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal input terminal matches the Physical
*    Channel I/O control. For further connection information, refer
*    to your hardware reference manual.
*
*********************************************************************/

#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include "NIDAQmx.h"

#define DAQmxErrChk(functionCall) { if( DAQmxFailed(error=(functionCall)) ) { goto Error; } }
#define PI	3.1415926535
#define SamplingRate 44100
#define StimulusDuration 3
#define ScalingFactor 2.5
#define SensorGain 5
#define Repeat 1

int main(void)//int argc, char *argv[])
{
	int32       error=0;
	TaskHandle  inputHandle=0, outputHandle=0;
	int32       read, totalRead=0, written;
	float64     *data, *measuredData;
	char        errBuff[2048]={'\0'};
	char		outputFileName[50];
	int i, j, k;
	int scaledown = 1;

	FILE*		outputFile;

	data = (float64*)malloc(SamplingRate*StimulusDuration*sizeof(float64));
	measuredData = (float64*)malloc(SamplingRate*StimulusDuration*3*sizeof(float64));

	srand(time(NULL));
	

	for (i = 0; i < SamplingRate*(StimulusDuration) / scaledown; i++) {
		if (rand() % 2 == 1) 
			for (j = 0; j < scaledown; j++) 
				data[i*scaledown + j] = 1*ScalingFactor;
		else 
			for (j = 0; j < scaledown; j++) 
				data[i*scaledown + j] = -1*ScalingFactor;
	}

	/*for (i=0;i<SamplingRate*StimulusDuration;i++) {
		data[i] = sin(2*3.141592*300*(double)i/(double)SamplingRate);
	}*/
	sprintf(outputFileName,"Measured_%dHz_%ds.dat", SamplingRate, StimulusDuration);
	outputFile = fopen(outputFileName, "w");

	fprintf(outputFile, "%d %d\n", SamplingRate, StimulusDuration);
	for (i = 0; i < SamplingRate*StimulusDuration; i++) {
		fprintf(outputFile, "%lf ", data[i]);
	}
	fprintf(outputFile, "\n");



	for (k = 0; k < Repeat; k++) {
	/*********************************************/
	/*/ DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&inputHandle));
	DAQmxErrChk (DAQmxCreateTask("",&outputHandle));
	// 가속도 센서 X축 Analog Input 0, 가속도 센서 Y축 Analog Input 1, 가속도 센서 Z축 Analog Input 2
	DAQmxErrChk (DAQmxCreateAIVoltageChan(inputHandle,"Dev2/ai2","",DAQmx_Val_Diff,-10.0,10.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(inputHandle,"",SamplingRate,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,SamplingRate*StimulusDuration));

//	GetTerminalNameWithDevPrefix(inputHandle,"Dev1/ai/StartTrigger", "trigName");
	// 출력 신호 Analog Output 0
	DAQmxErrChk (DAQmxCreateAOVoltageChan(outputHandle,"Dev2/ao0","",-10.0,10.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(outputHandle,"",SamplingRate,DAQmx_Val_Falling,DAQmx_Val_FiniteSamps,SamplingRate*StimulusDuration));
//	DAQmxSetDigEdgeStartTrigSrc(outputHandle, "Dev1/ai/StartTrigger");
//	DAQmxSetDigEdgeStartTrigSrc(outputHandle, "trigName");

	/*********************************************/
	/*/ DAQmx Start Code
	/*********************************************/

	DAQmxErrChk (DAQmxWriteAnalogF64(outputHandle,SamplingRate*StimulusDuration,0,10.0,DAQmx_Val_GroupByChannel,data,&written,NULL));
	printf("written samples: %d\n", written);
	
	DAQmxErrChk (DAQmxStartTask(inputHandle));
	DAQmxErrChk (DAQmxStartTask(outputHandle));



	/*********************************************/
	/*/ DAQmx Read Code
	/*********************************************/
	DAQmxErrChk (DAQmxWaitUntilTaskDone(outputHandle,10.0));
	DAQmxErrChk (DAQmxReadAnalogF64(inputHandle,SamplingRate*StimulusDuration, 0,DAQmx_Val_GroupByChannel,measuredData,SamplingRate*StimulusDuration*3,&read,NULL));
	DAQmxErrChk (DAQmxWaitUntilTaskDone(inputHandle,10.0));

	DAQmxStopTask(outputHandle);
	DAQmxStopTask(inputHandle);
	DAQmxClearTask(inputHandle);
	DAQmxClearTask(outputHandle);



	for (j = 0; j < 1; j++) {
		for (i = 0; i < SamplingRate*StimulusDuration; i++) {
			fprintf(outputFile, "%lf ", measuredData[SamplingRate*StimulusDuration * j + i]*SensorGain);
		}
		fprintf(outputFile, "\n");
	}
	}
	for (i = 0; i < SamplingRate*(StimulusDuration) / scaledown; i++) {
		for (j = 0; j < scaledown; j++) 
			data[i*scaledown + j] = 0;
	}

	printf("\nAcquired samples.\n");
	fclose(outputFile);

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( inputHandle!=0 || outputHandle != 0) {
		/*********************************************/
		/*/ DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(inputHandle);
		DAQmxStopTask(outputHandle);
		DAQmxClearTask(inputHandle);
		DAQmxClearTask(outputHandle);
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
	printf("End of program, press Enter key to quit\n");
	getchar();
	return 0;
}

