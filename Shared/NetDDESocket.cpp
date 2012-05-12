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

void CNetDDESocket::SendPacket(const CNetDDEPacket& oPacket)
{
	m_oSocket.Send(oPacket.Buffer());
}

/******************************************************************************
** Method:		RecvPacket()
**
** Description:	Try and read an entire packet.
**
** Parameters:	oPacket		The packet read.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CNetDDESocket::RecvPacket(CNetDDEPacket& oPacket)
{
	size_t nAvail = m_oSocket.Available();

	// Enough data to read packet header?
	if (nAvail >= sizeof(CNetDDEPacket::Header))
	{
		// Set buffer size to header only.
		oPacket.Buffer().Size(sizeof(CNetDDEPacket::Header));

		// Peek at packet header.
		size_t nRead = m_oSocket.Peek(oPacket.Buffer(), oPacket.Buffer().Size());

		// Peeked entire header?
		if (nRead == sizeof(CNetDDEPacket::Header))
		{
			// Calculate full packet size.
			uint nPktSize = oPacket.DataSize() + sizeof(CNetDDEPacket::Header);

			// Entire packet buffered?
			if (nAvail >= nPktSize)
			{
				oPacket.Buffer().Size(nPktSize);

				// Read entire packet.
				nRead = m_oSocket.Recv(oPacket.Buffer());

				ASSERT(nRead == oPacket.Buffer().Size());

				return true;
			}
		}
	}

	return false;
}
