
#include "RepoUtils.h"
#include "FileUtils.h"

#include <centralrepository.h>
#include <bautils.H>
//#include <dclcrkeys.h>
//#include <dclinternalcrkeys.h>




TInt RepoUtils::SetL(TInt uidVal, TInt key, const TInt& value)
	{
	TUid uid = TUid::Uid( uidVal );
	CRepository* cenRep = NULL;
	TRAPD( err, cenRep = CRepository::NewL(uid) );
	if (err != KErrNone)
		{
		return err;
		}
	CleanupStack::PushL(cenRep);
	TInt res = cenRep->Set(key, value);
	CleanupStack::PopAndDestroy( cenRep );
	return res;
	}

TInt RepoUtils::SetL(TInt uidVal, TInt key, const TDesC16& value)
	{
	TUid uid = TUid::Uid( uidVal );
	CRepository* cenRep = NULL;
	TRAPD( err, cenRep = CRepository::NewL(uid) );
	if (err != KErrNone)
		{
		return err;
		}
	CleanupStack::PushL(cenRep);
	TInt res = cenRep->Set(key, value);
	CleanupStack::PopAndDestroy( cenRep );
	return res;
	}


TInt RepoUtils::SetL(TInt uidVal, TInt key, const TDesC8& value)
	{
	TUid uid = TUid::Uid( uidVal );
	CRepository* cenRep = NULL;
	TRAPD( err, cenRep = CRepository::NewL(uid) );
	if (err != KErrNone)
		{
		return err;
		}
	CleanupStack::PushL(cenRep);
	TInt res = cenRep->Set(key, value);
	CleanupStack::PopAndDestroy( cenRep );
	return res;
	}


TInt RepoUtils::GetL(TInt uidVal, TInt key, TDes16& value)
	{
	TUid uid = TUid::Uid( uidVal );
	CRepository* cenRep = NULL;
	TRAPD( err, cenRep = CRepository::NewL(uid) );
	if (err != KErrNone)
		{
		return err;
		}
	CleanupStack::PushL(cenRep);
	TInt res = cenRep->Get(key, value);
	CleanupStack::PopAndDestroy( cenRep );
	return res;
	}

TInt RepoUtils::GetL(TInt uidVal, TInt key, TInt& value)
	{
	TUid uid = TUid::Uid( uidVal );
	CRepository* cenRep = NULL;
	TRAPD( err, cenRep = CRepository::NewL(uid) );
	if (err != KErrNone)
		{
		return err;
		}
	CleanupStack::PushL(cenRep);
	TInt res = cenRep->Get(key, value);
	CleanupStack::PopAndDestroy( cenRep );
	return res;
	}

