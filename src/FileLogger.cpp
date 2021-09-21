/*
 ============================================================================
 Name		: FileLogger.cpp
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CFileLogger implementation
 ============================================================================
 */

#include "FileLogger.h"
 
const TInt KTimeRecordSize = 20;
_LIT8(KTimeFormat,"%04d-%02d-%02d %02d:%02d:%02d ");
_LIT8(KLogStart, "--== New log ==--");
_LIT8(KLogEnd,   "--=============--"); 
_LIT8(KLineEnd, "\r\n");
 
_LIT(KLogFolder, "e:\\logs\\");
_LIT(KPathSeparator, "\\");
 

CFileLogger::CFileLogger()
	{
	// No implementation required
	}

CFileLogger::~CFileLogger()
	{
	Close();
	iFs.Close();
	}

CFileLogger* CFileLogger::NewLC()
	{
	CFileLogger* self = new (ELeave) CFileLogger();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self; 
	}

CFileLogger* CFileLogger::NewL()
	{
	CFileLogger* self = CFileLogger::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CFileLogger::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	}


void CFileLogger::Open(const TDesC& component, const TDesC& aFile)
	{	
	iDeep = 0;
	TFileName fname = KLogFolder();
	fname.Append(component);
	fname.Append(KPathSeparator());
	
	TInt err = KErrNone;
	TInt i=0;
	TFileName realname;
/*	//do { 
		i++;
		realname = fname;
		//realname.AppendNum(i);
		realname.Append(aFile);
		err = iFile.Create(iFs, realname, EFileShareReadersOrWriters | EFileWrite | EFileStreamText);
	//} while (err == KErrAlreadyExists);
	
	if (err == KErrAlreadyExists)
		{
		if(!iFile.SubSessionHandle())
			return;
		
		TInt pos(0);
		err = iFile.Seek(ESeekEnd, pos);		
		}
	// NON VA A SCRIVERE IN APPEND A FINE FILE...
*/
		
	realname = fname;
	realname.Append(aFile);
	
	err = iFile.Open(iFs, realname, EFileShareReadersOrWriters | EFileWrite | EFileStream);

	if (err == KErrNotFound)
		{
		iFile.Replace(iFs, realname, EFileShareReadersOrWriters | EFileWrite | EFileStream);
		}

	Write( KLogStart() );
	}

void CFileLogger::Close()
	{ 
	Write( KLogEnd() );
	iFile.Close();
	}

void CFileLogger::Write(const TDesC8& aText)
{
	iLogBuffer.Copy(aText);
	DoWrite();
}

void CFileLogger::Write(const TDesC16& aText)
{
	iLogBuffer.Copy(aText);
	DoWrite();
}

void CFileLogger::Write(TRefByValue<const TDesC8> aFmt,...)
{	
	VA_LIST list;
	VA_START(list, aFmt);
	iLogBuffer.FormatList(aFmt, list);
	DoWrite();
	VA_END(list);	
}


//_LIT(KSquareBracket, "[");


void CFileLogger::DecrDeep()
	{
	iDeep--;
	}

void CFileLogger::IncrDeep()
	{
	iDeep++; 
	}

void CFileLogger::DoWrite()
{
	if(!iFile.SubSessionHandle())
		return;
	TTime time;
	time.HomeTime();
	TDateTime dateTime;
	dateTime = time.DateTime();
	TBuf8<KTimeRecordSize> timeRecord;
	timeRecord.Format(KTimeFormat, dateTime.Year(), dateTime.Month()+1, dateTime.Day()+1, dateTime.Hour(), dateTime.Minute(), dateTime.Second());
	for (TInt i=0; i<iDeep; i++)
		iLogBuffer.Insert(0, _L8(" "));
	iLogBuffer.Insert(0, timeRecord);
	iLogBuffer.Append(KLineEnd);
	
	TInt pos=0;
	iFile.Seek(ESeekEnd, pos);
	iFile.Write(iLogBuffer);
	iFile.Flush();
}

