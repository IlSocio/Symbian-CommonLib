/*
 ============================================================================
 Name		: Contacts.cpp
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CContacts implementation
 ============================================================================
 */

#include "Contacts.h"
// #include <BaUtils.h>
// #include <FuzzyByte\FileUtils.h>
#include <CNTITEM.H>
#include <CNTFLDST.H>
#include <msvuids.h>
#include <cntfield.h>
 


CContacts::CContacts() : CBase()
	{
	// No implementation required
	}

CContacts::~CContacts()
	{
	delete iDb;
	__LOG_CLOSE
	}

CContacts* CContacts::NewLC()
	{
	CContacts* self = new (ELeave) CContacts();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CContacts* CContacts::NewL()
	{
	CContacts* self = CContacts::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CContacts::ConstructL()
	{
	__LOG_OPEN("CommonLib", "Contacts.txt")
	iDb = CContactDatabase::OpenL();
	iDb->SetDbViewContactType(KUidContactItem);
	
	// Sets sort-order
	typedef CContactDatabase::TSortPref SortPref;
	CArrayFixFlat<SortPref>* sortOrder = new (ELeave) CArrayFixFlat<SortPref>(2);
	CleanupStack::PushL(sortOrder);
	sortOrder->AppendL(SortPref(KUidContactFieldFamilyName, SortPref::EAsc));
	sortOrder->AppendL(SortPref(KUidContactFieldGivenName, SortPref::EAsc));
	iDb->SortL(sortOrder);
	CleanupStack::Pop(sortOrder);
	
	
	//const CContactIdArray* contacts = iDb->SortedItemsL();
	}


const CContactIdArray* CContacts::ItemsL()
	{
	return iDb->SortedItemsL();
	}


CContactItem* CContacts::GetContactLC(TContactItemId aContactId)
	{
	return iDb->ReadContactLC(aContactId);
	}


HBufC* CContacts::ResolveNumberL(const TDesC& number)
	{
	TContactItemId id = FindFirstContactMatchingNumberL(number);
	if (id == KNullContactId)
		return HBufC::NewL(0);

	CContactItem* item = GetContactLC(id);

	CContactItemFieldSet& fields = item->CardFields();
	__LOG_TXT("Check Emtpy Fields");
	if (HasEmptyFields(fields))
		{
		__LOG_TXT("*** SKIP: HAS EMPTY FIELDS ***");
		CleanupStack::PopAndDestroy(item); // item
		return HBufC::NewL(0);
		}
	TInt find_cogn = fields.Find(KUidContactFieldFamilyName);
	TInt find_nome = fields.Find(KUidContactFieldGivenName);		

	__LOG_TXT("GetFieldText Cogn");
	RBuf bufCogn( GetFieldTextL(fields, find_cogn) );	bufCogn.CleanupClosePushL();
	
	__LOG_TXT("GetFieldText Nome");
	RBuf bufNome( GetFieldTextL(fields, find_nome) );	bufNome.CleanupClosePushL();
	
	HBufC* res = HBufC::NewL( bufCogn.Length() + 2 + bufNome.Length() );
	res->Des().Append( bufNome );
	res->Des().Append( _L(" ") );
	res->Des().Append( bufCogn );
	
	CleanupStack::PopAndDestroy(&bufNome);
	CleanupStack::PopAndDestroy(&bufCogn);
	CleanupStack::PopAndDestroy(item);
	return res;
	}


TContactItemId CContacts::FindFirstContactMatchingNumberL(const TDesC& number)
	{
	TInt numberOfDigitsToMatch = 7;

// Notes: Due to the way numbers are stored in the database, it is recommended that at least 7 match digits are specified even when matching a number containing fewer digits. Failure to follow this guideline may (depending on the database contents) mean that the function will not return the expected Contacts Id set.			
//	if (number.Length() < 7)
//		numberOfDigitsToMatch = number.Length();
	
	TContactItemId res = KNullContactId;
	CContactIdArray* results;
	results = iDb->MatchPhoneNumberL(number, numberOfDigitsToMatch);
	CleanupStack::PushL( results );
	if (results->Count() > 0)
		{
		res = (*results)[0];
		}
	CleanupStack::PopAndDestroy( results );	
	return res;
	}


TBool CContacts::HasEmptyFields(CContactItemFieldSet& fields)
{
	for (TInt i=0; i<fields.Count(); i++)
	{
		CContactItemField& itemField = fields[i];
		if (itemField.StorageType() == KStorageTypeText)
		{
			CContactTextField* txtField = itemField.TextStorage();
			if (!txtField->IsFull())
				return ETrue;
		}
	}
	return EFalse;
}



HBufC* CContacts::GetFieldTextL(CContactItemFieldSet& fields, TInt field_id)
{
	__LOG("GetFieldText:%d", field_id);
	HBufC* ris = HBufC::NewLC(2);
	if (field_id == KErrNotFound)
		{
		CleanupStack::Pop(ris);
		return ris;
		}
	CContactItemField& itemField = fields[field_id];
	// ogni itemField ha StorageType, ContentType, Attributi e Label...
	CContactTextField* txtField = itemField.TextStorage();
	if (txtField == NULL)
	{
		__LOG_TXT("TEXT FIELD NULL !!!");
		CleanupStack::Pop(ris);
		return ris;
	}
	__LOG_TXT("Txt Field Not Null");

	if (!txtField->IsFull())
	{
		__LOG_TXT("TEXT FIELD EMPTY !!!");
		CleanupStack::Pop(ris);
		return ris;
	}
	__LOG_TXT("Txt Field Full");
	
	if (itemField.StorageType() != KStorageTypeText)
	{
		__LOG_TXT("NOT TEXT STORAGE!!!");
		CleanupStack::Pop(ris);
		return ris;
	}
	__LOG_TXT("Txt Field Storage");

//	__FLOG(itemField.Label());
//	__FLOG(txtField->Text());
//	__LOG("Deleted:%d"), itemField.IsDeleted());
//	__LOG("Hidden:%d"), itemField.IsHidden());
//	__LOG("Disabled:%d"), itemField.IsDisabled());
/*	__LOG("Id:%d"), itemField.Id());
	__LOG("OverridLab:%d"), itemField.OverRidesLabel());
	__LOG("HasExtraStrm:%d"), itemField.HasExtraStream());
	__LOG("UsesTemplType:%d"), itemField.UsesTemplateTypes());
	__LOG("UnspecLabel:%d"), itemField.LabelUnspecified());
	__LOG("IsFilerable:%d"), itemField.IsCustomFilterable());
	__LOG("UserFlags:%d"), itemField.UserFlags());
	__LOG("ReadOnly:%d"), itemField.IsReadOnly());
	__LOG("Template:%d"), itemField.IsTemplate());
	__LOG("TemplateField:%d"), itemField.IsTemplateLabelField());
	__LOG("UserAddedField:%d"), itemField.UserAddedField());*/

//	__LOG("StorageType:%d"), itemField.StorageType()); // KStorageTypeText, KStorageTypeStore, KStorageTypeContactItemId, KStorageTypeDateTime

/*	__LOG_TXT("ContentType:"));
	const CContentType& contType = itemField.ContentType();
	for (TInt i=0; i<contType.FieldTypeCount(); i++)
	{
		TFieldType type = contType.FieldType(i);
		TBuf<20> buf;
		buf.AppendNum(type.iUid, EHex);
		__FLOG(buf);
	}*/
	TPtrC txtPtr = txtField->Text();	
	if (txtPtr.Length() <= 0)
	{
		CleanupStack::Pop(ris);
		return ris;
	}
	CleanupStack::PopAndDestroy(ris);
	// Fare attenzione con l'uso di Realloc !!!
	ris = txtPtr.AllocL();
	ris->Des().Trim();
	return ris;
}
