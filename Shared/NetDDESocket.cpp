/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDESOCKET.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CNetDDESocket class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "NetDDESocket.hpp"
#include <NCL/Socket.hpp>
#include "NetDDEPacket.hpp"

/******************************************************************************
** Method:		Constructor.
**
** Description:	.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CNetDDESocket::CNetDDESocket(CSocket* pSocket)
	: m_oSocket(*pSocket)
{
}

/******************************************************************************
** Method:		Destructor.
**
** Description:	.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CNetDDESocket::~CNetDDESocket()
{
}

/******************************************************************************
** Method:		SendPacket()
**
** Description:	Send a packet.
**
** Parameters:	oPacket		The packet to send.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESocket::SendPacket(const NetDDEPacketPtr packet)
{
	m_oSocket.Send(packet->Buffer());
}

/******************************************************************************
** Method:		TryRecvPacket()
**
** Description:	Try and read an entire packet.
**
** Parameters:	oPacket		The packet read.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CNetDDESocket::TryRecvPacket(NetDDEPacketPtr& packet)
{
	size_t nAvail = m_oSocket.Available();

	// Enough data to read packet header?
	if (nAvail >= sizeof(CNetDDEPacket::Header))
	{
		packet = NetDDEPacketPtr(new CNetDDEPacket);

		// Set buffer size to header only.
		packet->Buffer().Size(sizeof(CNetDDEPacket::Header));

		// Peek at packet header.
		size_t nRead = m_oSocket.Peek(packet->Buffer(), packet->Buffer().Size());

		// Peeked entire header?
		if (nRead == sizeof(CNetDDEPacket::Header))
		{
			// Calculate full packet size.
			uint nPktSize = packet->DataSize() + sizeof(CNetDDEPacket::Header);

			// Entire packet buffered?
			if (nAvail >= nPktSize)
			{
				packet->Buffer().Size(nPktSize);

				// Read entire packet.
				nRead = m_oSocket.Recv(packet->Buffer());

				ASSERT(nRead == packet->Buffer().Size());

				return true;
			}
		}
	}

	return false;
}
