/**
 * This file is a basic template for implementing areaDetector plugins.
 * You must implement all of the functions already listed here along with any
 * additional plugin specific functions you require.
 *
 * Author:Michael Posada
 * Created on: 6/19/2019
 *
 */



 //include some standard libraries
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <filesystem>
#include <experimental/filesystem> // C++-standard header file name
#include <thread>
#include <sys/stat.h>

#include <libxml/parser.h>

//include epics/area detector libraries
#include <epicsMutex.h>
#include <epicsString.h>
#include <iocsh.h>
#include "NDArray.h"
// Include your plugin's header file here
#include "NDPluginTimelapse.h"
#include <epicsExport.h>

// include your external dependency libraries here
#include <opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>

//some basic namespaces
using namespace std;
using namespace cv;

// Name your plugin
static const char* pluginName = "NDPluginTimelapse";

asynStatus NDPluginTimelapse::writeOctet(asynUser* pasynUser, const char* value, size_t nChars, size_t* nActual)
{
	printf("Inside writeOctet\n");
	const char* functionName = "writeOctet";
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;
	printf("Inside writeOctet\n");

	status = setStringParam(function, value);


	asynPrint(this->pasynUserSelf, ASYN_TRACEIO_DRIVER, "%s::%s function = %d value=%d\n", pluginName, functionName, function, value);
	printf("Error after status inside Octet\n");
	// replace PLUGINNAME with your plugin (ex. BAR)
	if (function < ND_TIMELAPSE_FIRST_PARAM) {
		status = NDPluginDriver::writeOctet(pasynUser, value, nChars, nActual);
	}
	printf("Right before the callBack, inside Octet\n");
	callParamCallbacks();
	if (status) {
		asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Error writing Octet val to PV\n", pluginName, functionName);
	}
	return status;
}

/**
 * Override of NDPluginDriver function. Must be implemented by your plugin
 *
 * @params[in]: pasynUser	-> pointer to asyn User that initiated the transaction
 * @params[in]: value		-> value PV was set to
 * @return: success if PV was updated correctly, otherwise error
 */


asynStatus NDPluginTimelapse::writeInt32(asynUser* pasynUser, epicsInt32 value) {
	const char* functionName = "writeInt32";
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;
	int checkStatus;
	getIntegerParam(NDPluginTimelapseTlRecord, &checkStatus);
	if (checkStatus == 1 && value == 1)
	{
		//nothing happens
	}
	else if (checkStatus == 0 && value == 1)
	{
		//start recording
		//setIntegerParam(NDPluginTimelapseTlRecord, 1);
	}
	else if (checkStatus == 1 && value == 0)
	{
		//end recording
		//setIntegerParam(NDPluginTimelapseTlRecord, 0);
	}
	status = setIntegerParam(function, value);
	asynPrint(this->pasynUserSelf, ASYN_TRACEIO_DRIVER, "%s::%s function = %d value=%d\n", pluginName, functionName, function, value);
	// replace PLUGINNAME with your plugin (ex. BAR)
	if (function < ND_TIMELAPSE_FIRST_PARAM) {
		status = NDPluginDriver::writeInt32(pasynUser, value);
	}
	callParamCallbacks();
	if (status) {
		asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Error writing Int32 val to PV\n", pluginName, functionName);
	}
	return status;
}



/* Process callbacks function inherited from NDPluginDriver.
 * You must implement this function for your plugin to accept NDArrays
 *
 * @params[in]: pArray -> NDArray recieved by the plugin from the camera
 * @return: void
*/
void NDPluginTimelapse::processCallbacks(NDArray* pArray) {
	static const char* functionName = "processCallbacks";
	NDArray* pScratch;
	int camId;
	asynStatus status = asynSuccess;
	NDArrayInfo arrayInfo;
	//call base class and get information about frame
	NDPluginDriver::beginProcessCallbacks(pArray);
	// convert to Mat
	pArray->getInfo(&arrayInfo);

	//unlock the mutex for the processing portion
	this->unlock();

	// Process your image here.
	// Access data with pArray->pData
	// DO NOT CALL pArray.release()
	// If used, call pScratch.release()
	// use doCallbacksGenericPointer with pScratch to pass processed image to plugin array port
	int checkStatus;

	getIntegerParam(NDPluginTimelapseTlRecord, &checkStatus);

	char pathing[256];

	(asynStatus)getStringParam(NDPluginTimelapseTlFilename, sizeof(pathing), pathing);

	std::string finalPath = pathing;


	std::cout << finalPath << "\n";
	// change the if statement to do without filesystem
	struct stat buffer;

	FILE* path = fopen(pathing, "wb");

	if (path != NULL)
	{
		fclose(path);

		printf("Valid Pathing, we are good to record!\n");

		VideoCapture inputVideo(0);//input from camera!

		//Output here 
		int fourcc;
		fourcc = VideoWriter::fourcc('M', 'J', 'P', 'G');
		double fps = 5;
		//Size frameSize = Size ((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH), (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));
		Size frameSize = Size(arrayInfo.xSize, arrayInfo.ySize);
		bool isColor = true;

		VideoWriter cap(finalPath, fourcc, fps, frameSize, isColor); //opencv_ffmpeg.dll


		if (cap.isOpened() == true)
		{
			printf("It worked\n");

			Mat src, res;
			vector<Mat> spl;

			int channel = 2;
			unsigned char l;


			for (;;) //Show the image captured in the window and repeat
			{
				printf("Do I break after imdecode?");
				memcpy(&l, pArray->pData, arrayInfo.totalBytes);
				imdecode(l, 2, &src);
				printf("Do I break after imdecode?");
				//inputVideo >> src;             // read
				if (src.empty()) break;         // check if at end

				split(src, spl);                // process - extract only the correct channel
				for (int i = 0; i < 3; ++i)
				{
					if (i != channel)
						printf("In the for loop\n");

					spl[i] = Mat(frameSize, spl[0].type());
				}
				merge(spl, res);

				//outputVideo.write(res); //save or
				cap << res;
			}
			printf("Empty?");


		}
	}
	else {
		printf("Not valid");
	}


	this->lock();

	if (status == asynError) {
		asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Error, image not processed correctly\n", pluginName, functionName);
		return;
	}

	callParamCallbacks();
}

//A seprate thread for video recording

void NDPluginTimelapse::recording()
{
	char pathing;
	string finalPath;
	int addrs = 0;

	(asynStatus)getStringParam(addrs, NDPluginTimelapseTlFilename, &pathing); //Not allowed to do this

	finalPath = pathing + "";
	VideoCapture cap(finalPath);



}

//constructror from base class, replace with your plugin name
NDPluginTimelapse::NDPluginTimelapse(const char* portName, int queueSize, int blockingCallbacks,
	const char* NDArrayPort, int NDArrayAddr,
	int maxBuffers, size_t maxMemory,
	int priority, int stackSize)
	/* Invoke the base class constructor */
	: NDPluginDriver(portName, queueSize, blockingCallbacks,
		NDArrayPort, NDArrayAddr, 1, maxBuffers, maxMemory,
		asynInt32ArrayMask | asynFloat64ArrayMask | asynGenericPointerMask,
		asynInt32ArrayMask | asynFloat64ArrayMask | asynGenericPointerMask,
		ASYN_MULTIDEVICE, 1, priority, stackSize, 1)
{
	createParam(NDPluginTimelapseTlFilenameString, asynParamOctet, &NDPluginTimelapseTlFilename);
	createParam(NDPluginTimelapseTlRecordString, asynParamInt32, &NDPluginTimelapseTlRecord);

	char versionString[25];

	// Create PV parameters here
	// EXAMPLE:
	// createParam(PVString, 	asynParamOctet, 	&PVIndex);  -> string and waveform records
	// createParam(PVString, 	asynParamInt32, 	&PVIndex);  -> int records
	// createParam(PVString, 	asynParamFloat64, 	&PVIndex);  -> float records

	setStringParam(NDPluginDriverPluginType, "NDPluginTimelapse");
	epicsSnprintf(versionString, sizeof(versionString), "%d.%d.%d", TIMELAPSE_VERSION, TIMELAPSE_REVISION, TIMELAPSE_MODIFICATION);
	setStringParam(NDDriverVersion, versionString);
	connectToArrayPort();
}



/**
 * External configure function. This will be called from the IOC shell of the
 * detector the plugin is attached to, and will create an instance of the plugin and start it
 *
 * @params[in]	-> all passed to constructor
 */
extern "C" int NDTimelapseConfigure(const char* portName, int queueSize, int blockingCallbacks,
	const char* NDArrayPort, int NDArrayAddr,
	int maxBuffers, size_t maxMemory,
	int priority, int stackSize) {

	NDPluginTimelapse* pPlugin = new NDPluginTimelapse(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr,
		maxBuffers, maxMemory, priority, stackSize);
	return pPlugin->start();
}


/* IOC shell arguments passed to the plugin configure function */
static const iocshArg initArg0 = { "portName",iocshArgString };
static const iocshArg initArg1 = { "frame queue size",iocshArgInt };
static const iocshArg initArg2 = { "blocking callbacks",iocshArgInt };
static const iocshArg initArg3 = { "NDArrayPort",iocshArgString };
static const iocshArg initArg4 = { "NDArrayAddr",iocshArgInt };
static const iocshArg initArg5 = { "maxBuffers",iocshArgInt };
static const iocshArg initArg6 = { "maxMemory",iocshArgInt };
static const iocshArg initArg7 = { "priority",iocshArgInt };
static const iocshArg initArg8 = { "stackSize",iocshArgInt };
static const iocshArg* const initArgs[] = { &initArg0,
					&initArg1,
					&initArg2,
					&initArg3,
					&initArg4,
					&initArg5,
					&initArg6,
					&initArg7,
					&initArg8 };


// Define the path to your plugin's extern configure function above
static const iocshFuncDef initFuncDef = { "NDTimelapseConfigure",9,initArgs };


/* link the configure function with the passed args, and call it from the IOC shell */
static void initCallFunc(const iocshArgBuf* args) {
	NDTimelapseConfigure(args[0].sval, args[1].ival, args[2].ival,
		args[3].sval, args[4].ival, args[5].ival,
		args[6].ival, args[7].ival, args[8].ival);
}


/* function to register the configure function in the IOC shell */
extern "C" void NDTimelapseRegister(void) {
	iocshRegister(&initFuncDef, initCallFunc);
}


/* Exports plugin registration */
extern "C" {
	epicsExportRegistrar(NDTimelapseRegister);
}
