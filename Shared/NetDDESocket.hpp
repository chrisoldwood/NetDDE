/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDESOCKET.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CNetDDESocket class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef NETDDESOCKET_HPP
#define NETDDESOCKET_HPP

/******************************************************************************
** 
** The mixin class for NetDDE client/server sockets.
**
*******************************************************************************
*/

class CNetDDESocket
{
public:
	//
	// Constructors/Destructor.
	//
	CNetDDESocket(CSocket* pSocket);
	~CNetDDESocket();
	
	//
	// Methods.
	//
	void SendPacket(const CNetDDEPacket& oPacket);
	bool RecvPacket(CNetDDEPacket& oPacket);

protected:
	//
	// Members.
	//
	CSocket&	m_oSocket;

	// Disallow copies for now.
	CNetDDESocket(const CNetDDESocket&);
	void operator=(const CNetDDESocket&);
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif // NETDDESOCKET_HPP
