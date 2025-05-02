#pragma once
#include "AdEngine.h"
#include "Graphic/AdVKCommon.h"

namespace ade
{
	class AdVKGraphicContext;
	class AdVKQueue;
	class AdVKCommandPool;
	struct AdVkSettings
	{
		VkFormat surfaceFormat = VK_FORMAT_B8G8R8A8_UNORM;
		VkPresentModeKHR presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		uint32_t swapchainImageCount = 3;
		VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
	};

	class AdVKDevice
	{
	public:
		AdVKDevice(AdVKGraphicContext* context, uint32_t graphicQueueCount, uint32_t presentQueueCount, const AdVkSettings& settings = {});
		~AdVKDevice();
		VkDevice GetHandle() const { return m_Device; }
		AdVkSettings GetSettings()const { return m_setttings; }
		VkPipelineCache GetPipelineCache()const { return mPipelineCache; }
		AdVKQueue* GetFirstPresentQueue() { return  m_PresentQueues.empty() ? nullptr : m_PresentQueues[0].get(); ; }
		AdVKQueue* GetFirstGraphicQueue() { return  m_GraphicQueues.empty() ? nullptr : m_GraphicQueues[0].get(); ; }
		AdVKCommandPool* GetDefaultCmdPool() const { return mDefaultCmdPool.get(); }

		int32_t GetMemoryIndex(VkMemoryPropertyFlags memProps, uint32_t memoryTypeBits) const;
		VkCommandBuffer CreateAndBeginOneCmdBuffer();
		void SubmitOneCmdBuffer(VkCommandBuffer cmdBuffer);

		VkResult CreateSimpleSampler(VkFilter filter, VkSamplerAddressMode addressMode, VkSampler* outSampler);
	private:
		void CreatePipelineCache();
		void CreateDefaultCmdPool();
	private:
		VkDevice m_Device;
		AdVKGraphicContext* mContext;
		std::vector< std::shared_ptr<AdVKQueue>> m_GraphicQueues;
		std::vector< std::shared_ptr<AdVKQueue>> m_PresentQueues;
		std::shared_ptr<AdVKCommandPool> mDefaultCmdPool;

		AdVkSettings m_setttings;
		VkPipelineCache mPipelineCache = VK_NULL_HANDLE;
	};
}