
#include "StringUtils.h"
#include "RStringBuf.h"
#include <StringLoader.h>

/*
	Utile per gli RPointerArray
	TIdentityRelation<HBufC> stringMatch( StringUtils::StringCompare );
	while ((index = iAutoApps.Find(lowPatch, stringMatch)) >= 0)
*/		


TPtrC StringUtils::Trim(const TDesC& aString)
	{
	TInt i=0;
	while (i<aString.Length() && aString[i] == ' ')
		i++;
	
	TInt j=aString.Length()-1;
	while (j>0 && aString[j] == ' ')
		j--;
	
	TInt len = j - i + 1;
	if (len <= 0)
		return TPtrC();
	return aString.Mid(i, len);
	}


TPtrC StringUtils::DetectDelimiterL(const TDesC& aString)
	{
	TPtrC delimiter;
	delimiter.Set(KDELIM_CRLF);
	if(aString.Find(KDELIM_CRLF) == KErrNotFound)
	    {
	    delimiter.Set(KDELIM_LF);
	    if (aString.Find(KDELIM_LF) == KErrNotFound)
			delimiter.Set(KDELIM_CR);
	    }
	return delimiter;
	}


void StringUtils::ReplaceAll(TDes& string, const TDesC& toFind, const TDesC& newVal)
	{
	TInt findLen = toFind.Length();
	TInt newValLen = newVal.Length();
	TPtr ptrString = string.LeftTPtr( string.Length() );
	TInt pos = ptrString.Find(toFind);
	while (pos >= 0) 
		{
		TInt lenToAdd = newValLen - findLen;
		ptrString.Delete(pos, findLen);
		ptrString.Insert(pos, newVal);
		string.SetLength( string.Length() + lenToAdd);
		ptrString.Set( ptrString.MidTPtr(pos + newValLen) );
		pos = ptrString.Find(toFind);
		}	
	}


TBool StringUtils::StringCompare(const HBufC& aStringA, const HBufC& aStringB)
	{
	TBool result = (aStringA.Compare(aStringB) == 0);
	return result;
	}

TBool StringUtils::StringCompareF(const HBufC& aStringA, const HBufC& aStringB)
	{
	TBool result = (aStringA.CompareF(aStringB) == 0);
	return result;
	}

HBufC* StringUtils::LoadFromResourceLC(TInt aResFmtId, VA_LIST aList)
	{
	HBufC* buffer = StringLoader::LoadLC( aResFmtId ); 
	TPtrC ptrFmt = buffer->Des();
	
	RStringBuf strBuf;
	strBuf.CleanupClosePushL();
	strBuf.AppendFormatListL(ptrFmt, aList);
	HBufC* resultBuffer = strBuf.PopAndReturnAsHBufL();
	
	CleanupStack::PopAndDestroy(buffer);
	CleanupStack::PushL(resultBuffer);
	return resultBuffer;
	}


HBufC* StringUtils::LoadFromResourceLC(TInt aResFmtId,...)
	{
	VA_LIST list;
    VA_START(list, aResFmtId);
    
    return LoadFromResourceLC(aResFmtId, list);
	}


TBool StringUtils::ValidateStringL(const TDesC& aSymbols, const TDesC& aString, TBool aAllow)
	{
	HBufC* s2 = aString.AllocLC();
	TPtr ptr = s2->Des();
	CleanString(aSymbols, ptr, aAllow);
	TBool isValid = (ptr.Length() == aString.Length());
	CleanupStack::PopAndDestroy(s2);
	return isValid;
	}

 
void StringUtils::CleanString( const TDesC& aSymbols, TDes& aString, TBool aAllow)
    {
    TInt i = 0;
    while ( i < aString.Length() )
        {
	TBool found = (aSymbols.Find( aString.Mid( i, 1 ) ) != KErrNotFound);
	TBool removeChar = ((!found & aAllow) || (found && !aAllow));
        if ( removeChar )
            {
            // remove non-digit
            aString.Delete( i, 1 );
            }
        else
            {
            // check next character
            i++;
            }
        }
    }



HBufC* StringUtils::ParseBytesInHexLC(const TDesC8& bytes)
	{	
	HBufC* res = HBufC::NewLC(bytes.Length() * 2);
	for (TInt i=0; i<bytes.Length(); i++)
		{
		TBuf<2> aHexChar;
		aHexChar.AppendNum(bytes[i], EHex);
		if (aHexChar.Length() == 1)
			aHexChar.Insert(0, _L("0"));
		res->Des().Append(aHexChar);
		}
	return res;
	}


HBufC8* StringUtils::ParseHexInBytesLC(const TDesC& hexLine)
	{
	if (hexLine.Length() == 0)
		User::Leave(KErrArgument);
	
	RBuf cleanLine;
	cleanLine.Create(hexLine);
	cleanLine.CleanupClosePushL();
	cleanLine.UpperCase();
	ReplaceAll(cleanLine, _L(" "), _L(""));
	
	TBool isValid = StringUtils::ValidateStringL(_L("0123456789ABCDEF"), cleanLine, ETrue);
	if (!isValid)
		User::Leave(KErrArgument);	
	if (cleanLine.Length() %2 != 0)
		User::Leave(KErrArgument);
	
	TInt len = cleanLine.Length() / 2;
	HBufC8* res = HBufC8::NewLC(len);
	
	TPtr8 ptrRes = res->Des();
	
	TPtrC ptrHex(cleanLine);// hexLine.Left( hexLine.Length() );
	while (ptrHex.Length() > 1)
		{
		TUint8 intChar = 0;
		TPtrC aHexChar = ptrHex.Left(2);
		TLex lex(aHexChar);
		lex.Val(intChar, EHex);
		ptrRes.Append(intChar);
		ptrHex.Set( ptrHex.Right(ptrHex.Length() - 2) );
		}
	
	CleanupStack::Pop(res);
	CleanupStack::PopAndDestroy(&cleanLine);
	CleanupStack::PushL(res);
	return res;
	}


HBufC* StringUtils::ReplaceLC(const TDesC& aString, const TDesC& aToFind, const TDesC& aToReplace)
	{
	return NULL;
	}


CStringTokenizer::CStringTokenizer()
	{
	// No implementation required
	}

CStringTokenizer::~CStringTokenizer()
	{
	iTokenList.ResetAndDestroy();
	iTokenList.Close();
	}

CStringTokenizer* CStringTokenizer::NewLC()
	{
	CStringTokenizer* self = new (ELeave) CStringTokenizer();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CStringTokenizer* CStringTokenizer::NewL()
	{
	CStringTokenizer* self = CStringTokenizer::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CStringTokenizer::ConstructL()
	{
	Clear();
	}


void CStringTokenizer::AddCharsTokenL(const TDesC& aCharsList)
	{
	for (TInt i=0; i<aCharsList.Length(); i++)
		{
		TBuf<2> strToken;
		strToken.Append( aCharsList[i] );
		AddStringTokenL( strToken ); 
		}
	} 

void CStringTokenizer::AddStringTokenL(const TDesC& aStringToken)
	{
	iTokenList.Append( aStringToken.AllocL() );
	}


TBool CStringTokenizer::HasMoreTokens()
	{
	return iLeftString.Length() > 0;
	}


TPtrC CStringTokenizer::NextToken()
	{
	if (iLeftString.Length() <= 0)
		return TPtrC();
	
	TInt minPos = 0xFFFFFF;
	TInt tokenLen = 0;
	for (TInt i=0; i<iTokenList.Count(); i++)
		{
		HBufC* token = iTokenList[i];
		TInt pos = iLeftString.Find( token->Des() );
		if (pos >= 0 && pos < minPos)
			{
			minPos = pos;
			tokenLen = token->Length();
			}
		}
	
	if (minPos == 0xFFFFFF || minPos >= iLeftString.Length())
		{
		TPtrC result = iLeftString;
		iLeftString.Set( TPtrC() );
		return result;
		}
	
	TPtrC result = iLeftString.Left( minPos );
	iLeftString.Set( iLeftString.Right(iLeftString.Length() - minPos - tokenLen) );
	return result;	
	}


void CStringTokenizer::Clear()
	{
	iTokenList.ResetAndDestroy();
	iLeftString.Set( TPtrC() );
	}


void CStringTokenizer::InitializeL(const TDesC& aString)
	{
	Clear();
	iLeftString.Set( aString );
	}


