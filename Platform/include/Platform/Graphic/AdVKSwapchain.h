#pragma once

#include "Graphic/AdVKCommon.h"

namespace ade
{
	class AdVKGraphicContext;
	class AdVKDevice;

	struct SurfaceInfo
	{
		VkSurfaceCapabilitiesKHR capabilities;
		VkSurfaceFormatKHR surfaceFormat;
		VkPresentModeKHR presentMode;
	};

	class AdVKSwapchain
	{
	public:
		AdVKSwapchain(AdVKGraphicContext* context, AdVKDevice* device);
		~AdVKSwapchain();
		bool ReCreate();
	private:
		void SetupSurfaceCapabilities();
	private:
		VkSwapchainKHR m_Handle = VK_NULL_HANDLE;
		AdVKGraphicContext* m_Context;
		AdVKDevice* m_Device;
		SurfaceInfo m_surfaceInfo;
		std::vector<VkImage> m_images;
	};
} // namespace ade
