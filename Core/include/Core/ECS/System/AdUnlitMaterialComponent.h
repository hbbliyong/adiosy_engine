#pragma once

#include "Core/ECS/Component/Material/AdMaterialComponent.h"
namespace ade
{
	class AdUnlitMaterial :public AdMaterial
	{

	};

	class AdUnlitMaterialComponent :public AdMaterialComponent<AdUnlitMaterial>
	{

	};
}