#pragma once

#include "Core/ECS/AdSystem.h"
#include "Core/ECS/Component/Material/AdBaseMaterialComponent.h"
#include "AdGraphicContext.h"
namespace ade
{
	class AdVKPipelineLayout;
	class AdVKPipeline;
	
	struct PushConstants
	{
		glm::mat4 matrix{ 1.f };
		uint32_t colorType;
	};
	class AdBaseMaterialSystem : public AdMaterialSystem
	{
	public:
		// Í¨¹ý AdMaterialSystem ¼Ì³Ð
		virtual void OnInit(AdVKRenderPass* renderPass) override;

		virtual void OnRender(VkCommandBuffer cmdBuffer, AdRenderTarget* renderTarget) override;

		virtual void OnDestroy() override;
	private:
		std::shared_ptr<ade::AdVKPipelineLayout> mPipelineLayout;
		std::shared_ptr<ade::AdVKPipeline> mPipeline;
	};
}