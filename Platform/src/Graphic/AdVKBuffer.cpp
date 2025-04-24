#include "Graphic/AdVKBuffer.h"
#include "Graphic/AdVKDevice.h"
namespace ade
{
	AdVKBuffer::AdVKBuffer(AdVKDevice* device, VkBufferCreateFlags usage, size_t size, void* data):mDevice(device)
	{
		VkBuffer stageBuffer;
		VkDeviceMemory stageMemory;
		CreateBufferInternal(device,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			usage, size,&stageBuffer,&stageMemory);

		void* mapping;
		vkMapMemory(device->GetHandle(), stageMemory, 0, size, 0, &mapping);
		memcpy(mapping, data, size);
		vkUnmapMemory(device->GetHandle(), stageMemory);

		CreateBufferInternal(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			usage, size, &stageBuffer, &stageMemory);
	}
	void AdVKBuffer::CreateBufferInternal(ade::AdVKDevice* device, VkMemoryPropertyFlags memProps, const VkBufferCreateFlags& usage, const size_t& size,
		VkBuffer* outBuffer, VkDeviceMemory* outMemory)
	{
		VkBufferCreateInfo bufferInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = nullptr,
			.size = size,
			.usage = usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr
		};
		CALL_VK(vkCreateBuffer(device->GetHandle(), &bufferInfo, nullptr, outBuffer));

		//allocate memory
		VkMemoryRequirements memReqs;
		vkGetBufferMemoryRequirements(device->GetHandle(), *outBuffer, &memReqs);

		VkMemoryAllocateInfo allocateInfo = {
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = nullptr,
			.allocationSize = memReqs.size,
			.memoryTypeIndex = static_cast<uint32_t>(device->GetMemoryIndex(memProps,memReqs.memoryTypeBits))
		};
		CALL_VK(vkAllocateMemory(device->GetHandle(), &allocateInfo, nullptr, outMemory));
		CALL_VK(vkBindBufferMemory(device->GetHandle(), *outBuffer, *outMemory, 0));
	}
	AdVKBuffer::~AdVKBuffer()
	{
		VK_D(Buffer, mDevice->GetHandle(), mHandle);
		VK_F(mDevice->GetHandle(), mMemory);
	}
}