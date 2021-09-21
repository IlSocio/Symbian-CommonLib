
// LIBRARY	CentralRepository.lib platformenv.lib

#ifndef REPOUTIL_H
#define REPOUTIL_H

#include <e32base.h>	
#include <e32std.h>		
#include <e32cmn.h>
#include <f32file.h>


class RepoUtils
	{
public:
//	static TInt GetPicturesPathL(RFs& fs, TDes& aPath);
	static TInt SetL(TInt uidVal, TInt key, const TInt& value);
	static TInt SetL(TInt uidVal, TInt key, const TDesC8& value);
	static TInt SetL(TInt uidVal, TInt key, const TDesC16& value);
	static TInt GetL(TInt uidVal, TInt key, TDes16& value);
	static TInt GetL(TInt uidVal, TInt key, TInt& value);
	};

#endif
