/*
============================================================================
 Name        : ResReader.h
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : CResReader declaration
============================================================================
*/

#ifndef RESREADER_H
#define RESREADER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <BARSC.H>
#include <f32file.h>
#include <FuzzyByte\FileLogger.h>

// CLASS DECLARATION

/**
*  CResReader
*
*/
class CResReader : public CBase
{
public: // Constructors and destructor

	~CResReader();

        /**
        * Two-phased constructor.
        */
	static CResReader* NewL(RFs& fs, const TDesC& fileName);

        /**
        * Two-phased constructor.
        */
	static CResReader* NewLC(RFs& fs, const TDesC& fileName);

	void ReadL(TDes& buffer, TInt resId);
	
	HBufC* AllocReadL(TInt resId);
	HBufC* AllocReadLC(TInt resId);

private:
	/**
        * Constructor for performing 1st stage construction
        */
	CResReader(RFs& fs);

	void ConstructL(const TDesC& fileName);
	
private:
	__LOG_DECLARATION_MEMBER;
	RFs& iFs;
	RResourceFile iResFile;
};

#endif // RESREADER_H
