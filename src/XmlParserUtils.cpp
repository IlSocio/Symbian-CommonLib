/*
============================================================================
Name        : XmlParserUtils.cpp
Author      : Andrea MAGNI
Version     :
Copyright   : BITWEEN S.R.L.
Description : CXmlParserUtils implementation
============================================================================
*/

#include <senelement.h>
#include <senxmlutils.h>
#include "XmlParserUtils.h"


// -----------------------------------------------------------------------------
// TInt XmlParserUtils::ContentInt(CSenElement& aElement, const TDesC8& aName)
// Retrieves content of the node with given name and convert to integer
// -----------------------------------------------------------------------------
//
TInt XmlParserUtils::ContentIntL(CSenElement& aElement, const TDesC8& aName)
	{
	TInt value = 0;
	HBufC* data = ContentStringL( aElement, aName );
	if ( data->Length() != 0 )
		{
		TLex lex( *data );
		lex.Val( value );
		}
	delete data;

	return value;
	}

// -----------------------------------------------------------------------------
// HBufC* XmlParserUtils::ContentString(CSenElement& aElement, const TDesC8& aName)
// Retrieves content of the node with given name
// -----------------------------------------------------------------------------
//
HBufC* XmlParserUtils::ContentStringL(CSenElement& aElement, const TDesC8& aName)
	{
	HBufC* data = NULL;
	CSenElement* element = aElement.Element( aName );
	if ( element != NULL )
		{
		data = ContentStringL( *element );
		}
	else
		{
		data = KNullDesC().AllocL();
		}

	return data;
	}

// -----------------------------------------------------------------------------
// HBufC* XmlParserUtils::ContentStringL(CSenElement& aElement)
// Retrieves content of the current node
// -----------------------------------------------------------------------------
//
HBufC* XmlParserUtils::ContentStringL(CSenElement& aElement)
	{
	HBufC8* decoded = SenXmlUtils::DecodeHttpCharactersLC( aElement.Content() );//+PUSH
	HBufC* content = SenXmlUtils::ToUnicodeLC( *decoded );						//++PUSH
	CleanupStack::Pop( content );												//--POP
	CleanupStack::PopAndDestroy( decoded );										//-POP

	return content;
	}

// -----------------------------------------------------------------------------
// void XmlParserUtils::SetIntAttributeL(CSenElement& aElement, 
//		const TDesC& aName, TInt aValue)
// Adds attribute to the given element
// -----------------------------------------------------------------------------
//
void XmlParserUtils::SetIntAttributeL(CSenElement& aElement, 
	const TDesC8& aName, TInt aValue)
	{
	TBuf<16> value;
	value.Num( aValue );
	SetStringAttributeL( aElement, aName, value );
	}

// -----------------------------------------------------------------------------
// TBool XmlParserUtils::IntAttributeL(CSenElement& aElement, const TDesC& aName,
//		TInt& aValue)
// Retrieves attribute value from the given element
// -----------------------------------------------------------------------------
//
TBool XmlParserUtils::IntAttributeL(CSenElement& aElement, const TDesC8& aName,
	TInt& aValue)
	{
	TBool valid = EFalse;
	HBufC* strValue = StringAttributeL( aElement, aName );
	if ( strValue->Length() != 0 )
		{
		TLex lex( *strValue );
		valid = lex.Val( aValue ) == KErrNone;
		}
	delete strValue;

	return valid;
	}

// -----------------------------------------------------------------------------
// void XmlParserUtils::SetStringAttributeL(CSenElement& aElement, 
//		const TDesC& aName, const TDesC& aValue)
// Adds attribute to the given element
// -----------------------------------------------------------------------------
//
void XmlParserUtils::SetStringAttributeL(CSenElement& aElement,
	const TDesC8& aName, const TDesC& aValue)
	{
	HBufC8* value = SenXmlUtils::ToUtf8LC( aValue );							//+PUSH
	aElement.AddAttrL( aName, *value );
	CleanupStack::PopAndDestroy( value );										//-POP
	}

// -----------------------------------------------------------------------------
// HBufC* XmlParserUtils::StringAttributeL(CSenElement& aElement, 
//		const TDesC& aName)
// Retrieves attribute value from the given element
// -----------------------------------------------------------------------------
//
HBufC* XmlParserUtils::StringAttributeL(CSenElement& aElement, 
	const TDesC8& aName)
	{
	HBufC* value = NULL;
	const TDesC8* valueUtf8 = aElement.AttrValue( aName );
	if ( valueUtf8 != NULL )
		{
		value = SenXmlUtils::ToUnicodeLC( *valueUtf8 );
		CleanupStack::Pop( value );
		}
	else
		{
		value = KNullDesC().Alloc();
		}

	return value;
	}

// -----------------------------------------------------------------------------
// void XmlParserUtils::SetDateAttributeL(CSenElement& aElement, 
//		const TDesC8& aName, const TTime& aValue)
// Adds attribute to the given element in format HH:MM:SS
// -----------------------------------------------------------------------------
//
void XmlParserUtils::SetDateAttributeL(CSenElement& aElement, 
	const TDesC8& aName, const TTime& aValue)
	{
	TDateTime dateTime = aValue.DateTime();
	TBuf<8> value;
	value.Format( _L("%02d:%02d:%02d"), dateTime.Hour(),
		dateTime.Minute(), dateTime.Second() );
	SetStringAttributeL( aElement, aName, value );
	}

// -----------------------------------------------------------------------------
// TBool XmlParserUtils::DateAttributeL(CSenElement& aElement, 
//		const TDesC8& aName, TTime& aValue)
// Retrieves attribute value from the given element
// -----------------------------------------------------------------------------
//
TBool XmlParserUtils::DateAttributeL(CSenElement& aElement, const TDesC8& aName,
	TTime& aValue)
	{
	TBool valid = EFalse;
	HBufC* value = StringAttributeL( aElement, aName );
	if ( value->Length() != 0 )
		{
		TInt retParse = aValue.Parse( *value );
		if ( retParse >= 0 )
			valid = ETrue;
		}
	delete value;

	return valid;
	}
// End file
