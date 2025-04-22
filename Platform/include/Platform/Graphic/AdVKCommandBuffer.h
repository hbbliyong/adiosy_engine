#pragma once
#include "Graphic/AdVKCommon.h"

namespace ade
{
	class AdVKDevice;
	class AdVKCommandPool
	{
	public:
		AdVKCommandPool(AdVKDevice* device,uint32_t queueFamilyIndex);
		~AdVKCommandPool();

		static void BeginCommandBuffer(VkCommandBuffer cmdBuffer);
		static void EndCommandBuffer(VkCommandBuffer cmdBuffer);

		std::vector<VkCommandBuffer> AllocateCommandBuffer(uint32_t count) const;
		VkCommandPool GetHandel()const { return mHandle; }

	private:
		VkCommandPool mHandle;
		AdVKDevice* mDevice;
	};
}