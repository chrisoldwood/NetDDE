/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDESVCCFG.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CNetDDESvcCfg class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef NETDDESVCCFG_HPP
#define NETDDESVCCFG_HPP

/******************************************************************************
** 
** The data class used to hold a service configuation.
**
*******************************************************************************
*/

class CNetDDESvcCfg
{
public:
	//
	// Constructors/Destructor.
	//
	CNetDDESvcCfg();
	~CNetDDESvcCfg();
	
	//
	// Members.
	//
	CString	m_strRemName;		// Remote DDE Service name.
	CString	m_strLocName;		// Local DDE Service name.
	CString	m_strServer;		// Host name.
	uint	m_nServerPort;		// Server socket port.
	bool	m_bAsyncAdvises;	// Use Async advises?
	bool	m_bTextOnly;		// Only supports CF_TEXT?
	CString	m_strInitialVal;	// Initial value.
	CString m_strFailedVal;		// Failed value (for Async advises).
	bool	m_bReqInitalVal;	// Request initial value?
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

inline CNetDDESvcCfg::CNetDDESvcCfg()
	: m_bAsyncAdvises(false)
	, m_bTextOnly(false)
	, m_strInitialVal("#PNDG")
	, m_strFailedVal("#LINK")
	, m_bReqInitalVal(false)
{
}

inline CNetDDESvcCfg::~CNetDDESvcCfg()
{
}

#endif // NETDDESVCCFG_HPP
