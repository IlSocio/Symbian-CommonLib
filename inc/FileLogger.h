/*
 ============================================================================
 Name		: FileLogger.h
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CFileLogger declaration
 ============================================================================
 */

//LIBRARY	efsrv.lib

#ifndef FILELOGGER_H 
#define FILELOGGER_H


#ifndef _DEBUG
#undef _LOGGING  // disable the file logging feature for the Release Builds
#endif


// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>


#define ASSERT_PANIC(x, y) __ASSERT_ALWAYS(x, User::Panic(_L("MyPanic"), y));
#define ASSERT_LEAVE(x, y) __ASSERT_ALWAYS(x, User::Leave(y));

// __BREAKPOINT();


#ifdef _LOGGING

#define __LOG_DECLARATION_MEMBER CFileLogger* __logger;

#define __LOG_OPEN(subsys, compnt) \
		{_LIT(_KLogDir, subsys); \
		_LIT(_KLogFile, compnt); \
		__logger = CFileLogger::NewL(); \
		__logger->Open(_KLogDir, _KLogFile);}

#define __LOG_OPEN_LIT(subsys, compnt) \
		{__logger = CFileLogger::NewL(); \
		__logger->Open(subsys, compnt);}

#define __LOG_OPEN_ID(subsys, compnt) \
		{TUint32 _addr_ = (TUint32) (this); \
		TBuf<50> _KLogFile; \
		_KLogFile.AppendNum(_addr_, EHex); \
		_KLogFile.Append(_L("_")); \
		_KLogFile.Append(_L(compnt)); \
		_LIT(_KLogDir, subsys); \
		__logger = CFileLogger::NewL(); \
		__logger->Open(_KLogDir, _KLogFile);}

#define __LOG_CLOSE            {delete __logger; __logger = NULL;}
#define __LOG(txt, parm...) {_LIT8(KTxt, txt); __logger->Write(KTxt, parm);}
#define __LOG_TXT(txt) {_LIT8(KTxt, txt);  __logger->Write(KTxt());}
#define __LOG_DES(des) {__logger->Write(des);}

/*#define __LOG(txt, parm...) {_LIT8(KTxt, txt); __logger->Dummy();}
#define __LOG_TXT(txt) {_LIT8(KTxt, txt); __logger->Dummy();}
#define __LOG_DES(des) __logger->Dummy();*/

//#define __LOG(txt, parm...)
//#define __LOG_TXT(txt)
//#define __LOG_DES(des)

/*#define _LOG_ARG(txt,parm...) {_LIT8(KTxt, txt); CFileLogger::Write(KTxt, parm);}
#define _LOG_TXT(txt)         {_LIT8(KTxt, txt); CFileLogger::Write((const TDesC8&)KTxt);}
#define _LOG_DES8(des)        CFileLogger::Write((const TDesC8&)des);
#define _LOG_DES16(des)       CFileLogger::Write(des);
#define _LOG_ERR(txt,err)     if (err) LOGARG(txt, err)
#define _LOG_CALL(exp)        {LOGARG("Calling \"%s\"", #exp); exp; LOGARG("Call to \"%s\" passed.", #exp);}
#define _LOG_ENTER            LOGARG("%s start", __PRETTY_FUNCTION__)
#define _LOG_EXIT             LOGARG("%s end", __PRETTY_FUNCTION__)
#define _LOG_MEM(ptr)         LOGARG("%s [0x%x]", #ptr, (TUint)ptr)*/

#define __LOG_ERR(txt,err)     {if (err) __LOG(txt, err);}
#define __LOG_CALL(exp)        { __LOG("Calling \"%s\"", #exp); exp; __LOG("Call to \"%s\" passed.", #exp);}
#define __LOG_ENTER            {__LOG("+%s", __PRETTY_FUNCTION__) __logger->IncrDeep();}
#define __LOG_EXIT             {__logger->DecrDeep(); __LOG("-%s", __PRETTY_FUNCTION__)}
#define __LOG_HEXDUMP		

#else
	#define __LOG_DECLARATION_MEMBER ;
	#define __LOG_OPEN(subsys, compnt) {}
	#define __LOG_OPEN_ID(subsys, compnt) {}
	#define __LOG_OPEN_LIT(subsys, compnt) {}
	#define __LOG_CLOSE {}

	#define __LOG(txt, parm...) {}
	#define __LOG_TXT(txt) {}
	#define __LOG_DES(des) {}
	#define __LOG_ERR(txt,err) {}
	#define __LOG_CALL(exp) {}
	#define __LOG_ENTER {}     
	#define __LOG_EXIT {}          
	#define __LOG_HEXDUMP {}	
#endif


// CLASS DECLARATION

const TInt KMaxLogEntrySize = 2000;

/**
 *  CFileLogger
 * 
 */
class CFileLogger : public CBase
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CFileLogger();

	/**
	 * Two-phased constructor.
	 */
	static CFileLogger* NewL();

	/**
	 * Two-phased constructor.
	 */
	static CFileLogger* NewLC();

	void Open(const TDesC& component, const TDesC& aFile);
	void Write(const TDesC8& aText);
	void Write(const TDesC16& aText);
	void Write(TRefByValue<const TDesC8> aFmt,...);
	void Close();
	void IncrDeep();
	void DecrDeep();
	
private:
	void DoWrite();
	
	/**
	 * Constructor for performing 1st stage construction
	 */
	CFileLogger();

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL();

private:
	RFs iFs;
	RFile iFile;
	TBuf8<KMaxLogEntrySize> iLogBuffer;
	TInt iDeep;
	};

#endif // FILELOGGER_H
