// LIBRARY cntmodel.lib 

/*
 ============================================================================
 Name		: Contacts.h
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CContacts declaration
 ============================================================================
 */

#ifndef Contacts_H
#define Contacts_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <CNTDB.H> 
#include <FuzzyByte\FileLogger.h>

// CLASS DECLARATION

/**
 *  CContacts
 * 
 */
class CContacts : public CBase
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CContacts();

	/**
	 * Two-phased constructor.
	 */
	static CContacts* NewL();

	/**
	 * Two-phased constructor.
	 */
	static CContacts* NewLC();

	const CContactIdArray* ItemsL();
	
	// From Number to Name
	HBufC* ResolveNumberL(const TDesC& number);
	TContactItemId FindFirstContactMatchingNumberL(const TDesC& number);
	
	CContactItem* GetContactLC(TContactItemId aContactId);
	TBool HasEmptyFields(CContactItemFieldSet& fields);
	HBufC* GetFieldTextL(CContactItemFieldSet& fields, TInt field_id);
private:
	/**
	 * Constructor for performing 1st stage construction
	 */
	CContacts();

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL();

private:
	__LOG_DECLARATION_MEMBER
	CContactDatabase* iDb;
	};

#endif // Contacts_H
