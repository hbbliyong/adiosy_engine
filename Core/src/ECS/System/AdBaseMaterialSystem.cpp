#include "Core/ECS/System/AdBaseMaterialSystem.h"
#include "Platform/AdFileUtils.h"
#include "Platform/AdGeometryUtil.h"
#include "Platform/Graphic/AdVKPipeline.h"
#include "AdApplication.h"
#include "Render/AdRenderContext.h"
#include "Render/AdRenderTarget.h"

#include "Platform/Graphic/AdVKPipeline.h"
#include "Platform/Graphic/AdVKFrameBuffer.h"
#include "Core/ECS/AdEntity.h"
#include "Core/ECS/AdScene.h"

#include "Core/ECS/Component/AdTransformComponent.h"
#include "Core/ECS/Component/Material/AdBaseMaterialComponent.h"
#include "Core/ECS/Component/AdLookAtCameraComponent.h"

#include "entt/entity/view.hpp"
namespace ade
{
	void AdBaseMaterialSystem::OnInit(AdVKRenderPass* renderPass)
	{
		ade::AdVKDevice* device = GetDevice();

		ade::ShaderLayout shaderLayout = {
			.pushConstants =
			{
				{
					.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
					.offset = 0,
					.size = sizeof(PushConstants)
				}
			}
		};
		mPipelineLayout = std::make_shared<ade::AdVKPipelineLayout>(device,
			AD_RES_SHADER_DIR"01_hello_buffer.vert",
			AD_RES_SHADER_DIR"01_hello_buffer.frag", shaderLayout);

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
		PipelineRasterizationState rasterState{};
		rasterState.polygonMode = VK_POLYGON_MODE_FILL;
		mPipeline->SetRasterizationState(rasterState);
		mPipeline->Create();
	}
	void AdBaseMaterialSystem::OnRender(VkCommandBuffer cmdBuffer, AdRenderTarget* renderTarget)
	{
		AdScene* scene = GetScene();

		if (!scene)
		{
			return;
		}

		entt::registry& reg = scene->GetEcsRegistry();

		auto view = reg.view<AdTransformComponent, AdBaseMaterialComponent>();
		if (view.begin() == view.end())
		{
			return;
		}

		mPipeline->Bind(cmdBuffer);
		// setup global params
		AdVKFramebuffer* frameBuffer = renderTarget->GetFrameBuffer();
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
				.extent = { frameBuffer->GetWidth()/2, frameBuffer->GetHeight() }
		};
		vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);


		glm::mat4 projMat=GetProjMat(renderTarget);
		glm::mat4 viewMat=GetViewMat(renderTarget);

		view.each([this, &cmdBuffer, &projMat, &viewMat](const auto& e, const AdTransformComponent& transComp, const AdBaseMaterialComponent& materialComp)
			{
				auto meshMaterials = materialComp.GetMeshMaterials();
				for (const auto& entry : meshMaterials)
				{
					AdBaseMaterial* material = entry.first;
					if (!material)
					{
						LOG_W("TODO: default material or error material ?");
						continue;
					}
					PushConstants pushConstants{
										.matrix = projMat * viewMat * transComp.GetTransform(),
										.colorType = (uint32_t)material->colorType
					};
					vkCmdPushConstants(cmdBuffer, mPipelineLayout->GetHandle(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &pushConstants);

					for (const auto& meshIndex : entry.second)
					{
						AdMesh* mesh = materialComp.GetMesh(meshIndex);
						if (mesh)
						{
							mesh->Draw(cmdBuffer);
						}
					}
				}
			}
		);
	}
	void AdBaseMaterialSystem::OnDestroy()
	{
		mPipeline.reset();
		mPipelineLayout.reset();
	}
}