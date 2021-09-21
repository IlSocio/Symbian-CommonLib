

// LIBRARY etel3rdparty.lib

#ifndef PHONE_H
#define PHONE_H

#include <e32base.h>
#include <e32std.h>
#include <etel3rdparty.h>
#include <MSVAPI.H>
//#include "mmlist.h" // Piccolo hack per prelievo SCA...  (Non e' presente nell'sdk standard)
#include <FuzzyByte\FileLogger.h>
#include <MTCLREG.H>
#include <smsclnt.h>
#include <smut.h>
#include <smutset.h>
#include <smutsimparam.h>



enum TPhoneFunctions
	{
		ENoneFunc=0,
		EImei,
		EImsi,
		ECellID,
		ECharger,
		EBatteryInfo,
		ENetStatus,
		ENotifyNetworkStatusChange,
		ENetName
	};


enum TChargerStatus
	{
		EUnknown=0,
		EConnected,
		EDisconnected
	};

class MPhoneObserver
	{
	public:
		virtual void HandlePhoneEventL(TPhoneFunctions event)=0;
	};


class CPhone : public CActive
	{
	public:
		static CPhone* NewL();
		~CPhone();

		void SetObserver(MPhoneObserver* Observer);

	public: // New functions
		TChargerStatus IsChargerConnected();
		CTelephony::TBatteryStatus GetBatteryInfo();
		void NotifyNetworkStatusChange(TDes8& pckgNet);
		void GetNetworkStatus(CTelephony::TRegistrationStatus& regStatus);
		void GetImei(TDes& aImei);
		void GetImsi(TDes& aImsi);
		// Cell - LAC - MCC
		void GetCellID(TUint& aCellId, TUint& aLocationAreaCode, TDes& aNetworkId, TDes& aCountryCode, TDes& aOperName);
		void GetNetworkName(TDes& aNetworkName);
//		void GetSCA(TDes& aSCA);
//		void GetSCA_Beta1(TDes& aSCA);

	protected:
		void ConstructL();

	private: // From CActive
		CPhone();
		void RunL();
		void DoCancel();
		TInt RunError(TInt /*aError*/);

	private:
		void StartWait( );

	public:
		TBool iAbort;

	private:
		MPhoneObserver* iObserver;
		TPhoneFunctions iFunc;
		CTelephony::TBatteryInfoV1      iBatteryInfoV1;		// Battery
		CTelephony::TIndicatorV1 		iIndicatorV1;		// Charger
		CTelephony::TNetworkRegistrationV1 iNetStatus;
		CTelephony::TNetworkInfoV1 iNetInfo;	// CELL-ID
		CTelephony::TSubscriberIdV1 iSubId;		// IMSI
		CTelephony::TPhoneIdV1 iPhoneId;		// IMEI
		CTelephony::TNetworkNameV1 iNetwork;	// NETWORK NAME
		CTelephony* iTelephony;
		CActiveSchedulerWait* iWait;
		__LOG_DECLARATION_MEMBER;

/*		CSmsSimParamOperation* iOp;
		CMsvSession* iMsvSession;
		CClientMtmRegistry* iMtmReg;
		CSmsClientMtm* iSmsMtm;*/
	};

#endif

