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
	CString	m_strService;		// DDE Service name.
	CString	m_strServer;		// Host name.
	CString	m_strPipeName;		// Named Pipe name.
	bool	m_bAsyncAdvises;	// Use Async advises?
	bool	m_bTextOnly;		// Only supports CF_TEXT?
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
{
}

inline CNetDDESvcCfg::~CNetDDESvcCfg()
{
}

#endif // NETDDESVCCFG_HPP
