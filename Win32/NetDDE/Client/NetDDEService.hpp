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
typedef TPtrArray<CDDESvrConv> CConvs;
typedef TPtrArray<CDDELink> CLinks;

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
	// Members.
	//
	CString			m_strService;
	CString			m_strServer;
	CString			m_strPipeName;
	CNetDDECltPipe 	m_oConnection;
	HCONV			m_hSvrConv;
	CConvs			m_aoConvs;
	CLinks			m_aoLinks;
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

inline CNetDDEService::CNetDDEService()
	: m_hSvrConv(NULL)
{
}

inline CNetDDEService::~CNetDDEService()
{
}

#endif // NETDDESERVICE_HPP
