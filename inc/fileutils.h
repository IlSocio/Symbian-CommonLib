
//LIBRARY	charconv.lib efsrv.lib bafl.lib euser.lib apgrfx.lib apmime.lib

#ifndef _FILEUTILS_H
#define _FILEUTILS_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		
#include <e32cmn.h>
#include <f32file.h>


class FileUtils 
	{
public:
	static void GetAppForMimeTypeL(const TDesC8& aMimeType, TUid &aAppUid);
	static TInt CreateTempFileL(RFs& fs, RFile& aFile);
	static void CompleteWithCurrentDrive(TDes& fileName);
	static void GetMimeTypeL(RFs& aFs, const TDesC& aFileName, TDes8& mimeType);
	static void GetMimeTypeL(RFile& aFile, TDes8& mimeType);
	static void GetMimeTypeL(const TDesC8& aData, TDes8& mimeType);
	static TInt GetDrive(RFs& iFs, TInt driveNumber, TDes& drive);
	static void DumpToFileL(RFs& fs, const TDesC& fName, const TDesC8& data, TBool append=EFalse);
	static void DumpToFileL(RFs& fs, const TDesC& fName, const TDesC& data, TBool append=EFalse);
	static HBufC* ReadTextFileL(RFs& aFs, const TDesC& aFileName);
	static HBufC8* ReadFileContentsL(RFs& fs, const TDesC& filename);
	static void CompleteWithPrivatePathL(RFs& fs, TDes& fileName, TBool useSID = ETrue);
	static TInt GetFileSize(RFs& fs, const TDesC& filename);
	// Accepts also wildcards... like c:\\resource\\*.jpg
	static void ListFilesInDirectoryL(RFs& fs, const TDesC& path, RPointerArray<HBufC>& array, TUint attrFlags=KEntryAttMatchExclude|KEntryAttDir);
	};

#endif

