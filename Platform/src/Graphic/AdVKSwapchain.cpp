#include "Graphic/AdVKSwapchain.h"
#include "Graphic/AdVKDevice.h"
#include "Graphic/AdVKGraphicContext.h"
#include <algorithm>
namespace ade
{
	AdVKSwapchain::AdVKSwapchain(AdVKGraphicContext* context, AdVKDevice* device) :
		m_Context(context), m_Device(device)
	{

	}
	AdVKSwapchain::~AdVKSwapchain()
	{
		VK_D(SwapchainKHR, m_Device->GetHandle(), m_Handle);
	}
	bool AdVKSwapchain::ReCreate()
	{
		LOG_D("-----------------------------");
		SetupSurfaceCapabilities();
		LOG_D("currentExtent : {0} x {1}", m_surfaceInfo.capabilities.currentExtent.width, m_surfaceInfo.capabilities.currentExtent.height);
		LOG_D("surfaceFormat : {0}", vk_format_string(m_surfaceInfo.surfaceFormat.format));
		LOG_D("presentMode   : {0}", vk_present_mode_string(m_surfaceInfo.presentMode));
		LOG_D("-----------------------------");

		uint32_t imageCount = m_Device->GetSettings().swapchainImageCount;
		imageCount = std::clamp(imageCount, m_surfaceInfo.capabilities.minImageCount, m_surfaceInfo.capabilities.maxImageCount);

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
			.imageFormat = m_surfaceInfo.surfaceFormat.format,
			.imageColorSpace = m_surfaceInfo.surfaceFormat.colorSpace,
			.imageExtent = m_surfaceInfo.capabilities.currentExtent,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			.imageSharingMode = imageSharingMode,
			.queueFamilyIndexCount = queueFamilyIndexCount,
			.pQueueFamilyIndices = pQueueFamilyIndices,
			.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = m_surfaceInfo.presentMode,
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
			vk_format_string(m_surfaceInfo.surfaceFormat.format), vk_present_mode_string(m_surfaceInfo.presentMode));

		uint32_t swapchainImageCount;
		vkGetSwapchainImagesKHR(m_Device->GetHandle(), m_Handle, &swapchainImageCount, nullptr);
		m_images.resize(swapchainImageCount);
		 ret = vkGetSwapchainImagesKHR(m_Device->GetHandle(), m_Handle, &swapchainImageCount, m_images.data());

		return ret == VK_SUCCESS;
	}

	void AdVKSwapchain::SetupSurfaceCapabilities()
	{
		//capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Context->GetPhyDevice(), m_Context->GetSurface(), &m_surfaceInfo.capabilities);

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
		m_surfaceInfo.surfaceFormat = formats[foundFormatIndex];

		//present mode
		uint32_t presentModeCount;
		CALL_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_Context->GetPhyDevice(), m_Context->GetSurface(), &presentModeCount, nullptr));
		if (presentModeCount == 0)
		{
			LOG_E("{0} : num of surface presentModeCount is 0", __FUNCTION__);
			return;
		}
		std::vector<VkPresentModeKHR> presentModels(presentModeCount);
		CALL_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_Context->GetPhyDevice(), m_Context->GetSurface(), &presentModeCount, presentModels.data()));

		int32_t foundPresentModelIndex = -1;
		for (int i = 0; i < presentModeCount; i++)
		{
			if (presentModels[i] == settings.presentMode)
			{
				foundPresentModelIndex = i;
				break;
			}
		}
		if (foundPresentModelIndex == -1)
		{
			foundPresentModelIndex = 0;
		}
		m_surfaceInfo.presentMode = presentModels[foundFormatIndex];
	}
} // namespace ade
