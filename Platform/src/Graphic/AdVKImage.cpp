#include "Graphic/AdVKImage.h"
#include "Graphic/AdVKDevice.h"

namespace ade
{
	AdVKImage::AdVKImage(AdVKDevice* device, VkFormat format, VkExtent3D extent, VkImageUsageFlags usage) :
	mDevice(device),mFormat(format),mExtent(extent),mUsage(usage)
	{
		VkImageTiling tiling = VK_IMAGE_TILING_LINEAR;
		bool isDepthStencilFormat = IsDepthStencilFormat(format);
		if (isDepthStencilFormat)
			tiling = VK_IMAGE_TILING_OPTIMAL;

		VkImageCreateInfo imageInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = format,
			.extent = extent,
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = tiling,
			.usage = usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
		};
		CALL_VK(vkCreateImage(device->GetHandle(), &imageInfo, nullptr, &mHandle));

		//allocate memory
		VkMemoryRequirements memReqs;
		vkGetImageMemoryRequirements(mDevice->GetHandle(), mHandle, &memReqs);

		VkMemoryAllocateInfo allocateInfo = {
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = nullptr,
			.allocationSize = memReqs.size,
			.memoryTypeIndex = static_cast<uint32_t>(mDevice->GetMemoryIndex(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,memReqs.memoryTypeBits))
		};
		CALL_VK(vkAllocateMemory(mDevice->GetHandle(), &allocateInfo, nullptr, &mMemory));
		CALL_VK(vkBindImageMemory(mDevice->GetHandle(), mHandle, mMemory, 0));
	}

	AdVKImage::AdVKImage(AdVKDevice* device, VkFormat format, VkImage image, VkImageUsageFlags usage) :
		mHandle(image),mDevice(device), mFormat(format), bCreateImage(false)
	{

	}

	AdVKImage::~AdVKImage()
	{
		if (bCreateImage)
		{
			VK_D(Image, mDevice->GetHandle(), mHandle);
			VK_F(mDevice->GetHandle(), mMemory);
		}
	}
}
