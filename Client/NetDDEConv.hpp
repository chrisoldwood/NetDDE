/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDECONV.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CNetDDEConv class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef NETDDECONV_HPP
#define NETDDECONV_HPP

// Template shorthands.
typedef TPtrArray<CDDELink> CLinks;

/******************************************************************************
** 
** The data class for a NetDDE conversation.
**
*******************************************************************************
*/

class CNetDDEConv
{
public:
	//
	// Constructors/Destructor.
	//
	CNetDDEConv(HCONV hSvrConv, uint32 nSvrConvID);
	~CNetDDEConv();
	
public:
	// Template shorthands.
	typedef TList<CDDELink*> CLinkList;

	//
	// Members.
	//
	HCONV			m_hSvrConv;		// The server side conversation handle.
	uint32			m_nSvrConvID;	// The server side conversation ID;
	CDDESvrConv*	m_pCltConv;		// The client side conversation.
	CLinks			m_aoLinks;		// The active links.
	CLinkList		m_loNewLinks;	// The list of new links.
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

inline CNetDDEConv::CNetDDEConv(HCONV hSvrConv, uint32 nSvrConvID)
	: m_hSvrConv(hSvrConv)
	, m_nSvrConvID(nSvrConvID)
	, m_pCltConv(NULL)
{
	ASSERT(m_hSvrConv   != NULL);
	ASSERT(m_nSvrConvID != NULL);
}

inline CNetDDEConv::~CNetDDEConv()
{
}

#endif // NETDDECONV_HPP
