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

// Template shorthands.
typedef TPtrArray<CNetDDEConv> CNetConvs;

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

	//
	// Members.
	//
	CNetDDESvcCfg	m_oCfg;				// The service configuration.
	CNetDDECltPipe 	m_oConnection;		// The connection to the NetDDE server.
	CNetConvs		m_aoNetConvs;		// The active NetDDE conversations.
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

inline CNetDDEService::CNetDDEService()
{
}

inline CNetDDEService::~CNetDDEService()
{
	ASSERT(m_aoNetConvs.Size() == 0);
}

inline CNetDDEConv* CNetDDEService::FindNetConv(CDDESvrConv* pConv) const
{
	// For all service conversations.
	for (int j = 0; j < m_aoNetConvs.Size(); ++j)
	{
		CNetDDEConv* pNetConv = m_aoNetConvs[j];

		// Matches?
		if (pNetConv->m_pCltConv == pConv)
			return pNetConv;
	}

	return NULL;
}

#endif // NETDDESERVICE_HPP
