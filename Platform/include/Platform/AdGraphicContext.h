#pragma once
#include "AdEngine.h"

#define GLM_FORCE_RADIANS //ǿ��ʹ�û��ȶ����ǽǶ�
#define GLM_FORCE_DEPTH_ZERO_TO_ONE//������ȷ�ΧΪ(0~1),Ĭ����opengl����ȷ�Χ��-1~1��
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"//�任����Ŀ�
#include "glm/gtx/quaternion.hpp"//��Ԫ���Ŀ�
#include "glm/gtc/random.hpp"//��������������λ�ã������ת����
#include "glm/gtc/type_ptr.hpp"//�������߾���ת���һ��ָ��
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