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
		int32_t AcquireImage()const;
		void Present(int32_t imageIndex)const;
		const std::vector<VkImage>& GetImages() const
		{
			return  m_images;
		}

		uint32_t GetWidth()const { return m_surfaceInfo.capabilities.currentExtent.width; }
		uint32_t GetHeight()const { return m_surfaceInfo.capabilities.currentExtent.height; }
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
