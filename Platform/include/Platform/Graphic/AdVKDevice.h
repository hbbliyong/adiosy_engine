#pragma once
#include "AdEngine.h"
#include "Graphic/AdVKCommon.h"

namespace ade{
    class AdVKGraphicContext;
    class AdVKQueue;
    struct AdVkSettings
    {
        VkFormat surfaceFormat = VK_FORMAT_B8G8R8A8_UNORM;
        VkPresentModeKHR presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        uint32_t swapchainImageCount = 3;
    };
    
    class AdVKDevice{
    public:
        AdVKDevice(AdVKGraphicContext* context, uint32_t graphicQueueCount, uint32_t presentQueueCount, const AdVkSettings& settings = {});
        ~AdVKDevice();
        VkDevice GetHandle() const { return m_Device; }
        AdVkSettings GetSettings()const { return m_setttings; }
    private:
        VkDevice m_Device;

        std::vector< std::shared_ptr<AdVKQueue>> m_GraphicQueues;
        std::vector< std::shared_ptr<AdVKQueue>> m_PresentQueues;

        AdVkSettings m_setttings;
    };
}