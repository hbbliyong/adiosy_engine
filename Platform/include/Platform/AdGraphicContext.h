#pragma once
#include "AdEngine.h"

#define GLM_FORCE_RADIANS //强制使用弧度而不是角度
#define GLM_FORCE_DEPTH_ZERO_TO_ONE//定义深度范围为(0~1),默认是opengl的深度范围（-1~1）
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"//变换矩阵的库
#include "glm/gtx/quaternion.hpp"//四元数的库
#include "glm/gtc/random.hpp"//随机数，用于随机位置，随机旋转方向
#include "glm/gtc/type_ptr.hpp"//向量或者矩阵转变成一个指针
namespace ade
{
	class AdWindow;
	class AdGraphicContext
	{
	public:
		AdGraphicContext(const AdGraphicContext&) = delete;
		AdGraphicContext& operator&=(const AdGraphicContext&) = delete;
		virtual ~AdGraphicContext() = default;

		static std::unique_ptr<AdGraphicContext> Create(AdWindow* window);
	protected:
		AdGraphicContext() = default;
	};
}