/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDECLTSOCKET.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CNetDDECltSocket class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef NETDDECLTSOCKET_HPP
#define NETDDECLTSOCKET_HPP

// Forward declarations;
class CNetDDEService;

/******************************************************************************
** 
** The client end of a NetDDE connection.
**
*******************************************************************************
*/

class CNetDDECltSocket : public CTCPCltSocket, public CNetDDESocket
{
public:
	//
	// Constructors/Destructor.
	//
	CNetDDECltSocket(CNetDDEService* pService);
	~CNetDDECltSocket();
	
	//
	// Properties.
	//
	CNetDDEService* Service() const;

	void SetTimeOut(uint nTimeOut);

	//
	// Methods.
	//
	virtual void Close();

	void ReadResponsePacket(CNetDDEPacket& oPacket, uint nPacketID);

	void QueueResponsePacket(CNetDDEPacket* pPacket);

protected:
	// Template shorthands.
	typedef TPtrArray<CNetDDEPacket> CPackets;

	//
	// Members.
	//
	CNetDDEService*	m_pService;		// The service details.
	uint			m_nTimeOut;		// Socket wait time-out.
	CPackets		m_aoPackets;	// The received packet queue.
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

inline CNetDDEService* CNetDDECltSocket::Service() const
{
	return m_pService;
}

inline void CNetDDECltSocket::SetTimeOut(uint nTimeOut)
{
	m_nTimeOut = nTimeOut;
}

inline void CNetDDECltSocket::QueueResponsePacket(CNetDDEPacket* pPacket)
{
	m_aoPackets.Add(pPacket);
}

#endif // NETDDECLTSOCKET_HPP
