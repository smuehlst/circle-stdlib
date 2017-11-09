#ifndef _circle_stdlib_app_h
#define _circle_stdlib_app_h

#include <circle/actled.h>
#include <circle/koptions.h>
#include <circle/devicenameservice.h>
#include <circle/nulldevice.h>
#include <circle/exceptionhandler.h>
#include <circle/interrupt.h>
#include <circle/screen.h>
#include <circle/serial.h>
#include <circle/timer.h>
#include <circle/logger.h>
#include <circle/usb/dwhcidevice.h>
#include <SDCard/emmc.h>
#include <circle/fs/fat/fatfs.h>
#include <circle/input/console.h>
#include <circle/sched/scheduler.h>
#include <circle/net/netsubsystem.h>

#include <circle_glue.h>

class CStdlibApp
{
public:
	enum TAppType
	{
		AppTypeGPIO,
		AppTypeScreen,
		AppTypeStdio,
		AppTypeNetwork
	};

	enum TShutdownMode
	{
		ShutdownNone,
		ShutdownHalt,
		ShutdownReboot
	};

	CStdlibApp (TAppType type) :
		mAppType (type), mpPartitionName ("emmc1-1"),
		mpScreen (0), mpSerial (0), mpTimer (0), mpLogger (0),
		mpDWHCI (0), mpEMMC (0), mpFileSystem (0), mpConsole (0),
		mpScheduler (0), mpNet (0),
		FromKernel ("kernel")
	{
		if (mAppType >= AppTypeScreen)
		{
			mpScreen = new CScreenDevice (mOptions.GetWidth (), mOptions.GetHeight ());
			mpSerial = new CSerialDevice;
			mpTimer = new CTimer (&mInterrupt);
			mpLogger = new CLogger (mOptions.GetLogLevel (), mpTimer);
		}

		if (mAppType >= AppTypeStdio)
		{
			mpDWHCI = new CDWHCIDevice (&mInterrupt, mpTimer);
			mpEMMC = new CEMMCDevice (&mInterrupt, mpTimer, &mActLED);
			mpFileSystem = new CFATFileSystem;
			mpConsole = new CConsole (mpScreen);
		}

		if (mAppType >= AppTypeNetwork)
		{
			mpScheduler = new CScheduler;
			mpNet = new CNetSubSystem;
		}
	}

	virtual bool Initialize (void)
	{
		if (!mInterrupt.Initialize ())
		{
			return false;
		}

		if (mAppType >= AppTypeScreen)
		{
			if (!mpScreen->Initialize ())
			{
				return false;
			}

			if (!mpSerial->Initialize (115200))
			{
				return false;
			}

			CDevice *pTarget =
				mDeviceNameService.GetDevice (mOptions.GetLogDevice (), FALSE);
			if (pTarget == 0)
			{
				pTarget = mpScreen;
			}

			if (!mpLogger->Initialize (pTarget))
			{
				return false;
			}
		}

		if (mAppType >= AppTypeStdio)
		{
			if (!mpTimer->Initialize ())
			{
				return false;
			}

			if (!mpEMMC->Initialize ())
			{
				return false;
			}

			CDevice * const pPartition =
				mDeviceNameService.GetDevice (mpPartitionName, TRUE);
			if (pPartition == 0)
			{
				mpLogger->Write (FromKernel, LogError,
					       "Partition not found: %s", mpPartitionName);

				return false;
			}

			if (!mpFileSystem->Mount (pPartition))
			{
				mpLogger->Write (FromKernel, LogError,
						 "Cannot mount partition: %s", mpPartitionName);

				return false;
			}

			if (!mpDWHCI->Initialize ())
			{
				return false;
			}

			if (!mpConsole->Initialize ())
			{
				return false;
			}

			// Initialize newlib stdio with a reference to Circle's file system and console
			CGlueStdioInit (*mpFileSystem, *mpConsole);
		}

		if (mAppType >= AppTypeNetwork)
		{
			if (!mpNet->Initialize ())
			{
				return false;
			}
		}

		if (mAppType >= AppTypeScreen)
		{
			mpLogger->Write (FromKernel, LogNotice, "Compile time: " __DATE__ " " __TIME__);
		}

		return true;
	}

	virtual TShutdownMode Run (void) = 0;

	virtual void Cleanup (void)
	{
		if (mAppType >= AppTypeStdio)
		{
			mpFileSystem->UnMount ();
		}
	}

private:
	TAppType const mAppType;

	char const *mpPartitionName;

protected:
	// Defaults
	CActLED		   mActLED;
	CKernelOptions	   mOptions;
	CDeviceNameService mDeviceNameService;
	CNullDevice	   mNullDevice;
	CExceptionHandler  mExceptionHandler;
	CInterruptSystem   mInterrupt;

	// Options
	CScreenDevice	*mpScreen;
	CSerialDevice	*mpSerial;
	CTimer		*mpTimer;
	CLogger		*mpLogger;
	CDWHCIDevice	*mpDWHCI;
	CEMMCDevice	*mpEMMC;
	CFATFileSystem	*mpFileSystem;
	CConsole	*mpConsole;
	CScheduler	*mpScheduler;
	CNetSubSystem	*mpNet;

	char const *FromKernel;
};

#endif
