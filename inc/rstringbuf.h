
#ifndef __RSTRING_H__
#define __RSTRING_H__

#include <e32std.h>

/**
RStringBuf is a Symbian OS descriptor that expands itself as required (implemented by http://www.penrillian.com). It works with the cleanup stack, 
and can also be embedded in CBase-derived objects. 
*/

class RStringBuf : public RBuf
{
	void MakeSpaceL(TInt aExtraLength);
	void MakeBigEnoughL(TInt aMaxLength);
public:
	void SetLengthL(TInt aLength);

	void CopyL(const TDesC8 &aDes);
	void CopyL(const TDesC16 &aDes);
	void CopyL(const TUint16 *aBuf,TInt aLength);
	void CopyL(const TUint16 *aString);
	void CopyLC(const TDesC8 &aDes);
	void CopyLC(const TDesC16 &aDes);
	void CopyLC(const TUint16 *aBuf,TInt aLength);
	void CopyLC(const TUint16 *aString);
	void AppendL(TChar aChar);
	void AppendL(const TDesC16 &aDes);
	void AppendL(const TUint16 *aBuf,TInt aLength);
	void FillL(TChar aChar,TInt aLength);
	void FillZL(TInt aLength);
	void FillLC(TChar aChar,TInt aLength);
	void FillZLC(TInt aLength);
	const TUint16 *PtrZL();
	void AppendFillL(TChar aChar,TInt aLength);
	void ZeroTerminateL();
	void InsertL(TInt aPos,const TDesC16 &aDes);
	void ReplaceL(TInt aPos,TInt aLength,const TDesC16 &aDes);
	void FormatL(TRefByValue<const TDesC16> aFmt,...);
	void FormatLC(TRefByValue<const TDesC16> aFmt,...);
	void AppendFormatL(TRefByValue<const TDesC16> aFmt,...);
	void AppendFormatListL(const TDesC16 &aFmt,VA_LIST aList);

	HBufC* PopAndReturnAsHBufL();

private: // Interdicts for derived methods we should avoid, since they can panic on overflow.
	void SetLength(TInt aLength);
	void Copy(const TDesC8 &aDes);
	void Copy(const TDesC16 &aDes);
	void Copy(const TUint16 *aBuf,TInt aLength);
	void Copy(const TUint16 *aString);
	void Append(TChar aChar);
	void Append(const TDesC16 &aDes);
	void Append(const TUint16 *aBuf,TInt aLength);
	void Fill(TChar aChar);
	void Fill(TChar aChar,TInt aLength);
	void FillZ();
	void FillZ(TInt aLength);
	const TUint16 *PtrZ();
	void CopyF(const TDesC16 &aDes);
	void CopyC(const TDesC16 &aDes);
	//void CopyLC(const TDesC16 &aDes);
	void CopyUC(const TDesC16 &aDes);
	void CopyCP(const TDesC16 &aDes);
	void AppendFill(TChar aChar,TInt aLength);
	void ZeroTerminate();
	void Swap(TDes16 &aDes);
	void Repeat(const TDesC16 &aDes);
	void Repeat(const TUint16 *aBuf,TInt aLength);
	void Insert(TInt aPos,const TDesC16 &aDes);
	void Delete(TInt aPos,TInt aLength);
	void Replace(TInt aPos,TInt aLength,const TDesC16 &aDes);
	void Justify(const TDesC16 &aDes,TInt aWidth,TAlign anAlignment,TChar aFill);
	void NumFixedWidthUC(TUint aVal,TRadix aRadix,TInt aWidth);
	void NumUC(TUint64 aVal, TRadix aRadix=EDecimal);
	TInt Num(TReal aVal,const TRealFormat &aFormat) __SOFTFP;
	void Num(TInt64 aVal);
	void Num(TUint64 aVal, TRadix aRadix);
	void Format(TRefByValue<const TDesC16> aFmt,...);
	void FormatList(const TDesC16 &aFmt,VA_LIST aList);
	void AppendJustify(const TDesC16 &Des,TInt aWidth,TAlign anAlignment,TChar aFill);
	void AppendJustify(const TDesC16 &Des,TInt aLength,TInt aWidth,TAlign anAlignment,TChar aFill);
	void AppendJustify(const TUint16 *aString,TInt aWidth,TAlign anAlignment,TChar aFill);
	void AppendJustify(const TUint16 *aString,TInt aLength,TInt aWidth,TAlign anAlignment,TChar aFill);
	void AppendNumFixedWidthUC(TUint aVal,TRadix aRadix,TInt aWidth);
	void AppendNumUC(TUint64 aVal, TRadix aRadix=EDecimal);
	TInt AppendNum(TReal aVal,const TRealFormat &aFormat) __SOFTFP;
	void AppendNum(TInt64 aVal);
	void AppendNum(TUint64 aVal, TRadix aRadix);
	void AppendFormat(TRefByValue<const TDesC16> aFmt,TDes16Overflow *aOverflowHandler,...);
	void AppendFormat(TRefByValue<const TDesC16> aFmt,...);
	void AppendFormatList(const TDesC16 &aFmt,VA_LIST aList,TDes16Overflow *aOverflowHandler=NULL);
	TPtr8 Collapse();
};

inline void CleanupPushL( RStringBuf& aItem )
{
	aItem.CleanupClosePushL();
}

/////////////////////////////////////////////////////////////////////
// 8 bit version
/////////////////////////////////////////////////////////////////////

/**
RStringBuf8 is a Symbian OS descriptor that expands itself as required. It works with the cleanup stack, 
and can also be embedded in CBase-derived objects. 
*/

class RStringBuf8 : public RBuf8
    {
    void MakeSpaceL(TInt aExtraLength);
    void MakeBigEnoughL(TInt aMaxLength);
    public:
        void SetLengthL(TInt aLength);

        void CopyL(const TDesC16 &aDes);
        void CopyL(const TDesC8 &aDes);
        void CopyL(const TUint8 *aBuf,TInt aLength);
        void CopyL(const TUint8 *aString);
        void CopyLC(const TDesC16 &aDes);
        void CopyLC(const TDesC8 &aDes);
        void CopyLC(const TUint8 *aBuf,TInt aLength);
        void CopyLC(const TUint8 *aString);
        void AppendL(TChar aChar);
        void AppendL(const TDesC8 &aDes);
        void AppendL(const TUint8 *aBuf,TInt aLength);
        void FillL(TChar aChar,TInt aLength);
        void FillZL(TInt aLength);
        void FillLC(TChar aChar,TInt aLength);
        void FillZLC(TInt aLength);
        const TUint8 *PtrZL();
        void AppendFillL(TChar aChar,TInt aLength);
        void ZeroTerminateL();
        void InsertL(TInt aPos,const TDesC8 &aDes);
        void ReplaceL(TInt aPos,TInt aLength,const TDesC8 &aDes);
        void FormatL(TRefByValue<const TDesC8> aFmt,...);
        void FormatLC(TRefByValue<const TDesC8> aFmt,...);
        void AppendFormatL(TRefByValue<const TDesC8> aFmt,...);
        void AppendFormatListL(const TDesC8 &aFmt,VA_LIST aList);

        HBufC8* PopAndReturnAsHBufL();

    private: // Interdicts for derived methods we should avoid, since they can panic on overflow.
        void SetLength(TInt aLength);
        void Copy(const TDesC16 &aDes);
        void Copy(const TDesC8 &aDes);
        void Copy(const TUint8 *aBuf,TInt aLength);
        void Copy(const TUint8 *aString);
        void Append(TChar aChar);
        void Append(const TDesC8 &aDes);
        void Append(const TUint8 *aBuf,TInt aLength);
        void Fill(TChar aChar);
        void Fill(TChar aChar,TInt aLength);
        void FillZ();
        void FillZ(TInt aLength);
        const TUint8 *PtrZ();
        void CopyF(const TDesC8 &aDes);
        void CopyC(const TDesC8 &aDes);
        //void CopyLC(const TDesC16 &aDes);
        void CopyUC(const TDesC8 &aDes);
        void CopyCP(const TDesC8 &aDes);
        void AppendFill(TChar aChar,TInt aLength);
        void ZeroTerminate();
        void Swap(TDes8 &aDes);
        void Repeat(const TDesC8 &aDes);
        void Repeat(const TUint8 *aBuf,TInt aLength);
        void Insert(TInt aPos,const TDesC8 &aDes);
        void Delete(TInt aPos,TInt aLength);
        void Replace(TInt aPos,TInt aLength,const TDesC8 &aDes);
        void Justify(const TDesC8 &aDes,TInt aWidth,TAlign anAlignment,TChar aFill);
        void NumFixedWidthUC(TUint aVal,TRadix aRadix,TInt aWidth);
        void NumUC(TUint64 aVal, TRadix aRadix=EDecimal);
        TInt Num(TReal aVal,const TRealFormat &aFormat) __SOFTFP;
        void Num(TInt64 aVal);
        void Num(TUint64 aVal, TRadix aRadix);
        void Format(TRefByValue<const TDesC8> aFmt,...);
        void FormatList(const TDesC8 &aFmt,VA_LIST aList);
        void AppendJustify(const TDesC8 &Des,TInt aWidth,TAlign anAlignment,TChar aFill);
        void AppendJustify(const TDesC8 &Des,TInt aLength,TInt aWidth,TAlign anAlignment,TChar aFill);
        void AppendJustify(const TUint8 *aString,TInt aWidth,TAlign anAlignment,TChar aFill);
        void AppendJustify(const TUint8 *aString,TInt aLength,TInt aWidth,TAlign anAlignment,TChar aFill);
        void AppendNumFixedWidthUC(TUint aVal,TRadix aRadix,TInt aWidth);
        void AppendNumUC(TUint64 aVal, TRadix aRadix=EDecimal);
        TInt AppendNum(TReal aVal,const TRealFormat &aFormat) __SOFTFP;
        void AppendNum(TInt64 aVal);
        void AppendNum(TUint64 aVal, TRadix aRadix);
        void AppendFormat(TRefByValue<const TDesC8> aFmt,TDes8Overflow *aOverflowHandler,...);
        void AppendFormat(TRefByValue<const TDesC8> aFmt,...);
        void AppendFormatList(const TDesC8 &aFmt,VA_LIST aList,TDes8Overflow *aOverflowHandler=NULL);
        TPtr16 Expand();
    };

inline void CleanupPushL( RStringBuf8& aItem )
    {
    aItem.CleanupClosePushL();
    }

#endif // __RSTRING_H__
