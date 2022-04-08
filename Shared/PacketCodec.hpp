////////////////////////////////////////////////////////////////////////////////
//! \file   PacketCodec.hpp
//! \brief  Functions for encoding and decoding packets.
//! \author Chris Oldwood

// Check for previous inclusion
#ifndef APP_PACKETCODEC_HPP
#define APP_PACKETCODEC_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetDDEPacket.hpp"

namespace NetDDE
{

////////////////////////////////////////////////////////////////////////////////
//! Encode a create conversation packet.

NetDDEPacketPtr EncodeCreateConversationPacket(const CString& serviceName,
                                               const tchar* topic);

////////////////////////////////////////////////////////////////////////////////
//! Decode a create conversation packet.

void DecodeCreateConversationPacket(CNetDDEPacket& packet,
                                    CString& service,
                                    CString& topic);

////////////////////////////////////////////////////////////////////////////////
//! Encode a create conversation reply packet.

NetDDEPacketPtr EncodeCreateConversationReplyPacket(uint packetID,
                                                    bool result,
                                                    HCONV conversationHandle,
                                                    uint32 conversationID);

////////////////////////////////////////////////////////////////////////////////
//! Encode a destroy conversation packet.

NetDDEPacketPtr EncodeDestroyConversationPacket(HCONV conversationHandle,
                                                uint32 conversationID);

////////////////////////////////////////////////////////////////////////////////
//! Decode a destroy conversation packet.

void DecodeDestroyConversationPacket(CNetDDEPacket& packet,
                                     HCONV& conversationHandle,
                                     uint32& conversationID);

////////////////////////////////////////////////////////////////////////////////
//! Encode a conversation disconnect packet.

NetDDEPacketPtr EncodeConversationDisconnectPacket(HCONV conversationHandle);

////////////////////////////////////////////////////////////////////////////////
//! Encode an item request packet.

NetDDEPacketPtr EncodeRequestItemPacket(HCONV conversationHandle,
                                        uint32 conversationID,
                                        const tchar* item,
                                        uint format);

////////////////////////////////////////////////////////////////////////////////
//! Decode an item request packet.

void DecodeRequestItemPacket(CNetDDEPacket& packet,
                             HCONV& conversationHandle,
                             uint32& conversationID,
                             CString& item,
                             uint32& format);

////////////////////////////////////////////////////////////////////////////////
//! Encode an item request reply packet.

NetDDEPacketPtr EncodeRequestItemReplyPacket(uint packetID,
                                             bool result,
                                             const CBuffer& data);

////////////////////////////////////////////////////////////////////////////////
//! Encode a start advise packet.

NetDDEPacketPtr EncodeStartAdvisePacket(HCONV conversationHandle,
                                        uint32 conversationID,
                                        const tchar* item,
                                        uint format,
                                        bool asynchronous,
                                        bool requestInitialValue);

////////////////////////////////////////////////////////////////////////////////
//! Decode a start advise packet.

void DecodeStartAdvisePacket(CNetDDEPacket& packet,
                             HCONV& conversationHandle,
                             uint32& conversationID,
                             CString& item,
                             uint32& format,
                             bool& async,
                             bool& requestValue);

////////////////////////////////////////////////////////////////////////////////
//! Encode a start advise reply packet.

NetDDEPacketPtr EncodeStartAdviseReplyPacket(uint packetID,
                                             bool result);

////////////////////////////////////////////////////////////////////////////////
//! Encode an advise start failed packet.

NetDDEPacketPtr EncodeAdviseStartFailedPacket(HCONV conversationHandle,
                                              const CString& item,
                                              uint format);

////////////////////////////////////////////////////////////////////////////////
//! Encode an advise packet.

NetDDEPacketPtr EncodeAdvisePacket(HCONV conversationHandle,
                                   const CString& item,
                                   uint format,
                                   const CBuffer& data);

////////////////////////////////////////////////////////////////////////////////
//! Encode a stop advise packet.

NetDDEPacketPtr EncodeStopAdvisePacket(HCONV conversationHandle,
                                       uint32 conversationID,
                                       const CString& item,
                                       uint format);

////////////////////////////////////////////////////////////////////////////////
//! Decode a stop advise packet.

void DecodeStopAdvisePacket(CNetDDEPacket& packet,
                            HCONV& conversationHandle,
                            uint32& conversationID,
                            CString& item,
                            uint32& format);

////////////////////////////////////////////////////////////////////////////////
//! Encode an execute command packet.

NetDDEPacketPtr EncodeExecuteCommandPacket(HCONV conversationHandle,
                                           uint32 conversationID,
                                           const CString& command);

////////////////////////////////////////////////////////////////////////////////
//! Decode an execute command packet.

void DecodeExecuteCommandPacket(CNetDDEPacket& packet,
                                HCONV& conversationHandle,
                                uint32& conversationID,
                                CString& command);

////////////////////////////////////////////////////////////////////////////////
//! Encode an execute command reply packet.

NetDDEPacketPtr EncodeExecuteCommandReplyPacket(uint packetID,
                                                bool result);

////////////////////////////////////////////////////////////////////////////////
//! Encode an item poke packet.

NetDDEPacketPtr EncodePokeItemPacket(HCONV conversationHandle,
                                     uint32 conversationID,
                                     const tchar* item,
                                     uint format,
                                     const CBuffer& data);

////////////////////////////////////////////////////////////////////////////////
//! Decode an item poke packet.

void DecodePokeItemPacket(CNetDDEPacket& packet,
                          HCONV& conversationHandle,
                          uint32& conversationID,
                          CString& item,
                          uint32& format,
                          CBuffer& data);

////////////////////////////////////////////////////////////////////////////////
//! Encode an item poke reply packet.

NetDDEPacketPtr EncodePokeItemReplyPacket(uint packetID,
                                          bool result);

////////////////////////////////////////////////////////////////////////////////
//! Encode a client connect packet.

NetDDEPacketPtr EncodeClientConnectPacket(const CString& serviceName);

////////////////////////////////////////////////////////////////////////////////
//! Decode a client connect packet.

void DecodeClientConnectPacket(CNetDDEPacket& packet,
                               uint16& protocol,
                               CString& service,
                               CString& computer,
                               CString& user,
                               CString& process,
                               CString& version);

////////////////////////////////////////////////////////////////////////////////
//! Encode a client connect reply packet.

NetDDEPacketPtr EncodeClientConnectReplyPacket(uint packetID,
                                               bool result);

////////////////////////////////////////////////////////////////////////////////
//! Encode a client disconnect packet.

NetDDEPacketPtr EncodeClientDisconnectPacket(const CString& serviceName,
                                             const CString& computerName);

////////////////////////////////////////////////////////////////////////////////
//! Decode a client disconnect packet.

void DecodeClientDisconnectPacket(CNetDDEPacket& packet,
                                  CString& serviceName,
                                  CString& computerName);

//namespace NetDDE
}

#endif // APP_PACKETCODEC_HPP
