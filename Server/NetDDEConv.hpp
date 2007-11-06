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

#if _MSC_VER > 1000
#pragma once
#endif

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
	CNetDDEConv(CDDECltConv* pSvrConv, uint32 nSvrConvID);
	~CNetDDEConv();
	
public:
	//
	// Members.
	//
	CDDECltConv*	m_pSvrConv;		// The server side conversation.
	uint32			m_nSvrConvID;	// The server side conversation ID;
	CLinks			m_aoLinks;		// The active links.
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

inline CNetDDEConv::CNetDDEConv(CDDECltConv* pSvrConv, uint32 nSvrConvID)
	: m_pSvrConv(pSvrConv)
	, m_nSvrConvID(nSvrConvID)
{
	ASSERT(m_pSvrConv   != NULL);
	ASSERT(m_nSvrConvID != NULL);
}

inline CNetDDEConv::~CNetDDEConv()
{
}

#endif // NETDDECONV_HPP
