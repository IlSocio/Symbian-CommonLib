/*
============================================================================
Name        : CSettingsFile from SettingsFile.h
Author      :  Roberto LA RAGIONE
Version     : 1.0
Copyright   : BITWEEN S.R.L.
Description : Base settings for applications
============================================================================
*/

#ifndef SETTINGS_FILE_H
#define SETTINGS_FILE_H

#include <e32base.h>
#include <f32file.h>

class RWriteStream;
class RReadStream;

class CSettingsFile : public CBase
	{
public:
	/**
	 * Destroys the object
	 */
	virtual ~CSettingsFile();

	/**
	 * Loads application settings from the associated file
	 */
	TInt Load(const TDesC& aFileName, const TDesC& aCurrVersion);

	TInt ImportDictionary(TUid dictionaryUid, const TDesC& aSrcFile, const TDesC& aDestFile, const TDesC& aCurrVersion);
	
	/**
	 * Saves application settings to the associated file 
	 */
	void SaveL();

	virtual void ExternalizeL(RWriteStream& aStream) const = 0;
	virtual void InternalizeL(RReadStream& aStream) = 0;

protected:

	// Override this method if some decryption is needed
	virtual HBufC8* LoadBytesFromFileL();
	// Override this method if some encryption is needed
	virtual void SaveBytesToFileL(const TDesC8& buffer);

	virtual void UpgradeFromOldVersion(RReadStream& aStream, const TDesC& aOldVersion) = 0;
	
	/**
	 * Restores settings to default values
	 */
	virtual void RestoreToDefaults() = 0;

protected:
	/**
	* Constructor for performing 1st stage construction
	*/
	CSettingsFile(RFs& aFileServerSession);

private:
	TInt DoImportL(TUid dictionaryUid, const TDesC& aSrcFile);
	TInt DoLoadL();

	/**
	* Reads version header to the stream
	* @param aStream Stream, opened for writing
	*/
	void ReadVersionHeaderL(RReadStream& aStream, TDes& version);

	/**
	* Writes version header to the stream
	* @param aStream Stream, opened for writing
	*/
	void WriteVersionHeaderL(RWriteStream& aStream);
		
protected:
	RFs& iFs;
	RBuf iFileName;
	RBuf iCurrVersion;
	}; // CSettingsFile

#endif // SETTINGS_FILE_H
