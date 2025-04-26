#pragma once
#include "iostream"
#include "AdApplication.h"
extern ade::AdApplication* CreateApplicationEntryPoint();
						   
#if AD_ENGINE_PLATFORM_WIN32 || AD_ENGINE_PLATFORM_MACOS || AD_ENGINE_PLATFORM_LINUX

int main(int argc, char* argv[])
{
	std::cout << "Adiosy Engine starting..." << std::endl;

	ade::AdApplication* app = CreateApplicationEntryPoint();
	app->Start(argc, argv);
	app->MainLoop();
	app->Stop();
	delete app;
	return EXIT_SUCCESS;
}
#endif