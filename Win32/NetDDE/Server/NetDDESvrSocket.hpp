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

// Template shorthands.
typedef TPtrArray<CNetDDEConv> CNetConvs;

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

	//
	// Methods.
	//
	CNetDDEConv* FindNetConv(CDDECltConv* pConv, uint32 nConvID) const;

	bool IsLinkUsed(CDDELink* pLink) const;

public:
	//
	// Members.
	//
	CString		m_strService;	// DDE Service name.
	CString		m_strComputer;	// Computer name.
	CString		m_strUser;		// User name.
	CNetConvs	m_aoNetConvs;	// NetDDE Conversation list.
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif // NETDDESVRPIPE_HPP
