#ifndef _circle_glue_h
#define _circle_glue_h

class CFATFileSystem;
class CScreenDevice;

void CGlueStdioInit(CFATFileSystem& rFATFileSystem);

class CGlueIO
{
public:

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
     */
    virtual unsigned Close(void) = 0;
};

class CGlueIoFatFs : public CGlueIO
{
public:
	CGlueIoFatFs(CFATFileSystem& fatFileSystem, unsigned fileHandle) :
		mFat(fatFileSystem), mFileHandle(fileHandle)
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

#endif
