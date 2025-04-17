#pragma once

#include "Graphic/AdVKCommon.h"

namespace ade
{
	class AdVKGraphicContext;
	class AdVKDevice;

	class AdSwapchain
	{
	public:
		AdSwapchain(AdVKGraphicContext* context, AdVKDevice* device);
		~AdSwapchain();
		bool ReCreate();
	private:
		VkSwapchainKHR m_Handle = VK_NULL_HANDLE;
		AdVKGraphicContext* m_Context;
		AdVKDevice* m_Device;

	};
} // namespace ade
