
#include "utils.h"
#include <zipfile.h>
#include <APGWGNAM.H>
#include <bautils.h>

/*
HBufC* Utils::GetFullNameFromUid3L(const TDesC& aImageName, TUid uid)
	{
	const TUidType uidType(KNullUid, KNullUid, uid);
	
	RProcess proc;
	TInt err = proc.Create(aImageName, KNullDesC(), uidType);
	HBufC* res = NULL;
	if (err != KErrNone)
		res = HBufC::NewL(0);
	if (err == KErrNone)
		{
		proc.FileName().AllocL();		<<--- NON RESTITUISCE IL NOME DELL'ESEGUIBILE
		proc.Terminate(KErrNone); 
		}
	proc.Close();
	return res;
	}*/


TTime Utils::LocaleTime(TTime aTime)
	{
	TTime newTime(aTime);
	TLocale locale;
	TTimeIntervalSeconds universalTimeOffset( locale.UniversalTimeOffset() );
	newTime += universalTimeOffset;

	if (locale.QueryHomeHasDaylightSavingOn())
		{
		//__FLOG(_L("DayLightSaving"));
		TTimeIntervalHours daylightSaving(1);
		newTime -= daylightSaving;
		}
	return newTime;
	}



TUid Utils::GetForegroundAppUidL(RWsSession& wsSession)
	{
	TInt wgId = wsSession.GetFocusWindowGroup();
	TUid result = TUid::Uid(KNullUidValue);
	CApaWindowGroupName* wgName=CApaWindowGroupName::NewLC(wsSession, wgId);
	result = wgName->AppUid();
	CleanupStack::PopAndDestroy(wgName);
	return result;
	}


TInt Utils::ExistsDll(RFs& fs, const TDesC& aDllName)
	{
	RLibrary dll;
	TInt errCode = dll.Load( aDllName );
	dll.Close();
	return errCode;
	}


TBool Utils::ExistsExe(RFs& fs, const TDesC& aExeName, TUid uid3)
	{
	const TUidType domainUid(KNullUid, KNullUid, uid3); 
	RProcess domainProc;
	TInt err1 = domainProc.Create(aExeName, KNullDesC, domainUid);
	TBool res = (err1 == KErrNone);
	if (res)
		{
		domainProc.Terminate(KErrNone);
		}
	domainProc.Close();/**/
	return res;
	}

/*TUint32 Utils::GetUid3FromExeL(RFs& fs, const TDesC& aFileName)
	{
	RLibrary::TInfo info;
	TPckg<RLibrary::TInfo> pkg(info);
	User::LeaveIfError(RLibrary::GetInfo(aFileName, pkg));	// Feature not supported
	return info.iUids.MostDerived().iUid;
	}*/

TUint32 Utils::GetUid3FromSisL(RFs& fs, const TDesC& filename)
	{
	TUint32 uid3 = 0;
	RFileReadStream file;
	file.Open(fs, filename, EFileShareReadersOrWriters|EFileRead);
	CleanupClosePushL(file);
	TInt size = file.Source()->SizeL();
	if (size > 12)
		{
		file.ReadUint32L();
		file.ReadUint32L();
		uid3 = file.ReadUint32L();
		}
	CleanupStack::PopAndDestroy(&file);
	return uid3;
	}

/*
_LIT(KDirName,"C:\\DATA\\");
_LIT(KFileName,"C:\\DATA\\kkk.aaa.gz");
_LIT(KSrcFileName,"C:\\DATA\\kkk.aaa");

RFs fs;
User::LeaveIfError(fs.Connect());
fs.SetSessionPath(KDirName);

RFile file;
TInt err = file.Open(fs,KSrcFileName,EFileRead);

CEZFileToGZip* zip = CEZFileToGZip::NewLC(fs,KFileName,file);
while(zip->DeflateL()){;}
CleanupStack::PopAndDestroy();

file.Close();
fs.Close(); /**/



/* 
    //Open the source file in read mode
	_LIT(KInputFileLocation, "c:\\private\\E80000B7\\zip\\input\\input.doc");
	TFileName inputFile(KInputFileLocation);

    RFile input;
    err = input.Open(iFS, inputFile, EFileStream | EFileRead | EFileShareExclusive);
    if(err != KErrNone)
        {
        INFO_PRINTF1(KOpenFileError);
        User::Leave(err);
        }   
    
    //create a target file
	_LIT(KOutputFileLocation, "c:\\private\\E80000B7\\zip\\input\\output.gz");
	TFileName outputFile(KOutputFileLocation);
    
    CEZFileToGZip *compressor = CEZFileToGZip::NewLC(iFs, outputFile, input);
    
    while(compressor->DeflateL())
        {       
        }   
	input.Close();
/**/


/*
//Open input file
_LIT(KInputFileLocation, "c:\\private\\E80000B7\\zip\\input\\input.doc");
TFileName inputFile(KInputFileLocation);
RFile input;
err = input.Open(iFs, inputFile, EFileStream | EFileRead | EFileShareExclusive);
if(err != KErrNone)
    {
    INFO_PRINTF1(KOpenFileError);
    User::Leave(err);
    }   

//open output file
_LIT(KOutputFileLocation, "c:\\private\\E80000B7\\zip\\input\\output.zip");
TFileName outputFile(KOutputFileLocation);
RFile output;
err = output.Replace(iFs, outputFile, EFileStream | EFileWrite | EFileShareExclusive);
if(err != KErrNone)
    {
    INFO_PRINTF1(KCreateFileError);
    User::Leave(err);
    }   

CEZFileBufferManager *bufferManager = CEZFileBufferManager::NewLC(input, output);   
CEZCompressor *compressor = CEZCompressor::NewLC(*bufferManager, aLevel, aWindowBits, aMemLevel, aStrategy);

while(compressor->DeflateL())
    {       
    }   
output.Close(); 
input.Close();
*/

// NON e' possibile zippare piu' file in un unico archivio, bisognerebbe implementare quanto e' stato fatto in python...
// Meglio: e' possibile usare il compressor di Y-Browser (ho gia' preso i sorgenti)

TInt Utils::ExtractZipL(RFs& fs, const TDesC& aCompressedFile, const TDesC& aDestFolder)
	{
	TRAP_IGNORE( fs.MkDirAll(aDestFolder) );
	 
	if (!BaflUtils::FileExists(fs, aCompressedFile))
		return KErrNotFound;
	// Creating a new instance of CZipFile.
	// The first parameter of the two-phase constructor is a session of File Server.
	// The second parameter is the file name of the ZIP file.
	TInt res = 0;
	CZipFile* zipFile = CZipFile::NewL(fs, aCompressedFile);
	CleanupStack::PushL(zipFile);
	 
	// Getting the list of all files inside the ZIP.
	// The ownership of CZipFileMemberIterator will be passed to the caller,
	// thus we have to delete it after we are done reading the ZIP.
	CZipFileMemberIterator* members = zipFile->GetMembersL();
	CleanupStack::PushL(members);
	 
	// Iterating through all the files.
	// We have to call CZipFileMemberIterator::NextL() over and over
	// until it returns 0.
	CZipFileMember* member;
	while ((member = members->NextL()) != 0)
    		{
		res++;
		CleanupStack::PushL(member);
		
		RZipFileMemberReaderStream* stream;
		zipFile->GetInputStreamL(member, stream);
		CleanupStack::PushL(stream);
		
		HBufC8* buffer = HBufC8::NewLC(member->UncompressedSize());
		TPtr8 bufferPtr(buffer->Des());
		User::LeaveIfError(stream->Read(bufferPtr, member->UncompressedSize()));
		TFileName fname = aDestFolder;
		fname.Append( *member->Name() );
	
		RFile file;
		file.Replace(fs, fname, EFileShareReadersOrWriters); 
		file.Write( buffer->Des() );
		file.Close(); 
				
		CleanupStack::PopAndDestroy(buffer);
		CleanupStack::PopAndDestroy(stream);
		
		CleanupStack::PopAndDestroy(member);
    		}
 
	// Finally, don't forget to release all the resources that we have allocated.
	CleanupStack::PopAndDestroy(); // members
	CleanupStack::PopAndDestroy(); // zipFile

	return res;
	}



 TUint Utils::GetClientUID(const RMessagePtr2& aMsg)
 {
 	RThread ClientThread;
 	aMsg.Client(ClientThread);// get client thread
 	if(!ClientThread.Handle())// has handle???
 	{
 		return 0;
 	}
 	RProcess ClientProcess;
 	TInt ris = ClientThread.Process(ClientProcess);// then get the process
 	if (ris != KErrNone)
 	{
 		return 0;
 	}
 	TUint id=0;

 #ifdef __UIQ___
 	TUid uid = ClientProcess.Type()[3];
 	id = uid.iUid;
 #else
 	// __SERIES60_3X__
 	TSecureId prosID = ClientProcess.SecureId();// and check secure UID
 	id = prosID.iId;
 #endif
 	return id;
 }

 
/*
 z:\system\install\Series60v3.0.sis     | S60 3rd Edition (FP0)
 z:\system\install\Series60v3.1.sis     | S60 3rd Edition FP1
 z:\system\install\Series60v3.2.sis     | S60 3rd Edition FP2
 z:\system\install\Series60v5.0.sis     | S60 5th Edition - aka Symbian^1
 z:\system\install\Series60v5.1.sis     | Symbian^2	
 z:\system\install\Series60v5_2.sis		| Symbian^3	aka Anna
 z:\system\install\Series60v5_3.sis		| Belle
 z:\system\install\Series60v5_4.sis		| Belle FP1
 
* it seems they are accumulated. So, for example, in N78, which is a FP2 device, you will find all these three files.
*/

 /*
 z:\system\install\Series60v2.8.sis | S60 2nd Edition, Feature Pack 3
 z:\system\install\Series60v2.6.sis | S60 2nd Edition, Feature Pack 2
 z:\system\install\Series60v2.1.sis | S60 2nd Edition, Feature Pack 1
 z:\system\install\Series60v2.0.sis | S60 2nd Edition
 z:\system\install\Series60v1.2.sis | S60 1st Edition (1.2)
 z:\system\install\Series60v1.1.sis | (n/a)
 z:\system\install\Series60v1.0.sis | (n/a)
 z:\system\install\Series60v0.9.sis | S60 v0.9
 */
 
_LIT(KS60ProductIDFile, "Series60v*.sis");
_LIT(KROMInstallDir, "z:\\system\\install\\");
 
 void Utils::GetS60PlatformL( RFs& aFs, TUint& aMajor, TUint& aMinor )
    {
    TFindFile ff( aFs );
    CDir* result;
    TInt err = ff.FindWildByDir( KS60ProductIDFile, KROMInstallDir, result );
    if (err != KErrNone)
    	return;
    CleanupStack::PushL( result );
    if (result->Count() > 0)
    	{
    	User::LeaveIfError( result->Sort( ESortByName|EDescending ) );
		aMajor = (*result)[0].iName[9] - '0';
		aMinor = (*result)[0].iName[11] - '0';
		if (aMinor == 8) aMinor = 3;
		if (aMinor == 6) aMinor = 2;
    	}
    CleanupStack::PopAndDestroy(); // result
    }

