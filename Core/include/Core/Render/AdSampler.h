#pragma once
#include "Platform/Graphic/AdVKCommon.h"

namespace ade
{
	class AdSampler
	{
	public:
		AdSampler(VkFilter filter = VK_FILTER_LINEAR, VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);
		~AdSampler();

		VkSampler GetHandle()const
		{
			return mHandle;
		}
	private:
		VkSampler mHandle = VK_NULL_HANDLE;

		VkFilter mFilter;
		VkSamplerAddressMode mAddressMode;
	};
}