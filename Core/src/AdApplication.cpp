#include "AdApplication.h"
#include "AdLog.h"
#include "AdApplicationContext.h"
#include "Core/Render/AdRenderContext.h"
namespace ade
{
	AdAppContext AdApplication::sAppContext{};
	void AdApplication::Start(int argc, char* argv[])
	{
		AdLog::Init();

		ParseArgs(argc, argv);
		OnConfiguration(&mAppSettings);
		mWindow = AdWindow::Create(mAppSettings.width, mAppSettings.height, mAppSettings.title);
		// 创建渲染上下文，并将其存储为共享指针
		mRenderContext = std::make_shared<AdRenderContext>(mWindow.get());
		sAppContext.app = this;
		sAppContext.renderCxt = mRenderContext.get();

		OnInit();

		mLastTimePoint = std::chrono::steady_clock::now();
	}

	void AdApplication::Stop()
	{
		OnDestroy();
	}
	void AdApplication::MainLoop()
	{
		mLastTimePoint = std::chrono::steady_clock::now();
		while (!mWindow->ShouldClose())
		{
			mWindow->PollEvent();
			float deltaTime = std::chrono::duration<float>(std::chrono::steady_clock::now() - mLastTimePoint).count();
			mLastTimePoint = std::chrono::steady_clock::now();
			mFrameIndex++;
			if (!bPause)
			{
				OnUpdate(deltaTime);
			}

			OnRender();
			mWindow->SwapBuffer();
		}
	}

	void AdApplication::ParseArgs(int argc, char* argv[])
	{

	}

}