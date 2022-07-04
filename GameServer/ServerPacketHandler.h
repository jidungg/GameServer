#pragma once

struct BuffData
{
	uint64 buffId;
	float remainTime;
};
enum
{
	S_TEST = 1,
};
class ServerPacketHandler
{
public:
	static void HandlePacket(BYTE* buffer, int32 len);
	static SendBufferRef Make_S_TEST(uint32 id, uint32 hp, uint16 attack, vector<BuffData> buffs);
};

