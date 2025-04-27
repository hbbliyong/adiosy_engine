#pragma once

#include "Graphic/AdVKBuffer.h"
#include "AdGeometryUtil.h"
namespace ade
{
	class AdMesh
	{
	public:
		AdMesh(const std::vector<AdVertex> &vertices,const std::vector<uint32_t>& indices={});
		~AdMesh();

		void Draw(VkCommandBuffer cmdBuffer);
	private:
		std::shared_ptr<AdVKBuffer> mVertexBuffer;
		std::shared_ptr<AdVKBuffer> mIndexBuffer;

		uint32_t mVertexCount;
		uint32_t mIndexCount;
	};
}