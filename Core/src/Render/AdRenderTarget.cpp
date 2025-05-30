#include "core/Render/AdRenderTarget.h"
#include "AdApplication.h"
#include "Graphic/AdVKRenderPass.h"
#include "Graphic/AdVKImage.h"
#include "Core/ECS/Component/AdLookAtCameraComponent.h"
namespace ade
{
	AdRenderTarget::AdRenderTarget(AdVKRenderPass* renderPass)
	{
		AdRenderContext* renderCxt = AdApplication::GetAppContext()->renderCxt;
		AdVKSwapchain* swapchain = renderCxt->GetSwapchain();

		mRenderPass = renderPass;
		mExtent = { swapchain->GetWidth(),swapchain->GetHeight() };
		mBufferCount = swapchain->GetImages().size();
		bSwapchainTarget = true;

		Init();
		ReCreate();
	}
	AdRenderTarget::AdRenderTarget(AdVKRenderPass* renderPass, uint32_t bufferCount, VkExtent2D extent)
		:mRenderPass(renderPass), mBufferCount(bufferCount), mExtent(extent), bSwapchainTarget(false)
	{
		Init();
		ReCreate();
	}
	AdRenderTarget::~AdRenderTarget()
	{
	}

	void AdRenderTarget::Init()
	{
		mClearValues.resize(mRenderPass->GetAttachmentSize());
	
		SetColorClearValue({ 0.f,0.f,0.f,1.f });
		SetDepthStencilClearValue({ 1.f,0 });
	}
	void AdRenderTarget::ReCreate()
	{
		LOG_D("Hello spdlog: {0}", "创建FrameBuffers");
		if (mExtent.width == 0 || mExtent.height == 0)//等于0 不用重新创建
		{
			return;
		}
		mFrameBuffers.clear();
		mFrameBuffers.resize(mBufferCount);
		//逻辑设备
		AdRenderContext* renderCxt = AdApplication::GetAppContext()->renderCxt;
		AdVKDevice* device = renderCxt->GetDevice();
		AdVKSwapchain* swapchain = renderCxt->GetSwapchain();
		//randerPass 信息
		std::vector<Attachment> attachments = mRenderPass->GetAttachments();
		if (attachments.empty())
		{
			return;
		}

		std::vector<VkImage> swapchainImages = swapchain->GetImages();
		//创建FrameBuffers的数量
		for (int i = 0; i < mBufferCount; i++)
		{
			std::vector<std::shared_ptr<AdVKImage>> images;
			for (int j = 0; j < attachments.size(); j++)
			{
				Attachment attachment = attachments[j];
				if (bSwapchainTarget && attachment.finalLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR && attachment.samples == VK_SAMPLE_COUNT_1_BIT)
				{//image创建
					images.push_back(std::make_shared<AdVKImage>(device, swapchainImages[i], VkExtent3D{ mExtent.width, mExtent.height, 1 }, attachment.format, attachment.usage));
				}
				else
				{
					images.push_back(std::make_shared<AdVKImage>(device, VkExtent3D{ mExtent.width, mExtent.height, 1 }, attachment.format, attachment.usage, attachment.samples));
				}
			}
			//宽高
			mFrameBuffers[i] = std::make_shared<AdVKFramebuffer>(device, mRenderPass, images, mExtent.width, mExtent.height);
			images.clear();
		}
	}

	void AdRenderTarget::Begin(VkCommandBuffer cmdBuffer)
	{
		assert(!bBeginTarget && "You should not called Begin() again.");
		if (bShouldUpdate)
		{
			ReCreate();
			bShouldUpdate = false;
		}
		if (AdEntity::HasComponent<AdLookAtCameraComponent>(mCamera))
		{
			mCamera->GetComponent<AdLookAtCameraComponent>().SetAspect(mExtent.width * 1.f / mExtent.height);
		}
		if (bSwapchainTarget)
		{
			AdRenderContext* renderCxt = AdApplication::GetAppContext()->renderCxt;
			AdVKSwapchain* swapchain = renderCxt->GetSwapchain();
			mCurrentBufferIdx = swapchain->GetCurrentImageIndex();
		}
		else
		{
			mCurrentBufferIdx = (mCurrentBufferIdx + 1) % mBufferCount;
		}
		mRenderPass->Begin(cmdBuffer, GetFrameBuffer(), mClearValues);
		bBeginTarget = true;
	}
	void AdRenderTarget::End(VkCommandBuffer cmdBuffer)
	{
		if (bBeginTarget)
		{
			mRenderPass->End(cmdBuffer);
			bBeginTarget = false;
		}

	}

	void AdRenderTarget::SetExtent(const VkExtent2D& extent)
	{
		LOG_D("Hello spdlog: {0}", "渲染Target, 设置VkExtent2D");
		mExtent = extent;
		bShouldUpdate = true;
	}

	void AdRenderTarget::SetBufferCount(uint32_t bufferCount)
	{
		LOG_D("Hello spdlog: {0}", "渲染Target, 设置BufferCount");
		mBufferCount = bufferCount;
		bShouldUpdate = true;
	}

	void AdRenderTarget::SetColorClearValue(VkClearColorValue colorClearValue)
	{
		auto renderPassAttachments = mRenderPass->GetAttachments();
		for (int i = 0; i < renderPassAttachments.size(); i++)
		{
			if (!IsDepthStencilFormat(renderPassAttachments[i].format) && renderPassAttachments[i].loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR)
			{
				mClearValues[i].color = colorClearValue;
			}
		}
	}
	void AdRenderTarget::SetColorClearValue(uint32_t attachmentIndex, VkClearColorValue colorClearValue)
	{
		std::vector<Attachment> renderPassAttachments = mRenderPass->GetAttachments();
		if (attachmentIndex <= renderPassAttachments.size() - 1)
		{
			if (!IsDepthStencilFormat(renderPassAttachments[attachmentIndex].format) && renderPassAttachments[attachmentIndex].loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR)
			{
				mClearValues[attachmentIndex].color = colorClearValue;//颜色
			}
		}
	}
	void AdRenderTarget::SetDepthStencilClearValue(VkClearDepthStencilValue depthStencilValue)
	{
		LOG_D("Hello spdlog: {0}", "渲染Target, 设置DepthStencilClearValue");
		std::vector<Attachment> renderPassAttachments = mRenderPass->GetAttachments();
		for (int i = 0; i < renderPassAttachments.size(); i++)
		{
			if (IsDepthStencilFormat(renderPassAttachments[i].format) && renderPassAttachments[i].loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR)
			{
				mClearValues[i].depthStencil = depthStencilValue;//深度
			}
		}
	}
	void AdRenderTarget::SetDepthStencilClearValue(uint32_t attachmentIndex, VkClearDepthStencilValue depthStencilValue)
	{
		std::vector<Attachment> renderPassAttachments = mRenderPass->GetAttachments();
		if (attachmentIndex <= renderPassAttachments.size() - 1)
		{
			if (IsDepthStencilFormat(renderPassAttachments[attachmentIndex].format) && renderPassAttachments[attachmentIndex].loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR)
			{
				mClearValues[attachmentIndex].depthStencil = depthStencilValue;//深度
			}
		}
	}

} // namespace ade
