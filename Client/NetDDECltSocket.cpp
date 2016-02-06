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
	ASSERT(pService != NULL);
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
	Core::deleteAll(m_aoPackets);
}

/******************************************************************************
** Method:		ReadResponsePacket()
**
** Description:	Read the response packet, waiting if required.
**				NB: All other packets types are appended to a queue.
**
** Parameters:	oPacket		The packet received.
**				nPacketID	The packet ID expected.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltSocket::ReadResponsePacket(CNetDDEPacket& oPacket, uint nPacketID)
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
				CNetDDEPacket* pPacket = m_aoPackets[i];

				ASSERT(pPacket->PacketID() != CNetDDEPacket::ASYNC_PACKET_ID);

				// Packet ID we're after?
				if (pPacket->PacketID() == nPacketID)
				{
					oPacket = *pPacket;

					Core::deleteAt(m_aoPackets, i);

					return;
				}
			}
		}

		// Handle any socket messages.
		CWinSock::ProcessSocketMsgs();

		::Sleep(1);
	}

	throw CSocketException(CSocketException::E_WAIT_FAILED, WSAETIMEDOUT);
}
