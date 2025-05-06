#include "Core/ECS/System/AdMaterialSystem.h"
#include "AdApplication.h"
#include "Render/AdRenderContext.h"
#include "Render/AdRenderTarget.h"
#include "Core/ECS/AdEntity.h"
#include "Core/ECS/Component/AdLookAtCameraComponent.h"
#include "Core/ECS/Component/AdLookAtCameraComponent.h"
namespace ade
{

	//void AdMaterialSystem::OnInit(AdVKRenderPass* renderPass)
	//{

	//}

	//void AdMaterialSystem::OnRender(VkCommandBuffer cmdBuffer, AdRenderTarget* renderTarget)
	//{

	//}

	//void AdMaterialSystem::OnDestroy()
	//{

	//}

	ade::AdApplication* AdMaterialSystem::GetApp() const
	{
		AdAppContext* appContext = AdApplication::GetAppContext();
		if (appContext)
		{
			return appContext->app;
		}
		return nullptr;
	}

	ade::AdScene* AdMaterialSystem::GetScene() const
	{
		AdAppContext* appContext = AdApplication::GetAppContext();
		if (appContext)
		{
			return appContext->scene;
		}
		return nullptr;
	}

	ade::AdVKDevice* AdMaterialSystem::GetDevice() const
	{
		AdAppContext* appContext = AdApplication::GetAppContext();
		if (appContext&& appContext->renderCxt)
		{
			return appContext->renderCxt->GetDevice();
		}
		return nullptr;
	}

	const glm::mat4 AdMaterialSystem::GetProjMat(AdRenderTarget* renderTarget) const
	{
		glm::mat4 projMat{1.f};
		AdEntity* camera = renderTarget->GetCamera();
		if (AdEntity::HasComponent<AdLookAtCameraComponent>(camera))
		{
			AdLookAtCameraComponent cameraComp = camera->GetComponent<AdLookAtCameraComponent>();
			projMat = cameraComp.GetProjMat();
		}
		return projMat;
	}

	const glm::mat4 AdMaterialSystem::GetViewMat(AdRenderTarget* renderTarget) const
	{
		glm::mat4 viewMat{1.f};
		AdEntity* camera = renderTarget->GetCamera();
		if (AdEntity::HasComponent<AdLookAtCameraComponent>(camera))
		{
			AdLookAtCameraComponent cameraComp = camera->GetComponent<AdLookAtCameraComponent>();
			viewMat = cameraComp.GetViewMat();
		}
		return viewMat;
	}

}