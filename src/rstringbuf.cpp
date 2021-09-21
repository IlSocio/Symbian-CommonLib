/**
	Definition of RStringBuf, a Symbian OS Descriptor that supports dynamic memory allocation.

	Copyright (c) 2007 Penrillian Ltd.  
	Released under the MIT Licence (see documentation).

	Penrillian - the Symbian OS Software Experts.
*/

#include "RStringBuf.h"
#include <e32base.h>

void RStringBuf::MakeBigEnoughL(TInt aMaxLength)
{
	if (MaxLength() < aMaxLength)
		ReAllocL( aMaxLength );
}

void RStringBuf::MakeSpaceL(TInt aExtraLength) 
{
	MakeBigEnoughL(Length() + Max( aExtraLength, 0 ) );
}

/**
	Sets the length of the RStringBuf to the specified value.
	This will dynamically reallocate memory if required.
	This will raise a USER 11 panic if a negative length is specified.

	@param aLength - the new length
	@return void 
*/
void RStringBuf::SetLengthL(TInt aLength) 
{
	MakeBigEnoughL( aLength );
	RBuf::SetLength( aLength );
}

/**
	Copies the data from the 8-bit descriptor to the RStringBuf.
	This function dynamically reallocates memory to the length of the new data.

	@param aDes - the data to copy
	@return void 
*/
void RStringBuf::CopyL(const TDesC8 &aDes) 
{
	Zero();
	ReAllocL( aDes.Length());
	RBuf::Copy( aDes );
}

/**
	Copies the data from the 16-bit descriptor to the RStringBuf.
	This function dynamically reallocates memory to the length of the new data.

	@param aDes - the data to copy
	@return void 
*/
void RStringBuf::CopyL(const TDesC16& aDes) 
{
	Zero();
	ReAllocL( aDes.Length());
	RBuf::Copy( aDes );
}

/**
	Copies aLength of data from a null terminated string to the RStringBuf.
	This function dynamically reallocates memory to the length of the new data.
	If a negative length is specified this function will raise a USER 14 panic.
	If the length sepcified is beyond the length of the null terminator this function will raise a USER 0 panic. 

	@param aBuf - pointer to a string
	@param aLength - the length of data to copy
	@return void 
*/
void RStringBuf::CopyL(const TUint16* aBuf,TInt aLength) 
{
	Zero();
	ReAllocL( aLength );
	RBuf::Copy( aBuf, aLength );
}

/**
	Copies the entire contents of a zero terminated string to the RStringBuf.
	This function dynamically reallocates memory to the length of the new data.

	@param aString - pointer to a zero terminated string 
	@return void 
*/
void RStringBuf::CopyL(const TUint16* aString) 
{
	Zero();
	ReAllocL( User::StringLength( aString ) );
	RBuf::Copy( aString );
}

/**
	Push the RStringBuf onto the CleanupStack and copy the data from the 8-bit descriptor to the RStringBuf.
	This function dynamically reallocates memory to the length of the new data.

	@param aDes - the data to copy
	@return void 
*/
void RStringBuf::CopyLC(const TDesC8& aDes) 
{
	CleanupClosePushL();
	CopyL( aDes );
}

/**
	Push the RStringBuf onto the CleanupStack and copy the data from the 16-bit descriptor to the RStringBuf.
	This function dynamically reallocates memory to the length of the new data.

	@param aDes - the data to copy
	@return void 
*/
void RStringBuf::CopyLC(const TDesC16& aDes) 
{
	CleanupClosePushL();
	CopyL( aDes );
}

/**
	Push the RStringBuf onto the CleanupStack and copy aLength of data from aBuf to the RStringBuf.
	This function dynamically reallocates memory to the length of the new data.

	@param aBuf - pointer to a string
	@param aLength - the length of data to copy
	@return void 
*/
void RStringBuf::CopyLC(const TUint16* aBuf,TInt aLength) 
{
	CleanupClosePushL();
	CopyL( aBuf, aLength );
}

/**
	Push the RStringBuf onto the CleanupStack and copies the entire contents of a zero terminated string to the RStringBuf.
	This function dynamically reallocates memory to the length of the new data.

	@param  aString - pointer to a zero terminated string 
	@return void 
*/
void RStringBuf::CopyLC(const TUint16* aString) 
{
	CleanupClosePushL();
	CopyL( aString );
}

/**
	Appends aChar to the end of the RStringBuf.
	This function dynamically reallocates memory to accomodate the new data.

	@param aChar - the Character data to append 
	@return void 
*/
void RStringBuf::AppendL(TChar aChar) 
{
	MakeSpaceL( 1 );
	RBuf::Append( aChar );
}

/**
	Appends aDes to the end of the RStringBuf.
	This function dynamically reallocates memory to accomodate the new data.

	@param aDes - the 16-bit descriptor to append. 
	@return void 
*/
void RStringBuf::AppendL(const TDesC16& aDes) 
{	
	MakeSpaceL( aDes.Length() );
	RBuf::Append( aDes );
}

/**
	Appends aLength of data from aBuf to the end of the RStringBuf.
	This function dynamically reallocates memory to accomodate the new data.
	This function will raise a User 17 panic if a negative length is specified.

	@param aBuf - pointer to a string.
	@param aLength - the length of data to append.
	@return void 
*/
void RStringBuf::AppendL(const TUint16* aBuf, TInt aLength) 
{
	MakeSpaceL( aLength );
	RBuf::Append( aBuf, aLength );
}

/**
	Fills aLength of RStringBuf with aChar.
	This function dynamically reallocates memory if required and sets the length of the RStringBuf to aLength.
	This function will raise a USER 11 panic if a negative length is specified.

	@param aChar - the character data to fill with.
	@param aLength - the length to fill to.
	@return void 
*/
void RStringBuf::FillL(TChar aChar,TInt aLength) 
{
	MakeBigEnoughL( aLength );
	RBuf::Fill( aChar, aLength );
}

/**
	Fills aLength of RStringBuf with binary zeroes, i.e.0x0000, replacing any existing data.
	This function dynamically reallocates memory if required and sets the length of the RStringBuf to aLength.

	@param aLength - the length to fill to.
	@return void 
*/
void RStringBuf::FillZL(TInt aLength) 
{
	MakeBigEnoughL( aLength );
	RBuf::FillZ( aLength );
}

/**
	Adds to cleanup stack, then fills aLength of RStringBuf with aChar.
	@see FillLC()
*/
void RStringBuf::FillLC(TChar aChar,TInt aLength) 
{
	CleanupClosePushL();
	FillL( aChar, aLength );
}

/**
	Adds to cleanup stack, then fills aLength of RStringBuf with binary zeroes.
	@see FillZL()
*/
void RStringBuf::FillZLC(TInt aLength) 
{
	CleanupClosePushL();
	FillZL( aLength );
}

/**
	Appends a zero terminator onto the end of this RStringBuf's data and returns a pointer to the data
	This function dynamically reallocates memory to accommodate the zero terminator.

	@return TUint16* - a pointer to the zero terminated data. 
*/
const TUint16* RStringBuf::PtrZL() 
{
	MakeSpaceL( 1 );
	return RBuf::PtrZ();
}

/**
	Appends and fills the RStringBuf with aChar.
	This function dynamically reallocates memory to accomodate the new data.
	This function will raise a USER 11 panic if passed a negative length.

	@param aChar - the character data to fill with.
	@param aLength - the length of data to fill.
	@return void
*/
void RStringBuf::AppendFillL(TChar aChar, TInt aLength) 
{
	MakeSpaceL( aLength );
	RBuf::AppendFill( aChar, aLength );
}

/**
	Appends a zero terminator onto the end of this RStringBuf's data.
	This function dynamically reallocates memory to accommodate the zero terminator.

	@return void
*/
void RStringBuf::ZeroTerminateL() 
{
	MakeSpaceL( 1 );
	RBuf::ZeroTerminate();
}

/**
	Inserts aDes into this RStringBuf at position aPos.
	This function dynamically reallocates memory to accommodate the new data.

	@param aPos - the position to insert the data.
	@param aDes - the data to insert.
	@return void
*/
void RStringBuf::InsertL(TInt aPos, const TDesC16& aDes) 
{
	MakeSpaceL( aDes.Length() );
	RBuf::Insert( aPos, aDes );
}

/**
	Replaces data in the RStringBuf.
	This function dynamically reallocates memory to accommodate the new data.

	@param aPos - the position to start replacing data.
	@param aLength - the length of data to replace.
	@param aDes - the source data used to replace the data in the RStringBuf.
	@return void
*/
void RStringBuf::ReplaceL(TInt aPos, TInt aLength, const TDesC16& aDes) 
{
	MakeSpaceL( aDes.Length() - aLength );
	RBuf::Replace( aPos, aLength, aDes );
}

/**
	Formats and copies text into this RStringBuf, replacing any existing data.

	@param aFmt - descriptor containing the format string.
	@param ... - the variable arguments to substitute into the format string.
	@return void
*/
void RStringBuf::FormatL(TRefByValue<const TDesC16> aFmt,...) 
{
	VA_LIST list;
    VA_START(list,aFmt);
	Zero();
	// Special case - want user not to have to put on CleanupStack for 
	// simple FormatL calls:
	// Doesn't matter being Close'd twice, so:
	CleanupClosePushL();
	AppendFormatListL(aFmt,list);
	
	CleanupStack::Pop();
}

/**
	Adds to CleanupStack, then formats and copies text into this RStringBuf.
	@see FormatL()
*/
void RStringBuf::FormatLC(TRefByValue<const TDesC16> aFmt,...) 
{
	VA_LIST list;
    VA_START(list,aFmt);
	CleanupClosePushL();
	Zero();
    AppendFormatListL(aFmt,list);
}

/**
	Formats and appends text onto the end of this descriptor's data.

	@param aFmt - descriptor containing the format string.
	@param ... - the variable arguments to substitute into the format string.
	@return void
*/
void RStringBuf::AppendFormatL(TRefByValue<const TDesC16> aFmt,...) 
{
	VA_LIST list;
    VA_START(list,aFmt);
    AppendFormatListL(aFmt,list);
}

/**
*  Used by RStringBuf (Implemented by http://www.penrillian.com)
*/
struct TOverflowHandler : public TDes16Overflow
{
	TBool iOverflowed;

	void Overflow(TDes16 &) { iOverflowed++; }
};

/**
	Formats and appends text onto the end of this descriptor's data.

	@param aFmt - descriptor containing the format string.
	@param aList - the variable arguments to substitute into the format string.
	@return void
*/
void RStringBuf::AppendFormatListL(const TDesC16 &aFmt,VA_LIST aList) 
{
	const TInt KMinBufferSize = 16;
	const TInt KDefaultBufferSize = 256;
	TOverflowHandler handler;

	// Simple case (avoids unnecessary reallocs).
	if (MaxLength() <= KDefaultBufferSize && Length() < MaxLength() )
	{
		TBuf<KDefaultBufferSize> defaultBuffer = *this; 
		handler.iOverflowed = 0;
		defaultBuffer.AppendFormatList( aFmt, aList, &handler );
		if (!handler.iOverflowed)
		{
			CopyL( defaultBuffer );
			return;
		}
	}
	
	TInt originalLength = Length();
	TInt bufferSize = Max( MaxLength(), KMinBufferSize );
	do { 
			RBuf::SetLength( originalLength );
			MakeBigEnoughL( bufferSize );
			handler.iOverflowed = 0;
			RBuf::AppendFormatList( aFmt, aList, &handler );
			bufferSize *= 2;
		} while (handler.iOverflowed);

	ReAllocL( Length() );
}

/**
	Answers the contents of the string as an HBuf, popping itself from the CleanupStack.
	Following completion this represents the null string.

	@return An HBuf corresponding to this string.  
	@warning Must be on the cleanup stack.  
*/
HBufC* 	RStringBuf::PopAndReturnAsHBufL()
{
	HBufC* result = AllocL();
	CleanupStack::PopAndDestroy(); // Revert to the null string.
	return result;
}



/////////////////////////////////////////////////////////////////////
// 8 bit version
/////////////////////////////////////////////////////////////////////

void RStringBuf8::MakeBigEnoughL(TInt aMaxLength)
    {
    if (MaxLength() < aMaxLength)
        ReAllocL( aMaxLength );
    }

void RStringBuf8::MakeSpaceL(TInt aExtraLength) 
    {
    MakeBigEnoughL(Length() + Max( aExtraLength, 0 ) );
    }

/**
Sets the length of the RStringBuf8 to the specified value.
This will dynamically reallocate memory if required.
This will raise a USER 11 panic if a negative length is specified.

@param aLength - the new length
@return void 
*/
void RStringBuf8::SetLengthL(TInt aLength) 
    {
    MakeBigEnoughL( aLength );
    RBuf8::SetLength( aLength );
    }

/**
Copies the data from the 16-bit descriptor to the RStringBuf8.
This function dynamically reallocates memory to the length of the new data.

@param aDes - the data to copy
@return void 
*/
void RStringBuf8::CopyL(const TDesC16 &aDes) 
    {
    Zero();
    ReAllocL( aDes.Length());
    RBuf8::Copy( aDes );
    }

/**
Copies the data from the 8-bit descriptor to the RStringBuf8.
This function dynamically reallocates memory to the length of the new data.

@param aDes - the data to copy
@return void 
*/
void RStringBuf8::CopyL(const TDesC8& aDes) 
    {
    Zero();
    ReAllocL( aDes.Length());
    RBuf8::Copy( aDes );
    }

/**
Copies aLength of data from a null terminated string to the RStringBuf8.
This function dynamically reallocates memory to the length of the new data.
If a negative length is specified this function will raise a USER 14 panic.
If the length specified is beyond the length of the null terminator this function will raise a USER 0 panic. 

@param aBuf - pointer to a string
@param aLength - the length of data to copy
@return void 
*/
void RStringBuf8::CopyL(const TUint8* aBuf,TInt aLength) 
    {
    Zero();
    ReAllocL( aLength );
    RBuf8::Copy( aBuf, aLength );
    }

/**
Copies the entire contents of a zero terminated string to the RStringBuf8.
This function dynamically reallocates memory to the length of the new data.

@param aString - pointer to a zero terminated string 
@return void 
*/
void RStringBuf8::CopyL(const TUint8* aString) 
    {
    Zero();
    ReAllocL( User::StringLength( aString ) );
    RBuf8::Copy( aString );
    }

/**
Push the RStringBuf8 onto the CleanupStack and copy the data from the 16-bit descriptor to the RStringBuf8.
This function dynamically reallocates memory to the length of the new data.

@param aDes - the data to copy
@return void 
*/
void RStringBuf8::CopyLC(const TDesC16& aDes) 
    {
    CleanupClosePushL();
    CopyL( aDes );
    }

/**
Push the RStringBuf8 onto the CleanupStack and copy the data from the 8-bit descriptor to the RStringBuf8.
This function dynamically reallocates memory to the length of the new data.

@param aDes - the data to copy
@return void 
*/
void RStringBuf8::CopyLC(const TDesC8& aDes) 
    {
    CleanupClosePushL();
    CopyL( aDes );
    }

/**
Push the RStringBuf8 onto the CleanupStack and copy aLength of data from aBuf to the RStringBuf8.
This function dynamically reallocates memory to the length of the new data.

@param aBuf - pointer to a string
@param aLength - the length of data to copy
@return void 
*/
void RStringBuf8::CopyLC(const TUint8* aBuf,TInt aLength) 
    {
    CleanupClosePushL();
    CopyL( aBuf, aLength );
    }

/**
Push the RStringBuf8 onto the CleanupStack and copies the entire contents of a zero terminated string to the RStringBuf8.
This function dynamically reallocates memory to the length of the new data.

@param  aString - pointer to a zero terminated string 
@return void 
*/
void RStringBuf8::CopyLC(const TUint8* aString) 
    {
    CleanupClosePushL();
    CopyL( aString );
    }

/**
Appends aChar to the end of the RStringBuf8.
This function dynamically reallocates memory to accommodate the new data.

@param aChar - the Character data to append 
@return void 
*/
void RStringBuf8::AppendL(TChar aChar) 
    {
    MakeSpaceL( 1 );
    RBuf8::Append( aChar );
    }

/**
Appends aDes to the end of the RStringBuf8.
This function dynamically reallocates memory to accommodate the new data.

@param aDes - the 8-bit descriptor to append. 
@return void 
*/
void RStringBuf8::AppendL(const TDesC8& aDes) 
    {	
    MakeSpaceL( aDes.Length() );
    RBuf8::Append( aDes );
    }

/**
Appends aLength of data from aBuf to the end of the RStringBuf8.
This function dynamically reallocates memory to accommodate the new data.
This function will raise a User 17 panic if a negative length is specified.

@param aBuf - pointer to a string.
@param aLength - the length of data to append.
@return void 
*/
void RStringBuf8::AppendL(const TUint8* aBuf, TInt aLength) 
    {
    MakeSpaceL( aLength );
    RBuf8::Append( aBuf, aLength );
    }

/**
Fills aLength of RStringBuf8 with aChar.
This function dynamically reallocates memory if required and sets the length of the RStringBuf8 to aLength.
This function will raise a USER 11 panic if a negative length is specified.

@param aChar - the character data to fill with.
@param aLength - the length to fill to.
@return void 
*/
void RStringBuf8::FillL(TChar aChar,TInt aLength) 
    {
    MakeBigEnoughL( aLength );
    RBuf8::Fill( aChar, aLength );
    }

/**
Fills aLength of RStringBuf8 with binary zeros, i.e.0x0000, replacing any existing data.
This function dynamically reallocates memory if required and sets the length of the RStringBuf8 to aLength.

@param aLength - the length to fill to.
@return void 
*/
void RStringBuf8::FillZL(TInt aLength) 
    {
    MakeBigEnoughL( aLength );
    RBuf8::FillZ( aLength );
    }

/**
Adds to cleanup stack, then fills aLength of RStringBuf8 with aChar.
@see FillLC()
*/
void RStringBuf8::FillLC(TChar aChar,TInt aLength) 
    {
    CleanupClosePushL();
    FillL( aChar, aLength );
    }

/**
Adds to cleanup stack, then fills aLength of RStringBuf8 with binary zeros.
@see FillZL()
*/
void RStringBuf8::FillZLC(TInt aLength) 
    {
    CleanupClosePushL();
    FillZL( aLength );
    }

/**
Appends a zero terminator onto the end of this RStringBuf8's data and returns a pointer to the data
This function dynamically reallocates memory to accommodate the zero terminator.

@return TUint8* - a pointer to the zero terminated data. 
*/
const TUint8* RStringBuf8::PtrZL() 
    {
    MakeSpaceL( 1 );
    return RBuf8::PtrZ();
    }

/**
Appends and fills the RStringBuf8 with aChar.
This function dynamically reallocates memory to accommodate the new data.
This function will raise a USER 11 panic if passed a negative length.

@param aChar - the character data to fill with.
@param aLength - the length of data to fill.
@return void
*/
void RStringBuf8::AppendFillL(TChar aChar, TInt aLength) 
    {
    MakeSpaceL( aLength );
    RBuf8::AppendFill( aChar, aLength );
    }

/**
Appends a zero terminator onto the end of this RStringBuf8's data.
This function dynamically reallocates memory to accommodate the zero terminator.

@return void
*/
void RStringBuf8::ZeroTerminateL() 
    {
    MakeSpaceL( 1 );
    RBuf8::ZeroTerminate();
    }

/**
Inserts aDes into this RStringBuf8 at position aPos.
This function dynamically reallocates memory to accommodate the new data.

@param aPos - the position to insert the data.
@param aDes - the data to insert.
@return void
*/
void RStringBuf8::InsertL(TInt aPos, const TDesC8& aDes) 
    {
    MakeSpaceL( aDes.Length() );
    RBuf8::Insert( aPos, aDes );
    }

/**
Replaces data in the RStringBuf8.
This function dynamically reallocates memory to accommodate the new data.

@param aPos - the position to start replacing data.
@param aLength - the length of data to replace.
@param aDes - the source data used to replace the data in the RStringBuf.
@return void
*/
void RStringBuf8::ReplaceL(TInt aPos, TInt aLength, const TDesC8& aDes) 
    {
    MakeSpaceL( aDes.Length() - aLength );
    RBuf8::Replace( aPos, aLength, aDes );
    }

/**
Formats and copies text into this RStringBuf8, replacing any existing data.

@param aFmt - descriptor containing the format string.
@param ... - the variable arguments to substitute into the format string.
@return void
*/
void RStringBuf8::FormatL(TRefByValue<const TDesC8> aFmt,...) 
    {
    VA_LIST list;
    VA_START(list,aFmt);
    Zero();
    // Special case - want user not to have to put on CleanupStack for 
    // simple FormatL calls:
    // Doesn't matter being Close'd twice, so:
    CleanupClosePushL();
    AppendFormatListL(aFmt,list);

    CleanupStack::Pop();
    }

/**
Adds to CleanupStack, then formats and copies text into this RStringBuf8.
@see FormatL()
*/
void RStringBuf8::FormatLC(TRefByValue<const TDesC8> aFmt,...) 
    {
    VA_LIST list;
    VA_START(list,aFmt);
    CleanupClosePushL();
    Zero();
    AppendFormatListL(aFmt,list);
    }

/**
Formats and appends text onto the end of this descriptor's data.

@param aFmt - descriptor containing the format string.
@param ... - the variable arguments to substitute into the format string.
@return void
*/
void RStringBuf8::AppendFormatL(TRefByValue<const TDesC8> aFmt,...) 
    {
    VA_LIST list;
    VA_START(list,aFmt);
    AppendFormatListL(aFmt,list);
    }

/**
*  Used by RStringBuf8
*/
struct TOverflowHandler8 : public TDes8Overflow
    {
    TBool iOverflowed;

    void Overflow(TDes8 &) { iOverflowed++; }
    };

/**
Formats and appends text onto the end of this descriptor's data.

@param aFmt - descriptor containing the format string.
@param aList - the variable arguments to substitute into the format string.
@return void
*/
void RStringBuf8::AppendFormatListL(const TDesC8 &aFmt,VA_LIST aList) 
    {
    const TInt KMinBufferSize = 16;
    const TInt KDefaultBufferSize = 256;
    TOverflowHandler8 handler;

    // Simple case (avoids unnecessary reallocs).
    if (MaxLength() <= KDefaultBufferSize && Length() < MaxLength() )
        {
        TBuf8<KDefaultBufferSize> defaultBuffer = *this; 
        handler.iOverflowed = 0;
        defaultBuffer.AppendFormatList( aFmt, aList, &handler );
        if (!handler.iOverflowed)
            {
            CopyL( defaultBuffer );
            return;
            }
        }

    TInt originalLength = Length();
    TInt bufferSize = Max( MaxLength(), KMinBufferSize );
    do { 
        RBuf8::SetLength( originalLength );
        MakeBigEnoughL( bufferSize );
        handler.iOverflowed = 0;
        RBuf8::AppendFormatList( aFmt, aList, &handler );
        bufferSize *= 2;
        } while (handler.iOverflowed);

    ReAllocL( Length() );
    }

/**
Answers the contents of the string as an HBufC8, popping itself from the CleanupStack.
Following completion this represents the null string.

@return An HBuf corresponding to this string.  
@warning Must be on the cleanup stack.  
*/
HBufC8* RStringBuf8::PopAndReturnAsHBufL()
    {
    HBufC8* result = AllocL();
    CleanupStack::PopAndDestroy(); // Revert to the null string.
    return result;
    }









