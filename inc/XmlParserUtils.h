/*
============================================================================
Name        : XmlParserUtils.h
Author      : Andrea Magni
Version     :
Copyright   : BITWEEN S.R.L.
Description : CXmlParserUtils declaration
============================================================================
*/

#ifndef XMLPARSERUTILS_H
#define XMLPARSERUTILS_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// FORWARD DECLARATION
class CSenElement;

// CLASS DECLARATION

/**
*  XmlParserUtils
* Utilities for retrieving xml attributes or content of different type
*/
class XmlParserUtils
	{
public:
	/**
	 * Retrieves content of the node with given name and convert to integer
	 * @param aElement Parent element
	 * @param aName The name of node
	 * @return Integer value of retrieved data.
	 *			Zero if element with given name not found or data are empty
	 */
	static TInt ContentIntL(CSenElement& aElement, const TDesC8& aName);

	/**
	* Retrieves content of the child node with given name
	* @param aElement Parent element
	* @param aName The name of node
	* @return Buffer with retrieved text.
	*/
	static HBufC* ContentStringL(CSenElement& aElement, const TDesC8& aName);

	/**
	* Retrieves content of the current node
	* @param aElement Parent element
	* @return Buffer with retrieved text.
	*/
	static HBufC* ContentStringL(CSenElement& aElement);

	/**
	 * Adds attribute to the given element
	 * @param aElement Xml element
	 * @param aName Attribute name
	 * @param aValue Integer value of the attribute
	 */
	static void SetIntAttributeL(CSenElement& aElement, const TDesC8& aName,
		TInt aValue);

	/**
	* Retrieves attribute value from the given element
	* @param aElement Xml element
	* @param aName Attribute name
	* @param aValue Return integer value of the attribute
	* @return ETrue if param is taken, EFalse otherwise
	*/
	static TBool IntAttributeL(CSenElement& aElement, const TDesC8& aName,
		TInt& aValue);

	/**
	* Adds attribute to the given element
	* @param aElement Xml element
	* @param aName Attribute name
	* @param aValue String value of the attribute
	*/
	static void SetStringAttributeL(CSenElement& aElement, const TDesC8& aName,
		const TDesC& aValue);

	/**
	* Retrieves attribute value from the given element
	* @param aElement Xml element
	* @param aName Attribute name
	* @return String value of the attribute or KNullDesC allocated descriptor
	*/
	static HBufC* StringAttributeL(CSenElement& aElement, const TDesC8& aName);

	/**
	* Adds attribute to the given element in format HH:MM:SS
	* @param aElement Xml element
	* @param aName Attribute name
	* @param aValue Date value of the attribute
	*/
	static void SetDateAttributeL(CSenElement& aElement, const TDesC8& aName,
		const TTime& aValue);

	/**
	* Retrieves attribute value from the given element
	* @param aElement Xml element
	* @param aName Attribute name
	* @param aValue Time value of the attribute
	* @return ETrue if param is taken, EFalse otherwise
	*/
	static TBool DateAttributeL(CSenElement& aElement, const TDesC8& aName,
		TTime& aValue);

	};

#endif // XMLPARSERUTILS_H
