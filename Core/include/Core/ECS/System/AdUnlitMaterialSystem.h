#pragma once
#include "Core/ECS/System/AdMaterialSystem.h"
#include "Core/ECS/Component/Material/AdUnlitMaterialComponent.h"

namespace ade
{
#define NUM_MATERIAL_BATCH              16
#define NUM_MATERIAL_BATCH_MAX          2048

	class AdVKPipelineLayout;
	class AdVKPipeline;
	class AdVKDescriptorSetLayout;
	class AdVKDescriptorPool;
	class AdUnlitMaterialSystem :public AdMaterialSystem
	{
	public:
		// Í¨¹ý AdMaterialSystem ¼Ì³Ð
		virtual void OnInit(AdVKRenderPass* renderPass) override;

		virtual void OnRender(VkCommandBuffer cmdBuffer, AdRenderTarget* renderTarget) override;

		virtual void OnDestroy() override;

	private:
		void ReCreateMaterialDescPool(uint32_t materialCount);
		void UpdateFrameUboDescSet(AdRenderTarget* renderTarget);
		void UpdateMaterialParamsDescSet(VkDescriptorSet descSet, AdUnlitMaterial* material);
		void UpdateMaterialResourceDescSet(VkDescriptorSet descSet, AdUnlitMaterial* material);
	private:
		std::shared_ptr<AdVKDescriptorSetLayout> mFrameUboDescSetLayout;
		std::shared_ptr<AdVKDescriptorSetLayout> mMaterialParamDescSetLayout;
		std::shared_ptr<AdVKDescriptorSetLayout> mMaterialResourceDescSetLayout;

		std::shared_ptr<ade::AdVKPipelineLayout> mPipelineLayout;
		std::shared_ptr<ade::AdVKPipeline> mPipeline;

		std::shared_ptr<AdVKDescriptorPool> mDescriptorPool;
		std::shared_ptr<AdVKDescriptorPool> mMaterialDescriptorPool;

		VkDescriptorSet mFrameUboDescSet;
		std::shared_ptr<AdVKBuffer> mFrameUboBuffer;

		uint32_t mLastDescriptorSetCount = 0;
		std::vector<VkDescriptorSet> mMaterialDescSets;
		std::vector<VkDescriptorSet> mMaterialResourceDescSets;
		std::vector<std::shared_ptr<AdVKBuffer>> mMaterialBuffers;
	};
}