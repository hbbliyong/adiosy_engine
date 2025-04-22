#include "iostream"
#include "Platform/AdLog.h"
#include "Platform/AdWindow.h"
#include "Platform/AdGraphicContext.h"
#include "Platform/Graphic/AdVKGraphicContext.h"
#include "Platform/Graphic/AdVKDevice.h"
#include "Platform/Graphic/AdVKQueue.h"
#include "Platform/Graphic/AdVKSwapchain.h"
#include "Platform/Graphic/AdVKRenderPass.h"
#include "Platform/Graphic/AdVKFrameBuffer.h"
#include "Platform/Graphic/AdVKPipeline.h"
#include "Platform/Graphic/AdVKCommandBuffer.h"
#include "Platform/AdFileUtils.h"
#include <algorithm>
int main()
{
	ade::AdLog::Init();
	LOG_T("hello spdlog:{0},{1},{2}", __FUNCTION__, 1, 0.14, true);
	LOG_D("hello spdlog:{0},{1},{2}", __FUNCTION__, 1, 0.14, true);
	LOG_E("hello spdlog:{0},{1},{2}", __FUNCTION__, 1, 0.14, true);


	auto window = ade::AdWindow::Create(800, 600, "SandBox");
	auto graphicContext =ade::AdGraphicContext::Create(window.get());
	auto vkContext =dynamic_cast<ade::AdVKGraphicContext*> (graphicContext.get());
	
	auto device = std::make_shared<ade::AdVKDevice>(vkContext, 1, 1);
	auto swapchain = std::make_shared<ade::AdVKSwapchain>(vkContext, device.get());
	swapchain->ReCreate();

	auto renderPass = std::make_shared<ade::AdVKRenderPass>(device.get());
	std::vector<VkImage> swapchainImages = swapchain->GetImages();
	std::vector<std::shared_ptr<ade::AdVKFramebuffer>> framebuffers;
	std::ranges::transform(swapchainImages, std::back_inserter(framebuffers), [&](VkImage& image) {
		std::vector<VkImage> images = { image };
		return std::make_shared<ade::AdVKFramebuffer>(device.get(), renderPass.get(), images, swapchain->GetWidth(), swapchain->GetHeight());
		});

	auto pipelineLayout = std::make_shared<ade::AdVKPipelineLayout>(device.get(), 
		AD_RES_SHADER_DIR"00_hello_triangle.vert",
		AD_RES_SHADER_DIR"00_hello_triangle.frag");
	auto pipeline = std::make_shared<ade::AdVKPipeline>(device.get(), renderPass.get(), pipelineLayout.get());
	pipeline->SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)->EnableDepthTest();
	pipeline->SetDynamicState({VK_DYNAMIC_STATE_VIEWPORT,VK_DYNAMIC_STATE_SCISSOR});
	pipeline->Create();

	auto cmdPool = std::make_shared<ade::AdVKCommandPool>(device.get(), vkContext->GetPresentQueueFamilyInfo().queueFamilyIndex);
	auto cmdBuffers = cmdPool->AllocateCommandBuffer(swapchainImages.size());
	ade::AdVKQueue* graphicQueue = device->GetFirstPresentQueue();
	std::vector<VkClearValue> clearValues = {
		{.1f,.2f,.3f,1.0f}
	};

	//1.acquire swapchain image Semaphore
	//2.submit Semaphore
	//3.frame  fence
	const uint32_t numBuffer = 2;
	std::vector<VkSemaphore> imageAvailableSemaphores(numBuffer);
	std::vector<VkSemaphore> submitedSemaphores(numBuffer);
	std::vector<VkFence> frameFences(numBuffer);

	VkSemaphoreCreateInfo semaphoreInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0
	};

	VkFenceCreateInfo fenceInfo = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};


	for (int i=0;i<numBuffer;i++)
	{
		CALL_VK(vkCreateSemaphore(device->GetHandle(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]));
		CALL_VK(vkCreateSemaphore(device->GetHandle(), &semaphoreInfo, nullptr, &submitedSemaphores[i]));
		CALL_VK(vkCreateFence(device->GetHandle(), &fenceInfo, nullptr, &frameFences[i]));
	}
	uint32_t currentBuffer = 0;

	while (!window->ShouldClose())
	{
		window->PollEvent();
		vkWaitForFences(device->GetHandle(), 1, &frameFences[currentBuffer], VK_FALSE, UINT64_MAX);
		vkResetFences(device->GetHandle(), 1, &frameFences[currentBuffer]);
		//1.acquire swapchain image
		//int32_t imageIndex;
		auto imageIndex =swapchain->AcquireImage(imageAvailableSemaphores[currentBuffer]);
		//2.begin cmdbuffer
		ade::AdVKCommandPool::BeginCommandBuffer(cmdBuffers[imageIndex]);
		//3.begin renderpass,bind framebuffer
		renderPass->Begin(cmdBuffers[imageIndex], framebuffers[imageIndex].get(), clearValues);
		//4.bind resource ->pipeline 
		pipeline->Bind(cmdBuffers[imageIndex]);

		VkViewport viewport = {
			   .x = 0,
			   .y = 0,
			   .width = static_cast<float>(framebuffers[imageIndex]->GetWidth()),
			   .height = static_cast<float>(framebuffers[imageIndex]->GetHeight()),
			   .minDepth = 0.f,
			   .maxDepth = 1.f
		};
		vkCmdSetViewport(cmdBuffers[imageIndex], 0, 1, &viewport);

		VkRect2D scissor = {
		.offset = { 0, 0 },
		.extent = { framebuffers[imageIndex]->GetWidth(), framebuffers[imageIndex]->GetHeight() }
		};
		vkCmdSetScissor(cmdBuffers[imageIndex], 0, 1, &scissor);
		//5.draw
		vkCmdDraw(cmdBuffers[imageIndex], 3, 1, 0, 0);
		// 6.end renderpass
		renderPass->End(cmdBuffers[imageIndex]);
		// 7.end cmdbuffer
		ade::AdVKCommandPool::EndCommandBuffer(cmdBuffers[imageIndex]);
		// 8.submit cmdbuffer to queue
		
		graphicQueue->Submit({ cmdBuffers[imageIndex] },{imageAvailableSemaphores[currentBuffer]}, 
			{submitedSemaphores[currentBuffer]},frameFences[currentBuffer]);
		//graphicQueue->WaitIdle();
		// 9.present
		swapchain->Present(imageIndex, { submitedSemaphores[currentBuffer] });
		window->SwapBuffer();
		currentBuffer = (currentBuffer + 1) % numBuffer;
	}

	for (int i = 0;	 i < numBuffer; i++)
	{
		vkDeviceWaitIdle(device->GetHandle());
		VK_D(Semaphore, device->GetHandle(), imageAvailableSemaphores[i]);
		VK_D(Semaphore, device->GetHandle(), submitedSemaphores[i]);
		VK_D(Fence, device->GetHandle(), frameFences[i]);
	}
	return EXIT_SUCCESS;

}