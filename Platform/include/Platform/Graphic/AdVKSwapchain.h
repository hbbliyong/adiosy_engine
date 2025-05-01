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

		VkResult AcquireImage(int32_t* outImageIndex, VkSemaphore semaphore, VkFence fence=VK_NULL_HANDLE)const;
		VkResult Present(int32_t imageIndex, const std::vector<VkSemaphore>& waitSemaphores)const;

		const std::vector<VkImage>& GetImages() const
		{
			return  m_images;
		}

		uint32_t GetWidth()const { return mSurfaceInfo.capabilities.currentExtent.width; }
		uint32_t GetHeight()const { return mSurfaceInfo.capabilities.currentExtent.height; }
		const SurfaceInfo& GetSurfaceInfo() const { return mSurfaceInfo; }
		uint32_t GetCurrentImageIndex() const { return mCurrentImageIdx; }
	private:
		void SetupSurfaceCapabilities();
	private:
		VkSwapchainKHR m_Handle = VK_NULL_HANDLE;
		AdVKGraphicContext* m_Context;
		AdVKDevice* m_Device;
		SurfaceInfo mSurfaceInfo;
		std::vector<VkImage> m_images;
	mutable	uint32_t mCurrentImageIdx = -1;
	};
} // namespace ade
