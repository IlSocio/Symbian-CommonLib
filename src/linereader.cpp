/*471188***********************************************************
 This file is part of futils.
 
 futils is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 futils is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with futils.  If not, see <http://www.gnu.org/licenses/>.
 
 Markus Mertama @ http://koti.mbnet.fi/mertama
 ******************************************************************/
#include"linereader.h"

NONSHARABLE_CLASS(CFileLineReaderBase) : public CBase
	{
public:
	inline const TDesC& Buffer() const;
	inline TBool IsBuffer() const;
	~CFileLineReaderBase();
	TBool ReadL(TInt& aCursor);
protected:
	CFileLineReaderBase(RFile& aFile);
	virtual HBufC* CopyL(TInt aFromOld, const TDesC8& aLine) = 0;
private:
	RFile& iFile;
	HBufC* iBuffer;
	};

inline const TDesC& CFileLineReaderBase::Buffer() const
	{
	return *iBuffer;
	}

inline TBool CFileLineReaderBase::IsBuffer() const
	{
	return iBuffer != NULL;
	}

NONSHARABLE_CLASS(CFileLineReaderAscii) : public CFileLineReaderBase
	{
public:
	CFileLineReaderAscii(RFile& aFile);
private:
	HBufC* CopyL(TInt aFromOld, const TDesC8& aLine);
	};

NONSHARABLE_CLASS(CFileLineReaderUtf16LE) : public CFileLineReaderBase
	{
public:
	CFileLineReaderUtf16LE(RFile& aFile);
private:
	HBufC* CopyL(TInt aFromOld, const TDesC8& aLine);
	};

NONSHARABLE_CLASS(CFileLineReaderUtf16BE) : public CFileLineReaderBase
	{
public:
	CFileLineReaderUtf16BE(RFile& aFile);
private:
	HBufC* CopyL(TInt aFromOld, const TDesC8& aLine);
	};

CFileLineReader* CFileLineReader::DuplicateLC(RFile& aFile)
	{
	CFileLineReader* n = new (ELeave) CFileLineReader();
	CleanupStack::PushL(n);
	n->iFile.Duplicate(aFile);
	n->ConstructL();
	return n;
	}

CFileLineReader* CFileLineReader::DuplicateL(RFile& aFile)
	{
	CFileLineReader* n = CFileLineReader::DuplicateLC(aFile);
	CleanupStack::Pop();
	return n;
	}

void CFileLineReader::ConstructL()
	{
	TBuf8<8> sig;

	User::LeaveIfError(iFile.Read(sig));

	if (sig.Length() < sig.MaxLength())
		{
		sig.SetLength(sig.MaxLength());
		sig.Fill(TChar(0));
		}

	if (sig[0] == 0 && sig[1] == 0 && sig[2] == 0xFE && sig[3] == 0xFF) //UTF32 BE
		{
		User::Leave(KErrNotSupported);
		TInt pos = 4;
		iFile.Seek(ESeekStart, pos);
		return;
		}
	if (sig[0] == 0xFF && sig[1] == 0xFE && sig[2] == 0 && sig[3] == 0) //UTF32 LE
		{
		User::Leave(KErrNotSupported);
		TInt pos = 4;
		iFile.Seek(ESeekStart, pos);
		return;
		}
	if (sig[0] == 0xFF && sig[1] == 0xFE) //UTF16 LE
		{
		iReader = new (ELeave) CFileLineReaderUtf16LE(iFile);
		TInt pos = 2;
		iFile.Seek(ESeekStart, pos);
		return;
		}
	if (sig[0] == 0xFE && sig[1] == 0xFF) //UTF16 BE
		{
		iReader = new (ELeave) CFileLineReaderUtf16BE(iFile);
		TInt pos = 2;
		iFile.Seek(ESeekStart, pos);
		return;
		}
	if (sig[0] == 0xEF && sig[1] == 0xBB && sig[2] == 0xBF) //UTF8
		{
		iReader = new (ELeave) CFileLineReaderAscii(iFile); //UTF8 treated as ASCII
		TInt pos = 3;
		iFile.Seek(ESeekStart, pos);
		return;
		}
	// ASCII
	iReader = new (ELeave) CFileLineReaderAscii(iFile);
	TInt pos = 0;
	iFile.Seek(ESeekStart, pos);
	}

CFileLineReader* CFileLineReader::NewLC(RFs& aFs, const TDesC& aFileName)
	{
	CFileLineReader* n = new (ELeave) CFileLineReader();
	CleanupStack::PushL(n);
	User::LeaveIfError(n->iFile.Open(aFs, aFileName, EFileRead));
	n->ConstructL();
	return n;
	}

CFileLineReader* CFileLineReader::NewL(RFs& aFs, const TDesC& aFileName)
	{
	CFileLineReader* n = CFileLineReader::NewLC(aFs, aFileName);
	CleanupStack::Pop();
	return n;
	}

CFileLineReader::~CFileLineReader()
	{
	delete iReader;
	iFile.Close();
	}

TPtrC CFileLineReader::Current() const
	{
	return iReader->Buffer().Mid(iCursor, iSize);
	}

TBool CFileLineReader::NextL()
	{
	for (;;)
		{
		TInt sparecursor = -1;
		if (iReader->IsBuffer())
			{
			const TDesC& buf = iReader->Buffer();
			const TInt len = buf.Length();
			TInt cursor = iCursor + iSize;
			if (cursor < len && buf[cursor] == '\n')
				cursor++;
			if (cursor < len)
				{
				const TPtrC ptr = buf.Mid(cursor, len - cursor);
				const TInt pos = ptr.Locate('\n');
				if (pos > 0)
					{
					iCursor = cursor;
					iSize = pos;
					return ETrue;
					}
				else
					{
					sparecursor = cursor;
					}
				}
			}
		if (!iReader->ReadL(iCursor))
			{
			if (sparecursor >= 0)
				{
				iCursor = sparecursor;
				iSize = iReader->Buffer().Length() - sparecursor; //rest
				return ETrue;
				}
			return EFalse;
			}
		}
	// return EFalse;
	}

CFileLineReaderBase::CFileLineReaderBase(RFile& aFile) :
	iFile(aFile)
	{
	}

CFileLineReaderBase::~CFileLineReaderBase()
	{
	delete iBuffer;
	}

CFileLineReaderAscii::CFileLineReaderAscii(RFile& aFile) :
	CFileLineReaderBase(aFile)
	{
	}

CFileLineReaderUtf16LE::CFileLineReaderUtf16LE(RFile& aFile) :
	CFileLineReaderBase(aFile)
	{
	}

CFileLineReaderUtf16BE::CFileLineReaderUtf16BE(RFile& aFile) :
	CFileLineReaderBase(aFile)
	{
	}

TBool CFileLineReaderBase::ReadL(TInt& aCursor)
	{
	TBuf8<1024> line;
	User::LeaveIfError(iFile.Read(line));
	if (line.Length() <= 0)
		return EFalse;
	const TInt fromold = iBuffer != NULL ? iBuffer->Length() - aCursor : 0;

	HBufC* buffer = CopyL(fromold, line);

	if (fromold > 0)
		{
		aCursor = 0;
		const TPtrC rest = iBuffer->Right(fromold);
		TPtr ptr = buffer->Des();
		ptr.Insert(0, rest);
		}
	delete iBuffer;
	iBuffer = buffer;
	return ETrue;
	}

HBufC* CFileLineReaderUtf16LE::CopyL(TInt aFromOld, const TDesC8& aLine)
	{
	const TInt newlen = aFromOld + aLine.Length() / 2;
	HBufC* buffer = HBufC::NewL(newlen);
	TPtr ptr = buffer->Des();
	ptr.Copy((TUint16*) (aLine.Ptr()), aLine.Length() / 2);
	return buffer;
	}

HBufC* CFileLineReaderUtf16BE::CopyL(TInt aFromOld, const TDesC8& aLine)
	{
	const TInt newlen = aFromOld + aLine.Length() / 2;
	HBufC* buffer = HBufC::NewL(newlen);
	TPtr ptr = buffer->Des();
	TUint8* pp = (TUint8*) (aLine.Ptr());

	for (TInt i = 0; i < aLine.Length(); i += 2)
		{
		const TUint16 c = pp[i] | (pp[i + 1] << 8);
		ptr.Append(TChar(c));
		}
	return buffer;
	}

HBufC* CFileLineReaderAscii::CopyL(TInt aFromOld, const TDesC8& aLine)
	{
	const TInt newlen = Max(0, aFromOld) + aLine.Length();
	HBufC* buffer = HBufC::NewL(newlen);
	TPtr ptr = buffer->Des();
	ptr.Copy(aLine);
	return buffer;
	}

/*
 TBool CFileLineReaderUtf16LE::ReadL(TInt& aCursor)
 {
 TBuf8<1024> line;
 User::LeaveIfError(iFile.Read(line));
 if(line.Length() <= 0)
 return EFalse;
 const TInt fromold = iBuffer != NULL ?  iBuffer->Length() - aCursor : 0;
 
 const TInt newlen = fromold + line.Length() / 2;
 HBufC* buffer = HBufC::NewL(newlen);
 TPtr ptr = buffer->Des();
 ptr.Copy((TUint16*)(line.Ptr()), line.Length() / 2);
 
 
 if(fromold > 0)
 {
 aCursor = 0;
 const TPtrC rest = iBuffer->Right(fromold);
 ptr.Insert(0, rest);
 }
 delete iBuffer;
 iBuffer = buffer;
 return ETrue;
 }

 TBool CFileLineReaderUtf16BE::ReadL(TInt& aCursor)
 {
 TBuf8<1024> line;
 User::LeaveIfError(iFile.Read(line));
 if(line.Length() <= 0)
 return EFalse;
 const TInt fromold = iBuffer != NULL ?  iBuffer->Length() - aCursor : 0;
 
 
 const TInt newlen = fromold + line.Length() / 2;
 HBufC* buffer = HBufC::NewL(newlen);
 TPtr ptr = buffer->Des();
 TUint8* pp = (TUint8*)(line.Ptr());
 
 for(TInt i = 0; i < line.Length() ; i+=2)
 {
 const TUint16 c = pp[i] | (pp[i + 1] << 8);
 ptr.Append(TChar(c));
 }
 if(fromold > 0)
 {
 aCursor = 0;
 const TPtrC rest = iBuffer->Right(fromold);
 ptr.Insert(0, rest);
 }
 delete iBuffer;
 iBuffer = buffer;
 return ETrue;
 }

 TBool CFileLineReaderAscii::ReadL(TInt& aCursor)
 {
 TBuf8<1024> line;
 User::LeaveIfError(iFile.Read(line));
 if(line.Length() <= 0)
 return EFalse;
 const TInt fromold = iBuffer != NULL ?  iBuffer->Length() - aCursor : 0;
 
 const TInt newlen = Max(0, fromold) + line.Length();
 HBufC* buffer = HBufC::NewL(newlen);
 TPtr ptr = buffer->Des();
 ptr.Copy(line);
 
 
 if(fromold > 0)
 {
 aCursor = 0;
 const TPtrC rest = iBuffer->Right(fromold);
 ptr.Insert(0, rest);
 }
 delete iBuffer;
 iBuffer = buffer;
 return ETrue;
 }
 */
