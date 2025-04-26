#pragma once
namespace ade
{
	class AdApplication;
	class AdScene;
	class AdRenderContext;

	struct AdAppContext
	{
		AdApplication* app;
		AdScene* scene;
		AdRenderContext* renderCxt;
	};
}