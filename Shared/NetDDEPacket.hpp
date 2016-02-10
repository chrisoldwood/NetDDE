/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDEPACKET.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CNetDDEPacket class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef NETDDEPACKET_HPP
#define NETDDEPACKET_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/Buffer.hpp>

/******************************************************************************
** 
** The data packet passed between the client and server.
**
*******************************************************************************
*/

class CNetDDEPacket
{
public:
	//
	// Constructors/Destructor.
	//
	CNetDDEPacket();
	CNetDDEPacket(uint nDataType);
	CNetDDEPacket(uint nDataType, const CBuffer& oBuffer);
	CNetDDEPacket(uint nDataType, uint nPacketID, const CBuffer& oBuffer);
	CNetDDEPacket(const CNetDDEPacket& oPacket);
	virtual ~CNetDDEPacket();
	
	//
	// Packet buffer methods.
	//
	const CBuffer& Buffer() const;
	CBuffer&       Buffer();

	//
	// Packet data methods.
	//
	uint        DataSize() const;
	uint        DataType() const;
	uint        PacketID() const;
	const void* DataBuffer() const;

	/**************************************************************************
	** The NetDDE message types.
	*/

	enum DataTypes
	{
		// Packet type bitmasks.
		PACKET_SYNC_MASK			= 0xF000,
		PACKET_TYPE_MASK			= 0x0FFF,

		// Packet handling type.
		SYNC_PACKET					= 0x0000,
		ASYNC_PACKET				= 0xF000,

		// Client -> Server internal packets.
		NETDDE_CLIENT_CONNECT		= (SYNC_PACKET | 0x0010),
		NETDDE_CLIENT_DISCONNECT	= (SYNC_PACKET | 0x0011),

		// Client -> Server DDE request packets.
		DDE_CREATE_CONVERSATION		= (SYNC_PACKET | 0x0020),
		DDE_DESTROY_CONVERSATION	= (SYNC_PACKET | 0x0021),
		DDE_REQUEST					= (SYNC_PACKET | 0x0022),
		DDE_START_ADVISE			= (SYNC_PACKET | 0x0023),
		DDE_STOP_ADVISE				= (SYNC_PACKET | 0x0024),
		DDE_EXECUTE					= (SYNC_PACKET | 0x0025),
		DDE_POKE					= (SYNC_PACKET | 0x0026),

		// Server -> Client DDE notification packets.
		NETDDE_SERVER_DISCONNECT	= (ASYNC_PACKET | 0x0030),
		DDE_DISCONNECT				= (ASYNC_PACKET | 0x0031),
		DDE_ADVISE					= (ASYNC_PACKET | 0x0032),
		DDE_START_ADVISE_FAILED		= (ASYNC_PACKET | 0x0033),
	};

	/**************************************************************************
	** Packet header.
	*/

	struct Header
	{
		uint m_nDataSize;	// Size of entire packet.
		uint m_nDataType;	// Message type.
		uint m_nPacketID;	// Unique packet ID.
	};

	// Packet ID for async packets.
	static const uint ASYNC_PACKET_ID = UINT_MAX;

protected:
	//
	// Members.
	//
	CBuffer	m_oBuffer;

	//
	// Class members.
	//
	static uint s_nNextPktID;

	//
	// Internal methods.
	//
	void    Create(uint nDataType, uint nPacketID, const void* pData, size_t nDataSize);

	const Header* GetHeader() const;
	Header*       GetHeader();

	static uint GeneratePktID();
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

inline CNetDDEPacket::CNetDDEPacket()
	: m_oBuffer(sizeof(Header))
{
}

inline CNetDDEPacket::CNetDDEPacket(uint nDataType)
{
	uint nPacketID = ASYNC_PACKET_ID;

	if ((nDataType & PACKET_SYNC_MASK) == SYNC_PACKET)
		nPacketID = GeneratePktID();

	Create(nDataType, nPacketID, nullptr, 0);
}

inline CNetDDEPacket::CNetDDEPacket(uint nDataType, const CBuffer& oBuffer)
{
	uint nPacketID = ASYNC_PACKET_ID;

	if ((nDataType & PACKET_SYNC_MASK) == SYNC_PACKET)
		nPacketID = GeneratePktID();

	Create(nDataType, nPacketID, oBuffer.Buffer(), oBuffer.Size());
}

inline CNetDDEPacket::CNetDDEPacket(uint nDataType, uint nPacketID, const CBuffer& oBuffer)
{
	Create(nDataType, nPacketID, oBuffer.Buffer(), oBuffer.Size());
}

inline CNetDDEPacket::CNetDDEPacket(const CNetDDEPacket& oPacket)
	: m_oBuffer(oPacket.m_oBuffer)
{
}

inline CNetDDEPacket::~CNetDDEPacket()
{
}

inline const CBuffer& CNetDDEPacket::Buffer() const
{
	return m_oBuffer;
}

inline CBuffer& CNetDDEPacket::Buffer()
{
	return m_oBuffer;
}

inline uint CNetDDEPacket::DataSize() const
{
	return GetHeader()->m_nDataSize;
}

inline uint CNetDDEPacket::DataType() const
{
	return GetHeader()->m_nDataType;
}

inline uint CNetDDEPacket::PacketID() const
{
	return GetHeader()->m_nPacketID;
}

inline const void* CNetDDEPacket::DataBuffer() const
{
	ASSERT(m_oBuffer.Buffer() != nullptr);

	const byte* pBuffer = static_cast<const byte*>(m_oBuffer.Buffer());

	return pBuffer + sizeof(Header);
}

inline const CNetDDEPacket::Header* CNetDDEPacket::GetHeader() const
{
	ASSERT(m_oBuffer.Buffer() != nullptr);

	return static_cast<const Header*>(m_oBuffer.Buffer());
}

inline CNetDDEPacket::Header* CNetDDEPacket::GetHeader()
{
	ASSERT(m_oBuffer.Buffer() != nullptr);

	return static_cast<Header*>(m_oBuffer.Buffer());
}

inline uint CNetDDEPacket::GeneratePktID()
{
	uint nPacketID = s_nNextPktID++;

	if (nPacketID == ASYNC_PACKET_ID)
		nPacketID = s_nNextPktID++;

	return nPacketID;
}

#endif // NETDDEPACKET_HPP
