#pragma once
#include "Graphic/AdVKGraphicContext.h"
#include "Graphic/AdVKSwapchain.h"
#include "Graphic/AdVKDevice.h"

namespace ade
{
	class AdRenderContext
	{
	public:
		AdRenderContext(AdWindow* window);
		~AdRenderContext();

		AdGraphicContext* GetGraphicContext() const { return mGraphicContext.get(); }
		AdVKDevice* GetDevice() const { return mDevice.get(); }
		AdVKSwapchain* GetSwapchain() const { return mSwapchain.get(); }
	private:
		std::shared_ptr<AdGraphicContext> mGraphicContext;
		std::shared_ptr<AdVKDevice> mDevice;
		std::shared_ptr<AdVKSwapchain> mSwapchain;
	};
}