/**
 * Template header file for NDPluginTimelapse
 *
 *
 * Author:
 * Created on:
 *
 */

#ifndef NDPluginTimelapseH
#define NDPluginTimelapseH

 //Define necessary includes here


 // include your external dependency libraries here
#include <opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>

//include base plugin driver
#include "NDPluginDriver.h"


using namespace std;
using namespace cv;

//version numbers
#define TIMELAPSE_VERSION      	0
#define TIMELAPSE_REVISION     	0
#define TIMELAPSE_MODIFICATION 	0


#define NDPluginTimelapseTlFPSString "TL_FPS" //asynParamFloat64
#define NDPluginTimelapseTlFilenameString 		"TL_FILENAME" 	//asynParamOctet
#define NDPluginTimelapseTlRecordString 		"TL_RECORD" 	//asynParamInt32



// Define the PVStrings for all of your PV values here in the following format
//#define NDPluginTimelapsePVNameString 	"PV_STRING" 		//asynOctet


// define all necessary structs and enums here


/* Plugin class, extends plugin driver */
class NDPluginTimelapse : public NDPluginDriver {
public:
	NDPluginTimelapse(const char* portName, int queueSize, int blockingCallbacks,
		const char* NDArrayPort, int NDArrayAddr, int maxBuffers,
		size_t maxMemory, int priority, int stackSize);

	//~NDPlugin___();

	void processCallbacks(NDArray* pArray);

	virtual asynStatus writeInt32(asynUser* pasynUser, epicsInt32 value);
	virtual asynStatus writeOctet(asynUser* pasynUser, const char* value, size_t nChars, size_t* nActual);
	virtual asynStatus writeFloat64(asynUser* pasynUser, epicsFloat64 value);
	virtual void recording();
protected:

	//in this section i define the coords of database vals

	//Place PV indexes here, define first and last as appropriate, replace PLUGINNAME with name, 
	int NDPluginTimelapseTlRecord;
#define ND_TIMELAPSE_FIRST_PARAM NDPluginTimelapseTlRecord

	int NDPluginTimelapseTlFilename;
	int NDPluginTimelapseTlFPS;
#define ND_TIMELAPSE_LAST_PARAM NDPluginTimelapseTlFPS

private:

	// init all global variables here
	VideoWriter cap;
	bool onORoff;
	bool valid;
	// init all plugin additional functions here

};

// Replace PLUGINNAME with plugin name ex. BAR
#define NUM_TIMELAPSE_PARAMS ((int)(&ND_TIMELAPSE_LAST_PARAM - &ND_TIMELAPSE_FIRST_PARAM + 1))

#endif