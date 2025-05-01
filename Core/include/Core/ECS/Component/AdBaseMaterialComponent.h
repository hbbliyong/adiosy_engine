#pragma once
#include "Core/ECS/AdComponent.h"

namespace ade
{
	enum BaseMaterialColor
	{
		COLOR_TYPE_NORMAL = 0,
		COLOR_TYPE_TEXCOORD = 1
	};

	struct AdBaseMaterialComponent:public AdComponent
	{
		BaseMaterialColor colorType;
	};
}