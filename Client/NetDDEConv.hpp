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
	CNetDDEConv(HCONV hSvrConv);
	~CNetDDEConv();
	
public:
	//
	// Members.
	//
	HCONV			m_hSvrConv;		// The server side conversation.
	CDDESvrConv*	m_pCltConv;		// The client side conversation.
	CLinks			m_aoLinks;		// The active links.
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

inline CNetDDEConv::CNetDDEConv(HCONV hSvrConv)
	: m_hSvrConv(hSvrConv)
	, m_pCltConv(NULL)
{
}

inline CNetDDEConv::~CNetDDEConv()
{
}

#endif // NETDDECONV_HPP
