#pragma once
#include "Graphic/AdVKCommon.h"
namespace ade
{
	class AdVKDevice;
	class AdVKImage
	{
	public:
		AdVKImage(AdVKDevice* device, VkFormat format,VkExtent3D extent,VkImageUsageFlags usage);
		AdVKImage(AdVKDevice* device, VkFormat format,VkImage image,    VkImageUsageFlags usage);
		~AdVKImage();

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
