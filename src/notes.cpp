/*
============================================================================
 Name        : Notes.cpp
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Notes implementation
============================================================================
*/

#include "Notes.h"
#include "stringutils.h"
#include "rstringbuf.h"
#include "FileUtils.h"

#include <bautils.H>
#include <EIKENV.H>
#include <aknnotewrappers.h>
#include <aknglobalconfirmationquery.h> 
#include <aknmessagequerydialog.h>
#include <stringloader.h>


void Notes::ShowLongTextL(RFs& aFs, const TDesC& aFileName, TInt aResId, TInt aResTitle)
	{
	HBufC* title = StringUtils::LoadFromResourceLC(aResTitle);
	TFileName fullFileName = aFileName;
	FileUtils::CompleteWithPrivatePathL(aFs, fullFileName);

#ifdef __WINSCW__
	if (BaflUtils::FileExists(aFs, fullFileName))
#endif		
		{
		RBuf text(FileUtils::ReadTextFileL(aFs, fullFileName));
		text.CleanupClosePushL();
		CAknMessageQueryDialog *dialog = CAknMessageQueryDialog::NewL(text);
		dialog->PrepareLC(aResId);
		dialog->SetHeaderTextL(*title);
		dialog->RunLD();
		CleanupStack::PopAndDestroy(&text);
		}
	CleanupStack::PopAndDestroy(title);
	}


void Notes::Debug_NotifyL(const TDesC& title, TRefByValue<const TDesC16> aFmt, ...)
{
	VA_LIST list;
	VA_START(list, aFmt);
	RStringBuf buffer;
	buffer.CleanupClosePushL();
	buffer.AppendFormatListL(aFmt, list);
	HBufC* hbuf = buffer.PopAndReturnAsHBufL();
	CleanupStack::PushL(hbuf);
	TPtrC ptr = hbuf->Des();

	RNotifier notif;
	User::LeaveIfError( notif.Connect() );
	CleanupClosePushL(notif);

	TInt btn;
    TRequestStatus theStat = KRequestPending;
	notif.Notify(title, ptr, _L("Ok"), _L(""), btn, theStat);
    User::WaitForRequest(theStat);
    
	CleanupStack::PopAndDestroy(&notif);
	CleanupStack::PopAndDestroy(hbuf);
}

void Notes::Debug_InfoL(TRefByValue<const TDesC16> aFmt, ...)
{
	VA_LIST list;
	VA_START(list, aFmt);
	RStringBuf buffer;
	buffer.CleanupClosePushL();
	buffer.AppendFormatListL(aFmt, list);
	HBufC* hbuf = buffer.PopAndReturnAsHBufL();
	CleanupStack::PushL(hbuf);
	TPtrC ptr = hbuf->Des();

    CAknGlobalConfirmationQuery* globalQ = CAknGlobalConfirmationQuery::NewL();
    CleanupStack::PushL(globalQ);
 
    TRequestStatus theStat = KRequestPending;
    globalQ->ShowConfirmationQueryL(theStat, ptr, R_AVKON_SOFTKEYS_OK_EMPTY, R_QGN_NOTE_INFO_ANIM);
//	globalQ->ShowConfirmationQueryL(theStat, buffer, R_AVKON_SOFTKEYS_OK_EMPTY, R_QGN_NOTE_INFO_ANIM);
    User::WaitForRequest(theStat);
	   
    CleanupStack::PopAndDestroy(globalQ);   

	CleanupStack::PopAndDestroy(hbuf);
}


TInt Notes::QueryL(TInt aSoftKeyRes, TInt resource_id, ...)
{
	VA_LIST list;
	VA_START(list, resource_id);
	HBufC* buffer = StringUtils::LoadFromResourceLC(resource_id, list);
	
//	HBufC* buffer = CEikonEnv::Static()->AllocReadResourceLC( resource_id );
	TPtrC ptr = buffer->Des();
    CAknGlobalConfirmationQuery* globalQ = CAknGlobalConfirmationQuery::NewL();
    CleanupStack::PushL(globalQ);
 
    TRequestStatus theStat = KRequestPending;
	globalQ->ShowConfirmationQueryL(theStat, ptr, aSoftKeyRes, R_QGN_NOTE_QUERY_ANIM);
    User::WaitForRequest(theStat);
	   
    CleanupStack::PopAndDestroy(globalQ);

    CleanupStack::PopAndDestroy( buffer );
    return theStat.Int();
}

void Notes::InformationL(TInt resource_id, ...)
{
	VA_LIST list;
	VA_START(list, resource_id);
	HBufC* buffer = StringUtils::LoadFromResourceLC(resource_id, list);
	
//	HBufC* buffer = CEikonEnv::Static()->AllocReadResourceLC( resource_id );
	TPtrC ptr = buffer->Des();
    CAknGlobalConfirmationQuery* globalQ = CAknGlobalConfirmationQuery::NewL();
    CleanupStack::PushL(globalQ);
 
    TRequestStatus theStat = KRequestPending;
	globalQ->ShowConfirmationQueryL(theStat, ptr, R_AVKON_SOFTKEYS_OK_EMPTY, R_QGN_NOTE_INFO_ANIM);
    User::WaitForRequest(theStat);
	   
    CleanupStack::PopAndDestroy(globalQ);

    CleanupStack::PopAndDestroy( buffer );
// 1	
	/*
	CAknInformationNote* dialog = new ( ELeave ) CAknInformationNote(ETrue);
	dialog->ExecuteLD( buff->Des() );
	CleanupStack::PopAndDestroy(buff);
	*/
	/*
	CAknConfirmationNote* dialog = new ( ELeave ) CAknConfirmationNote(ETrue); 
	dialog->ExecuteLD( buff->Des() );
	CleanupStack::PopAndDestroy(buff);
	*/
	/*
	CAknErrorNote* dialog = new ( ELeave ) CAknErrorNote(ETrue);
	dialog->ExecuteLD( buff->Des() );
	CleanupStack::PopAndDestroy(buff);
	*/
	/*
	CAknWarningNote* dialog = new ( ELeave ) CAknWarningNote(ETrue);
	dialog->ExecuteLD( buff->Des() );
	CleanupStack::PopAndDestroy(buff);
	*/


// 2
/*
RESOURCE DIALOG R_QUERY_SIM_ADDED
    {
    flags = EGeneralQueryFlags;
    buttons = R_AVKON_SOFTKEYS_OK_EMPTY;
    items =
        {
        DLG_LINE
            {
            type = EAknCtQuery;
            id = EGeneralQuery;
            control = AVKON_CONFIRMATION_QUERY
                {
                layout = EConfirmationQueryLayout;
                animation = R_QGN_NOTE_OK_ANIM;   
		// animation = R_QGN_NOTE_INFO_ANIM;
                };
            }
        };
    }

	CAknQueryDialog* dlg = CAknQueryDialog::NewL();
	dlg->ExecuteLD(R_QUERY_SIM_ADDED, buff);
	CleanupStack::PopAndDestroy();		*/


// 3 Non-Blocking > Blocking (a differenza di 2 non c'e' bisogno di risorse)
/*    CAknGlobalConfirmationQuery* globalQ = CAknGlobalConfirmationQuery::NewL();
    CleanupStack::PushL(globalQ);
 
    TRequestStatus theStat = KRequestPending;
    globalQ->ShowConfirmationQueryL(theStat, aMessage, R_AVKON_SOFTKEYS_OK_EMPTY, R_QGN_NOTE_INFO_ANIM);
    User::WaitForRequest(theStat);
	   
    CleanupStack::PopAndDestroy(globalQ);*/


// 4 Non-Blocking
/* CAknGlobalMsgQuery...  (a differenza di 3 ha anche un header)
   Known-Issue: Parte bassa delle lettere tagliate */


// 5 Non-Blocking
/*	CAknGlobalNote* note = CAknGlobalNote::NewLC();
	note->SetSoftkeys( R_AVKON_SOFTKEYS_OK_EMPTY );
	note->ShowNoteL( EAknGlobalInformationNote, buffer );
	CleanupStack::PopAndDestroy( note );*/
}


/*
void Notes::ConfirmationL(TInt resource_id)
{
//	HBufC* buffer = CEikonEnv::Static()->AllocReadResourceLC( resource_id );
	HBufC* buffer = StringLoader::LoadLC( resource_id );
	TPtrC ptr = buffer->Des();
	CAknGlobalConfirmationQuery* globalQ = CAknGlobalConfirmationQuery::NewL();
	CleanupStack::PushL(globalQ);
	
	TRequestStatus theStat = KRequestPending;
	globalQ->ShowConfirmationQueryL(theStat, ptr, R_AVKON_SOFTKEYS_OK_EMPTY, R_QGN_NOTE_INFO_ANIM);
	User::WaitForRequest(theStat);
	   
	CleanupStack::PopAndDestroy(globalQ);
	
    CleanupStack::PopAndDestroy( buffer );
}*/


void Notes::ConfirmationL(TInt resource_id,...)
{
	VA_LIST list;
	VA_START(list, resource_id);
	HBufC* buffer = StringUtils::LoadFromResourceLC(resource_id, list);
	
	TPtrC ptr = buffer->Des();
	CAknGlobalConfirmationQuery* globalQ = CAknGlobalConfirmationQuery::NewL();
	CleanupStack::PushL(globalQ);
	
	TRequestStatus theStat = KRequestPending;
	globalQ->ShowConfirmationQueryL(theStat, ptr, R_AVKON_SOFTKEYS_OK_EMPTY, R_QGN_NOTE_INFO_ANIM);
	User::WaitForRequest(theStat);
	   
	CleanupStack::PopAndDestroy(globalQ);
	
    CleanupStack::PopAndDestroy( buffer );
}


void Notes::ErrorL(TInt resource_id,...)
{
	VA_LIST list;
	VA_START(list, resource_id);
	HBufC* buffer = StringUtils::LoadFromResourceLC(resource_id, list);

//	HBufC* buffer = CEikonEnv::Static()->AllocReadResourceLC( resource_id );
//	HBufC* buffer = StringLoader::LoadLC( resource_id );
	TPtrC ptr = buffer->Des();
	CAknGlobalConfirmationQuery* globalQ = CAknGlobalConfirmationQuery::NewL();
	CleanupStack::PushL(globalQ);
	
	TRequestStatus theStat = KRequestPending;
	globalQ->ShowConfirmationQueryL(theStat, ptr, R_AVKON_SOFTKEYS_OK_EMPTY, R_QGN_NOTE_ERROR_ANIM);
	User::WaitForRequest(theStat);
	   
	CleanupStack::PopAndDestroy(globalQ);
	
    CleanupStack::PopAndDestroy( buffer );
}


void Notes::WarningL(TInt resource_id,...)
{
	VA_LIST list;
	VA_START(list, resource_id);
	HBufC* buffer = StringUtils::LoadFromResourceLC(resource_id, list);
	
//	HBufC* buffer = CEikonEnv::Static()->AllocReadResourceLC( resource_id );
//	HBufC* buffer = StringLoader::LoadLC( resource_id );
	TPtrC ptr = buffer->Des();

	CAknGlobalConfirmationQuery* globalQ = CAknGlobalConfirmationQuery::NewL();
	CleanupStack::PushL(globalQ);
	
	TRequestStatus theStat = KRequestPending;
	globalQ->ShowConfirmationQueryL(theStat, ptr, R_AVKON_SOFTKEYS_OK_EMPTY, R_QGN_NOTE_WARNING_ANIM);
	User::WaitForRequest(theStat);
	   
	CleanupStack::PopAndDestroy(globalQ);
	
    CleanupStack::PopAndDestroy( buffer );
}


