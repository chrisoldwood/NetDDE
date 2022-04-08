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
//! Decode a create conversation packet.

void DecodeCreateConversationPacket(CNetDDEPacket& packet,
                                    CString& service,
                                    CString& topic)
{
	CMemStream stream(packet.Buffer());

	stream.Open();
	stream.Seek(sizeof(CNetDDEPacket::Header));

	stream >> service;
	stream >> topic;

	stream.Close();
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
//! Decode a create conversation reply packet.

void DecodeCreateConversationReplyPacket(CNetDDEPacket& packet,
                                         bool& result,
                                         HCONV& conversationHandle,
                                         uint32& conversationID)
{
	CMemStream stream(packet.Buffer());

	stream.Open();
	stream.Seek(sizeof(CNetDDEPacket::Header));

	// Get result.
	stream >> result;
	stream.Read(&conversationHandle, sizeof(HCONV));
	stream >> conversationID;

	stream.Close();
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
//! Decode a destroy conversation packet.

void DecodeDestroyConversationPacket(CNetDDEPacket& packet,
                                     HCONV&  conversationHandle,
                                     uint32& conversationID)
{
	CMemStream stream(packet.Buffer());

	stream.Open();
	stream.Seek(sizeof(CNetDDEPacket::Header));

	stream.Read(&conversationHandle, sizeof(HCONV));
	stream >> conversationID;

	stream.Close();
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
//! Decode a conversation disconnect packet.

void DecodeConversationDisconnectPacket(CNetDDEPacket& packet,
                                        HCONV& conversationHandle)
{
	CMemStream stream(packet.Buffer());

	stream.Open();
	stream.Seek(sizeof(CNetDDEPacket::Header));

	stream.Read(&conversationHandle, sizeof(HCONV));

	stream.Close();
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
//! Decode an item request packet.

void DecodeRequestItemPacket(CNetDDEPacket& packet,
                             HCONV& conversationHandle,
                             uint32& conversationID,
                             CString& item,
                             uint32& format)
{
	CMemStream stream(packet.Buffer());

	stream.Open();
	stream.Seek(sizeof(CNetDDEPacket::Header));

	stream.Read(&conversationHandle, sizeof(HCONV));
	stream >> conversationID;
	stream >> item;
	stream >> format;

	stream.Close();
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
//! Decode an item request reply packet.

void DecodeRequestItemReplyPacket(CNetDDEPacket& packet,
                                  bool& result,
                                  CBuffer& data)
{
	CMemStream stream(packet.Buffer());

	stream.Open();
	stream.Seek(sizeof(CNetDDEPacket::Header));

	stream >> result;
	stream >> data;

	stream.Close();
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
//! Decode a start advise packet.

void DecodeStartAdvisePacket(CNetDDEPacket& packet,
                             HCONV& conversationHandle,
                             uint32& conversationID,
                             CString& item,
                             uint32& format,
                             bool& async,
                             bool& requestValue)
{
	CMemStream stream(packet.Buffer());

	stream.Open();
	stream.Seek(sizeof(CNetDDEPacket::Header));

	stream.Read(&conversationHandle, sizeof(HCONV));
	stream >> conversationID;
	stream >> item;
	stream >> format;
	stream >> async;
	stream >> requestValue;

	stream.Close();
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
//! Decode a start advise reply packet.

void DecodeStartAdviseReplyPacket(CNetDDEPacket& packet,
                                  bool& result)
{
	CMemStream stream(packet.Buffer());

	stream.Open();
	stream.Seek(sizeof(CNetDDEPacket::Header));

	stream >> result;

	stream.Close();
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
//! Decode an advise start failed packet.

void DecodeAdviseStartFailedPacket(CNetDDEPacket& packet,
                                   HCONV& conversationHandle,
                                   CString& item,
                                   uint32& format)
{
	bool endOfPacket = false;
	CMemStream oStream(packet.Buffer());

	oStream.Open();
	oStream.Seek(sizeof(CNetDDEPacket::Header));

	oStream.Read(&conversationHandle, sizeof(HCONV));
	oStream >> item;
	oStream >> format;
	oStream >> endOfPacket;

	// Multiple advises per packet not implemented.
	ASSERT(endOfPacket);

	oStream.Close();
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
//! Decode an advise packet.

void DecodeAdvisePacket(CNetDDEPacket& packet,
                        HCONV& conversationHandle,
                        CString& item,
                        uint32& format,
                        CBuffer& data)
{
	bool endOfPacket = false;
	CMemStream stream(packet.Buffer());

	stream.Open();
	stream.Seek(sizeof(CNetDDEPacket::Header));

	stream.Read(&conversationHandle, sizeof(HCONV));
	stream >> item;
	stream >> format;
	stream >> data;
	stream >> endOfPacket;

	// Multiple advises per packet not implemented.
	ASSERT(endOfPacket);

	stream.Close();
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
//! Decode a stop advise packet.

void DecodeStopAdvisePacket(CNetDDEPacket& packet,
                            HCONV& conversationHandle,
                            uint32& conversationID,
                            CString& item,
                            uint32& format)
{
	CMemStream stream(packet.Buffer());

	stream.Open();
	stream.Seek(sizeof(CNetDDEPacket::Header));

	stream.Read(&conversationHandle, sizeof(HCONV));
	stream >> conversationID;
	stream >> item;
	stream >> format;

	stream.Close();
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
//! Decode an execute command packet.

void DecodeExecuteCommandPacket(CNetDDEPacket& packet,
                                HCONV& conversationHandle,
                                uint32& conversationID,
                                CString& command)
{
	CMemStream stream(packet.Buffer());

	stream.Open();
	stream.Seek(sizeof(CNetDDEPacket::Header));

	stream.Read(&conversationHandle, sizeof(HCONV));
	stream >> conversationID;
	stream >> command;

	stream.Close();
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
//! Decode an execute command reply packet.

void DecodeExecuteCommandReplyPacket(CNetDDEPacket& packet,
                                     bool& result)
{
	CMemStream stream(packet.Buffer());

	stream.Open();
	stream.Seek(sizeof(CNetDDEPacket::Header));

	stream >> result;

	stream.Close();
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
//! Decode an item poke packet.

void DecodePokeItemPacket(CNetDDEPacket& packet,
                          HCONV& conversationHandle,
                          uint32& conversationID,
                          CString& item,
                          uint32& format,
                          CBuffer& data)
{
	CMemStream stream(packet.Buffer());

	stream.Open();
	stream.Seek(sizeof(CNetDDEPacket::Header));

	stream.Read(&conversationHandle, sizeof(HCONV));
	stream >> conversationID;
	stream >> item;
	stream >> format;
	stream >> data;

	stream.Close();
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
//! Decode an item poke reply packet.

void DecodePokeItemReplyPacket(CNetDDEPacket& packet,
                               bool& result)
{
	CMemStream stream(packet.Buffer());

	stream.Open();
	stream.Seek(sizeof(CNetDDEPacket::Header));

	stream >> result;

	stream.Close();
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
//! Decode a client connect packet.

void DecodeClientConnectPacket(CNetDDEPacket& packet,
                               uint16& protocol,
                               CString& service,
                               CString& computer,
                               CString& user,
                               CString& process,
                               CString& version)
{
	CMemStream stream(packet.Buffer());

	stream.Open();
	stream.Seek(sizeof(CNetDDEPacket::Header));

	stream >> protocol;
	stream >> service;
	stream >> computer;
	stream >> user;
	stream >> process;
	stream >> version;

	stream.Close();
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
//! Decode a client connect reply packet.

void DecodeClientConnectReplyPacket(CNetDDEPacket& packet,
                                    bool& result,
                                    CString& version)
{
	CMemStream stream(packet.Buffer());

	stream.Open();
	stream.Seek(sizeof(CNetDDEPacket::Header));

	stream >> result;
	stream >> version;

	stream.Close();
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
