#include "fileutils.h"
#include <CHARCONV.H>
#include <bautils.h>
#include <f32file.h>
#include <APGCLI.H>
#include "StringUtils.H"


void FileUtils::GetAppForMimeTypeL(const TDesC8& aMimeType, TUid &aAppUid)
	{
	RApaLsSession session; 
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);
	TDataType type(aMimeType);
	session.AppForDataType(type, aAppUid);
	CleanupStack::PopAndDestroy(&session);	
	}

	
void FileUtils::GetMimeTypeL(RFs& aFs, const TDesC& aFileName, TDes8& mimeType)
	{
	mimeType.Zero();
	RFile file;
	User::LeaveIfError(file.Open(aFs, aFileName, EFileShareReadersOrWriters | EFileStream | EFileRead));
	CleanupClosePushL(file);
	GetMimeTypeL(file, mimeType);
	CleanupStack::PopAndDestroy(&file);
	}

void FileUtils::GetMimeTypeL(RFile& aFile, TDes8& mimeType)
	{	
	mimeType.Zero();
	RApaLsSession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);
	TDataRecognitionResult fileDataType;
	session.RecognizeData(aFile, *&fileDataType);
	mimeType.Copy(fileDataType.iDataType.Des8());
	CleanupStack::PopAndDestroy(&session);
	}

void FileUtils::GetMimeTypeL(const TDesC8& aData, TDes8& mimeType)
	{
	mimeType.Zero();
	RApaLsSession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);
	TDataRecognitionResult fileDataType;
	session.RecognizeData(KNullDesC(), aData, *&fileDataType);
	mimeType.Copy(fileDataType.iDataType.Des8());
	CleanupStack::PopAndDestroy(&session);
	}

void FileUtils::DumpToFileL(RFs& fs, const TDesC& fName, const TDesC8& data,
		TBool append)
	{
	// Su Belle l'append implementato cosi' non funziona... e non so perche'.
	RFile file;
	CleanupClosePushL( file );
	TInt err = KErrNone;
	if (append)
		{
		err = file.Open(fs, fName, EFileShareReadersOrWriters | EFileStream | EFileWrite);
		}

	if ((!append) || (err == KErrNotFound))
		{
		fs.MkDirAll( fName );
		file.Replace(fs, fName, EFileShareReadersOrWriters | EFileStream | EFileWrite);
		}

	TInt pos=0;
	err = file.Seek(ESeekEnd, pos);
	if (err == KErrNone)
		{
		err = file.Write(data);
		err = file.Flush();
		}
	CleanupStack::PopAndDestroy( &file );
	 
	// TInt size = GetFileSize(fs, fName);
	//	file.Close();/**/	
	}

void FileUtils::DumpToFileL(RFs& fs, const TDesC& fName, const TDesC& data,
		TBool append)
	{
	RBuf8 buf;
	buf.CleanupClosePushL();
	buf.Create(data.Length() * 2);
	buf.Copy(data);
	FileUtils::DumpToFileL(fs, fName, buf, append);
	CleanupStack::PopAndDestroy(&buf);
	}

void FileUtils::CompleteWithCurrentDrive(TDes& fileName)
	{
#ifdef __WINSCW__
	fileName.Insert(0, _L("C:"));
	return;
#endif
	TParsePtrC parse(RProcess().FileName());
	//    TParsePtrC parse((CEikonEnv::Static()->EikAppUi()->Application())->AppFullName());
	fileName.Insert(0, parse.Drive());
	}


TInt FileUtils::CreateTempFileL(RFs& fs, RFile& aFile)
	{
	TFileName path;
	CompleteWithPrivatePathL(fs, path);
	path.Replace(0, 1, _L("D"));
	fs.MkDirAll(path);
	TFileName newName;
	return aFile.Temp(fs, path, newName, EFileShareReadersOrWriters | EFileStream | EFileWrite);
	}


void FileUtils::CompleteWithPrivatePathL(RFs& fs, TDes& fileName, TBool useSID)
	{
	TFileName fullFileName;
	fs.PrivatePath(fullFileName);

	if (useSID)
		{
		// TUint32 uid3 = RProcess().Type().MostDerived().iUid;		// Gets UID3

		TParsePtr parse(fullFileName);
		User::LeaveIfError(parse.PopDir()); // Remove UID3

		TUint32 sid = RProcess().SecureId().iId; // Add SID
		TBuf<10> sidDir;
		sidDir.AppendNum(sid, EHex);

		parse.AddDir(sidDir);
		}

	fullFileName.Append(fileName);
	FileUtils::CompleteWithCurrentDrive(fullFileName);
	fileName = fullFileName;
	}

HBufC* FileUtils::ReadTextFileL(RFs& aFs, const TDesC& aFileName)
	{
	RFile file;
	User::LeaveIfError(file.Open(aFs, aFileName, EFileShareReadersOrWriters	| EFileStreamText | EFileRead));
	CleanupClosePushL(file);

	TInt size;
	User::LeaveIfError(file.Size(size));

	RBuf8 tmp;
	tmp.CreateL(size);
	CleanupClosePushL(tmp);
	User::LeaveIfError(file.Read(tmp));

	HBufC *text = HBufC::NewL(size);

	TUint conv = KCharacterSetIdentifierUtf8;
	TInt state = CCnvCharacterSetConverter::KStateDefault;
	_LIT8(KBOM_UTF8, "\xEF\xBB\xBF"); //notepad uses bom (byte order mark) when saving as UTF-8
	_LIT8(KBOM_UCS16_LE, "\xFF\xFE");
	_LIT8(KBOM_UCS16_BE, "\xFE\xFF");
	TPtrC8 remainderOfForeignText(tmp);
	if (remainderOfForeignText.Find(KBOM_UTF8) != KErrNotFound)
		{
		remainderOfForeignText.Set(remainderOfForeignText.Right(
				remainderOfForeignText.Length() - KBOM_UTF8().Length()));
		conv = KCharacterSetIdentifierUtf8;
		}
	if (remainderOfForeignText.Find(KBOM_UCS16_LE) != KErrNotFound)
		{
		remainderOfForeignText.Set(remainderOfForeignText.Right(
				remainderOfForeignText.Length() - KBOM_UCS16_LE().Length()));
		conv = KCharacterSetIdentifierUnicodeLittle;
		}
	if (remainderOfForeignText.Find(KBOM_UCS16_BE) != KErrNotFound)
		{
		remainderOfForeignText.Set(remainderOfForeignText.Right(
				remainderOfForeignText.Length() - KBOM_UCS16_BE().Length()));
		conv = KCharacterSetIdentifierUnicodeBig;
		}

	CCnvCharacterSetConverter * converter = CCnvCharacterSetConverter::NewLC();
	converter->PrepareToConvertToOrFromL(conv, aFs);

	for (;;)
		{
		TPtr textPtr(text->Des());
		TInt retValue = converter->ConvertToUnicode(textPtr,
				remainderOfForeignText, state);
		if (retValue == CCnvCharacterSetConverter::EErrorIllFormedInput)
			User::Leave(KErrCorrupt);
		else if (retValue < 0)
			User::Leave(KErrGeneral);

		if (retValue == 0)
			break;

		remainderOfForeignText.Set(remainderOfForeignText.Right(retValue));
		}

	CleanupStack::PopAndDestroy(converter);
	CleanupStack::PopAndDestroy(2);
	return text;
	}

TInt FileUtils::GetFileSize(RFs& fs, const TDesC& filename)
	{
	TInt size = 0;
	RFile file;
	TInt err = file.Open(fs, filename, EFileShareReadersOrWriters | EFileRead);
	if (err != KErrNone)
		{
		file.Close();
		return 0;
		}
	file.Size(size);
	file.Close();
	return size;
	}

TInt FileUtils::GetDrive(RFs& iFs, TInt driveNumber, TDes& drive)
	{
	TDriveInfo driveInfo;
	drive.Zero();
	iFs.Drive(driveInfo, driveNumber);
	if (driveInfo.iDriveAtt == (TUint) KDriveAbsent)
		return KErrNotFound;

	TChar driveChar;
	if (iFs.DriveToChar(driveNumber, driveChar) != KErrNone)
		return KErrGeneral;
	driveChar.LowerCase();
	drive.Append(driveChar);
	drive.Append(_L(":"));
	return KErrNone;
	}

HBufC8* FileUtils::ReadFileContentsL(RFs& fs, const TDesC& filename)
	{
	RFile file;
	TInt err = file.Open(fs, filename, EFileShareReadersOrWriters | EFileRead);
	User::LeaveIfError( err );
	CleanupClosePushL(file);

	TInt fileSize;
	User::LeaveIfError(file.Size(fileSize));

	HBufC8* buf = HBufC8::NewL(fileSize);
	TPtr8 bufPtr = buf->Des();
	User::LeaveIfError(file.Read(bufPtr));

	CleanupStack::PopAndDestroy(&file);
	return buf;
	}


void FileUtils::ListFilesInDirectoryL(RFs& fs, const TDesC& path,
		RPointerArray<HBufC>& array, TUint attrFlags)
	{
	TParsePtrC parser(path);
	if (!BaflUtils::PathExists(fs, parser.DriveAndPath()))
		return;
	//	if ( !BaflUtils::PathExists(fs, path) )
	//		return;
	// nel caso in cui Path non esista
	// Initializing it to NULL
	// valid Directory in the file system tree
	CDirScan* dirScan = CDirScan::NewLC(fs);
	dirScan->SetScanDataL(path, attrFlags, ESortByName, CDirScan::EScanDownTree);

	CDir* dir = NULL;
	dirScan->NextL(dir);
	while (dir)
		{
		CleanupStack::PushL(dir);
		for (TInt i = 0; i < dir->Count(); i++)
			{
			TEntry entry = (*dir)[i];
			TPtrC fullPath = dirScan->FullPath();
			HBufC* fname = HBufC::NewLC(fullPath.Length()
					+ entry.iName.Length() + 2);
			fname->Des().Append(fullPath);
			fname->Des().Append(entry.iName);
			if (entry.IsDir())
				{
				fname->Des().Append(_L("\\"));
				}
			array.AppendL(fname);
			CleanupStack::Pop(fname);
			}
		CleanupStack::PopAndDestroy(dir);
		dirScan->NextL(dir);
		}
	CleanupStack::PopAndDestroy(dirScan);
	}

