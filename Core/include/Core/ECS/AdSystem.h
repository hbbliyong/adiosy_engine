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
}