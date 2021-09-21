
#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <e32base.h>	
#include <e32std.h>		
#include <e32cmn.h>
#include <f32file.h>


_LIT(KDELIM_CRLF,"\r\n"); 	//windows newline
_LIT(KDELIM_LF,"\n"); 		//linux newline 
_LIT(KDELIM_CR,"\r"); 		//windows newline
	
class StringUtils
	{
public:	

	static HBufC* ReplaceLC(const TDesC& aString, const TDesC& aToFind, const TDesC& aToReplace);
	static void CleanString( const TDesC& aSymbols, TDes& aString, TBool aAllow=ETrue);
	static TBool ValidateStringL(const TDesC& aSymbols, const TDesC& aString, TBool aAllow);
	static HBufC8* ParseHexInBytesLC(const TDesC& hexLine);
	static HBufC* ParseBytesInHexLC(const TDesC8& bytes);
	static HBufC* LoadFromResourceLC(TInt aResFmtId,...);
	static HBufC* LoadFromResourceLC(TInt aResFmtId, VA_LIST aList);
	static TBool StringCompare(const HBufC& aStringA, const HBufC& aStringB);
	static TBool StringCompareF(const HBufC& aStringA, const HBufC& aStringB);
	static void ReplaceAll(TDes& string, const TDesC& toFind, const TDesC& newVal);
	static TPtrC DetectDelimiterL(const TDesC& aString);
	static TPtrC Trim(const TDesC& aString);
	};

 
 
class CStringTokenizer : public CBase
	{
public:
	~CStringTokenizer();
	static CStringTokenizer* NewL();
	static CStringTokenizer* NewLC();
	void InitializeL(const TDesC& aString);
	void AddCharsTokenL(const TDesC& aCharsList);
	void AddStringTokenL(const TDesC& aStringToken);
	void Clear();
	TBool HasMoreTokens();
	TPtrC NextToken();
	
private:
	CStringTokenizer();
	void ConstructL();
	
private:
	RPointerArray<HBufC> iTokenList;
	TPtrC iLeftString;
	};

#endif
