/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDESVRPIPE.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CNetDDESvrPipe class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef NETDDESVRPIPE_HPP
#define NETDDESVRPIPE_HPP

/******************************************************************************
** 
** The server end of a NetDDE pipe.
**
*******************************************************************************
*/

class CNetDDESvrPipe : public CServerPipe, public CNetDDEPipe
{
public:
	//
	// Constructors/Destructor.
	//
	CNetDDESvrPipe();
	~CNetDDESvrPipe();
	
protected:
	//
	// Members.
	//
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif // NETDDESVRPIPE_HPP
