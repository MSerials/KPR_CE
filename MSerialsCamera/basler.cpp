#include "basler.h"
#include <iostream>
//不能使用stdafx.h
//#include "stdafx.h"

//#if _WIN32
//#include "stdafx.h"
//#endif
#define GIGE_PACKET_SIZE       1500 /* Size of one Ethernet packet. */
#define GIGE_PROTOCOL_OVERHEAD 36   /* Total number of bytes of protocol overhead. */
#define CHECK( errc ) printErrorAndExit( errc )

int printErrorAndExit(GENAPIC_RESULT errc)
{
	if (GENAPI_E_OK == errc) return 1;
	char *errMsg;
	size_t length;

	/* Retrieve the error message.
	... First find out how big the buffer must be, */
	GenApiGetLastErrorMessage(NULL, &length);
	errMsg = (char*)alloca(length);
	/* ... and retrieve the message. */
	GenApiGetLastErrorMessage(errMsg, &length);

	fprintf(stderr, "%s (%#08x).\n", errMsg, (unsigned int)errc);

	PylonTerminate();  /* Releases all pylon resources. */
	return 0;
}


void getMinMax(const unsigned char* pImg, int32_t width, int32_t height,
	unsigned char* pMin, unsigned char* pMax)
{
	unsigned char min = 255;
	unsigned char max = 0;
	unsigned char val;
	const unsigned char *p;

	for (p = pImg; p < pImg + width * height; p++)
	{
		val = *p;
		if (val > max)
			max = val;
		if (val < min)
			min = val;
	}
	*pMin = min;
	*pMax = max;
}

Basler::~Basler()
{
	//puts("freeing basler camera\n");
	_close();  
}

int Basler::refresh_list()
{
	/* Before using any pylon methods, the pylon runtime must be initialized. */
	PylonInitialize();
	/* Enumerate all devices. You must call
	PylonEnumerateDevices() before creating a device. */
	res = PylonEnumerateDevices(&numDevicesAvail);

	if (numDevicesAvail < 1)
	{
		std::cout << "no basler camera found" << std::endl;
		PylonTerminate();
		error = -1;
		return 1;
	}

	camera_num = numDevicesAvail<NUM_DEVICES? numDevicesAvail: NUM_DEVICES ;
	printf("%d basler cameras found\n",camera_num);

	/* Create wait objects (must be done outside of the loop). */
	res = PylonWaitObjectsCreate(&wos);
	if (!CHECK(res)) return 2;

#ifdef GENAPIC_WIN_BUILD
	/* Create a Windows timer, wrap it in a pylon C wait object, and add it to
	the wait object set. */
	hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
	if (hTimer == NULL)
	{
		PylonTerminate();
		error = -2;
		return 3;
	}
	res = PylonWaitObjectFromW32(hTimer, 0, &woTimer);
	error = res;
	if (!CHECK(res)) return 4;
	
#else
	/* Create a Linux timer, wrap it in a pylon C wait object, and add it to
	the wait object set. */
	fdTimer = timerfd_create(CLOCK_MONOTONIC, 0);
	if (fdTimer == -1)
	{
		fprintf(stderr, "timerfd_create() failed. %s\n", strerror(errno));
		PylonTerminate();
		pressEnterToExit();
		exit(EXIT_FAILURE);
	}
	res = PylonWaitObjectFromFd(fdTimer, &woTimer);
	CHECK(res);
#endif
	res = PylonWaitObjectsAdd(wos, woTimer, NULL);
	error = res;
	if (!CHECK(res)) return 5;

	
	/* Open cameras and set parameters. */
	for (deviceIndex = 0; deviceIndex < camera_num; ++deviceIndex)
	{
		PylonDeviceInfo_t di;

		/* Get a handle for the device. */
		res = PylonCreateDeviceByIndex(deviceIndex, &hDev[deviceIndex]);
		error = res;
		if(!CHECK(res)) return 6;

		/* Before using the device, it must be opened. Open it for setting
		parameters and for grabbing images. */
		res = PylonDeviceOpen(hDev[deviceIndex], PYLONC_ACCESS_MODE_CONTROL | PYLONC_ACCESS_MODE_STREAM);
		error = res;
		if (!CHECK(res)) return 7;

		/* Print out the name of the camera we are using. */
		{
			char buf[256];
			size_t siz = sizeof(buf);
			_Bool isReadable;

			isReadable = PylonDeviceFeatureIsReadable(hDev[deviceIndex], "DeviceModelName");
			if (isReadable)
			{
				res = PylonDeviceFeatureToString(hDev[deviceIndex], "DeviceModelName", buf, &siz);
				error = res;
				if (!CHECK(res)) return 8;
				printf("Using camera '%s'\n", buf);
			}
		}

		/* Set the pixel format to Mono8, where gray values will be output as 8 bit values for each pixel. */
		/* ... First check to see if the device supports the Mono8 format. */
		isAvail = PylonDeviceFeatureIsAvailable(hDev[deviceIndex], "EnumEntry_PixelFormat_Mono8");
		if (!isAvail)
		{
			/* Feature is not available. */
			fprintf(stderr, "Device doesn't support the Mono8 pixel format");
			PylonTerminate();
			//pressEnterToExit();
			exit(EXIT_FAILURE);
		}

		/* ... Set the pixel format to Mono8. */
		res = PylonDeviceFeatureFromString(hDev[deviceIndex], "PixelFormat", "Mono8");
		error = res;
		if (!CHECK(res)) return 9;

		/* Disable acquisition start trigger if available. */
		isAvail = PylonDeviceFeatureIsAvailable(hDev[deviceIndex], "EnumEntry_TriggerSelector_AcquisitionStart");
		if (isAvail)
		{
			res = PylonDeviceFeatureFromString(hDev[deviceIndex], "TriggerSelector", "AcquisitionStart");
			error = res;
			if (!CHECK(res)) return 10;
			res = PylonDeviceFeatureFromString(hDev[deviceIndex], "TriggerMode", "Off");
			error = res;
			if (!CHECK(res)) return 11;
		}

		/* Disable frame burst start trigger if available. */
		isAvail = PylonDeviceFeatureIsAvailable(hDev[deviceIndex], "EnumEntry_TriggerSelector_FrameBurstStart");
		if (isAvail)
		{
			res = PylonDeviceFeatureFromString(hDev[deviceIndex], "TriggerSelector", "FrameBurstStart");
			error = res;
			if (!CHECK(res)) return 12;
			res = PylonDeviceFeatureFromString(hDev[deviceIndex], "TriggerMode", "Off");
			error = res;
			if (!CHECK(res)) return 13;
		}

		/* Disable frame start trigger if available. */
		isAvail = PylonDeviceFeatureIsAvailable(hDev[deviceIndex], "EnumEntry_TriggerSelector_FrameStart");
		if (isAvail)
		{
			res = PylonDeviceFeatureFromString(hDev[deviceIndex], "TriggerSelector", "FrameStart");
			error = res;
			if (!CHECK(res)) return 14;
			res = PylonDeviceFeatureFromString(hDev[deviceIndex], "TriggerMode", "Off");
			error = res;
			if (!CHECK(res)) return 15;
		}

		/* We will use the Continuous frame mode, i.e., the camera delivers images continuously. */
		res = PylonDeviceFeatureFromString(hDev[deviceIndex], "AcquisitionMode", "Continuous");
		error = res;
		if (!CHECK(res)) return 14;


		res = PylonDeviceGetDeviceInfo(hDev[deviceIndex], &di);
		error = res;
		if (!CHECK(res)) return 15;
		if (strcmp(di.DeviceClass, "BaslerGigE") == 0)
		{
			/* For GigE cameras, we recommend increasing the packet size for better
			performance. When the network adapter supports jumbo frames, set the packet
			size to a value > 1500, e.g., to 8192. In this sample, we only set the packet size
			to 1500.

			Also we set the Inter-Packet and the Frame Transmission delay
			so the switch can line up packets better.
			*/

			res = PylonDeviceSetIntegerFeature(hDev[deviceIndex], "GevSCPSPacketSize", GIGE_PACKET_SIZE);
			error = res;
			if (!CHECK(res)) return 16;

			res = PylonDeviceSetIntegerFeature(hDev[deviceIndex], "GevSCPD", (GIGE_PACKET_SIZE + GIGE_PROTOCOL_OVERHEAD)*(NUM_DEVICES - 1));
			error = res;
			if (!CHECK(res)) return 17;

			res = PylonDeviceSetIntegerFeature(hDev[deviceIndex], "GevSCFTD", (GIGE_PACKET_SIZE + GIGE_PROTOCOL_OVERHEAD) * deviceIndex);
			error = res;
			if (!CHECK(res)) return 18;
		}
#ifdef GENAPIC_WIN_BUILD
		else if (strcmp(di.DeviceClass, "Basler1394") == 0)
		{
			/* For FireWire we just set the PacketSize node to limit the bandwidth we're using. */

			/* We first divide the available bandwidth (4915 for FW400, 9830 for FW800)
			by the number of devices we are using. */
			int64_t newPacketSize = 4915 / NUM_DEVICES;
			int64_t recommendedPacketSize = 0;

			/* Get the recommended packet size from the camera. */
			res = PylonDeviceGetIntegerFeature(hDev[deviceIndex], "RecommendedPacketSize", &recommendedPacketSize);
			error = res;
			if (!CHECK(res)) return 19;

			if (newPacketSize < recommendedPacketSize)
			{
				/* Get the increment value for the packet size.
				We must make sure that the new value we're setting is dividable by the increment of that feature. */
				int64_t packetSizeInc = 0;
				res = PylonDeviceGetIntegerFeatureInc(hDev[deviceIndex], "PacketSize", &packetSizeInc);
				error = res;
				if (!CHECK(res)) return 20;

				/* Adjust the new packet size so is dividable by its increment. */
				newPacketSize -= newPacketSize % packetSizeInc;
			}
			else
			{
				/* The recommended packet size should always be valid. Accordingly, there will be no need to check against the increment. */
				newPacketSize = recommendedPacketSize;
			}


			/* Set the new packet size. */
			res = PylonDeviceSetIntegerFeature(hDev[deviceIndex], "PacketSize", newPacketSize);
			error = res;
			if (!CHECK(res)) return 21;

#if __STDC_VERSION__ >= 199901L
			printf("Using packetsize: %lld\n", newPacketSize);
#else
			printf("Using packetsize: %I64d\n", newPacketSize);
#endif
		}
#endif
	}


	/* Allocate and register buffers for grab. */
	for (deviceIndex = 0; deviceIndex < camera_num; ++deviceIndex)
	{
		size_t i;
		PYLON_WAITOBJECT_HANDLE hWait;
		int32_t payloadSize;


		/* Determine the required size of the grab buffer. */
		res = PylonDeviceGetIntegerFeatureInt32(hDev[deviceIndex], "PayloadSize", &payloadSize);
		error = res;
		if (!CHECK(res)) return 22;


		/* Image grabbing is done using a stream grabber.
		A device may be able to provide different streams. A separate stream grabber must
		be used for each stream. In this sample, we create a stream grabber for the default
		stream, i.e., the first stream ( index == 0 ). */

		/* Get the number of streams supported by the device and the transport layer. */
		res = PylonDeviceGetNumStreamGrabberChannels(hDev[deviceIndex], &i);
		error = res;
		if (!CHECK(res)) return 23;

		if (i < 1)
		{
			fprintf(stderr, "The transport layer doesn't support image streams.\n");
			PylonTerminate();
			//pressEnterToExit();
			//exit(EXIT_FAILURE);
		}

		/* Create and open a stream grabber for the first channel. */
		res = PylonDeviceGetStreamGrabber(hDev[deviceIndex], 0, &hGrabber[deviceIndex]);
		error = res;
		if (!CHECK(res)) return 24;

		res = PylonStreamGrabberOpen(hGrabber[deviceIndex]);
		error = res;
		if (!CHECK(res)) return 25;


		/* Get a handle for the stream grabber's wait object. The wait object
		allows waiting for buffers to be filled with grabbed data. */
		res = PylonStreamGrabberGetWaitObject(hGrabber[deviceIndex], &hWait);
		error = res;
		if (!CHECK(res)) return 26;


		/* Add the stream grabber's wait object to our wait objects.
		This is needed to be able to wait until at least one camera has
		grabbed an image in the grab loop below. */
		res = PylonWaitObjectsAdd(wos, hWait, NULL);
		error = res;
		if (!CHECK(res)) return 27;



		/* Allocate memory for grabbing.  */
		for (i = 0; i < NUM_BUFFERS; ++i)
		{
			buffers[deviceIndex][i] = (unsigned char *)malloc(payloadSize);
			if (NULL == buffers[deviceIndex][i])
			{
				puts("Out of memory.\n");
				PylonTerminate();
				error = res;
				if (!CHECK(res)) return 28;

			}
		}

		/* We must tell the stream grabber the number and size of the buffers we are using. */
		/* .. We will not use more than NUM_BUFFERS for grabbing. */
		res = PylonStreamGrabberSetMaxNumBuffer(hGrabber[deviceIndex], NUM_BUFFERS);
		error = res;
		if (!CHECK(res)) return 29;
		/* .. We will not use buffers bigger than payloadSize bytes. */
		res = PylonStreamGrabberSetMaxBufferSize(hGrabber[deviceIndex], payloadSize);
		error = res;
		if (!CHECK(res)) return 30;


		/* Allocate the resources required for grabbing. After this, critical parameters
		that impact the payload size must not be changed until FinishGrab() is called. */
		res = PylonStreamGrabberPrepareGrab(hGrabber[deviceIndex]);
		error = res;
		if (!CHECK(res)) return 31;


		/* Before using the buffers for grabbing, they must be registered at
		the stream grabber. For each registered buffer, a buffer handle
		is returned. After registering, these handles are used instead of the
		raw pointers. */
		for (i = 0; i < NUM_BUFFERS; ++i)
		{
			res = PylonStreamGrabberRegisterBuffer(hGrabber[deviceIndex], buffers[deviceIndex][i], payloadSize, &bufHandles[deviceIndex][i]);
			error = res;
			if (!CHECK(res)) return 32;
		}

		/* Feed the buffers into the stream grabber's input queue. For each buffer, the API
		allows passing in a pointer to additional context information. This pointer
		will be returned unchanged when the grab is finished. In our example, we use the index of the
		buffer as context information. */
		for (i = 0; i < NUM_BUFFERS; ++i)
		{
			res = PylonStreamGrabberQueueBuffer(hGrabber[deviceIndex], bufHandles[deviceIndex][i], (void *)i);
			error = res;
			if (!CHECK(res)) return 33;
		}
	}


	/* The stream grabber is now prepared. As soon the camera starts to acquire images,
	the image data will be grabbed into the provided buffers.  */
	for (deviceIndex = 0; deviceIndex < camera_num; ++deviceIndex)
	{
		/* Let the camera acquire images. */
		res = PylonDeviceExecuteCommandFeature(hDev[deviceIndex], "AcquisitionStart");
		/* do not call CHECK() here! Instead exit the loop */
		if (res != GENAPI_E_OK)
		{
			break;
		}
	}

	return 0;
}

void Basler::_close()
{
	printf("closing basler camera    \n");
	if (error != GENAPI_E_OK)
		return;
	/* Clean up. */

	/* Stop the image acquisition on the cameras. */
	for (deviceIndex = 0; deviceIndex < camera_num; ++deviceIndex)
	{
		/* ... Stop the camera. */
		res = PylonDeviceExecuteCommandFeature(hDev[deviceIndex], "AcquisitionStop");
		CHECK(res);
	}

	/* Remove all wait objects from waitobjects. */
	res = PylonWaitObjectsRemoveAll(wos);
	CHECK(res);
	res = PylonWaitObjectDestroy(woTimer);
	CHECK(res);
	res = PylonWaitObjectsDestroy(wos);
	CHECK(res);


	for (deviceIndex = 0; deviceIndex < camera_num; ++deviceIndex)
	{
		size_t i;
		_Bool rdy;
		PylonGrabResult_t grabResult;

		/* ... We must issue a cancel call to ensure that all pending buffers are put into the
		stream grabber's output queue. */
		res = PylonStreamGrabberCancelGrab(hGrabber[deviceIndex]);
		CHECK(res);

		/* ... The buffers can now be retrieved from the stream grabber. */
		do
		{
			res = PylonStreamGrabberRetrieveResult(hGrabber[deviceIndex], &grabResult, &rdy);
			CHECK(res);
		} while (rdy);

		/* ... When all buffers are retrieved from the stream grabber, they can be deregistered.
		After deregistering the buffers, it is safe to free the memory. */

		for (i = 0; i < NUM_BUFFERS; ++i)
		{
			res = PylonStreamGrabberDeregisterBuffer(hGrabber[deviceIndex], bufHandles[deviceIndex][i]);
			CHECK(res);
			free(buffers[deviceIndex][i]);
		}

		/* ... Release grabbing related resources. */
		res = PylonStreamGrabberFinishGrab(hGrabber[deviceIndex]);
		CHECK(res);

		/* After calling PylonStreamGrabberFinishGrab(), parameters that impact the payload size (e.g.,
		the AOI width and height parameters) are unlocked and can be modified again. */

		/* ... Close the stream grabber. */
		res = PylonStreamGrabberClose(hGrabber[deviceIndex]);
		CHECK(res);


		/* ... Close and release the pylon device. The stream grabber becomes invalid
		after closing the pylon device. Don't call stream grabber related methods after
		closing or releasing the device. */
		res = PylonDeviceClose(hDev[deviceIndex]);
		CHECK(res);
		res = PylonDestroyDevice(hDev[deviceIndex]);
		CHECK(res);
	}


	PylonTerminate();
	printf("closing basler camera  over  \n");
}

int Basler::error_info()
{
	return error;
}

const char * Basler::vendor()
{
	return "Basler";
}

const char * Basler::sdk_version()
{
	return "pylon_5_Runtime_5.0.11.10913 https://www.baslerweb.com/cn/support/downloads/software-downloads/#type=pylonsoftware;language=all;version=5.0.11;os=all \n"; 
	return nullptr;
}

const char * Basler::CamerasInfo()
{
	return "no information in basler camera??";
}

int Basler::device_quantity()
{
	return camera_num;
}

int Basler::Snap(int & width, int & height, unsigned char ** data, int & ch, int camera_index, int delay)
{
	/* Only start the grab loop if all cameras have been "started" */
	if (error != GENAPI_E_OK )
	{
		width = 0; height = 0; *data = nullptr; ch = 0;return 1;
	}
	static std::mutex mtx[NUM_DEVICES];
	std::lock_guard<std::mutex> lck(mtx[camera_index]);
	Sleep(delay);
	_Bool isReady;
	size_t woidx;
	unsigned char min, max;
	PylonGrabResult_t grabResult;
	res = PylonStreamGrabberRetrieveResult(hGrabber[camera_index], &grabResult, &isReady);
	*data = (unsigned char*)grabResult.pBuffer;
	width = grabResult.SizeX;
	height = grabResult.SizeY;
	ch = 1;
	return 0;
	{

		LARGE_INTEGER intv;
		intv.QuadPart = -50000000I64;
		if (!SetWaitableTimer(hTimer, &intv, 0, NULL, NULL, FALSE))
		{
			printf("SetWaitableTimer() failed.\n");
			return 2;
			PylonTerminate();
		}




		/* Wait for the next buffer to be filled. Wait up to 1000 ms. */
		res = PylonWaitObjectsWaitForAny(wos, delay, &woidx, &isReady);
		if(!CHECK(res)) return 3;
		if (!isReady)
		{
			printf("basler grab time out\n");
			return 4;
		}

		/* Retrieve the grab result. */
		res = PylonStreamGrabberRetrieveResult(hGrabber[woidx], &grabResult, &isReady);
		if (!CHECK(res)) return 5;
		if (!isReady)
		{
				/* Oops. No grab result available? We should never have reached this point.
				Since the wait operation above returned without a timeout, a grab result
				should be available. */
				printf("Failed to retrieve a grab result\n");
				return 5;
		}

			/* Check to see if the image was grabbed successfully. */
			if (grabResult.Status == Grabbed)
			{
				/* Success. Perform image processing. Since we passed more than one buffer
				to the stream grabber, the remaining buffers are filled while
				we do the image processing. The processed buffer won't be touched by
				the stream grabber until we pass it back to the stream grabber. */

				/* Pointer to the buffer attached to the grab result
				Get the buffer pointer from the result structure. Since we also got the buffer index,
				we could alternatively use buffers[bufferIndex]. */
				//获取图像地方
				* data = (unsigned char*)grabResult.pBuffer;
				width = grabResult.SizeX;
				height = grabResult.SizeY;
				ch = 1;
				//unsigned char* buffer = (unsigned char*)grabResult.pBuffer;
				//getMinMax(data, grabResult.SizeX, grabResult.SizeY, &min, &max);
				/* Perform processing. */
				//getMinMax(buffer, grabResult.SizeX, grabResult.SizeY, &min, &max);


#ifdef GENAPIC_WIN_BUILD
				/* Display image */
				res = PylonImageWindowDisplayImageGrabResult(woidx, &grabResult);
				if (!CHECK(res)) return 6;
#endif
			}
			else if (grabResult.Status == Failed)
			{
				printf( "Frame  wasn't grabbed successfully.  Error code = 0x%08X\n", grabResult.ErrorCode);
			}

			/* Once finished with the processing, requeue the buffer to be filled again. */
			res = PylonStreamGrabberQueueBuffer(hGrabber[woidx], grabResult.hBuffer, grabResult.Context);
			if(!CHECK(res)) return 7;
		}
	



	return 0;
}

void Basler::set_exposure(double exposure, int camera_index)
{
	return;
}
