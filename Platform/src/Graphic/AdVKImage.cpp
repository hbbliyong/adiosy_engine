#include "Graphic/AdVKImage.h"
#include "Graphic/AdVKDevice.h"
#include "Graphic/AdVKBuffer.h"

namespace ade
{
    AdVKImage::AdVKImage(AdVKDevice* device, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkSampleCountFlagBits sampleCount) :
        mDevice(device), mExtent(extent), mFormat(format), mUsage(usage)
    {
        VkImageTiling tiling = VK_IMAGE_TILING_LINEAR;
        bool isDepthStencilFormat = IsDepthStencilFormat(format);
        if (isDepthStencilFormat || sampleCount > VK_SAMPLE_COUNT_1_BIT)
        {
            tiling = VK_IMAGE_TILING_OPTIMAL;
        }

        VkImageCreateInfo imageInfo =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = format,
            .extent = extent,
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = sampleCount,
            .tiling = tiling,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };
        CALL_VK(vkCreateImage(mDevice->GetHandle(), &imageInfo, nullptr, &mHandle));

        // allocate memory
        VkMemoryRequirements memReqs;
        vkGetImageMemoryRequirements(mDevice->GetHandle(), mHandle, &memReqs);

        VkMemoryAllocateInfo allocateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = memReqs.size,
            .memoryTypeIndex = static_cast<uint32_t>(mDevice->GetMemoryIndex(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memReqs.memoryTypeBits))
        };
        CALL_VK(vkAllocateMemory(mDevice->GetHandle(), &allocateInfo, nullptr, &mMemory));
        CALL_VK(vkBindImageMemory(mDevice->GetHandle(), mHandle, mMemory, 0));
    }

    AdVKImage::AdVKImage(AdVKDevice* device, VkImage image, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkSampleCountFlagBits sampleCount)
        : mHandle(image), mDevice(device), mExtent(extent), mFormat(format), mUsage(usage), bCreateImage(false)
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
    void AdVKImage::CopyFromBuffer(VkCommandBuffer cmdBuffer, AdVKBuffer* buffer)
    {
        VkBufferImageCopy region = {
            .bufferOffset = 0,
            .bufferRowLength = mExtent.width,
            .bufferImageHeight = mExtent.height,
            .imageSubresource = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel = 0,
                    .baseArrayLayer = 0,
                    .layerCount = 1
            },
            .imageOffset = { 0, 0, 0 },
            .imageExtent = { mExtent.width, mExtent.height, 1 }
        };
        vkCmdCopyBufferToImage(cmdBuffer, buffer->GetHandle(), mHandle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    }
}