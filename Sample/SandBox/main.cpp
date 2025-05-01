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

#include "Platform/Graphic/AdVKCommandBuffer.h"
#include "Platform/Graphic/AdVKImage.h"
#include "Platform/Graphic/AdVKImageView.h"
#include "Platform/Graphic/AdVKBuffer.h"
#include "Platform/Graphic/AdVKDescriptorSet.h"

#include "Core/AdEntryPoint.h"
#include "Core/Render/AdRenderTarget.h"
#include "Core/Render/AdRenderContext.h"
#include "Core/Render/AdMesh.h"
#include "Core/Render/AdTexture.h"
#include "Core/Render/AdRenderer.h"

struct GlobalUbo
{
	glm::mat4 projMat{ 1.f };
	glm::mat4 viewMat{ 1.f };
};

struct InstanceUbo
{
	glm::mat4 modelMat{ 1.f };
};

class SandBoxApp : public ade::AdApplication
{
protected:
	void OnConfiguration(ade::AppSettings* appSettings) override
	{
		appSettings->width = 1360;
		appSettings->height = 768;
		appSettings->title = "SandBox";
	}

	void OnInit() override
	{
		ade::AdRenderContext* renderCxt = AdApplication::GetAppContext()->renderCxt;
		ade::AdVKDevice* device = renderCxt->GetDevice();
		ade::AdVKSwapchain* swapchain = renderCxt->GetSwapchain();

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
					   .format = device->GetSettings().depthFormat,
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
				.sampleCount = VK_SAMPLE_COUNT_4_BIT
			}
		};
		mRenderPass = std::make_shared<ade::AdVKRenderPass>(device, attachments, subpasses);

		mRenderTarget = std::make_shared<ade::AdRenderTarget>(mRenderPass.get());
		mRenderTarget->SetColorClearValue({ 0.1f, 0.2f, 0.3f, 1.f });
		mRenderTarget->SetDepthStencilClearValue({ 1, 0 });

		mRenderer = std::make_shared<ade::AdRenderer>();

		std::vector<VkDescriptorSetLayoutBinding> desctLayoutBindings = {
			{
				.binding = 0,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
			},
			{
				.binding = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
			},
			{
				.binding = 2,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
			},
			{
				.binding = 3,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
			},
		};
		mDescriptorSetLayout = std::make_shared<ade::AdVKDescriptorSetLayout>(device, desctLayoutBindings);


		std::vector<VkDescriptorPoolSize> poolSizes = {
			{
				.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 2
			},
			{
				.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 2
			}
		};
		mDescriptorPool = std::make_shared<ade::AdVKDescriptorPool>(device, 1, poolSizes);
		mDescriptorSets = mDescriptorPool->AllocateDescriptorSet(mDescriptorSetLayout.get(), 1);

		mGlobalBuffer = std::make_shared<ade::AdVKBuffer>(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(GlobalUbo), nullptr, true);
		mInstanceBuffer = std::make_shared<ade::AdVKBuffer>(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(InstanceUbo), nullptr, true);

		mTexture0 = std::make_shared<ade::AdTexture>(AD_RES_TEXTURE_DIR"awesomeface.png");
		mTexture1 = std::make_shared<ade::AdTexture>(AD_RES_TEXTURE_DIR"R-C.jpeg");




		mCmdBuffers = device->GetDefaultCmdPool()->AllocateCommandBuffer(swapchain->GetImages().size());
		std::vector<ade::AdVertex> vertices;
		std::vector<uint32_t> indices;
		ade::AdGeometryUtil::CreateCube(-0.3f, 0.3f, -0.3f, 0.3f, -0.3f, 0.3f, vertices, indices);
		mCubeMesh = std::make_shared<ade::AdMesh>(vertices, indices);
	}

	void OnUpdate(float deltaTime) override
	{
		ade::AdRenderContext* renderCxt = AdApplication::GetAppContext()->renderCxt;
		ade::AdVKSwapchain* swapchain = renderCxt->GetSwapchain();

		float time = std::chrono::duration<float>(std::chrono::steady_clock::now() - mStartTimePoint).count();
		mInstanceUbo.modelMat = glm::rotate(glm::mat4(1.f), glm::radians(17.f), glm::vec3(1, 0, 0));
		mInstanceUbo.modelMat = glm::rotate(mInstanceUbo.modelMat, glm::radians(time * 100.f), glm::vec3(0, 1, 0));
		//mPushConstants.matrix = glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f) * mPushConstants.matrix;
		mGlobalUbo.projMat = glm::perspective(glm::radians(65.f), swapchain->GetWidth() * 1.f / swapchain->GetHeight(), 0.01f, 100.f);
		mGlobalUbo.projMat[1][1] *= -1.f;
		mGlobalUbo.viewMat = glm::lookAt(glm::vec3{ 0, 0, 1.5f }, glm::vec3{ 0, 0, -1 }, glm::vec3{ 0, 1, 0 });

	}

	void OnRender() override
	{
		ade::AdRenderContext* renderCxt = AdApplication::GetAppContext()->renderCxt;
		ade::AdVKDevice* device = renderCxt->GetDevice();
		ade::AdVKSwapchain* swapchain = renderCxt->GetSwapchain();

		int32_t imageIndex = -1;
		bool bShouldUpdateTarget = mRenderer->Begin(&imageIndex);
		if (bShouldUpdateTarget)
		{
			mRenderTarget->SetExtent({ swapchain->GetWidth(),swapchain->GetHeight() });
		}

		VkCommandBuffer cmdBuffer = mCmdBuffers[imageIndex];
		ade::AdVKCommandPool::BeginCommandBuffer(cmdBuffer);

		mRenderTarget->Begin(cmdBuffer);
		ade::AdVKFramebuffer* frameBuffer = mRenderTarget->GetFrameBuffer();

		mPipeline->Bind(cmdBuffer);
		VkViewport viewport = {
				.x = 0,
				.y = 0,
				.width = static_cast<float>(frameBuffer->GetWidth()),
				.height = static_cast<float>(frameBuffer->GetHeight()),
				.minDepth = 0.f,
				.maxDepth = 1.f
		};
		vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
		VkRect2D scissor = {
				.offset = { 0, 0 },
				.extent = { frameBuffer->GetWidth(), frameBuffer->GetHeight() }
		};
		vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

		mGlobalBuffer->WriteData(&mGlobalUbo);
		mInstanceBuffer->WriteData(&mInstanceUbo);

		UpdateDescriptorSets();

		vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->GetHandle(),
			0, 1, mDescriptorSets.data(), 0, nullptr);

		mCubeMesh->Draw(cmdBuffer);

		mRenderTarget->End(cmdBuffer);

		ade::AdVKCommandPool::EndCommandBuffer(cmdBuffer);
		bShouldUpdateTarget = mRenderer->End(imageIndex, { cmdBuffer });
		if (bShouldUpdateTarget)
		{
			mRenderTarget->SetExtent({ swapchain->GetWidth(),swapchain->GetHeight() });
		}
	}

	void OnDestroy() override
	{
		ade::AdRenderContext* renderCxt = ade::AdApplication::GetAppContext()->renderCxt;
		ade::AdVKDevice* device = renderCxt->GetDevice();
		vkDeviceWaitIdle(device->GetHandle());
		mGlobalBuffer.reset();
		mInstanceBuffer.reset();
		mTexture0.reset();
		mTexture1.reset();
		mCubeMesh.reset();
		mCmdBuffers.clear();
		mDescriptorPool.reset();
		mDescriptorSetLayout.reset();
		mPipeline.reset();
		mPipelineLayout.reset();
		mRenderTarget.reset();
		mRenderPass.reset();
		mRenderer.reset();
	}
	void UpdateDescriptorSets()
	{
		ade::AdRenderContext* renderCxt = ade::AdApplication::GetAppContext()->renderCxt;
		ade::AdVKDevice* device = renderCxt->GetDevice();

		VkDescriptorBufferInfo globalBufferInfo = {
			   .buffer = mGlobalBuffer->GetHandle(),
			   .offset = 0,
			   .range = sizeof(mGlobalUbo)
		};

		VkDescriptorBufferInfo instanceBufferInfo = {
			.buffer = mInstanceBuffer->GetHandle(),
			.offset = 0,
			.range = sizeof(mInstanceUbo)
		};

		VkDescriptorImageInfo textureImageInfo0 = {
			.sampler = mTexture0->GetSampler(),
			.imageView = mTexture0->GetImageView()->GetHandle(),
			.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};

		VkDescriptorImageInfo textureImageInfo1 = {
			.sampler = mTexture1->GetSampler(),
			.imageView = mTexture1->GetImageView()->GetHandle(),
			.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};

		VkDescriptorSet descriptorSet = mDescriptorSets[0];
		std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
			{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = descriptorSet,
				.dstBinding = 0,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.pBufferInfo = &globalBufferInfo
			},
			{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = descriptorSet,
				.dstBinding = 1,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.pBufferInfo = &instanceBufferInfo
			},
			{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = descriptorSet,
				.dstBinding = 2,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.pImageInfo = &textureImageInfo0
			},
			{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = descriptorSet,
				.dstBinding = 3,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.pImageInfo = &textureImageInfo1
			},
		};
		vkUpdateDescriptorSets(device->GetHandle(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
	}
private:
	std::shared_ptr<ade::AdVKRenderPass> mRenderPass;
	std::shared_ptr<ade::AdRenderTarget> mRenderTarget;
	std::shared_ptr<ade::AdRenderer> mRenderer;

	std::shared_ptr<ade::AdVKDescriptorSetLayout> mDescriptorSetLayout;
	std::shared_ptr<ade::AdVKDescriptorPool> mDescriptorPool;
	std::vector<VkDescriptorSet> mDescriptorSets;


	std::vector<VkCommandBuffer> mCmdBuffers;

	std::shared_ptr<ade::AdMesh> mCubeMesh;
	GlobalUbo mGlobalUbo;
	InstanceUbo mInstanceUbo;
	std::shared_ptr<ade::AdVKBuffer> mGlobalBuffer;
	std::shared_ptr<ade::AdVKBuffer> mInstanceBuffer;
	std::shared_ptr<ade::AdTexture> mTexture0;
	std::shared_ptr<ade::AdTexture> mTexture1;

};

ade::AdApplication* CreateApplicationEntryPoint()
{
	return new SandBoxApp();
}