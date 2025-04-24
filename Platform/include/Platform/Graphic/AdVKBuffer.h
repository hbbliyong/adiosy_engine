#pragma once
#include "Graphic/AdVKCommon.h"
namespace ade
{
	class AdVKDevice;
	class AdVKBuffer
	{
	public:
		AdVKBuffer(AdVKDevice* device, VkBufferCreateFlags usage, size_t size, void* data);

		~AdVKBuffer();
		static void CreateBufferInternal( ade::AdVKDevice* device,VkMemoryPropertyFlags memProps,const VkBufferCreateFlags& usage,const size_t& size, 
			VkBuffer *outBuffer,VkDeviceMemory *outMemory);
		VkBuffer GetHandle() { return mHandle; }
	private:
		VkBuffer mHandle;

		AdVKDevice* mDevice;
		VkDeviceMemory mMemory;
	};
}