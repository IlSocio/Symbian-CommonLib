/*
============================================================================
 Name        : UpdateLock.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : UpdateLock implementation
============================================================================
*/

#include "UpdateLock.h"
#include <EIKENV.H>
#include <BaUtils.h>
#include <S32STRM.H>
#include <f32file.h> 
#include <S32FILE.H>
#include <FuzzyByte\FileUtils.h>

// TODO: Decifra questo...
_LIT(KFileUpd, "upd.dat");


void UpdateLock::WriteLastUpdateDayL(TInt day)
{
	RFs &fs= CEikonEnv::Static()->FsSession();
	TFileName updFile = KFileUpd();
	FileUtils::CompleteWithPrivatePathL(fs, updFile);
	RFileWriteStream strm;
#ifdef __WINSCW__
	fs.MkDirAll( updFile );
#endif
	User::LeaveIfError( strm.Replace(fs, updFile, EFileWrite|EFileShareAny) );
	CleanupClosePushL(strm);
	strm.WriteInt32L(day);
	CleanupStack::PopAndDestroy();
}


TInt32 UpdateLock::ReadLastUpdateDayL()
{
	RFs &fs= CEikonEnv::Static()->FsSession();
	TFileName updFile = KFileUpd();
	FileUtils::CompleteWithPrivatePathL(fs, updFile);
	if (!BaflUtils::FileExists(fs, updFile))
		{
		return 0;
		}
	RFileReadStream strm;
	strm.Open(fs, updFile, EFileShareReadersOrWriters|EFileRead );
	CleanupClosePushL(strm);
	TInt32 ris = 0;
	ris = strm.ReadInt32L();
	CleanupStack::PopAndDestroy();
	return ris;
}


TBool UpdateLock::NeedsUpdateL()
{
	TTime curr;
	curr.HomeTime();
	TInt32 currentDay = curr.DateTime().Day() + curr.DateTime().Month() * 30;

	TInt32 lastUpdateDay = ReadLastUpdateDayL();
	if (lastUpdateDay == 0)
	{
		lastUpdateDay = currentDay;
		WriteLastUpdateDayL(lastUpdateDay);
	}
	TInt diff = (lastUpdateDay - currentDay);
	if (diff < 0) diff *= -1;
	if (diff > 15) 
		{
		WriteLastUpdateDayL(0xFFF);
		return ETrue;
		}
	return EFalse;
}


void UpdateLock::DoneUpdateL()
{
	TTime curr;
	curr.HomeTime();
	TInt32 currentDay = curr.DateTime().Day() + curr.DateTime().Month() * 30;
	WriteLastUpdateDayL(currentDay);
}


