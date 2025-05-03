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
#include "Core/ECS/System/AdBaseMaterialSystem.h"
#include "Core/ECS/AdEntity.h"
#include "Core/ECS/Component/AdMeshComponent.h"
#include "Core/ECS/Component/AdTransformComponent.h"
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
		appSettings->title = "04_ECS_Entity";
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
		mRenderTarget->AddMaterialSystem<ade::AdBaseMaterialSystem>();
		mRenderer = std::make_shared<ade::AdRenderer>();


		mCmdBuffers = device->GetDefaultCmdPool()->AllocateCommandBuffer(swapchain->GetImages().size());
		std::vector<ade::AdVertex> vertices;
		std::vector<uint32_t> indices;
		ade::AdGeometryUtil::CreateCube(-0.3f, 0.3f, -0.3f, 0.3f, -0.3f, 0.3f, vertices, indices);
		mCubeMesh = std::make_shared<ade::AdMesh>(vertices, indices);
	}

	void OnSceneInit(ade::AdScene* scene)override
	{
		float x = -1.f;
		for (int i = 0; i < mSmallCubeSize.x; i++, x += 0.5f)
		{
			float y = -1.f;
			for (int j = 0; j < mSmallCubeSize.y; j++, y += 0.5f)
			{
				float z = -3.f;
				for (int k = 0; k < mSmallCubeSize.z; k++, z += 0.5f)
				{
					ade::AdEntity* cube = scene->CreateEntity("Cube");
					auto& materialComp = cube->AddComponent<ade::AdBaseMaterialComponent>();
					materialComp.colorType = ade::COLOR_TYPE_NORMAL;
					auto& meshComp = cube->AddComponent<ade::AdMeshComponent>();
					meshComp.mMesh = mCubeMesh.get();
					auto& transComp = cube->GetComponent<ade::AdTransformComponent>();
					transComp.position = { x, y, z };
					transComp.scale = { 0.5f, 0.5f, 0.5f };
				}
			}
		}
		/*{
			ade::AdEntity* cube = scene->CreateEntity("Cube 0");
			auto& materialComp = cube->AddComponent<ade::AdBaseMaterialComponent>();
			materialComp.colorType = ade::COLOR_TYPE_TEXCOORD;
			auto& meshComp = cube->AddComponent<ade::AdMeshComponent>();
			meshComp.mMesh = mCubeMesh.get();
			auto& transComp = cube->GetComponent<ade::AdTransformComponent>();
			transComp.scale = { 1.f, 1.f, 1.f };
			transComp.position = { 0.f, 0.f, 0.0f };
			transComp.rotation = { 17.f, 30.f, 0.f };
		}
		{
			ade::AdEntity* cube = scene->CreateEntity("Cube 1");
			auto& materialComp = cube->AddComponent<ade::AdBaseMaterialComponent>();
			materialComp.colorType = ade::COLOR_TYPE_NORMAL;
			auto& meshComp = cube->AddComponent<ade::AdMeshComponent>();
			meshComp.mMesh = mCubeMesh.get();
			auto& transComp = cube->GetComponent<ade::AdTransformComponent>();
			transComp.scale = { 0.5f, 0.5f, 0.5f };
			transComp.position = { -1.f, 0.f, 0.0f };
			transComp.rotation = { 17.f, 30.f, 0.f };
		}
		{
			ade::AdEntity* cube = scene->CreateEntity("Cube 2");
			auto& materialComp = cube->AddComponent<ade::AdBaseMaterialComponent>();
			materialComp.colorType = ade::COLOR_TYPE_TEXCOORD;
			auto& meshComp = cube->AddComponent<ade::AdMeshComponent>();
			meshComp.mMesh = mCubeMesh.get();
			auto& transComp = cube->GetComponent<ade::AdTransformComponent>();
			transComp.scale = { 0.5f, 0.5f, 0.5f };
			transComp.position = { 1.f, 0.f, 0.0f };
			transComp.rotation = { 17.f, 30.f, 0.f };
		}*/
	}

	void OnSceneDestroy(ade::AdScene* scene) override
	{

	}


	void OnUpdate(float deltaTime) override
	{
		//ade::AdRenderContext* renderCxt = AdApplication::GetAppContext()->renderCxt;
		//ade::AdVKSwapchain* swapchain = renderCxt->GetSwapchain();

		//float time = std::chrono::duration<float>(std::chrono::steady_clock::now() - mStartTimePoint).count();
		//mInstanceUbo.modelMat = glm::rotate(glm::mat4(1.f), glm::radians(17.f), glm::vec3(1, 0, 0));
		//mInstanceUbo.modelMat = glm::rotate(mInstanceUbo.modelMat, glm::radians(time * 100.f), glm::vec3(0, 1, 0));
		////mPushConstants.matrix = glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f) * mPushConstants.matrix;
		//mGlobalUbo.projMat = glm::perspective(glm::radians(65.f), swapchain->GetWidth() * 1.f / swapchain->GetHeight(), 0.01f, 100.f);
		//mGlobalUbo.projMat[1][1] *= -1.f;
		//mGlobalUbo.viewMat = glm::lookAt(glm::vec3{ 0, 0, 1.5f }, glm::vec3{ 0, 0, -1 }, glm::vec3{ 0, 1, 0 });

	}

	void OnRender() override
	{
		ade::AdRenderContext* renderCxt = AdApplication::GetAppContext()->renderCxt;
		ade::AdVKDevice* device = renderCxt->GetDevice();
		ade::AdVKSwapchain* swapchain = renderCxt->GetSwapchain();

		int32_t imageIndex = -1;
		if (mRenderer->Begin(&imageIndex))
		{
			mRenderTarget->SetExtent({ swapchain->GetWidth(), swapchain->GetHeight() });
		}

		VkCommandBuffer cmdBuffer = mCmdBuffers[imageIndex];
		ade::AdVKCommandPool::BeginCommandBuffer(cmdBuffer);

		mRenderTarget->Begin(cmdBuffer);
		mRenderTarget->RenderMaterialSystems(cmdBuffer);
		mRenderTarget->End(cmdBuffer);

		ade::AdVKCommandPool::EndCommandBuffer(cmdBuffer);
		if (mRenderer->End(imageIndex, { cmdBuffer }))
		{
			mRenderTarget->SetExtent({ swapchain->GetWidth(), swapchain->GetHeight() });
		}
	}

	void OnDestroy() override
	{
		ade::AdRenderContext* renderCxt = ade::AdApplication::GetAppContext()->renderCxt;
		ade::AdVKDevice* device = renderCxt->GetDevice();
		vkDeviceWaitIdle(device->GetHandle());
		mCubeMesh.reset();
		mCmdBuffers.clear();
		mRenderTarget.reset();
		mRenderPass.reset();
		mRenderer.reset();
	}

private:
	std::shared_ptr<ade::AdVKRenderPass> mRenderPass;
	std::shared_ptr<ade::AdRenderTarget> mRenderTarget;
	std::shared_ptr<ade::AdRenderer> mRenderer;

	std::vector<VkCommandBuffer> mCmdBuffers;
	std::shared_ptr<ade::AdMesh> mCubeMesh;
	glm::ivec3 mSmallCubeSize{5, 5, 5};
};

ade::AdApplication* CreateApplicationEntryPoint()
{
	return new SandBoxApp();
}