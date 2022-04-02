////////////////////////////////////////////////////////////////////////////////
//! \file   PacketCodec.cpp
//! \brief  Functions for encoding and decoding packets.
//! \author Chris Oldwood

#include "Common.hpp"
#include "PacketCodec.hpp"
#include <WCL/MemStream.hpp>

namespace NetDDE
{

////////////////////////////////////////////////////////////////////////////////
//! Format the client disconnect packet.

NetDDEPacketPtr EncodeClientDisconnectPacket(const CString& serviceName,
                                             const CString& computerName)
{
	CBuffer    buffer;
	CMemStream stream(buffer);

	stream.Create();

	stream << serviceName;
	stream << computerName;

	stream.Close();

	return NetDDEPacketPtr(new CNetDDEPacket(CNetDDEPacket::NETDDE_CLIENT_DISCONNECT, buffer));
}

////////////////////////////////////////////////////////////////////////////////
//! Decode a client disconnect packet.

void DecodeClientDisconnectPacket(CNetDDEPacket& packet,
                                  CString& serviceName,
                                  CString& computerName)
{
	ASSERT(packet.DataType() == CNetDDEPacket::NETDDE_CLIENT_DISCONNECT);

	CMemStream stream(packet.Buffer());

	stream.Open();
	stream.Seek(sizeof(CNetDDEPacket::Header));

	stream >> serviceName;
	stream >> computerName;

	ASSERT(stream.IsEOF());
	stream.Close();
}

//namespace NetDDE
}
