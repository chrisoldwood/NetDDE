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
typedef TPtrArray<CDDECltConv> CConvs;
typedef TPtrArray<CDDELink> CLinks;

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
	// Conversation methods.
	//
	void AddConversation(CDDECltConv* pConv);
	void RemoveConversation(CDDECltConv* pConv);
	bool UsesConversation(CDDECltConv* pConv);
	void GetConversations(CConvs& aoConvs);

	//
	// Link methods.
	//
	void AddLink(CDDELink* pLink);
	void RemoveLink(CDDELink* pLink);
	void RemoveAllConvLinks(CDDECltConv* pConv);
	bool UsesLink(CDDELink* pLink);
	void GetLinks(CLinks& aoLinks);

protected:
	//
	// Members.
	//
	CConvs	m_aoConvs;	// DDE Conversation list.
	CLinks	m_aoLinks;	// DDE links list.
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif // NETDDESVRPIPE_HPP
