/*
 ============================================================================
 Name		: IapManager.h
 Author	  : Marco Bellino
 Version	 : 1.0
 Copyright   : Guardian-Mobile
 Description : CIapManager declaration
 ============================================================================
 */

#ifndef IAPMANAGER_H
#define IAPMANAGER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>

// CLASS DECLARATION
//class CMDBSession;
//class CCDIAPRecord;
#include <CommsDat.h>
#include <CommsDatTypesv1_1.h>
#include <CommsDatTypeInfoV1_1.h>
#include <COMMDBCONNPREF.H>

using namespace CommsDat;



/**
 *  CIapManager
 * 
 */
class IapManager
	{
public:
	static void GetDefaultConnectionPrefL(CMDBSession& aDatabase, TCommDbConnPref& connectPref);
	static TBool ExistsApnL(CMDBSession& db, const TDesC& apn);
	static TInt CreateNewIAPL(RFs& fs, CMDBSession& db, const TDesC& aAccountName,  const TDesC& aApn, const TDesC& aUser, const TDesC& aPass);

private:
	static TInt CreateNewProxyEntryL(CMDBSession& aDatabase, TInt aIapServiceId, const TDesC& aName);
	static TInt CreateNewWapBearerRecordL(CMDBSession& aDatabase, TInt aWapAP, TInt aIAP, const TDesC& aName);
	static TInt CreateNewWapAPRecordL(CMDBSession& aDatabase, const TDesC& aName);
	static TInt CreateNewNetworkRecordL(CMDBSession& aDatabase, CCDIAPRecord* aIapRecord, const TDesC& aName);
	static TInt CreateNewServiceRecordL(CMDBSession& aDatabase, CCDIAPRecord* aIapRecord, const TDesC& aName, const TDesC& apn, const TDesC& aUser, const TDesC& aPass);
	};

#endif // IAPMANAGER_H
