#include "Core/ESC/AdUUID.h"
#include <random>
namespace ade
{
	// 静态成员变量声明（类内）
	static std::random_device s_RandomDevice;// 随机数设备（用于生成种子）
	static std::mt19937_64 s_Engine(s_RandomDevice);// 64位 Mersenne Twister 随机数引擎
	static std::uniform_int_distribution<uint32_t> s_UniformDistribution(1, UINT32_MAX); // 均匀分布生成器
	AdUUID::AdUUID():mUUID(s_UniformDistribution(s_Engine))
	{
	}
	AdUUID::AdUUID(uint32_t uuid):mUUID(uuid)
	{
	}
}