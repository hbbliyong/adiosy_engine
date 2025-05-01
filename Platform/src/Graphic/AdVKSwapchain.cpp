#include "Graphic/AdVKSwapchain.h"
#include "Graphic/AdVKDevice.h"
#include "Graphic/AdVKQueue.h"
#include "Graphic/AdVKGraphicContext.h"
#include <algorithm>
namespace ade
{
	AdVKSwapchain::AdVKSwapchain(AdVKGraphicContext* context, AdVKDevice* device) :
		m_Context(context), m_Device(device)
	{
		ReCreate();
	}
	AdVKSwapchain::~AdVKSwapchain()
	{
		VK_D(SwapchainKHR, m_Device->GetHandle(), m_Handle);
	}
	bool AdVKSwapchain::ReCreate()
	{
		LOG_D("-----------------------------");
		SetupSurfaceCapabilities();
		LOG_D("currentExtent : {0} x {1}", mSurfaceInfo.capabilities.currentExtent.width, mSurfaceInfo.capabilities.currentExtent.height);
		LOG_D("surfaceFormat : {0}", vk_format_string(mSurfaceInfo.surfaceFormat.format));
		LOG_D("presentMode   : {0}", vk_present_mode_string(mSurfaceInfo.presentMode));
		LOG_D("-----------------------------");

		uint32_t imageCount = m_Device->GetSettings().swapchainImageCount;
		imageCount = std::clamp(imageCount, mSurfaceInfo.capabilities.minImageCount, mSurfaceInfo.capabilities.maxImageCount);

		VkSharingMode imageSharingMode;
		uint32_t queueFamilyIndexCount;
		uint32_t pQueueFamilyIndices[2] = { 0,0 };
		if (m_Context->IsSameGraphicPresentQueueFamily())
		{
			imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			queueFamilyIndexCount = 0;
		}
		else
		{
			imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			queueFamilyIndexCount = 2;
			pQueueFamilyIndices[0] = m_Context->GetGraphicQueueFamilyInfo().queueFamilyIndex;
			pQueueFamilyIndices[1] = m_Context->GetPresentQueueFamilyInfo().queueFamilyIndex;
		}

		VkSwapchainKHR oldSwapchain = m_Handle;

		VkSwapchainCreateInfoKHR swapchainInfo = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.pNext = nullptr,
			.flags = 0,
			.surface = m_Context->GetSurface(),
			.minImageCount = imageCount,
			.imageFormat = mSurfaceInfo.surfaceFormat.format,
			.imageColorSpace = mSurfaceInfo.surfaceFormat.colorSpace,
			.imageExtent = mSurfaceInfo.capabilities.currentExtent,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			.imageSharingMode = imageSharingMode,
			.queueFamilyIndexCount = queueFamilyIndexCount,
			.pQueueFamilyIndices = pQueueFamilyIndices,
			.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = mSurfaceInfo.presentMode,
			.clipped = VK_FALSE,
			.oldSwapchain = oldSwapchain
		};
		VkResult ret = vkCreateSwapchainKHR(m_Device->GetHandle(), &swapchainInfo, nullptr, &m_Handle);
		if (ret != VK_SUCCESS)
		{
			LOG_E("{0} : {1}", __FUNCTION__, vk_result_string(ret));
			return false;
		}
		LOG_T("Swapchain {0} : old: {1}, new: {2}, image count: {3}, format: {4}, present mode : {5}", __FUNCTION__, (void*)oldSwapchain, (void*)m_Handle, imageCount,
			vk_format_string(mSurfaceInfo.surfaceFormat.format), vk_present_mode_string(mSurfaceInfo.presentMode));

		uint32_t swapchainImageCount;
		vkGetSwapchainImagesKHR(m_Device->GetHandle(), m_Handle, &swapchainImageCount, nullptr);
		m_images.resize(swapchainImageCount);
		ret = vkGetSwapchainImagesKHR(m_Device->GetHandle(), m_Handle, &swapchainImageCount, m_images.data());

		return ret == VK_SUCCESS;
	}

	VkResult AdVKSwapchain::AcquireImage(int32_t* outImageIndex, VkSemaphore semaphore, VkFence fence) const
	{
		uint32_t imageIndex;
		VkResult ret = vkAcquireNextImageKHR(m_Device->GetHandle(), m_Handle, UINT64_MAX, semaphore, fence, &imageIndex);
		if (fence != VK_NULL_HANDLE)
		{
			vkWaitForFences(m_Device->GetHandle(), 1, &fence, VK_FALSE, UINT64_MAX);
			vkResetFences(m_Device->GetHandle(), 1, &fence);
		}
		if (ret == VK_SUCCESS || ret == VK_SUBOPTIMAL_KHR)
		{
			*outImageIndex = imageIndex;
			mCurrentImageIdx = imageIndex;
		}

		return ret;
	}

	VkResult AdVKSwapchain::Present(int32_t imageIndex, const std::vector<VkSemaphore>& waitSemaphores) const
	{
		VkPresentInfoKHR presentInfo = {
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.pNext = nullptr,
			.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size()),
			.pWaitSemaphores = waitSemaphores.data(),
			.swapchainCount = 1,
			.pSwapchains = &m_Handle,
			.pImageIndices = reinterpret_cast<const uint32_t*>(&imageIndex)
		};
		VkResult ret = vkQueuePresentKHR(m_Device->GetFirstPresentQueue()->GetHandle(), &presentInfo);
		m_Device->GetFirstPresentQueue()->WaitIdle();
	
		return ret;
	}

	void AdVKSwapchain::SetupSurfaceCapabilities()
	{
		//capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Context->GetPhyDevice(), m_Context->GetSurface(), &mSurfaceInfo.capabilities);

		AdVkSettings settings = m_Device->GetSettings();
		//format
		uint32_t formatCount;
		CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_Context->GetPhyDevice(), m_Context->GetSurface(), &formatCount, nullptr));
		if (formatCount == 0)
		{
			LOG_E("{0} : num of surface format is 0", __FUNCTION__);
			return;
		}
		std::vector<VkSurfaceFormatKHR> formats(formatCount);
		CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_Context->GetPhyDevice(), m_Context->GetSurface(), &formatCount, formats.data()));

		int32_t foundFormatIndex = -1;
		for (int i = 0; i < formatCount; i++)
		{
			if (formats[i].format == settings.surfaceFormat && formats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
			{
				foundFormatIndex = i;
				break;
			}
		}
		if (foundFormatIndex == -1)
		{
			foundFormatIndex = 0;
		}
		mSurfaceInfo.surfaceFormat = formats[foundFormatIndex];

		//present mode
		uint32_t presentModeCount;
		CALL_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_Context->GetPhyDevice(), m_Context->GetSurface(), &presentModeCount, nullptr));
		if (presentModeCount == 0)
		{
			LOG_E("{0} : num of surface presentModeCount is 0", __FUNCTION__);
			return;
		}
		std::vector<VkPresentModeKHR> presentModes(presentModeCount);
		CALL_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_Context->GetPhyDevice(), m_Context->GetSurface(), &presentModeCount, presentModes.data()));

		int32_t foundPresentModeIndex = -1;
		for (int i = 0; i < presentModeCount; i++)
		{
			if (presentModes[i] == settings.presentMode)
			{
				foundPresentModeIndex = i;
				break;
			}
		}
		if (foundPresentModeIndex >= 0)
		{
			mSurfaceInfo.presentMode = presentModes[foundPresentModeIndex];
		}
		else
		{
			mSurfaceInfo.presentMode = presentModes[0];
		}
	}
} // namespace ade
