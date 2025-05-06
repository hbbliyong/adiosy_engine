#pragma once
#include "Platform/Graphic/AdVKCommon.h"
#include "Core/ECS/AdSystem.h"
#include "glm/glm.hpp"
namespace ade
{
	class AdVKRenderPass;
	class AdRenderTarget;
	class AdApplication;
	class AdScene;
	class AdVKDevice;

	class AdMaterialSystem :public AdSystem
	{
	public:
		virtual void OnInit(AdVKRenderPass* renderPass) = 0;
		virtual void OnRender(VkCommandBuffer cmdBuffer, AdRenderTarget* renderTarget) = 0;
		virtual void OnDestroy() = 0;

	protected:
		AdApplication* GetApp() const;
		AdScene* GetScene() const;
		AdVKDevice* GetDevice()const;
		const glm::mat4 GetProjMat(AdRenderTarget* renderTarget)const;
		const glm::mat4 GetViewMat(AdRenderTarget* renderTarget)const;
	};
}