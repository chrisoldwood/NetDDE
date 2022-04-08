/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDECLTSOCKET.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CNetDDECltSocket class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "NetDDECltSocket.hpp"
#include "NetDDECltApp.hpp"
#include "NetDDEPacket.hpp"
#include <NCL/WinSock.hpp>
#include <NCL/SocketException.hpp>
#include <Core/Algorithm.hpp>

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

CNetDDECltSocket::CNetDDECltSocket(CNetDDEService* pService)
	: CTCPCltSocket(ASYNC)
	, CNetDDESocket(this)
	, m_pService(pService)
	, m_nTimeOut(App.m_nNetTimeOut)
{
	ASSERT(pService != nullptr);
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

CNetDDECltSocket::~CNetDDECltSocket()
{
	Close();
}

/******************************************************************************
** Method:		Close()
**
** Description:	Close the socket.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltSocket::Close()
{
	// Forward to base class.
	CTCPCltSocket::Close();

	// Cleanup packet queue.
	m_aoPackets.clear();
}

/******************************************************************************
** Method:		ReadResponsePacket()
**
** Description:	Read the response packet, waiting if required.
**				NB: All other packets types are appended to a queue.
**
** Parameters:	nPacketID	The packet ID expected.
**
** Returns:		The packet received.
**
*******************************************************************************
*/

NetDDEPacketPtr CNetDDECltSocket::ReadResponsePacket(uint nPacketID)
{
	DWORD dwStartTime = ::GetTickCount();

	// Until timed-out...
	while ((::GetTickCount() - dwStartTime) < m_nTimeOut)
	{
		// Any response packets received?
		if (m_aoPackets.size())
		{
			// Find our response.
			for (size_t i = 0; i < m_aoPackets.size(); ++i)
			{
				NetDDEPacketPtr packet = m_aoPackets[i];

				ASSERT(packet->PacketID() != CNetDDEPacket::ASYNC_PACKET_ID);

				// Packet ID we're after?
				if (packet->PacketID() == nPacketID)
				{
					Core::eraseAt(m_aoPackets, i);

					return packet;
				}
			}
		}

		// Handle any socket messages.
		CWinSock::ProcessSocketMsgs();

		::Sleep(1);
	}

	throw CSocketException(CSocketException::E_WAIT_FAILED, WSAETIMEDOUT);
}
