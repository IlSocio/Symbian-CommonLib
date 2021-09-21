#include "aes.h"
#include <cryptosymmetric.h> 		// For AES Stuff
#include <cryptopadding.h> 			// For AES Stuff
#include <FuzzyByte\FileLogger.h>

HBufC8* AES::DecryptL(const TDesC8& encryptedData, const TDesC8& iv, const TDesC8& key)
	{
	ASSERT_LEAVE( key.Length() % 16 == 0, KErrArgument);
	CBlockTransformation* aesDecr = CAESDecryptor::NewL(key);
	CModeCBCDecryptor* cbcMode = CModeCBCDecryptor::NewL(aesDecr, iv);
	CPadding* padding = CPaddingPKCS7::NewL(aesDecr->BlockSize());
	CBufferedDecryptor* bufDecryptor = CBufferedDecryptor::NewLC(cbcMode, padding);

	// Data Length must be multiple of BlockSize() or the ProcessFinalL() will raise a Panic()
	ASSERT_LEAVE( encryptedData.Length() > 0, KErrArgument);
	ASSERT_LEAVE( encryptedData.Length() % aesDecr->BlockSize() == 0, KErrArgument);
	if (encryptedData.Length() % aesDecr->BlockSize() != 0)
		{ 
		CleanupStack::PopAndDestroy(bufDecryptor);
		return HBufC8::NewL(0);
		}

	HBufC8* plain = HBufC8::NewLC(bufDecryptor->MaxFinalOutputLength(encryptedData.Length()));
	TPtr8 plainPtr = plain->Des();
	bufDecryptor->ProcessFinalL(encryptedData, plainPtr);
	CleanupStack::Pop(plain);

	CleanupStack::PopAndDestroy(bufDecryptor);
	return plain;
	}

HBufC8* AES::EncryptL(const TDesC8& plainData, const TDesC8& iv, const TDesC8& key)
	{
	ASSERT_LEAVE( key.Length() % 16 == 0, KErrArgument);
	CBlockTransformation* aesEncr = CAESEncryptor::NewL(key);
	CModeCBCEncryptor* cbcMode = CModeCBCEncryptor::NewL(aesEncr, iv); 
	CPadding* padding = CPaddingPKCS7::NewL(aesEncr->BlockSize());
	CBufferedEncryptor* bufEncryptor = CBufferedEncryptor::NewLC(cbcMode, padding);

	/*
	// Data Length must be multiple of BlockSize() or the ProcessFinalL() will raise a Panic()
	// Add Padding...
	TUint32 paddedLen = plainData.Length() + aesEncr->BlockSize() - 1;
	paddedLen = paddedLen - (paddedLen % aesEncr->BlockSize());
	  
	RBuf8 paddedPlainData;
	paddedPlainData.CleanupClosePushL();
	paddedPlainData.Create(paddedLen);  
	paddedPlainData.SetMax();
	paddedPlainData.FillZ();
	paddedPlainData.Copy(plainData);
	paddedPlainData.SetMax();
	ASSERT( paddedPlainData.Length() % aesEncr->BlockSize() == 0);

	HBufC8* encryptedData = HBufC8::NewLC(bufEncryptor->MaxFinalOutputLength(paddedPlainData.Length()));
	TPtr8 encryptedPtr = encryptedData->Des();
	bufEncryptor->ProcessFinalL(paddedPlainData, encryptedPtr);
	CleanupStack::Pop(encryptedData);
 
	CleanupStack::PopAndDestroy(&paddedPlainData);
	CleanupStack::PopAndDestroy(bufEncryptor);*/

	HBufC8* encryptedData = HBufC8::NewLC(bufEncryptor->MaxFinalOutputLength(plainData.Length()));
	TPtr8 encryptedPtr = encryptedData->Des();
	bufEncryptor->ProcessFinalL(plainData, encryptedPtr);
	CleanupStack::Pop(encryptedData);

	CleanupStack::PopAndDestroy(bufEncryptor);
	
	return encryptedData;
	}
