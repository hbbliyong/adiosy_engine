#pragma once
#include "AdWindow.h"
#include "AdApplicationContext.h"
namespace ade{
    struct AppSettings
    {
        uint32_t width=800;
        uint32_t height=600;
        const char* title="Adiosy Engine";
    };
    class AdApplication{
        public:
            static AdAppContext* GetAppContext() { return &sAppContext; };
            void Start(int argc, char* argv[]);
            void Stop();
            void MainLoop();

            void Pause() { bPause = true; }
            void Resume() { if (bPause)bPause = false; }
    protected:
        virtual void OnConfiguration(AppSettings* appSettings) {};
        virtual void OnInit() {};
        virtual void OnUpdate(float deltaTime) {};
        virtual void OnRender() {};
        virtual void OnDestroy() {};
    private:
        void ParseArgs(int argc ,char* argv[]);
        std::unique_ptr<AdWindow> mWindow;
        AppSettings mAppSettings;

        std::chrono::steady_clock::time_point mStartTimePoint;
        std::chrono::steady_clock::time_point mLastTimePoint;
        uint64_t mFrameIndex;
        bool bPause = false;

        static AdAppContext sAppContext;
    };
}