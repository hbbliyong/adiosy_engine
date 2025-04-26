#pragma once

#include "Graphic/AdVKFrameBuffer.h"
#include "Core/Render/AdRenderContext.h"
namespace ade
{
    class AdRenderTarget
    {
    public:
        AdRenderTarget(AdVKRenderPass* renderPass);
        AdRenderTarget(AdVKRenderPass* renderPass, uint32_t bufferCount, VkExtent2D extent);

        ~AdRenderTarget();

        void Begin(VkCommandBuffer cmdBuffer);
        void End(VkCommandBuffer cmdBuffer);

        AdVKRenderPass* GetRenderPass()const { return mRenderPass; }
        AdVKFramebuffer* GetFrameBuffer()const { return mFrameBuffers[mCurrentBufferIdx].get(); }


        void SetExtent(const VkExtent2D& extent);
        void SetBufferCount(uint32_t bufferCount);

        void SetColorClearValue(VkClearColorValue colorClearValue);//清空颜色
        void SetColorClearValue(uint32_t attachmentIndex, VkClearColorValue colorClearValue);
        void SetDepthStencilClearValue(VkClearDepthStencilValue depthStencilValue);//清空深度
        void SetDepthStencilClearValue(uint32_t attachmentIndex, VkClearDepthStencilValue depthStencilValue);

    private:
        void Init();
        void ReCreate();
    private:
        std::vector<std::shared_ptr<AdVKFramebuffer>> mFrameBuffers;

        AdVKRenderPass* mRenderPass;
        std::vector<VkClearValue> mClearValues;
        uint32_t mBufferCount;
        uint32_t mCurrentBufferIdx = 0;
        VkExtent2D mExtent;
        

        bool bSwapchainTarget = false;
        bool bBeginTarget = false;

        bool bShouldUpdate = false;
    };
} // namespace ade
