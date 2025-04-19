#include "iostream"
#include "Platform/AdLog.h"
#include "Platform/AdWindow.h"
#include "Platform/AdGraphicContext.h"
#include "Platform/Graphic/AdVKGraphicContext.h"
#include "Platform/Graphic/AdVKDevice.h"
#include "Platform/Graphic/AdVKSwapchain.h"
#include "Platform/Graphic/AdVKRenderPass.h"
#include "Platform/Graphic/AdVKFrameBuffer.h"
#include <algorithm>
int main()
{
	ade::AdLog::Init();
	LOG_T("hello spdlog:{0},{1},{2}", __FUNCTION__, 1, 0.14, true);
	LOG_D("hello spdlog:{0},{1},{2}", __FUNCTION__, 1, 0.14, true);
	LOG_E("hello spdlog:{0},{1},{2}", __FUNCTION__, 1, 0.14, true);


	auto window = ade::AdWindow::Create(800, 600, "SandBox");
	auto graphicContext = ade::AdGraphicContext::Create(window.get());
	auto device = std::make_shared<ade::AdVKDevice>(dynamic_cast<ade::AdVKGraphicContext*>(graphicContext.get()), 1, 1);
	auto swapchain = std::make_shared<ade::AdVKSwapchain>(dynamic_cast<ade::AdVKGraphicContext*>(graphicContext.get()), device.get());
	swapchain->ReCreate();

	auto renderPass = std::make_shared<ade::AdVKRenderPass>(device.get());
	std::vector<VkImage> swapchainImages = swapchain->GetImages();
	std::vector<std::shared_ptr<ade::AdVKFramebuffer>> framebuffers;
	std::ranges::transform(swapchainImages, std::back_inserter(framebuffers), [&](VkImage& image) {
		std::vector<VkImage> images = { image };
		return std::make_shared<ade::AdVKFramebuffer>(device.get(), renderPass.get(), images, swapchain->GetWidth(), swapchain->GetHeight());
		});

	while (!window->ShouldClose())
	{
		window->PollEvent();
		window->SwapBuffer();
	}
	std::cout << "hello world" << std::endl;
}