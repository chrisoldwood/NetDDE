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
	CNetDDEPacket(uint nDataType, const void* pData, uint nDataSize);
	CNetDDEPacket(uint nDataType, const CBuffer& oBuffer);
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
	const void* DataBuffer() const;

	/**************************************************************************
	** The NetDDE message types.
	*/

	enum DataTypes
	{
		// Client -> Server internal packets.
		NETDDE_CLIENT_CONNECT		= 0x0010,
		NETDDE_CLIENT_DISCONNECT	= 0x0011,

		// Client -> Server DDE request packets.
		DDE_CREATE_CONVERSATION		= 0x0020,
		DDE_DESTROY_CONVERSATION	= 0x0021,
		DDE_REQUEST					= 0x0022,
		DDE_START_ADVISE			= 0x0023,
		DDE_STOP_ADVISE				= 0x0024,
		DDE_START_ADVISE_ASYNC		= 0x0025,

		// Server -> Client DDE notification packets.
		NETDDE_SERVER_DISCONNECT	= 0x0030,
		DDE_DISCONNECT				= 0x0031,
		DDE_ADVISE					= 0x0032,
	};

	/**************************************************************************
	** Packet header.
	*/

	struct Header
	{
		uint m_nDataSize;
		uint m_nDataType;
	};

protected:
	//
	// Members.
	//
	CBuffer	m_oBuffer;

	//
	// Internal methods.
	//
	Header* GetHeader() const;
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

inline CNetDDEPacket::CNetDDEPacket(uint nDataType, const void* pData, uint nDataSize)
	: m_oBuffer(sizeof(Header) + nDataSize)
{
	ASSERT((pData != NULL) || (nDataSize == 0));

	Header* pHeader = GetHeader();

	pHeader->m_nDataSize = nDataSize;
	pHeader->m_nDataType = nDataType;

	m_oBuffer.Set(pData, nDataSize, sizeof(Header));
}

inline CNetDDEPacket::CNetDDEPacket(uint nDataType, const CBuffer& oBuffer)
	: m_oBuffer(sizeof(Header) + oBuffer.Size())
{
	Header* pHeader = GetHeader();

	pHeader->m_nDataSize = oBuffer.Size();
	pHeader->m_nDataType = nDataType;

	m_oBuffer.Set(oBuffer.Buffer(), oBuffer.Size(), sizeof(Header));
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

inline const void* CNetDDEPacket::DataBuffer() const
{
	ASSERT(m_oBuffer.Buffer() != NULL);

	byte* pBuffer = (byte*) m_oBuffer.Buffer();

	return pBuffer + sizeof(Header);
}

inline CNetDDEPacket::Header* CNetDDEPacket::GetHeader() const
{
	ASSERT(m_oBuffer.Buffer() != NULL);

	return (Header*) m_oBuffer.Buffer();
}

#endif // NETDDEPACKET_HPP
