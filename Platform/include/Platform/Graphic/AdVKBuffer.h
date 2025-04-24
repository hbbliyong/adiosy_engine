#pragma once
#include "Graphic/AdVKCommon.h"
namespace ade
{
	class AdVKDevice;
	class AdVKBuffer
	{
	public:
		AdVKBuffer(AdVKDevice* device, VkBufferCreateFlags usage, size_t size, void* data, bool bHostVisible = false);

		~AdVKBuffer();

		void CreateBuffer(VkBufferUsageFlags usage, void* data);
		static void CreateBufferInternal(ade::AdVKDevice* device, VkMemoryPropertyFlags memProps, const VkBufferCreateFlags& usage, const size_t& size,
			VkBuffer* outBuffer, VkDeviceMemory* outMemory);
		static void CopyToBuffer(AdVKDevice* device, VkBuffer srcBuffer, VkBuffer dstBuffer, size_t size);

		VkResult WriteData(void* data);

		VkBuffer GetHandle() { return mHandle; }
	private:
		VkBuffer mHandle;

		AdVKDevice* mDevice;
		VkDeviceMemory mMemory;
		size_t mSize;
		bool bHostVisible;
	};
}