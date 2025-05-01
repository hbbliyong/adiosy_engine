#include "Core/ESC/AdUUID.h"
#include <random>
namespace ade
{
	// ��̬��Ա�������������ڣ�
	static std::random_device s_RandomDevice;// ������豸�������������ӣ�
	static std::mt19937_64 s_Engine(s_RandomDevice);// 64λ Mersenne Twister ���������
	static std::uniform_int_distribution<uint32_t> s_UniformDistribution(1, UINT32_MAX); // ���ȷֲ�������
	AdUUID::AdUUID():mUUID(s_UniformDistribution(s_Engine))
	{
	}
	AdUUID::AdUUID(uint32_t uuid):mUUID(uuid)
	{
	}
}