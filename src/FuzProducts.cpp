
#include "FuzProducts.h"

#include <hal.h>
#include <hal_data.h>
#include <browserlauncher.h>
#include <browseroverriddensettings.h>
#include "phone.h"


#define KIMEI_LEN 20
typedef TBuf<KIMEI_LEN> TImei;

	
LOCAL_C TInt SumData(const TDesC& data)
{
	TInt ris=0;
	for (TInt i=0; i<data.Length(); i++)
	{
		TInt val = (TInt) data[i];
		if (val == '0')
			val = 'O';
		ris = ris + val;
		ris &= 0xFFFFFF;
	}
	return ris;
}


TBool FuzProducts::IsGoodLicenseID(const TDesC& parte1, const TDesC& parte2)
{
	TBuf<16> licenseID;
	licenseID.Append(parte1);
	licenseID.Append(parte2);
	licenseID.TrimAll();
	if (licenseID.Length() < 16)
		return EFalse;

	TBuf<16> licenseIDok;
	licenseIDok = licenseID.Left(14);
	TInt crc1 = SumData(parte1) % 35;
	TInt crc2 = SumData(licenseIDok) % 35;
	// 26 lettere alfab + 9 numeri => range di 35 valori
	if (crc1 < 9)
	{	// Numero 1-9
		licenseIDok.Append(TChar(1 + 48 + crc1)); // 49 + 0  => 49  '1'
	} else
	{	// Lettera A-Z
		licenseIDok.Append(TChar(1 + 55 + crc1)); // 86 + 10 => 96  'A'
	}
	if (crc2 < 9)
	{	// Numero 1-9
		licenseIDok.Append(TChar(1 + 48 + crc2)); // 49 + 0  => 49  '1'
	} else
	{	// Lettera A-Z
		licenseIDok.Append(TChar(1 + 55 + crc2)); // 86 + 10 => 96  'A'
	}
	return (licenseID == licenseIDok);
}


void FuzProducts::BuySoftwareL(const TDesC& aUrl, TInt productId)
{	
#ifndef __WINSCW__
	TBuf<300> url = aUrl;
	// Va direttamente su PayPal ??? => NO MEGLIO PASSARE PER IL MIO SERVER PRIMA... Cosi' posso passare ad altro sistema di pagamento senza problemi...
	CBrowserLauncher* browser;
	browser = CBrowserLauncher::NewLC();
	// GetImei
	TImei imei;
	CPhone* phone = CPhone::NewL();
	CleanupStack::PushL(phone);
	phone->GetImei(imei);
	CleanupStack::PopAndDestroy(phone);

	// SYMBIAN BUG!!!!
	// SONO COSTRETTO A RILEVARE MACHINEUID QUI DENTRO PERCHE' ALTRIMENTI LA SELEZIONE DEGLI IAP NON FUNZIONA A DOVERE
	// MAGARI PROVA A VEDERE COME SI COMPORTA USANDO AsyncWaiter fuori da CPhone
	TInt machineId;
	HAL::Get(HALData::EMachineUid, machineId);
	 
	TLanguage lang = User::Language();
	url.Append(_L("imei="));
	url.Append(imei);
	url.Append(_L("&m_lang="));
	url.AppendNum(lang);
	url.Append(_L("&mac="));
	url.AppendNum(machineId, EHex);
	url.Append(_L("&type="));
	url.AppendNum(productId);	// 1 = PLATINUM		0 = GOLD
//	url = _L("file://prova.bin");
//	url = _L("http://localhost:1105/GuardianMobile/prova.txt");
//	url = _L("http://www.Guardian-Mobile.com/prova.txt");
    //Overridden user preferences.
   TBrowserOverriddenSettings settings;
//   settings.SetBrowserSetting(EBrowserOverSettingsAutoLoadImages, 0);
   settings.SetBrowserSetting(EBrowserOverSettingsSmallScreen, 0);
   
	TInt ris=0;
	TRAP_IGNORE( ris = browser->LaunchBrowserSyncEmbeddedL(url, NULL, &settings));
	CleanupStack::PopAndDestroy(browser);
#endif
}

