#pragma  once
#include "Core/ECS/AdComponent.h"
#include "Core/Render/AdMesh.h"

namespace ade
{
	struct AdMeshComponent:public AdComponent
	{
		AdMesh* mMesh = nullptr;
	};
}