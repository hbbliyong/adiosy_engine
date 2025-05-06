#include "iostream"
#include "Platform/AdLog.h"
#include "Platform/AdFileUtils.h"
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
#include "Platform/Event/AdEventObserver.h"

#include "Core/AdEntryPoint.h"
#include "Core/ECS/AdEntity.h"
#include "Core/Render/AdRenderTarget.h"
#include "Core/Render/AdRenderContext.h"
#include "Core/Render/AdMesh.h"
#include "Core/Render/AdTexture.h"
#include "Core/Render/AdRenderer.h"
#include "Core/Render/AdMaterial.h"

#include "Core/ECS/System/AdBaseMaterialSystem.h"
#include "Core/ECS/System/AdUnlitMaterialSystem.h"

#include "Core/ECS/Component/AdMeshComponent.h"
#include "Core/ECS/Component/AdTransformComponent.h"
#include "Core/ECS/Component/AdLookAtCameraComponent.h"
#include "Core/ECS/Component/Material/AdBaseMaterialComponent.h"
#include "Core/ECS/Component/Material/AdUnlitMaterialComponent.h"
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
		mRenderTarget->AddMaterialSystem<ade::AdUnlitMaterialSystem>();
		mRenderer = std::make_shared<ade::AdRenderer>();


		mCmdBuffers = device->GetDefaultCmdPool()->AllocateCommandBuffer(swapchain->GetImages().size());

		mObserver = std::make_shared<ade::AdEventObserver>();
		mObserver->OnEvent<ade::AdMouseScrollEvent>([this](const ade::AdMouseScrollEvent& event)
			{
				ade::AdEntity* camera = mRenderTarget->GetCamera();
				if (ade::AdEntity::HasComponent<ade::AdLookAtCameraComponent>(camera))
				{
					auto& cameraComp = camera->GetComponent<ade::AdLookAtCameraComponent>();
					float radius = cameraComp.GetRadius() + event.mYOffset * -0.3f;
					if (radius < 0.1f)
					{
						radius = 0.1f;
					}
					cameraComp.SetRadius(radius);
				}
			});


		std::vector<ade::AdVertex> vertices;
		std::vector<uint32_t> indices;

		ade::AdGeometryUtil::CreateCube(-0.1f, 0.1f, -0.1f, 0.1f, -0.1f, 0.1f, vertices, indices);
		mCubeMesh = std::make_shared<ade::AdMesh>(vertices, indices);

		mDefaultSampler = std::make_shared<ade::AdSampler>(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT);

		ade::RGBAColor whiteColor = { 255,255,255,255 };
		ade::RGBAColor blackColor = { 0,0,0,0 };
		ade::RGBAColor multiColors[4] = {
			255,255,255,255,
			192,192,192,255,
			192,192,192,255,
			255,255,255,255,
		};

		mWhiteTexture = std::make_shared<ade::AdTexture>(1, 1, &whiteColor);
		mBlackTexture = std::make_shared<ade::AdTexture>(1, 1, &blackColor);
		mMultiPixelTexture = std::make_shared<ade::AdTexture>(2, 2, multiColors);
		mFileTexture = std::make_shared<ade::AdTexture>(AD_RES_TEXTURE_DIR"R-C.jpeg");
	}

	void OnSceneInit(ade::AdScene* scene)override
	{
		ade::AdEntity* camera = scene->CreateEntity("Editor Camear");
		camera->AddComponent<ade::AdLookAtCameraComponent>();
		mRenderTarget->SetCamera(camera);

		auto baseMaterial0 = ade::AdMaterialFactory::GetInstance()->CreateMaterial<ade::AdBaseMaterial>();
		auto baseMaterial1 = ade::AdMaterialFactory::GetInstance()->CreateMaterial<ade::AdBaseMaterial>();
		baseMaterial1->colorType = ade::COLOR_TYPE_TEXCOORD;

		mUnlitMaterials[0] = ade::AdMaterialFactory::GetInstance()->CreateMaterial<ade::AdUnlitMaterial>();

		mUnlitMaterials[0]->SetTextureView(ade::UNLIT_MAT_BASE_COLOR_0, mFileTexture.get(), mDefaultSampler.get());
		mUnlitMaterials[0]->SetTextureView(ade::UNLIT_MAT_BASE_COLOR_1, mMultiPixelTexture.get(), mDefaultSampler.get());
		mUnlitMaterials[0]->UpdateTextureViewEnable(ade::UNLIT_MAT_BASE_COLOR_0, true);
		mUnlitMaterials[0]->UpdateTextureViewEnable(ade::UNLIT_MAT_BASE_COLOR_1, true);
		mUnlitMaterials[0]->UpdateTextureViewUVScale(ade::UNLIT_MAT_BASE_COLOR_1, {2,2});
		mUnlitMaterials[0]->UpdateTextureViewUVRotation(ade::UNLIT_MAT_BASE_COLOR_1, glm::radians(-60.f));
		mUnlitMaterials[0]->SetMixValue(0.5f);

		uint32_t index = 0;
		float x = -2.f;
		for (int i = 0; i < mSmallCubeSize.x; i++, x += 0.5f)
		{
			float y = -2.f;
			for (int j = 0; j < mSmallCubeSize.y; j++, y += 0.5f)
			{
				float z = -3.f;
				for (int k = 0; k < mSmallCubeSize.z; k++, z += 0.5f)
				{
					ade::AdEntity* cube = scene->CreateEntity("Cube");
					auto& materialComp = cube->AddComponent<ade::AdUnlitMaterialComponent>();
					materialComp.AddMesh(mCubeMesh.get(), mUnlitMaterials[0]);

					auto& transComp = cube->GetComponent<ade::AdTransformComponent>();
					transComp.position = { x, y, z };
				}
				index = (index + 1) % 2;
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
		ade::AdEntity* camera = mRenderTarget->GetCamera();
		if (ade::AdEntity::HasComponent<ade::AdLookAtCameraComponent>(camera))
		{
			if (!mWindow->IsMouseDown())
			{
				bFirstMouseDrag = true;
				return;
			}

			glm::vec2 mousePos;
			mWindow->GetMousePos(mousePos);
			glm::vec2 mousePosDelta = { mLastMousePos.x - mousePos.x, mLastMousePos.y - mousePos.y };
			mLastMousePos = mousePos;

			if (abs(mousePosDelta.x) > 0.1f || abs(mousePosDelta.y) > 0.1f)
			{
				if (bFirstMouseDrag)
				{
					bFirstMouseDrag = false;
				}
				else
				{
					auto& transComp = camera->GetComponent<ade::AdTransformComponent>();
					float yaw = transComp.rotation.x;
					float pitch = transComp.rotation.y;

					yaw += mousePosDelta.x * mMouseSensitivity;
					pitch += mousePosDelta.y * mMouseSensitivity;

					if (pitch > 89.f)
					{
						pitch = 89.f;
					}
					if (pitch < -89.f)
					{
						pitch = -89.f;
					}
					transComp.rotation.x = yaw;
					transComp.rotation.y = pitch;
				}
			}
		}
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


		mWhiteTexture.reset();
		mBlackTexture.reset();
		mMultiPixelTexture.reset();
		mFileTexture.reset();
		mDefaultSampler.reset();
	}

private:
	std::shared_ptr<ade::AdVKRenderPass> mRenderPass;
	std::shared_ptr<ade::AdRenderTarget> mRenderTarget;
	std::shared_ptr<ade::AdRenderer> mRenderer;
	std::shared_ptr<ade::AdEventObserver> mObserver;

	std::vector<VkCommandBuffer> mCmdBuffers;
	std::shared_ptr<ade::AdMesh> mCubeMesh;

	std::shared_ptr<ade::AdTexture> mWhiteTexture;
	std::shared_ptr<ade::AdTexture> mBlackTexture;
	std::shared_ptr<ade::AdTexture> mMultiPixelTexture;
	std::shared_ptr<ade::AdTexture> mFileTexture;
	std::shared_ptr<ade::AdSampler> mDefaultSampler;
	glm::ivec3 mSmallCubeSize{10, 10, 10};

	std::vector<ade::AdUnlitMaterial*> mUnlitMaterials{1};

	bool bFirstMouseDrag = true;
	glm::vec2 mLastMousePos;
	float mMouseSensitivity = 0.25f;
};

ade::AdApplication* CreateApplicationEntryPoint()
{
	return new SandBoxApp();
}