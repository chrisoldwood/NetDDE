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
	// Members.
	//
	CString			m_strService;
	CString			m_strServer;
	CString			m_strPipeName;
	CNetDDECltPipe 	m_oConnection;
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif // NETDDESERVICE_HPP
