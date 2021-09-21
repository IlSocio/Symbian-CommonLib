/*
 ============================================================================
 Name		: AgentCalendar.h
 Author	  : Marco Bellino
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CAgentCalendar declaration
 ============================================================================
 */

#ifndef AGENTCALENDAR_H
#define AGENTCALENDAR_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION

/**
 *  CAgentCalendar
 * 
 */
class CAgentCalendar : public CBase
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CAgentCalendar();

	/**
	 * Two-phased constructor.
	 */
	static CAgentCalendar* NewL();

	/**
	 * Two-phased constructor.
	 */
	static CAgentCalendar* NewLC();

private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CAgentCalendar();

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL();

	};

#endif // AGENTCALENDAR_H
