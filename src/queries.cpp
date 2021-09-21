/*
============================================================================
 Name        : Notes.cpp
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Notes implementation
============================================================================
*/

#include "Queries.h"
#include <AknQueryDialog.h>
#include <EIKENV.H>
#include "stringutils.h"
#include <aknglobalconfirmationquery.h> 
/*#include <stringloader.h>
#include "stringutils.h"
#include "rstringbuf.h"*/



TBool Queries::AskTextL(TInt aResID, TDes& text, TBool aPredectiveFlag)
{
	CAknTextQueryDialog* dlg = new( ELeave ) CAknTextQueryDialog( text );
	// TODO: FIX: Stranamente sui Cell. Touch abilitando il Predictive, la tastiera diventa numerica e schianta quando arriva all'immissione manuale del testo non riconosciuto dal T9.
	// TODO: FIX: Questo se la query e' con flag EEikDialogFlagVirtualInput
	dlg->SetPredictiveTextInputPermitted( aPredectiveFlag );
	dlg->PrepareLC( aResID );
/*	if (aTitleId >= 0)
		dlg->SetTitleL(aTitleId);*/
//	dlg->SetTitleL(_L("Test"));  -- non funzia
	dlg->SetMaxLength( text.MaxLength() );
	if ( dlg->RunLD() ) return ETrue;
	return EFalse;
}


TBool Queries::OkCancelL(TInt resource_id, ...)
{
	VA_LIST list;
	VA_START(list, resource_id);
	HBufC* buffer = StringUtils::LoadFromResourceLC(resource_id, list);
	//HBufC* buffer = StringLoader::LoadLC( resource_id );
	TPtrC ptr = buffer->Des();
    CAknGlobalConfirmationQuery* globalQ = CAknGlobalConfirmationQuery::NewL();
    CleanupStack::PushL(globalQ);
 
    TRequestStatus theStat = KRequestPending;
	globalQ->ShowConfirmationQueryL(theStat, ptr, R_AVKON_SOFTKEYS_OK_CANCEL, R_QGN_NOTE_QUERY_ANIM);
    User::WaitForRequest(theStat);
       	
    CleanupStack::PopAndDestroy(globalQ);

    CleanupStack::PopAndDestroy( buffer );
    return (theStat.Int() == EAknSoftkeyYes || theStat.Int() == EAknSoftkeyOk);
}


