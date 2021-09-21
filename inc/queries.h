


#ifndef QUERIES_H
#define QUERIES_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>


class Queries
{
public:
	static TBool AskTextL(TInt aResID, TDes& text, TBool aPredectiveFlag=ETrue);
	static TBool OkCancelL(TInt resource_id, ...);
};

#endif // NOTES_H
