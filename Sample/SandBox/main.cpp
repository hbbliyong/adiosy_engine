#include "iostream"
#include "Platform/AdLog.h"
#include "Platform/AdWindow.h"
#include "Platform/AdGraphicContext.h"
#include "Platform/Graphic/AdVKGraphicContext.h"
#include "Platform/Graphic/AdVKDevice.h"
int main()
{
	ade::AdLog::Init();
	LOG_T("hello spdlog:{0},{1},{2}", __FUNCTION__, 1, 0.14, true);
	LOG_D("hello spdlog:{0},{1},{2}", __FUNCTION__, 1, 0.14, true);
	LOG_E("hello spdlog:{0},{1},{2}", __FUNCTION__, 1, 0.14, true);


	auto window = ade::AdWindow::Create(800, 600, "SandBox");
	auto graphicContext = ade::AdGraphicContext::Create(window.get());
	auto device = std::make_shared<ade::AdVKDevice>(dynamic_cast<ade::AdVKGraphicContext*>(graphicContext.get()), 1, 1);
	while (!window->ShouldClose())
	{
		window->PollEvent();
		window->SwapBuffer();
	}
	std::cout << "hello world" << std::endl;
}