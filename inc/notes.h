/*
============================================================================
 Name        : Notes.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Notes declaration
============================================================================
*/

// library aknnotify.lib eiksrv.lib commonengine.lib 

#ifndef NOTES_H
#define NOTES_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>


class Notes
{
public:
	static void ShowLongTextL(RFs& aFs, const TDesC& aFileName, TInt aResId, TInt aResTitle);
	static void Debug_NotifyL(const TDesC& title, TRefByValue<const TDesC16> aFmt, ...);
	static void Debug_InfoL(TRefByValue<const TDesC16> aFmt, ...);
	static void InformationL(TInt aResFmtId, ...);
	static TInt QueryL(TInt aSoftKeyRes, TInt aResFmtId, ...);
	static void ConfirmationL(TInt aResFmtId, ...);
	static void ErrorL(TInt aResFmtId, ...);
	static void WarningL(TInt aResFmtId, ...);
};

#endif // NOTES_H
