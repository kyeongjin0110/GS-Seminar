#include "PacketFactory.h"

GamePacket PacketFactory::createLoginPacket(const std::string& name)
{
	flatbuffers::FlatBufferBuilder builder;

	auto fname = builder.CreateString(name);
	auto name_data = Packets::CreateLoginPacket(builder, fname);
	builder.Finish(name_data);

	return GamePacket(builder, kLogin);
}

GamePacket PacketFactory::createOkayPacket(int id)
{
	flatbuffers::FlatBufferBuilder builder;

	auto id_data = Packets::CreateOkayPacket(builder, id);
	builder.Finish(id_data);

	return GamePacket(builder, kOkay);
}

GamePacket PacketFactory::createMessagePacket(const std::string& name, const std::string& message)
{
	flatbuffers::FlatBufferBuilder builder;

	auto fname = builder.CreateString(name);
	auto fmessage = builder.CreateString(message);
	auto message_data = Packets::CreateMessagePacket(builder, fname, fmessage);
	builder.Finish(message_data);

	return GamePacket(builder, kMessage);
}

