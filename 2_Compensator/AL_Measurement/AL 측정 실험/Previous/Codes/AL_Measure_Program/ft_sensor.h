#ifndef __FT_SENSOR__
#define __FT_SENSOR__

#include ".\atidaq\ftconfig.h"


//. F/T sensor calibration file info
#define FT_SENSOR_CAL_FILE "d:\\\\program files\\ati daq ft\\ft5251.cal"
#define FT_SENSOR_INDEX 1

//****************************
// a structure for force sensor calibration
struct ft_sensor{
	char *pcCalFilePath;	// name of calibration file
	unsigned short iIndex;	// index of calibration in file (second parameter; default = 1)
	Calibration *pCal;	// struct containing calibration information
	float pfFTBias[7];	// force sensor bias
};

#endif
