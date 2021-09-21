
// LIBRARY ezip.lib ws32.lib charconv.lib euser.lib efsrv.lib bafl.lib apgrfx.lib msgs.lib estor.lib



#ifndef UTILS_H
#define UTILS_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		
#include <e32cmn.h>
#include <f32file.h>
#include <W32STD.H>


class Utils 
	{
public:
//	static HBufC* GetFullNameFromUid3L(const TDesC& aFileName, TUid uid);
	static TTime LocaleTime(TTime aTime);
	static TBool ExistsExe(RFs& fs, const TDesC& aExeName, TUid uid3);
	static TInt ExistsDll(RFs& fs, const TDesC& aDllName);
	static TUid GetForegroundAppUidL(RWsSession& wsSession);
	static TUint32 GetUid3FromSisL(RFs& fs, const TDesC& filename);
	static TInt ExtractZipL(RFs& fs, const TDesC& aCompressedFile, const TDesC& aDestFolder);
	
	/*
	 z:\system\install\Series60v3.0.sis     | S60 3rd Edition (FP0)
	 z:\system\install\Series60v3.1.sis     | S60 3rd Edition FP1
	 z:\system\install\Series60v3.2.sis     | S60 3rd Edition FP2
	 z:\system\install\Series60v5.0.sis     | S60 5th Edition
	 z:\system\install\Series60v5.1.sis     | Symbian^2
	 z:\system\install\Series60v5_2.sis		| Symbian^3
	 z:\system\install\Series60v5_3.sis		| Belle
	 */
	static void GetS60PlatformL( RFs& aFs, TUint& aMajor, TUint& aMinor );
	static TUint GetClientUID(const RMessagePtr2& aMsg);
	};

#endif
