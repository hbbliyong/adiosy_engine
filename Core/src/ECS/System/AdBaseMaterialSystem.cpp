#include "Core/ECS/System/AdBaseMaterialSystem.h"
#include "Platform/AdFileUtils.h"
#include "Platform/AdGeometryUtil.h"
#include "Platform/Graphic/AdVKPipeline.h"
#include "AdApplication.h"
#include "Render/AdRenderContext.h"
#include "Render/AdRenderTarget.h"

#include "Graphic/AdVKPipeline.h"
#include "Graphic/AdVKFrameBuffer.h"
#include "Core/ECS/AdEntity.h"
namespace ade
{
	void AdBaseMaterialSystem::OnInit(AdVKRenderPass* renderPass)
	{
		ade::AdRenderContext* renderCxt = AdApplication::GetAppContext()->renderCxt;
		ade::AdVKDevice* device = renderCxt->GetDevice();

		ade::ShaderLayout shaderLayout = {
		.descriptorSetLayouts = {mDescriptorSetLayout->GetHandle()}
		};
		mPipelineLayout = std::make_shared<ade::AdVKPipelineLayout>(device,
			AD_RES_SHADER_DIR"02_descriptor_set.vert",
			AD_RES_SHADER_DIR"02_descriptor_set.frag", shaderLayout);
		std::vector<VkVertexInputBindingDescription> vertexBindings = {
				{
						.binding = 0,
						.stride = sizeof(ade::AdVertex),
						.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
				}
		};
		std::vector<VkVertexInputAttributeDescription> vertexAttrs = {
				{
						.location = 0,
						.binding = 0,
						.format = VK_FORMAT_R32G32B32_SFLOAT,
						.offset = offsetof(ade::AdVertex, position)
				},
				{
						.location = 1,
						.binding = 0,
						.format = VK_FORMAT_R32G32_SFLOAT,
						.offset = offsetof(ade::AdVertex, texcoord0)
				},
				{
						.location = 2,
						.binding = 0,
						.format = VK_FORMAT_R32G32B32_SFLOAT,
						.offset = offsetof(ade::AdVertex, normal)
				},
		};
		mPipeline = std::make_shared<ade::AdVKPipeline>(device, renderPass, mPipelineLayout.get());
		mPipeline->SetVertexInputState(vertexBindings, vertexAttrs);
		mPipeline->SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)->EnableDepthTest();
		mPipeline->SetDynamicState({ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR });
		mPipeline->SetMultisampleState(VK_SAMPLE_COUNT_4_BIT, VK_FALSE);
		mPipeline->Create();
	}
	void AdBaseMaterialSystem::OnRender(VkCommandBuffer cmdBuffer, AdRenderTarget* renderTarget)
	{
	}
	void AdBaseMaterialSystem::OnDestroy()
	{
	}
}