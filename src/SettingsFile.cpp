/*
============================================================================
Name        : CSettingsFile from AppSettings.h
Author      : Roberto LA RAGIONE
Version     : 1.0
Copyright   : BITWEEN S.R.L.
Description : CSettingsFile implementation
============================================================================
*/

#include "SettingsFile.h"
#include "FileUtils.h"

#include <s32file.h>
#include <s32mem.h>
#include <bautils.h>

// -----------------------------------------------------------------------------
// CSettingsFile::CSettingsFile(RFs& aFSession)
// Standard EPOC constructor
// -----------------------------------------------------------------------------
//
CSettingsFile::CSettingsFile(RFs& aFSession) : 
	CBase(), iFs( aFSession )
	{
	}

// -----------------------------------------------------------------------------
// CSettingsFile::~CSettingsFile()
// Destroys the object
// -----------------------------------------------------------------------------
//
CSettingsFile::~CSettingsFile()
	{
	iFileName.Close();
	iCurrVersion.Close();
	}


HBufC8* CSettingsFile::LoadBytesFromFileL()
	{
	return FileUtils::ReadFileContentsL(iFs, iFileName);
	}

void CSettingsFile::SaveBytesToFileL(const TDesC8& buffer)
	{
	return FileUtils::DumpToFileL(iFs, iFileName, buffer, EFalse);
	}


TInt CSettingsFile::DoImportL(TUid dictionaryUid, const TDesC& aSrcFile)
	{
	RestoreToDefaults();
	// controlla se il e' esistente
	TEntry entry;
	TInt ris = iFs.Entry(aSrcFile, entry);
	if (ris != KErrNone)
		{
		return ris;
		}

	CDictionaryStore* dictstore = CDictionaryFileStore::OpenLC (iFs, aSrcFile, dictionaryUid );
	
	RDictionaryReadStream in;
	in.OpenLC(*dictstore, dictionaryUid);
	
	TBuf<20> fileVersion;
	ReadVersionHeaderL(in, fileVersion);
	if (fileVersion == iCurrVersion)
		{		
		in >> *(this);
		} else
		{
		UpgradeFromOldVersion(in, fileVersion);
		}
	
	in.Close();
	CleanupStack::PopAndDestroy(&in);
	CleanupStack::PopAndDestroy(dictstore);
	return KErrNone;
	}


TInt CSettingsFile::ImportDictionary(TUid dictionaryUid, const TDesC& aSrcFile, const TDesC& aDestFile, const TDesC& aVersion)
	{
	iCurrVersion.Close();
	iCurrVersion.Create( aVersion );
	iFileName.Close();
	iFileName.Create( aDestFile );
	
	TInt res = KErrNone;
	TRAPD(err, res = DoImportL(dictionaryUid, aSrcFile));
	if (err != KErrNone)
		{
		// Config Corrupt...
		res = KErrCorrupt;
		}
	if (res != KErrNone)
		RestoreToDefaults();
	return res;
	}


TInt CSettingsFile::Load(const TDesC& aFileName, const TDesC& aVersion)
	{
	iCurrVersion.Close();
	iCurrVersion.Create( aVersion );
	iFileName.Close();
	iFileName.Create( aFileName );
	
	TInt res = KErrNone;
	TRAPD(err, res = DoLoadL());
	if (err != KErrNone)
		{
		// Config Corrupt...
		res = KErrCorrupt;
		}
	if (res != KErrNone)
		RestoreToDefaults();
	return res;
	}


// -----------------------------------------------------------------------------
// void CSettingsFile::LoadL()
// Loads application settings from the associated file
// -----------------------------------------------------------------------------
//
TInt CSettingsFile::DoLoadL()
	{
	RestoreToDefaults();
	
	// controlla se il e' esistente
	TEntry entry;
	TInt ris = iFs.Entry(iFileName, entry);
	if (ris != KErrNone)
		{
		return ris;
		}

	RBuf8 configBuffer( LoadBytesFromFileL() );
	configBuffer.CleanupClosePushL();
	TBool isValid = (configBuffer.Length() > 0);
	if (isValid)
		{
		RMemReadStream memStream(configBuffer.Ptr(), configBuffer.Length());
		CleanupClosePushL(memStream);
		
		TBuf<20> fileVersion;
		ReadVersionHeaderL(memStream, fileVersion);
		if (fileVersion == iCurrVersion)
			{		
			memStream >> *(this);
			} else
			{
			UpgradeFromOldVersion(memStream, fileVersion);
			}
		CleanupStack::PopAndDestroy(&memStream);
		}
	CleanupStack::PopAndDestroy(&configBuffer);

	if (!isValid)
		return KErrNotFound;
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CSettingsFile::SaveL()
// Saves application settings to the associated file
// -----------------------------------------------------------------------------
//
void CSettingsFile::SaveL()
	{
	CBufBase* buffer = CBufFlat::NewL(50);
	CleanupStack::PushL(buffer);
	RBufWriteStream strm;
	CleanupClosePushL(strm);
	strm.Open(*buffer);
	WriteVersionHeaderL(strm);
	strm << (*this);
	strm.CommitL();
	CleanupStack::PopAndDestroy(&strm);

	SaveBytesToFileL( buffer->Ptr(0) );

	CleanupStack::PopAndDestroy(buffer);
	}


void CSettingsFile::ReadVersionHeaderL(RReadStream& aStream, TDes& version)
	{
	aStream >> version;
	}

// -----------------------------------------------------------------------------
// void CSettingsFile::WriteVersionHeaderL(RWriteStream& aStream)
// Writes version header to the stream
// -----------------------------------------------------------------------------
//
void CSettingsFile::WriteVersionHeaderL(RWriteStream& aStream)
	{
	aStream << iCurrVersion;
	}

/*
void CSettingsFile::ExternalizeL(RWriteStream& aStream) const
	{	
	}


void CSettingsFile::InternalizeL(RReadStream& aStream)
	{	
	}
*/
// End file 
