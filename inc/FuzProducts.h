
#ifndef FUZPRODUCTS_H
#define FUZPRODUCTS_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>
#include <e32cmn.h>
#include <f32file.h>


enum TProductId
	{
	EGuardianPlatinum = 1001,
	EGuardianGold = 1002,
	EGuardianUpgradeGoldPlat = 1003,
	EGuardianUpgradeGoldIridium = 1004,
	EGuardianUpgradePlatIridium = 1005,
	EGuardianIridium = 1010,
	EIconHiderFull = 2001
	};


class FuzProducts 
	{
public:
	static TBool IsGoodLicenseID(const TDesC& parte1, const TDesC& parte2);
	static void BuySoftwareL(const TDesC& aUrl, TInt productId);
	};


#endif
