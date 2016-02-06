/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDESVRSOCKET.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CNetDDESvrSocket class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef NETDDESVRSOCKET_HPP
#define NETDDESVRSOCKET_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <NCL/TCPCltSocket.hpp>
#include "NetDDESocket.hpp"
#include <vector>

// Forward declarations;
class CDDECltConv;
class CNetDDEConv;
class CDDELink;

// Template shorthands.
typedef std::vector<CNetDDEConv*> CNetConvs;

/******************************************************************************
** 
** The server end of a NetDDE connection.
**
*******************************************************************************
*/

class CNetDDESvrSocket : public CTCPCltSocket, public CNetDDESocket
{
public:
	//
	// Constructors/Destructor.
	//
	CNetDDESvrSocket();
	~CNetDDESvrSocket();

	//
	// Methods.
	//
	virtual void Close();

	CNetDDEConv* FindNetConv(CDDECltConv* pConv, uint32 nConvID) const;

	bool IsLinkUsed(CDDELink* pLink) const;

public:
	//
	// Members.
	//
	CString		m_strService;	// DDE Service name.
	CString		m_strComputer;	// Computer name.
	CString		m_strUser;		// User name.
	CNetConvs	m_aoNetConvs;	// NetDDE Conversation list.
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif // NETDDESVRSOCKET_HPP
