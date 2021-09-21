/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Central repository keys and repository identifiers for the S60
*                messaging UI components
*
*/



#ifndef MESSAGINGINTERNALCRKEYS_H
#define MESSAGINGINTERNALCRKEYS_H

//
// Common Messaging UI Settings API
const TUid KCRUidMuiuSettings = {0x101F87EB};
const TUint32 KMuiuSentItemsCount = 0x00000001;
const TUint32 KMuiuSentItemsInUse = 0x00000002;
const TUint32 KMuiuMemoryInUse = 0x00000003;
const TUint32 KMuiuToInputMode = 0x00000004;
// Deprecated. Use KCRUidMessagingUiSettings / KMessagingEditorLocalUiZoom instead.
const TUint32 KMuiuEditorLocalUiZoom = 0x00000007;
const TUint32 KMuiuZoomValue = 0x00000008;
const TUint32 KMuiuMailMessageListType = 0x00000009;
const TUint32 KMuiuDescriptionLength = 0x0000000A;
const TUint32 KMuiuInboxMessageListType = 0x0000000B;
const TUint32 KMuiuMceMainViewRow1  = 0x0000000C;
const TUint32 KMuiuMceMainViewRow2  = 0x0000000D;
const TUint32 KMuiuMceMainViewRow3  = 0x0000000E;
const TUint32 KMuiuMceMainViewRow4  = 0x0000000F;
const TUint32 KMuiuMceMainViewRow5  = 0x00000010;
const TUint32 KMuiuMceMainViewRow6  = 0x00000011;
const TUint32 KMuiuMceMainViewRow7  = 0x00000012;
const TUint32 KMuiuMceMainViewRow8  = 0x00000013;
const TUint32 KMuiuMceMainViewRow9  = 0x00000014;
const TUint32 KMuiuMceMainViewRow10 = 0x00000015;
const TUint32 KMuiuMceMainViewRow11 = 0x00000016;
const TUint32 KMuiuMceMainViewRow12 = 0x00000017;
const TUint32 KMuiuMceMainViewRow13 = 0x00000018;
const TUint32 KMuiuMceMainViewRow14 = 0x00000019;
const TUint32 KMuiuMceMainViewRow15 = 0x0000001A;
const TUint32 KMuiuMceMainViewRow16 = 0x0000001B;

//
// MMS UI Settings API
const TUid KCRUidMmsui = {0x101F87EC};
const TUint32 KMmsuiCsdEnabled = 0x00000001;
const TUint32 KMmsuiHandsfreeVolume = 0x00000002;

//
// The field controls which additional header fields are visible in the MMS Editor.
// The state of each header field is represented by one bit.
const TUint32 KMmsuiAdditionalHeaders = 0x00000003;
const TInt KMmsuiHeaderSubject = 1;
const TInt KMmsuiHeaderCC      = 2;
const TInt KMmsuiHeaderBCC     = 4;

//
// The field controls how the sender is composed when
// the address-present-token is sent as part of the From Field in an MMS message.
// The format of the sender address is
//
//  [+] [MCC|IDC] MIN|MDN
//
//  Where 
//  + = includes the plus sign before the number. Optional.
//	MCC = Mobile Country Code. Optional.
//  IDC = International Dialing Code. Optional.
//  MIN = Mobile Identification Number	
//  MDN = Mobile Directory Number
const TUint32 KMmsuiSenderSettings = 0x00000004;
const TInt KMmsuiSenderSetAddress =  1;	// Address plus address token should be sent
const TInt KMmsuiSenderPlusSign   =  2;	// Include "+" sign
const TInt KMmsuiSenderCC	  	    =  4;	// Include Country Code
const TInt KMmsuiSenderMCC	      =  8; // Include MCC instead of IDC
const TInt KMmsuiSenderMIN        = 16;	// Include MIN instead of MDN

//
// Messaging UI Variation API
const TUint KCRUidMuiuVariationValue = 0x101F87ED;
const TUid KCRUidMuiuVariation = {KCRUidMuiuVariationValue};
const TUint32 KMuiuEmailConfigFlags = 0x00000001;
const TUint32 KMuiuMceFeatures = 0x00000002;
const TUint32 KMuiuMmsFeatures = 0x00000003;
const TUint32 KMuiuMsgEditorFeatures = 0x00000004;
const TUint32 KMuiuSmilFeatures = 0x00000005;
const TUint32 KMuiuSmsFeatures = 0x00000006;
const TUint32 KMuiuPostcardFeatures = 0x00000007;
const TUint32 KMuiuUniEditorFeatures = 0x00000008;

//
// SMIL UI Settings API
const TUid KCRUidSmilui = {0x101F87EE};
const TUint32 KSmilPlayerVolume = 0x00000001;

//
// SMS UI Settings API
const TUid KCRUidSmum = {0x101F87EF};
const TUint32 KSmumDeliveryReport      = 0x00000001;
const TUint32 KSmumMessageConversion   = 0x00000002;
const TUint32 KSmumPreferredConnection = 0x00000003;
const TUint32 KSmumReplyViaSameCentre  = 0x00000004;
const TUint32 KSmumSmsStatus           = 0x00000006;
const TUint32 KSmumValidityPeriod      = 0x00000007;
const TUint32 KSmumCharSupport         = 0x00000008;
const TUint32 KSumEmailSC              = 0x00000009;
const TUint32 KSumEmailGateway         = 0x0000000A;
const TUint32 KSumEmailModifiable      = 0x0000000B;
const TUint32 KSmumMaxSubMsgCount      = 0x0000000C;
const TUint32 KSmumSendOptDelNote      = 0x0000000D;
const TUint32 KSmumCallbackNumber      = 0x0000000E;
const TUint32 KSmumMsgPriority         = 0x0000000F;
const TUint32 KSmumMsgQueueTimeout     = 0x00000010;
const TUint32 KSmumMsgQueue            = 0x00000011;
const TUint32 KSmumDefDelReport        = 0x00000012;
const TUint32 KSmumMaxNumOfUserDataChar             = 0x00000013;
const TUint32 KSmumMaxEmailAddressLength            = 0x00000014;
const TUint32 KSmumMaxRecipientPhoneNumberLength    = 0x00000015;
const TUint32 KSmumMultipleRecipient                = 0x00000016;
const TUint32 KSmumRemoveReplyViaSameCentre			    = 0x00000017;
const TUint32 KSmumPreviousSimSmscNumber            = 0x00000018;
const TUint32 KSmumShowSMSCTimeStamp                = 0x00000019;

//
// Send UI Settings API
const TUid KCRUidSendUi = {0x101F8873};
const TUint32 KSUMmsUploadAddress1 = 0x00000001;
const TUint32 KSUMmsUploadAddress2 = 0x00000002;
const TUint32 KSUMmsUploadAddress3 = 0x00000003;
const TUint32 KSUMmsUploadAddress4 = 0x00000004;
const TUint32 KSUMmsUploadType1    = 0x00000005;
const TUint32 KSUMmsUploadType2    = 0x00000006;
const TUint32 KSUMmsUploadType3    = 0x00000007;
const TUint32 KSUMmsUploadType4    = 0x00000008;
const TUint32 KSUMmsUploadServiceName1      = 0x00000009;
const TUint32 KSUMmsUploadServiceName2      = 0x0000000A;
const TUint32 KSUMmsUploadServiceName3      = 0x0000000B;
const TUint32 KSUMmsUploadServiceName4      = 0x0000000C;
const TUint32 KSUMmsUploadServiceMenuName1  = 0x0000000D;
const TUint32 KSUMmsUploadServiceMenuName2  = 0x0000000E;
const TUint32 KSUMmsUploadServiceMenuName3  = 0x0000000F;
const TUint32 KSUMmsUploadServiceMenuName4  = 0x00000010;

//
// Email Always Online Settings API
const TUid KCRUidAOSettings = {0x102071F2};
const TUint32 KAODisabledPluginsCount = 0x00000001;

//
// Unified SMS and MMS Editor Settings API
const TUid KCRUidUniEditor = {0x102072E5};
const TUint32 KUniEditorSoftLimitSubMsgCount    = 0x00000001;
const TUint32 KUniEditorMaxRecipientCount       = 0x00000002;
const TUint32 KUniEditorSoftLimitRecipientCount = 0x00000003;
const TUint32 KUniEditorMaxSmsCharacterCount	= 0x00000004;
const TUint32 KUniEditorMaxSlideCount           = 0x00000005;
const TUint32 KUniEditorSoftLimitEnable         = 0x00000006;
const TUint32 KUniEditorSMSSizeWarning          = 0x00000007;

//
// Audio Message Settings API
const TUid KCRUidAudioMsg = {0x1028281E};
const TUint32 KAudioMsgSpeaker      = 0x00000001;

#endif // MESSAGINGINTERNALCRKEYS_H