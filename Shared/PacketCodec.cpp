////////////////////////////////////////////////////////////////////////////////
//! \file   PacketCodec.cpp
//! \brief  Functions for encoding and decoding packets.
//! \author Chris Oldwood

#include "Common.hpp"
#include "PacketCodec.hpp"
#include "NetDDEDefs.hpp"
#include <WCL/MemStream.hpp>
#include <WCL/Path.hpp>
#include <WCL/SysInfo.hpp>
#include <WCL/VerInfoReader.hpp>

namespace NetDDE
{

////////////////////////////////////////////////////////////////////////////////
//! Get the application version number from the resource file.

CString GetAppVersion()
{
	// Extract details from the resources.
	tstring filename  = CPath::Application();
	tstring version   = WCL::VerInfoReader::GetStringValue(filename, WCL::VerInfoReader::PRODUCT_VERSION);

#ifdef _DEBUG
	version += TXT(" [Debug]");
#endif

	return version.c_str();
}

////////////////////////////////////////////////////////////////////////////////
//! Encode a create conversation packet.

NetDDEPacketPtr EncodeCreateConversationPacket(const CString& serviceName,
                                               const tchar* topic)
{
	CBuffer    buffer;
	CMemStream stream(buffer);

	stream.Create();

	stream << serviceName;
	stream << topic;

	stream.Close();

	return NetDDEPacketPtr(new CNetDDEPacket(CNetDDEPacket::DDE_CREATE_CONVERSATION, buffer));
}

////////////////////////////////////////////////////////////////////////////////
//! Encode a create conversation reply packet.

NetDDEPacketPtr EncodeCreateConversationReplyPacket(uint packetID,
                                                    bool result,
                                                    HCONV conversationHandle,
                                                    uint32 conversationID)
{
	CBuffer    buffer;
	CMemStream stream(buffer);

	stream.Create();

	stream << result;
	stream.Write(&conversationHandle, sizeof(HCONV));
	stream << conversationID;

	stream.Close();

	return NetDDEPacketPtr(new CNetDDEPacket(CNetDDEPacket::DDE_CREATE_CONVERSATION, packetID, buffer));
}

////////////////////////////////////////////////////////////////////////////////
//! Encode a destroy conversation packet.

NetDDEPacketPtr EncodeDestroyConversationPacket(HCONV conversationHandle,
                                                uint32 conversationID)
{
	CBuffer    buffer;
	CMemStream stream(buffer);

	stream.Create();

	stream.Write(&conversationHandle, sizeof(HCONV));
	stream << conversationID;

	stream.Close();

	return NetDDEPacketPtr(new CNetDDEPacket(CNetDDEPacket::DDE_DESTROY_CONVERSATION, buffer));
}

////////////////////////////////////////////////////////////////////////////////
//! Encode a conversation disconnect packet.

NetDDEPacketPtr EncodeConversationDisconnectPacket(HCONV conversationHandle)
{
	CBuffer    buffer;
	CMemStream stream(buffer);

	stream.Create();
	stream.Write(&conversationHandle, sizeof(HCONV));
	stream.Close();

	return NetDDEPacketPtr(new CNetDDEPacket(CNetDDEPacket::DDE_DISCONNECT, buffer));
}

////////////////////////////////////////////////////////////////////////////////
//! Encode an item request packet.

NetDDEPacketPtr EncodeRequestItemPacket(HCONV conversationHandle,
                                        uint32 conversationID,
                                        const tchar* item,
                                        uint format)
{
	CBuffer    buffer;
	CMemStream stream(buffer);

	stream.Create();

	stream.Write(&conversationHandle, sizeof(HCONV));
	stream << conversationID;
	stream << item;
	stream << (uint32) format;

	stream.Close();

	return NetDDEPacketPtr(new CNetDDEPacket(CNetDDEPacket::DDE_REQUEST, buffer));
}

////////////////////////////////////////////////////////////////////////////////
//! Encode an item request reply packet.

NetDDEPacketPtr EncodeRequestItemReplyPacket(uint packetID,
                                             bool result,
                                             const CBuffer& data)
{
	CBuffer    buffer;
	CMemStream stream(buffer);

	stream.Create();

	stream << result;
	stream << data;

	stream.Close();

	return NetDDEPacketPtr(new CNetDDEPacket(CNetDDEPacket::DDE_REQUEST, packetID, buffer));
}

////////////////////////////////////////////////////////////////////////////////
//! Encode a start advise packet.

NetDDEPacketPtr EncodeStartAdvisePacket(HCONV conversationHandle,
                                        uint32 conversationID,
                                        const tchar* item,
                                        uint format,
                                        bool asynchronous,
                                        bool requestInitialValue)
{
	CBuffer    buffer;
	CMemStream stream(buffer);

	stream.Create();

	stream.Write(&conversationHandle, sizeof(HCONV));
	stream << conversationID;
	stream << item;
	stream << (uint32) format;
	stream << asynchronous;
	stream << requestInitialValue;

	stream.Close();

	return NetDDEPacketPtr(new CNetDDEPacket(CNetDDEPacket::DDE_START_ADVISE, buffer));
}

////////////////////////////////////////////////////////////////////////////////
//! Encode a start advise reply packet.

NetDDEPacketPtr EncodeStartAdviseReplyPacket(uint packetID,
                                             bool result)
{
	CBuffer    buffer;
	CMemStream stream(buffer);

	stream.Create();

	stream << result;

	stream.Close();

	return NetDDEPacketPtr(new CNetDDEPacket(CNetDDEPacket::DDE_START_ADVISE, packetID, buffer));
}

////////////////////////////////////////////////////////////////////////////////
//! Encode an advise start failed packet.

NetDDEPacketPtr EncodeAdviseStartFailedPacket(HCONV conversationHandle,
                                              const CString& item,
                                              uint format)
{
	CBuffer    buffer;
	CMemStream stream(buffer);

	stream.Create();

	stream.Write(&conversationHandle, sizeof(HCONV));
	stream << item;
	stream << (uint32) format;
	stream << true;

	stream.Close();

	return NetDDEPacketPtr(new CNetDDEPacket(CNetDDEPacket::DDE_START_ADVISE_FAILED, buffer));
}

////////////////////////////////////////////////////////////////////////////////
//! Encode an advise packet.

NetDDEPacketPtr EncodeAdvisePacket(HCONV conversationHandle,
                                   const CString& item,
                                   uint format,
                                   const CBuffer& data)
{
	CBuffer    buffer;
	CMemStream stream(buffer);

	stream.Create();

	stream.Write(&conversationHandle, sizeof(HCONV));
	stream << item;
	stream << (uint32) format;
	stream << data;
	stream << true;

	stream.Close();

	return NetDDEPacketPtr(new CNetDDEPacket(CNetDDEPacket::DDE_ADVISE, buffer));
}

////////////////////////////////////////////////////////////////////////////////
//! Encode a stop advise packet.

NetDDEPacketPtr EncodeStopAdvisePacket(HCONV conversationHandle,
                                       uint32 conversationID,
                                       const CString& item,
                                       uint format)
{
	CBuffer    buffer;
	CMemStream stream(buffer);

	stream.Create();

	stream.Write(&conversationHandle, sizeof(HCONV));
	stream << conversationID;
	stream << item;
	stream << (uint32) format;

	stream.Close();

	return NetDDEPacketPtr(new CNetDDEPacket(CNetDDEPacket::DDE_STOP_ADVISE, buffer));
}

////////////////////////////////////////////////////////////////////////////////
//! Encode an execute command packet.

NetDDEPacketPtr EncodeExecuteCommandPacket(HCONV conversationHandle,
                                           uint32 conversationID,
                                           const CString& command)
{
	CBuffer    buffer;
	CMemStream stream(buffer);

	stream.Create();

	stream.Write(&conversationHandle, sizeof(HCONV));
	stream << conversationID;
	stream << command;

	stream.Close();

	return NetDDEPacketPtr(new CNetDDEPacket(CNetDDEPacket::DDE_EXECUTE, buffer));
}

////////////////////////////////////////////////////////////////////////////////
//! Encode an execute command reply packet.

NetDDEPacketPtr EncodeExecuteCommandReplyPacket(uint packetID,
                                                bool result)
{
	CBuffer    buffer;
	CMemStream stream(buffer);

	stream.Create();

	stream << result;

	stream.Close();

	return NetDDEPacketPtr(new CNetDDEPacket(CNetDDEPacket::DDE_EXECUTE, packetID, buffer));
}

////////////////////////////////////////////////////////////////////////////////
//! Encode an item poke packet.

NetDDEPacketPtr EncodePokeItemPacket(HCONV conversationHandle,
                                     uint32 conversationID,
                                     const tchar* item,
                                     uint format,
                                     const CBuffer& data)
{
	CBuffer    buffer;
	CMemStream stream(buffer);

	stream.Create();

	stream.Write(&conversationHandle, sizeof(HCONV));
	stream << conversationID;
	stream << item;
	stream << (uint32) format;
	stream << data;

	stream.Close();

	return NetDDEPacketPtr(new CNetDDEPacket(CNetDDEPacket::DDE_POKE, buffer));
}

////////////////////////////////////////////////////////////////////////////////
//! Encode an item poke reply packet.

NetDDEPacketPtr EncodePokeItemReplyPacket(uint packetID,
                                          bool result)
{
	CBuffer    buffer;
	CMemStream stream(buffer);

	stream.Create();

	stream << result;

	stream.Close();

	return NetDDEPacketPtr(new CNetDDEPacket(CNetDDEPacket::DDE_POKE, packetID, buffer));
}

////////////////////////////////////////////////////////////////////////////////
//! Encode a client connect packet.

NetDDEPacketPtr EncodeClientConnectPacket(const CString& serviceName)
{
	CBuffer    buffer;
	CMemStream stream(buffer);

	stream.Create();

	stream << NETDDE_PROTOCOL;
	stream << serviceName;
	stream << CSysInfo::ComputerName();
	stream << CSysInfo::UserName();
	stream << CPath::Application().FileName();
	stream << GetAppVersion();

	stream.Close();

	return NetDDEPacketPtr(new CNetDDEPacket(CNetDDEPacket::NETDDE_CLIENT_CONNECT, buffer));
}

////////////////////////////////////////////////////////////////////////////////
//! Encode a client connect reply packet.

NetDDEPacketPtr EncodeClientConnectReplyPacket(uint packetID,
                                               bool result)
{
	CBuffer    buffer;
	CMemStream stream(buffer);

	stream.Create();

	stream << result;
	stream << GetAppVersion();

	stream.Close();

	return NetDDEPacketPtr(new CNetDDEPacket(CNetDDEPacket::NETDDE_CLIENT_CONNECT, packetID, buffer));
}

////////////////////////////////////////////////////////////////////////////////
//! Encode the client disconnect packet.

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
