/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDEPACKET.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CNetDDEPacket class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "NetDDEPacket.hpp"

/******************************************************************************
**
** Class members.
**
*******************************************************************************
*/

uint CNetDDEPacket::s_nNextPktID = 1;

/******************************************************************************
** Method:		Create()
**
** Description:	Internal method to create the entire packet.
**
** Parameters:	nDataType	The packet type.
**				nPacketID	The packet ID.
**				pData		The data.
**				nDataSize	The size of the data.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDEPacket::Create(uint nDataType, uint nPacketID, const void* pData, size_t nDataSize)
{
	ASSERT((pData != nullptr) || (nDataSize == 0));

	// Allocate memory for the entire packet.
	m_oBuffer.Size(sizeof(Header) + nDataSize);

	Header* pHeader = GetHeader();

	// Set the packet header.
	pHeader->m_nDataSize = static_cast<uint>(nDataSize);
	pHeader->m_nDataType = nDataType;
	pHeader->m_nPacketID = nPacketID;

	// Set the packet data.
	if (nDataSize > 0)
		m_oBuffer.Set(pData, nDataSize, sizeof(Header));
}
