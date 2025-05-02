#pragma once
#include "Graphic/AdVKCommon.h"

namespace ade
{
	class AdVKRenderPass;
	class AdRenderTarget;

	class AdSystem
	{
	public:
		virtual void OnUpdate(float deltaTime) {};
	};

	class AdMaterialSystem :public AdSystem
	{
	public:
		virtual void OnInit(AdVKRenderPass* renderPass) = 0;
		virtual void OnRender(VkCommandBuffer cmdBuffer, AdRenderTarget* renderTarget) = 0;
		virtual void OnDestroy() = 0;
	};
}