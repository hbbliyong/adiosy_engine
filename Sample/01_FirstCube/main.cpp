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
#include "Platform/Graphic/AdVKImage.h"
#include "Platform/Graphic/AdVKBuffer.h"
#include "Platform/AdFileUtils.h"
#include "Platform/AdGeometryUtil.h"
#include <algorithm>


struct PushConstants
{
	glm::mat4 matrix = { 1.0f };
};
int main()
{
	ade::AdLog::Init();
	LOG_T("hello spdlog:{0},{1},{2}", __FUNCTION__, 1, 0.14, true);
	LOG_D("hello spdlog:{0},{1},{2}", __FUNCTION__, 1, 0.14, true);
	LOG_E("hello spdlog:{0},{1},{2}", __FUNCTION__, 1, 0.14, true);


	auto window = ade::AdWindow::Create(800, 600, "First Cube");
	auto graphicContext = ade::AdGraphicContext::Create(window.get());
	auto vkContext = dynamic_cast<ade::AdVKGraphicContext*> (graphicContext.get());

	auto device = std::make_shared<ade::AdVKDevice>(vkContext, 1, 1);
	auto swapchain = std::make_shared<ade::AdVKSwapchain>(vkContext, device.get());
	swapchain->ReCreate();

	VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
	// render pass
	std::vector<ade::Attachment> attachments = {
			{
					.format = swapchain->GetSurfaceInfo().surfaceFormat.format,
					.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
					.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
					.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
					.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
			},
			{
					.format = depthFormat,
					.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
					.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
					.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
					.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
					.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
			}
	};
	std::vector<ade::RenderSubPass> subpasses = {
			{
					.colorAttachments = { 0 },
					.depthStencilAttachments = { 1 },
					.sampleCount = VK_SAMPLE_COUNT_1_BIT
			}
	};

	auto renderPass = std::make_shared<ade::AdVKRenderPass>(device.get(), attachments, subpasses);
	//auto renderPass = std::make_shared<ade::AdVKRenderPass>(device.get());


	std::vector<VkImage> swapchainImages = swapchain->GetImages();
	uint32_t swapchainImageSize = swapchainImages.size();
	std::vector<std::shared_ptr<ade::AdVKFramebuffer>> framebuffers;

	VkExtent3D imageExtent = { swapchain->GetWidth(),swapchain->GetHeight(),1 };

	for (int i = 0; i < swapchainImageSize; i++)
	{
		std::vector<std::shared_ptr<ade::AdVKImage>> images = {
  std::make_shared<ade::AdVKImage>(device.get(), swapchainImages[i], imageExtent, device->GetSettings().surfaceFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT),
			std::make_shared<ade::AdVKImage>(device.get(), imageExtent, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
		};

		framebuffers.push_back(std::make_shared<ade::AdVKFramebuffer>(device.get(), renderPass.get(), images, swapchain->GetWidth(), swapchain->GetHeight()));
	}

	ade::ShaderLayout shaderLayout = {
		.pushConstants = {
			{
							.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
							.offset = 0,
							.size = sizeof(PushConstants)
			}
		}
	};
	auto pipelineLayout = std::make_shared<ade::AdVKPipelineLayout>(device.get(),
		AD_RES_SHADER_DIR"01_hello_buffer.vert",
		AD_RES_SHADER_DIR"01_hello_buffer.frag",shaderLayout);
	auto pipeline = std::make_shared<ade::AdVKPipeline>(device.get(), renderPass.get(), pipelineLayout.get());

	std::vector<VkVertexInputAttributeDescription> vertexAttributes = {
		{
			.location = 0,
			.binding = 0,
			.format = VK_FORMAT_R32G32B32_SFLOAT,
			.offset = offsetof(ade::AdVertex,position)
		},
		{
			.location = 1,
			.binding = 0,
			.format = VK_FORMAT_R32G32_SFLOAT,
			.offset = offsetof(ade::AdVertex,texcoord0)
		},
		{
			.location = 2,
			.binding = 0,
			.format = VK_FORMAT_R32G32B32_SFLOAT,
			.offset = offsetof(ade::AdVertex,normal)
		},
	};
	std::vector<VkVertexInputBindingDescription> vertexBindings = {
		{
			.binding = 0,
			.stride = sizeof(ade::AdVertex),
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		}
	};

	pipeline->SetVertexInputState(vertexBindings, vertexAttributes);
	pipeline->SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);//->EnableDepthTest();
	pipeline->SetDynamicState({ VK_DYNAMIC_STATE_VIEWPORT,VK_DYNAMIC_STATE_SCISSOR });
	pipeline->Create();

	auto cmdPool = std::make_shared<ade::AdVKCommandPool>(device.get(), vkContext->GetPresentQueueFamilyInfo().queueFamilyIndex);
	auto cmdBuffers = cmdPool->AllocateCommandBuffer(swapchainImages.size());
	ade::AdVKQueue* graphicQueue = device->GetFirstGraphicQueue();

	PushConstants pc{};
	//Geomerty
	std::vector<ade::AdVertex> vertices;
	std::vector<uint32_t> indices;
	ade::AdGeometryUtil::CreateCube(-0.3f, 0.3f, -0.3f, 0.3f, -0.3f, 0.3f, vertices, indices);
	auto vertexBuffer = std::make_shared<ade::AdVKBuffer>(device.get(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		sizeof(vertices[0]) * vertices.size(), vertices.data());
	auto indexBuffer = std::make_shared<ade::AdVKBuffer>(device.get(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		sizeof(indices[0]) * indices.size(), indices.data());

	std::vector<VkClearValue> clearValues = {
		 {.color = { 0.1f, 0.2f, 0.3f, 1.f }}, {.depthStencil = { 1, 0 } }
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


	for (int i = 0; i < numBuffer; i++)
	{
		CALL_VK(vkCreateSemaphore(device->GetHandle(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]));
		CALL_VK(vkCreateSemaphore(device->GetHandle(), &semaphoreInfo, nullptr, &submitedSemaphores[i]));
		CALL_VK(vkCreateFence(device->GetHandle(), &fenceInfo, nullptr, &frameFences[i]));
	}

	std::chrono::time_point lastTimePoint = std::chrono::steady_clock::now();

	uint32_t currentBuffer = 0;

	while (!window->ShouldClose())
	{
		window->PollEvent();
		CALL_VK(vkWaitForFences(device->GetHandle(), 1, &frameFences[currentBuffer], VK_TRUE, UINT64_MAX));
		CALL_VK(vkResetFences(device->GetHandle(), 1, &frameFences[currentBuffer]));
		//1.acquire swapchain image
		int32_t imageIndex; 
		swapchain->AcquireImage(&imageIndex,imageAvailableSemaphores[currentBuffer]);

		float time = std::chrono::duration<float>(std::chrono::steady_clock::now() - lastTimePoint).count();
		pc.matrix = glm::rotate(glm::mat4(1.0f), glm::radians(-17.f), glm::vec3(1, 0, 0));
		pc.matrix = glm::rotate(pc.matrix, glm::radians(time * 100.f), glm::vec3(0, 1, 0));
		pc.matrix = glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f) * pc.matrix;

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
			   .minDepth = 0.0f,
			   .maxDepth = 1.0f
		};
		vkCmdSetViewport(cmdBuffers[imageIndex], 0, 1, &viewport);

		VkRect2D scissor = {
		.offset = { 0, 0 },
		.extent = { framebuffers[imageIndex]->GetWidth(), framebuffers[imageIndex]->GetHeight() }
		};
		vkCmdSetScissor(cmdBuffers[imageIndex], 0, 1, &scissor);
	
		vkCmdPushConstants(cmdBuffers[imageIndex], pipelineLayout->GetHandle(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(pc), &pc);
		//bind buffers
		VkBuffer vertexBuffers[] = { vertexBuffer->GetHandle() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(cmdBuffers[imageIndex], 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(cmdBuffers[imageIndex], indexBuffer->GetHandle(), 0,VK_INDEX_TYPE_UINT32);

		//5.draw
		//vkCmdDraw(cmdBuffers[imageIndex], 3, 1, 0, 0);
		vkCmdDrawIndexed(cmdBuffers[imageIndex], indices.size(), 1, 0, 0, 0);
		// 6.end renderpass
		renderPass->End(cmdBuffers[imageIndex]);
		// 7.end cmdbuffer
		ade::AdVKCommandPool::EndCommandBuffer(cmdBuffers[imageIndex]);
		// 8.submit cmdbuffer to queue

		graphicQueue->Submit({ cmdBuffers[imageIndex] }, { imageAvailableSemaphores[currentBuffer] },
			{ submitedSemaphores[currentBuffer] }, frameFences[currentBuffer]);
		//graphicQueue->WaitIdle();
		// 9.present
		swapchain->Present(imageIndex, { submitedSemaphores[currentBuffer] });
		window->SwapBuffer();
		currentBuffer = (currentBuffer + 1) % numBuffer;
	}

	for (int i = 0; i < numBuffer; i++)
	{
		vkDeviceWaitIdle(device->GetHandle());
		VK_D(Semaphore, device->GetHandle(), imageAvailableSemaphores[i]);
		VK_D(Semaphore, device->GetHandle(), submitedSemaphores[i]);
		VK_D(Fence, device->GetHandle(), frameFences[i]);
	}
	return EXIT_SUCCESS;

}