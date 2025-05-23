#include "Graphic/AdVKDevice.h"
#include "Graphic/AdVKGraphicContext.h"
#include "Graphic/AdVKQueue.h"
#include "Graphic/AdVKCommandBuffer.h"
namespace ade
{
	const DeviceFeature requestedExtensions[] = {
		{ VK_KHR_SWAPCHAIN_EXTENSION_NAME, true },
#ifdef AD_ENGINE_PLATFORM_WIN32
#elif AD_ENGINE_PLATFORM_MACOS
			{ "VK_KHR_portability_subset", true },
#elif AD_ENGINE_PLATFORM_LINUX
#endif
	};

	AdVKDevice::AdVKDevice(AdVKGraphicContext* context, uint32_t graphicQueueCount, uint32_t presentQueueCount, const AdVkSettings& settings):
		m_setttings(settings),mContext(context)
	{
		if (!context)
		{
			LOG_E("Must create a vulkan graphic context before create device.");
			return;
		}

		QueueFamilyInfo graphicQueueFamilyInfo = context->GetGraphicQueueFamilyInfo();
		QueueFamilyInfo presentQueueFamilyInfo = context->GetPresentQueueFamilyInfo();

		bool bSameQueueFamilyIndex = context->IsSameGraphicPresentQueueFamily();
		if (graphicQueueCount > graphicQueueFamilyInfo.queueCount)
		{
			LOG_E("this queue family has {0} queue, but request {1}", graphicQueueFamilyInfo.queueCount, graphicQueueCount);
			return;
		}
		if (presentQueueCount > presentQueueFamilyInfo.queueCount)
		{
			LOG_E("this queue family has {0} queue, but request {1}", presentQueueFamilyInfo.queueCount, presentQueueCount);
			return;
		}
		std::vector<float> graphicQueuePriorities(graphicQueueCount, 0.f);
		std::vector<float> presentQueuePriorities(graphicQueueCount, 1.f);

		uint32_t sameQueueCount = graphicQueueCount;
		if (bSameQueueFamilyIndex)
		{
			sameQueueCount += presentQueueCount;
			if (sameQueueCount > graphicQueueFamilyInfo.queueCount)
			{
				sameQueueCount = graphicQueueFamilyInfo.queueCount;
			}
			graphicQueuePriorities.insert(graphicQueuePriorities.end(), presentQueuePriorities.begin(), presentQueuePriorities.end());
		}

		VkDeviceQueueCreateInfo queueInfos[2] = {
				{
					.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.queueFamilyIndex = static_cast<uint32_t>(graphicQueueFamilyInfo.queueFamilyIndex),
					.queueCount = sameQueueCount,
					.pQueuePriorities = graphicQueuePriorities.data()
				}
		};

		if (!bSameQueueFamilyIndex)
		{
			queueInfos[1] = {
					.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.queueFamilyIndex = static_cast<uint32_t>(presentQueueFamilyInfo.queueFamilyIndex),
					.queueCount = presentQueueCount,
					.pQueuePriorities = presentQueuePriorities.data()
			};
		}

		uint32_t availableExtensionCount;
		CALL_VK(vkEnumerateDeviceExtensionProperties(context->GetPhyDevice(), "", &availableExtensionCount, nullptr));
		std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
		CALL_VK(vkEnumerateDeviceExtensionProperties(context->GetPhyDevice(), "", &availableExtensionCount, availableExtensions.data()));
		uint32_t enableExtensionCount;
		const char* enableExtensions[32];
		if (!checkDeviceFeatures("Device Extension", true, availableExtensionCount, availableExtensions.data(),
			ARRAY_SIZE(requestedExtensions), requestedExtensions, &enableExtensionCount, enableExtensions))
		{
			return;
		}

		VkDeviceCreateInfo deviceInfo =
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueCreateInfoCount = static_cast<uint32_t>(bSameQueueFamilyIndex?1:2),
			.pQueueCreateInfos = queueInfos,
			.enabledLayerCount = 0,
			.ppEnabledLayerNames = nullptr,
			.enabledExtensionCount = enableExtensionCount,
			.ppEnabledExtensionNames = enableExtensionCount>0?enableExtensions:nullptr,
			.pEnabledFeatures = nullptr

		};
		CALL_VK(vkCreateDevice(context->GetPhyDevice(), &deviceInfo, nullptr, &m_Device));
		LOG_T("VkDevice: {0}", (void*)m_Device);

		for (int i = 0; i < graphicQueueCount; i++)
		{
			VkQueue queue;
			vkGetDeviceQueue(m_Device, graphicQueueFamilyInfo.queueFamilyIndex, i, &queue);
			m_GraphicQueues.push_back(std::make_shared<AdVKQueue>(graphicQueueFamilyInfo.queueFamilyIndex,i,queue,false));
		}
		for (int i = 0; i < presentQueueCount; i++)
		{
			VkQueue queue;
			vkGetDeviceQueue(m_Device, presentQueueFamilyInfo.queueFamilyIndex, i, &queue);
			m_PresentQueues.push_back(std::make_shared<AdVKQueue>(presentQueueFamilyInfo.queueFamilyIndex, i, queue, true));
		}
		CreatePipelineCache();
		CreateDefaultCmdPool();
	}

	AdVKDevice::~AdVKDevice()
	{
		vkDeviceWaitIdle(m_Device);
		mDefaultCmdPool = nullptr;
		VK_D(PipelineCache, m_Device, mPipelineCache);
		vkDestroyDevice(m_Device, nullptr);
	}
	int32_t AdVKDevice::GetMemoryIndex(VkMemoryPropertyFlags memProps, uint32_t memoryTypeBits) const
	{
		VkPhysicalDeviceMemoryProperties phyDeviceMemProps = mContext->GetPhyDeviceMemProperties();
		if (phyDeviceMemProps.memoryTypeCount == 0)
		{
			LOG_E("Physical device memory type count is 0");
			return -1;
		}
		for (int i = 0; i < phyDeviceMemProps.memoryTypeCount; i++)
		{
			if (memoryTypeBits & (1 << i) && (phyDeviceMemProps.memoryTypes[i].propertyFlags & memProps) == memProps)
			{
				return i;
			}
		}
		LOG_E("Can not find memory type index: type bit: {0}", memoryTypeBits);
		return 0;
	}

	VkCommandBuffer AdVKDevice::CreateAndBeginOneCmdBuffer()
	{
		VkCommandBuffer cmdBuffer = mDefaultCmdPool->AllocateOneCommandBuffer();
		mDefaultCmdPool->BeginCommandBuffer(cmdBuffer);
		return cmdBuffer;
	}

	void AdVKDevice::SubmitOneCmdBuffer(VkCommandBuffer cmdBuffer)
	{
		mDefaultCmdPool->EndCommandBuffer(cmdBuffer);
		AdVKQueue* queue = GetFirstGraphicQueue();
		queue->Submit({ cmdBuffer });
		queue->WaitIdle();
	}

	VkResult AdVKDevice::CreateSimpleSampler(VkFilter filter, VkSamplerAddressMode addressMode, VkSampler* outSampler)
	{
		VkSamplerCreateInfo samplerInfo = {
			   .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			   .pNext = nullptr,
			   .flags = 0,
			   .magFilter = filter,
			   .minFilter = filter,
			   .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			   .addressModeU = addressMode,
			   .addressModeV = addressMode,
			   .addressModeW = addressMode,
			   .mipLodBias = 0,
			   .anisotropyEnable = VK_FALSE,
			   .maxAnisotropy = 0,
			   .compareEnable = VK_FALSE,
			   .compareOp = VK_COMPARE_OP_NEVER,
			   .minLod = 0,
			   .maxLod = 1,
			   .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
			   .unnormalizedCoordinates = VK_FALSE
		};
		return vkCreateSampler(m_Device, &samplerInfo, nullptr, outSampler);
	}

	void AdVKDevice::CreatePipelineCache()
	{
		VkPipelineCacheCreateInfo createInfo = {
		.sType=VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
//.initialDataSize=,
//.pInitialData=
		};
		CALL_VK(vkCreatePipelineCache(m_Device, &createInfo, nullptr, &mPipelineCache));
	}
	void AdVKDevice::CreateDefaultCmdPool()
	{
		mDefaultCmdPool = std::make_shared<AdVKCommandPool>(this, mContext->GetGraphicQueueFamilyInfo().queueFamilyIndex);
	}
}