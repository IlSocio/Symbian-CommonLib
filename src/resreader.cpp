/*
============================================================================
 Name        : ResReader.cpp
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : CResReader implementation
============================================================================
*/

#include "ResReader.h"
#include "BAUTILS.H"


CResReader::CResReader(RFs& fs) : CBase(), iFs(fs)
{
	// No implementation required
}


CResReader::~CResReader()
{
	iResFile.Close();
	__LOG_TXT("End Destructor");
	__LOG_CLOSE;
}

CResReader* CResReader::NewLC(RFs& fs, const TDesC& fileName)
{
	CResReader* self = new (ELeave)CResReader(fs);
	CleanupStack::PushL(self);
	self->ConstructL(fileName);
	return self;
}

 CResReader* CResReader::NewL(RFs& fs, const TDesC& fileName)
{
	CResReader* self=CResReader::NewLC(fs, fileName);
	CleanupStack::Pop(self); // self;
	return self;
}

void CResReader::ConstructL(const TDesC& fileName)
{
	__LOG_OPEN("CommonLib", "ResReader.txt");
	TFileName newFile = fileName;
	BaflUtils::NearestLanguageFile( iFs, newFile );
	__LOG_DES(newFile);
	iResFile.OpenL(iFs, fileName);
	__LOG_TXT("Opened");
	iResFile.ConfirmSignatureL();
	__LOG_TXT("Confirmed");
}


HBufC* CResReader::AllocReadLC(TInt resId)
{
	/*HBufC8* buff = iResFile.AllocReadLC(resId);
	TResourceReader reader;
	reader.SetBuffer(buff);
	TPtrC16 textdata = reader.ReadTPtrC16();
	txt.Copy(textdata);
	CleanupStack::PopAndDestroy(buff);*/

// TODO: Contolla che succede quando si ha a che fare con una risorsa a 8bit...
	HBufC8* buff = iResFile.AllocReadLC(resId);
	TResourceReader reader;
	reader.SetBuffer(buff);
	TPtrC16 textdata = reader.ReadTPtrC16();
	HBufC* result = textdata.AllocL();
	CleanupStack::PopAndDestroy(buff);
	CleanupStack::PushL(result);
	return result;
}


void CResReader::ReadL(TDes& buffer, TInt resId)
	{
	HBufC* tmp = AllocReadLC(resId);
	buffer.Copy( tmp->Des() );
	CleanupStack::PopAndDestroy( tmp );
	}


HBufC* CResReader::AllocReadL(TInt resId)
	{
	HBufC* res = AllocReadLC(resId);
	CleanupStack::Pop(res);
	return res;
	}
