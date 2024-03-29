////////////////////////////////////////////////////////////////////////////////
//! \file   Pa.cpp
//! \brief  The unit tests for the packet encoding/decoding functions.
//! \author Chris Oldwood

#include "Common.hpp"
#include <Core/UnitTest.hpp>
#include "PacketCodec.hpp"
#include <Core/AnsiWide.hpp>
#include <WCL/MemStream.hpp>

using namespace NetDDE;

namespace
{

size_t calcStringDataSizeInBytes(const tchar* string)
{
	return Core::numBytes<char>(tstrlen(string)+1);
}

size_t calcStringFieldLengthInBytes(const tchar* string)
{
	return sizeof(uint32) + calcStringDataSizeInBytes(string);
}

size_t calcStringDataSizeInBytes(const CString& string)
{
	return Core::numBytes<char>(string.Capacity());
}

size_t calcStringFieldLengthInBytes(const CString& string)
{
	return sizeof(uint32) + calcStringDataSizeInBytes(string);
}

}

TEST_SET(PacketCodec)
{

TEST_CASE("The client disconnect packet can be encoded")
{
	const CString serviceName(TXT("ServiceName"));
	const CString computerName(TXT("ComputerName"));

	const NetDDEPacketPtr packet = EncodeClientDisconnectPacket(serviceName, computerName);

	TEST_TRUE(packet->DataType() == CNetDDEPacket::NETDDE_CLIENT_DISCONNECT);
	TEST_TRUE(packet->PacketID() != CNetDDEPacket::ASYNC_PACKET_ID);

	const size_t packetDataSize = calcStringFieldLengthInBytes(serviceName)
                                + calcStringFieldLengthInBytes(computerName);
	TEST_TRUE(packet->DataSize() == packetDataSize);

	CBuffer buffer(packet->Buffer());
	CMemStream stream(buffer);
	stream.Open();
	stream.Seek(sizeof(CNetDDEPacket::Header));
	
	uint32 capacity;
	CBuffer string(packet->Buffer().Size());

	stream >> capacity;
	TEST_TRUE(capacity == serviceName.Capacity());
	stream.Read(buffer.Buffer(), calcStringDataSizeInBytes(serviceName));
	TEST_TRUE(strcmp(T2A(serviceName), reinterpret_cast<const char*>(buffer.Buffer())) == 0);	

	stream >> capacity;
	TEST_TRUE(capacity == computerName.Capacity());
	stream.Read(buffer.Buffer(), calcStringDataSizeInBytes(computerName));
	TEST_TRUE(strcmp(T2A(computerName), reinterpret_cast<const char*>(buffer.Buffer())) == 0);	

	TEST_TRUE(stream.IsEOF());
	stream.Close();
}
TEST_CASE_END

TEST_CASE("The client disconnect packet can be decoded")
{
	const char* serviceNameSent = "ServiceName";
	const char* computerNameSent = "ComputerName";

	CBuffer buffer;
	CMemStream stream(buffer);
	stream.Create();

	uint32 capacity = static_cast<uint32>(strlen(serviceNameSent)+1);
	stream << capacity;
	stream.Write(serviceNameSent, Core::numBytes<char>(capacity));
	
	capacity = static_cast<uint32>(strlen(computerNameSent)+1);
	stream << capacity;
	stream.Write(computerNameSent, Core::numBytes<char>(capacity));

	stream.Close();
	NetDDEPacketPtr packet = NetDDEPacketPtr(new CNetDDEPacket(CNetDDEPacket::NETDDE_CLIENT_DISCONNECT, buffer));

	CString serviceNameReceived;
	CString computerNameReceived;

	DecodeClientDisconnectPacket(packet, serviceNameReceived, computerNameReceived);

	TEST_TRUE(strcmp(T2A(serviceNameReceived), serviceNameSent) == 0);
	TEST_TRUE(strcmp(T2A(computerNameReceived), computerNameSent) == 0);
}
TEST_CASE_END

TEST_CASE("A client disconnect packet can be round-tripped")
{
	const CString serviceNameSent(TXT("ServiceName"));
	const CString computerNameSent(TXT("ComputerName"));

	const NetDDEPacketPtr packet = EncodeClientDisconnectPacket(serviceNameSent, computerNameSent);

	CString serviceNameReceived;
	CString computerNameReceived;

	DecodeClientDisconnectPacket(packet, serviceNameReceived, computerNameReceived);

	TEST_TRUE(serviceNameReceived == serviceNameSent);
	TEST_TRUE(computerNameReceived == computerNameSent);
}
TEST_CASE_END

}
TEST_SET_END
