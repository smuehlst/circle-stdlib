#ifndef _circle_glue_h
#define _circle_glue_h

#include <circle/fs/fat/fatfs.h>
#include <circle/input/console.h>
#include <circle/sched/scheduler.h>

/**
 * Initialize stdio for file system and console I/O (stdin/stdout/stderr).
 * Must only be called once.
 */
void CGlueStdioInit (CFATFileSystem& rFATFileSystem, CConsole& rConsole);

/**
 * Initialize stdio only for file system I/O.
 * Must only be called once.
 */
void CGlueStdioInit (CFATFileSystem& rFATFileSystem);

/**
 * Initialize stdio only for console I/O  (stdin/stdout/stderr).
 * Must only be called once.
 */
void CGlueStdioInit (CConsole& rConsole);

class CGlueIO
{
public:
        static constexpr unsigned GeneralFailure = ~0U;

        virtual ~CGlueIO () {}

        /**
         * Read from file sequentially
         *
         * Params:  hFile	File handle
         *	    pBuffer	Buffer to copy data to
         *	    ulCount	Number of bytes to read
         * Returns: != 0	Number of bytes read
         *	    0		End of file
         *	    0xFFFFFFFF	General failure
         */
        virtual unsigned Read (void *pBuffer, unsigned nCount) = 0;

        /**
         * Write to file sequentially
         *
         * Params:  hFile	File handle
         *	    pBuffer	Buffer to copy data from
         *	    nCount	Number of bytes to write
         * Returns: != 0	Number of bytes written
         *	    0xFFFFFFFF	General failure
         */
        virtual unsigned Write (const void *pBuffer, unsigned nCount) = 0;

        /**
         * Close file
         *
         * Returns: != 0	Success
         *      0		Failure
         */
        virtual unsigned Close(void) = 0;
};

class CGlueIoFatFs : public CGlueIO
{
public:
	CGlueIoFatFs (CFATFileSystem& fatFileSystem, unsigned fileHandle) :
		mFat (fatFileSystem), mFileHandle (fileHandle)
		{}

	unsigned Read (void *pBuffer, unsigned nCount)
	{
		return mFat.FileRead (mFileHandle, pBuffer, nCount);
	}

	unsigned Write (const void *pBuffer, unsigned nCount)
	{
		return mFat.FileWrite (mFileHandle, pBuffer, nCount);
	}

	unsigned Close(void)
	{
		return mFat.FileClose (mFileHandle);
	}

private:
	CFATFileSystem &mFat;
	unsigned mFileHandle;
};

class CGlueConsole : public CGlueIO
{
public:
	enum TConsoleMode
	{
		ConsoleModeRead,
		ConsoleModeWrite
	};

	CGlueConsole (CConsole& rConsole, TConsoleMode mode) :
		mConsole (rConsole), mMode (mode)
		{}

	unsigned Read (void *pBuffer, unsigned nCount)
	{
		int nResult = 0;

		if (mMode == ConsoleModeRead)
                {
                        bool const schedulerActive = CScheduler::IsActive ();
                        CScheduler * const scheduler = schedulerActive ? CScheduler::Get () : nullptr;
			while ((nResult = mConsole.Read (pBuffer, nCount)) == 0)
                        {
                                if (schedulerActive)
                                {
                                        scheduler->Yield ();
                                }
			}
		}

		return nResult < 0 ? GeneralFailure : static_cast<unsigned>(nResult);
	}

	unsigned Write (const void *pBuffer, unsigned nCount)
	{
	        int const nResult = mMode == ConsoleModeWrite ? mConsole.Write (pBuffer, nCount) : -1;

	        return nResult < 0 ? GeneralFailure : static_cast<unsigned>(nResult);
	}

	unsigned Close(void)
	{
		// TODO: Cannot close console file handle currently
		return 0;
	}

private:
	CConsole &mConsole;
	TConsoleMode const mMode;
};

#endif
