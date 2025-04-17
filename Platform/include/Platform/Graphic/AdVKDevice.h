#pragma once
#include "AdEngine.h"
#include "Graphic/AdVKCommon.h"

namespace ade{
    class AdVKGraphicContext;
    class AdVKQueue;
    struct AdVkSettings
    {

    };
    
    class AdVKDevice{
    public:
        AdVKDevice(AdVKGraphicContext* context, uint32_t graphicQueueCount, uint32_t presentQueueCount, const AdVkSettings& settings = {});
        ~AdVKDevice();
        VkDevice GetHandle() const { return m_Device; }
    private:
        VkDevice m_Device;

        std::vector< std::shared_ptr<AdVKQueue>> m_GraphicQueues;
        std::vector< std::shared_ptr<AdVKQueue>> m_PresentQueues;
    };
}