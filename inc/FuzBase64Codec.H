// BASE64.H
//
// Copyright (c) 1997-1999 Symbian Ltd.  All rights reserved.
//
#ifndef __BASE64_H
#define __BASE64_H


const TInt KErrInvalidSource=(-1);
const TUint8 KErrInvalidCharacter=65;

enum TBase64Panic
	{
	EBase64Overflow
	};

class CFuzBase64Codec : public CBase
	{
public:
	void Reset();
	TInt Encode(const TDesC8& aSource, TDes8& aResult) const;
	TInt Decode(const TDesC8& aSource, TDes8& aResult);
	TUint8 Base64Char(TText8 aChar) const;
private:
	TUint8 iQuart[4];
	TInt iQuartCount;
	};

#endif
