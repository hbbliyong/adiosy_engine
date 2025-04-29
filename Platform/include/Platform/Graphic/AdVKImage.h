#pragma once
#include "Graphic/AdVKCommon.h"
namespace ade
{
	class AdVKDevice;
	class AdVKBuffer;
	class AdVKImage
	{
	public:
		AdVKImage(AdVKDevice* device, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);
		AdVKImage(AdVKDevice* device, VkImage image, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);
		~AdVKImage();

		void CopyFromBuffer(VkCommandBuffer cmdBuffer, AdVKBuffer* buffer);

		VkFormat GetFormat()const { return mFormat; }
		VkImage GetHandle()const { return mHandle; }
	private:
		VkImage mHandle=VK_NULL_HANDLE;
		VkDeviceMemory mMemory= VK_NULL_HANDLE;
		VkFormat mFormat;
		AdVKDevice* mDevice;
		bool bCreateImage = true;

		VkExtent3D mExtent;
		VkImageUsageFlags mUsage;
   };
} // namespace ade
