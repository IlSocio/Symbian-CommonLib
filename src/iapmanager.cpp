/*
 ============================================================================
 Name		: IapManager.cpp
 Author	  : Marco Bellino
 Version	 : 1.0
 Copyright   : Guardian-Mobile
 Description : CIapManager implementation
 ============================================================================
 */

#include "IapManager.h"
#include <FuzzyByte\iapmanager.rsg>
#include <FuzzyByte\FileLogger.h>
#include <bautils.h>

// TODO: Converti per usare CMDB...ecc...
void IapManager::GetDefaultConnectionPrefL(CMDBSession& aDatabase, TCommDbConnPref& connectPref)
{
	CMDBRecordSet<CCDConnectionPrefsRecord>* recordSet = new (ELeave) CMDBRecordSet<CCDConnectionPrefsRecord>(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(recordSet);
	recordSet->LoadL(aDatabase);
	const TInt count = recordSet->iRecords.Count();
		
/*	connectPref->iDirection = ECommDbConnectionDirectionOutgoing;
	connectPref->iRanking = 1;
    if(!connPrefRecord->FindL(*dbSession))
        {
        User::Leave(KErrNotFound);        
        }            
    iapID = connPrefRecord->iDefaultIAP;
    iap.iIAP = iapID;
    iap.iDialogPref = ECommDbDialogPrefDoNotPrompt;*/
    
	for(TInt i = 0; i < count; i++)
		{
		CCDConnectionPrefsRecord* prefRecord = (*recordSet)[i];
		// TODO: Check prefRecord->iDirection
		// TODO: Check prefRecord->iBearerSet
		TMDBElementId elemId = prefRecord->iDefaultIAP.iLinkedRecord->ElementId();		
		}
	
/*    // open the IAP communications database
	CCommsDatabase* commDB = CCommsDatabase::NewL(EDatabaseTypeIAP);
	CleanupStack::PushL(commDB);

	// initialize a view
	CCommsDbConnectionPrefTableView* commDBView =
	commDB->OpenConnectionPrefTableInRankOrderLC(ECommDbConnectionDirectionUnknown);

	// go to the first record
	User::LeaveIfError(commDBView->GotoFirstRecord());

	// Declare a prefTableView Object.
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref;

	// read the connection preferences
	commDBView->ReadConnectionPreferenceL(pref);
	TUint32 iapID = pref.iBearer.iIapId;

	// pop and destroy the IAP View
	CleanupStack::PopAndDestroy(commDBView); // commDBView

	// pop and destroy the database object
	CleanupStack::PopAndDestroy(commDB); // commDB

	// Now we have the iap Id. Use it to connect for the connection.
	// Create a connection preference variable.
	//TCommDbConnPref connectPref;

	// setup preferences
	connectPref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	connectPref.SetDirection(ECommDbConnectionDirectionUnknown);
	connectPref.SetBearerSet(ECommDbBearerGPRS);
	//Sets the CommDb ID of the IAP to use for this connection
	connectPref.SetIapId(iapID);*/
}

LOCAL_C TInt GetBearerIndexL(CMDBSession& aDatabase) 
	{
	TInt index = -1;
	  
	CMDBRecordSet<CCDModemBearerRecord>* recordSet = new (ELeave) CMDBRecordSet<CCDModemBearerRecord>(KCDTIdModemBearerRecord);
	CleanupStack::PushL(recordSet);
	recordSet->LoadL(aDatabase);
	const TInt count = recordSet->iRecords.Count();
		
	for(TInt i = 0; i < count; i++)
		{
		CCDModemBearerRecord* bearerRecord = (*recordSet)[i];
		
		//const TMDBElementId KCDTIdBearerTechnology  	= 0x007B0000;
	    //const TMDBElementId KCDTIdBearerAgent		    = 0x007C0000;
		// KCDMaskShowRecordType	= 0x7f800000
		//TUint32 elemId = bearerRecord->ElementId();			//0x087f0200			0x087f0200		0x087f0300
		//TPtrC tsy(bearerRecord->iTsyName);					//phonetsy				phonetsy		phonetsy
		//TPtrC csy(bearerRecord->iCsyName);					//dataport				ecuart			ecuart
		TPtrC agent(bearerRecord->iBearerAgent);			//gprsumtsagent.agt		nullagt.agt		csd.agt
		
		TBuf<30> buff = agent;
		buff.LowerCase();
		TInt pos = buff.FindC(_L("gprs"));
		TInt pos2 = buff.FindC(_L("null"));
		if (pos >= 0 || pos2 >= 0)
			{
			index = bearerRecord->RecordId();
			}
		}
	
	CleanupStack::PopAndDestroy(recordSet);
	if (index < 0)
		{
		User::Leave(KErrNotFound);
		}
	return index;	
	}


TBool IapManager::ExistsApnL(CMDBSession& db, const TDesC& apn)
	{
	CMDBRecordSet<CCDOutgoingGprsRecord>* table = new (ELeave) CMDBRecordSet<CCDOutgoingGprsRecord>(KCDTIdOutgoingGprsRecord);
	CleanupStack::PushL(table);
	
	TRAPD(err, table->LoadL(db));	// se da Errore la Tabella non esiste...
	if (err == KErrNone)
		{
		const TInt totEntries = table->iRecords.Count();
		for (TInt i=0; i<totEntries; i++)
			{
			CCDOutgoingGprsRecord* record = (*table)[i];
			TPtrC gprsApn = TPtrC( record->iGPRSAPN );
			if (gprsApn.MatchC(apn) == 0)
				return ETrue;
			}
		}
	CleanupStack::PopAndDestroy(table);
	return EFalse;
	}


TInt IapManager::CreateNewServiceRecordL(CMDBSession& aDatabase, CCDIAPRecord* aIapRecord, const TDesC& aName, const TDesC& aApn, const TDesC& aUser, const TDesC& aPass)
	{
	TBool userPwdRequired = (aUser.Length() > 0 || aPass.Length() >0);
//	_LIT( authName, "");						// ***
//	_LIT( authPass, "");						// ***
//	_LIT( accessPointName, "internet.wind" );	// ***
	TBool ipAddrFromServer = ETrue;				// ***
	TBool ipDnsAddrFromServer = ETrue;			// ***
	_LIT( ipNameServer1, "0.0.0.0" );			// ***
	_LIT( ipNameServer2, "0.0.0.0" );			// ***
	_LIT( ipAddr, "0.0.0.0" );					// ***
	TBool disablePlainTextAuth = EFalse;		// ***
	TBool enableLCPExtensions = EFalse;			// ***
	
	CCDOutgoingGprsRecord* serviceRecord = static_cast<CCDOutgoingGprsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdOutgoingGprsRecord));
	serviceRecord->SetRecordId(KCDNewRecordRequest);
	aIapRecord->iService.iLinkedRecord = serviceRecord;

	TBool prompt = EFalse;
	if ( userPwdRequired )
		{
		prompt = aUser.Length()==0 || aPass.Length()==0;
		}

	serviceRecord->iGPRSPDPType = 0; // IP4
	serviceRecord->iGPRSPDPAddress = ipAddr;
	
	serviceRecord->iGPRSIfAuthName.SetL(aUser);
	
	serviceRecord->iGPRSIfAuthPass.SetL(aPass);
	
	serviceRecord->iGPRSAPN.SetL(aApn);	// accessPointName

	serviceRecord->iGPRSIfPromptForAuth	= prompt;

	// IP-Address
	serviceRecord->iGPRSIPAddrFromServer = ipAddrFromServer;
	
	serviceRecord->iGPRSIPAddr.SetL(ipAddr);
	
	// DNS-Addresses (primary and secondary)
	serviceRecord->iGPRSIPDNSAddrFromServer = ipDnsAddrFromServer;	
	
	serviceRecord->iGPRSIPNameServer1.SetL(ipNameServer1);
	serviceRecord->iGPRSIPNameServer2.SetL(ipNameServer2);
	
	serviceRecord->iGPRSDisablePlainTextAuth	= disablePlainTextAuth;
	serviceRecord->iGPRSEnableLCPExtension		= enableLCPExtensions;
	serviceRecord->iRecordName.SetL( aName );
	serviceRecord->StoreL(aDatabase);
	// serviceRecord = NULL; // The IAPRecord owns this.
	return serviceRecord->RecordId();
	}


TInt IapManager::CreateNewNetworkRecordL(CMDBSession& aDatabase, CCDIAPRecord* aIapRecord, const TDesC& aName)
	{
	CCDNetworkRecord* networkRecord = static_cast<CCDNetworkRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdNetworkRecord));
	networkRecord->SetRecordId(KCDNewRecordRequest);
	aIapRecord->iNetwork.iLinkedRecord = networkRecord;
	
	// New Network
//	networkRecord->iRecordName.SetMaxLengthL( aName.Length() );
	networkRecord->iRecordName.SetL( aName );
//	networkRecord->iHostName = 
	networkRecord->StoreL( aDatabase);
	return networkRecord->RecordId();
	}



TInt IapManager::CreateNewWapAPRecordL(CMDBSession& aDatabase, const TDesC& aName)
	{
	_LIT( startPage, "http://");	// ***
	CCDWAPAccessPointRecord* apRecord = static_cast<CCDWAPAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWAPAccessPointRecord));
	CleanupStack::PushL(apRecord);
	apRecord->SetRecordId(KCDNewRecordRequest);
	
	// New Network
//	apRecord->iRecordName.SetMaxLengthL( aName.Length() );
	apRecord->iRecordName.SetL( aName );
	apRecord->iWAPCurrentBearer = TPtrC( KCDTypeNameWAPIPBearer );
	apRecord->iWAPStartPage.SetL( startPage );
	apRecord->StoreL(aDatabase);
	TInt id = apRecord->RecordId();
	CleanupStack::PopAndDestroy(apRecord);
	return id;
	}


TInt IapManager::CreateNewWapBearerRecordL(CMDBSession& aDatabase, TInt aWapAP, TInt aIAP, const TDesC& aName)
	{
	_LIT( gatewayIP, "0.0.0.0");	// ***
	_LIT( login, "");				// ***
	_LIT( pass, "");				// ***
	TInt port = 0;					// ***
	TBool security = EFalse;		// ***
	
	CCDWAPIPBearerRecord* wapIpRecord = static_cast<CCDWAPIPBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWAPIPBearerRecord));
	CleanupStack::PushL(wapIpRecord);
	wapIpRecord->SetRecordId(KCDNewRecordRequest);
	
	wapIpRecord->iWAPAccessPointId = aWapAP;
	wapIpRecord->iWAPIAP = aIAP;
	wapIpRecord->iRecordName.SetL( aName );
	wapIpRecord->iWAPGatewayAddress.SetL( gatewayIP );
	wapIpRecord->iWAPProxyLoginName.SetL( login );
	wapIpRecord->iWAPProxyLoginPass.SetL( pass );
	wapIpRecord->iWAPProxyPort = port;
	//	wapIpRecord->iWAPWSPOption = 0;
	wapIpRecord->iWAPSecurity = security;
	wapIpRecord->StoreL(aDatabase);
	TInt id = wapIpRecord->RecordId();
	CleanupStack::PopAndDestroy(wapIpRecord);
	return id;
	}


#define KSERVICE_TABLE TPtrC(KCDTypeNameOutgoingWCDMA)
#define KBEARER_TABLE  TPtrC(KCDTypeNameModemBearer)



TInt IapManager::CreateNewProxyEntryL(CMDBSession& aDatabase, TInt aIapServiceId, const TDesC& aName)
	{
	TPtrC tmpServ = KSERVICE_TABLE;
	// TInt servLen = tmpServ.Length();
	TBool proxyUseProxyServer = EFalse;	// ***
	TUint32 proxyPort = 0;				// ***
	_LIT( proxyServerName, "");			// ***
	_LIT( proxyProtocol, "");			// ***
	_LIT( proxyExceptions, "");			// ***
	
	CCDProxiesRecord* proxyRecord = static_cast<CCDProxiesRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdProxiesRecord));
	CleanupStack::PushL(proxyRecord);
	proxyRecord->iService = aIapServiceId;
	proxyRecord->iServiceType.SetL( KSERVICE_TABLE );

	// Set the name to the same as for the IAP, i.e. the account name
	proxyRecord->iRecordName.SetL( aName );

	proxyRecord->iUseProxyServer = proxyUseProxyServer;
	proxyRecord->iPortNumber = proxyPort;
	
	proxyRecord->iServerName.SetL( proxyServerName );

	proxyRecord->iProtocolName.SetL( proxyProtocol );

	proxyRecord->iExceptions.SetL( proxyExceptions );

	proxyRecord->SetRecordId(KCDNewRecordRequest);
	proxyRecord->StoreL(aDatabase);
	
	TInt id = proxyRecord->RecordId();
	CleanupStack::PopAndDestroy(proxyRecord);
	return id;
	}



// Se il file non e' presente esce...
// 222_50.rsc
// 222_88.rsc
TInt IapManager::CreateNewIAPL(RFs& fs, CMDBSession& db, const TDesC& aAccountName, const TDesC& aApn, const TDesC& aUser, const TDesC& aPass)	
	{
/*__FLOG_DECLARATION_MEMBER;
__FLOG_OPEN("CommonLib", "GUI_IapManager.txt");
__FLOG(_T8("-----------------"));
__FLOG(_T8("ResReader"));
__FLOG(aResourceFile);
__FLOG_CLOSE;

	// READ APN
	CResReader* resReader = CResReader::NewLC(fs, aResourceFile);
	HBufC* tmpApn = resReader->ReadLC(R_APN);
	HBufC* tmpUser = resReader->ReadLC(R_USER);
	HBufC* tmpPass = resReader->ReadLC(R_PASS);
	TBuf<30> apn = tmpApn->Des(); 
	TBuf<20> user = tmpUser->Des(); 
	TBuf<20> pass = tmpPass->Des(); 
	CleanupStack::PopAndDestroy(tmpPass);
	CleanupStack::PopAndDestroy(tmpUser);
	CleanupStack::PopAndDestroy(tmpApn);
	CleanupStack::PopAndDestroy(resReader);*/
	
	TInt err = KErrNone;
	if (ExistsApnL(db, aApn))
		{
		User::Leave(KErrAlreadyExists);
		}
	
	CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(iapRecord);
	db.OpenTransactionL();

	// New IAP
	iapRecord->SetRecordId(KCDNewRecordRequest);
	
	TInt iapNetworkId = 0;	// 11
	TRAP( err, iapNetworkId = CreateNewNetworkRecordL(db, iapRecord, aAccountName) );
	if(err!=KErrNone)
		{
		db.RollbackTransactionL();		
		User::LeaveIfError(err);
		}
	
	TInt iapServiceId = 0;	// 7
	TRAP( err, iapServiceId = CreateNewServiceRecordL(db, iapRecord, aAccountName, aApn, aUser, aPass) );
	if(err!=KErrNone)
		{
		db.RollbackTransactionL();		
		User::LeaveIfError(err);
		}

	//
	// Set the IAP information
	//
	iapRecord->iRecordName.SetL( aAccountName );
	iapRecord->iService = iapServiceId;
	iapRecord->iServiceType.SetL( KSERVICE_TABLE );
	iapRecord->iNetwork = iapNetworkId;
	iapRecord->iNetworkWeighting = 0;	// Network concept not used set to 0 for now
	iapRecord->SetAttributes( 0 );
	iapRecord->iBearer = GetBearerIndexL(db);
	iapRecord->iBearerType.SetL( KBEARER_TABLE );
	iapRecord->iLocation = 2;			// Location concept not used set to 2 for now
	TRAP(err, iapRecord->StoreL(db));
	if(err!=KErrNone)
		{
		db.RollbackTransactionL();		
		User::LeaveIfError(err);
		}
	
	TInt newIapId = iapRecord->RecordId();	// 11
	
	TInt proxyId = 0;
	TRAP(err, proxyId = CreateNewProxyEntryL(db, newIapId, aAccountName));
	if(err!=KErrNone)
		{
		db.RollbackTransactionL();
		User::LeaveIfError(err);
		}

	/*
---------------------------------------------
	[WAPAccessPoint]
	ADD_SECTION
	# COMMDB_ID = 2
		Name=prova_access_
		CurrentBearer=WAPIPBearer
		StartPage=http://
		FIELD_COUNT=2
	END_ADD 
---------------------------------------------
	*/
	TInt wapApRec = CreateNewWapAPRecordL(db, aAccountName);
	
	/*
---------------------------------------------
	[WAPIPBearer]
	ADD_SECTION
	# COMMDB_ID = 2
		AccessPointId=2
		GatewayAddress=0.0.0.0
		IAP=3
		WSPOption=CONNECTIONORIENTED
		Security=FALSE
		ProxyPortNumber=0
		FIELD_COUNT=6
	END_ADD
---------------------------------------------
	 */
	TInt wapBearRec = CreateNewWapBearerRecordL(db, wapApRec, newIapId, aAccountName);
	
	db.CommitTransactionL();
	
	CleanupStack::PopAndDestroy(iapRecord);
	return newIapId;
	}
	
