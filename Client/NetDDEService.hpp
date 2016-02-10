/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDESERVICE.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CNetDDEService class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef NETDDESERVICE_HPP
#define NETDDESERVICE_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetDDESvcCfg.hpp"
#include "NetDDECltSocket.hpp"
#include "NetDDEConv.hpp"
#include <vector>

// Template shorthands.
typedef std::vector<CNetDDEConv*> CNetConvs;

/******************************************************************************
** 
** Data class used to hold the DDE services to bridge.
**
*******************************************************************************
*/

class CNetDDEService
{
public:
	//
	// Constructors/Destructor.
	//
	CNetDDEService();
	~CNetDDEService();

	//
	// Methods.
	//
	CNetDDEConv* FindNetConv(CDDESvrConv* pConv) const;

public:
	//
	// Members.
	//
	CNetDDESvcCfg		m_oCfg;				// The service configuration.
	CNetDDECltSocket	m_oConnection;		// The connection to the NetDDE server.
	CNetConvs			m_aoNetConvs;		// The active NetDDE conversations.
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

inline CNetDDEService::CNetDDEService()
	: m_oConnection(this)
{
}

inline CNetDDEService::~CNetDDEService()
{
	ASSERT(m_aoNetConvs.size() == 0);
}

inline CNetDDEConv* CNetDDEService::FindNetConv(CDDESvrConv* pConv) const
{
	// For all service conversations.
	for (size_t j = 0; j < m_aoNetConvs.size(); ++j)
	{
		CNetDDEConv* pNetConv = m_aoNetConvs[j];

		// Matches?
		if (pNetConv->m_pCltConv == pConv)
			return pNetConv;
	}

	return nullptr;
}

#endif // NETDDESERVICE_HPP
