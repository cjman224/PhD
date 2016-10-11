// To use NI DAQ library and data types

#include "NIDAQmx.h"


// To use force sensor library

#include ".\atidaq\ftconfig.h"

//**************************************************
// I/O Channel set up of the DAQ board 
// -------------------------------------------------
//
// Analog input:
// Channel 0: SG0 of the nano 17 F/T sensor
// Channel 1: SG1 of the nano 17 F/T sensor
// Channel 2: SG2 of the nano 17 F/T sensor
// Channel 3: SG3 of the nano 17 F/T sensor
// Channel 4: SG4 of the nano 17 F/T sensor
// Channel 5: SG5 of the nano 17 F/T sensor
// Channel 6: z axis output of the accelerometer
// Channel 7: connected to Channel 0 of the analog output to get the shaker input voltage
//
// Analog output:
// Channel 0: input to the shaker
//**************************************************


#define NUMBER_INPUT_CHANNEL 9	// number of input channels used 

#define FORCE_INPUT_CHANNEL_0 0	// force sensor output 
#define FORCE_INPUT_CHANNEL_1 1	// force sensor output 
#define FORCE_INPUT_CHANNEL_2 2	// force sensor output 
#define FORCE_INPUT_CHANNEL_3 3	// force sensor output 
#define FORCE_INPUT_CHANNEL_4 4	// force sensor output 
#define FORCE_INPUT_CHANNEL_5 5	// force sensor output
#define FORCE_INPUT_CHANNEL_TEMP 6	// force sensor output 
#define ACC_INPUT_CHANNEL 1		// accelerometer output 
#define AMP_OUTPUT_CHANNEL  6	// to measure the actual shaker input voltage coming out of the amp


#define ACC_AMP_GAIN 5 	// accelerometer amplifier gain (old one)

#define NUMBER_OUTPUT_CHANNELS 1	// number of output channels



// macros for output waveform generation
#define OP_START 1
#define OP_CLEAR 0

//. F/T sensor calibration file info
#define FT_SENSOR_CAL_FILE "d:\\\\program files\\ati daq ft\\ft5251.cal"
#define FT_SENSOR_INDEX 1

// miscellaneous macros
//#define PI 3.141592			//PI


//****************************
// a structure for the computation of sine analog output wavewform
struct sine_wave {
	float64 A, F, UR, SD;
	float64 del_t;
};


//****************************
// a structure for force sensor calibration
struct ft_sensor{
	char *pcCalFilePath;	// name of calibration file
	unsigned short iIndex;	// index of calibration in file (second parameter; default = 1)
	Calibration *pCal;	// struct containing calibration information
	float pfFTBias[7];	// force sensor bias
};

