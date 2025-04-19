#include "Graphic/AdVKFrameBuffer.h"
#include "Graphic/AdVKRenderPass.h"
#include "Graphic/AdVKDevice.h"
#include "Graphic/AdVKImageView.h"

namespace ade
{
	AdVKFramebuffer::AdVKFramebuffer(AdVKDevice* device, AdVKRenderPass* renderPass, const std::vector<VkImage>& images, uint32_t width, uint32_t height)
		:mDevice(device), mRenderPass(renderPass), mWidth(width), mHeight(height)
	{
		ReCreate(images, width, height);
	}
	AdVKFramebuffer::~AdVKFramebuffer()
	{
		VK_D(Framebuffer, mDevice->GetHandle(), mHandle);
	}
	bool AdVKFramebuffer::ReCreate(const std::vector<VkImage>& images, uint32_t width, uint32_t height)
	{
		mWidth = width;
		mHeight = height;

		mImageViews.clear();
		std::vector< VkImageView> attachments;

		for (const auto& image : images)
		{
			mImageViews.push_back(std::make_shared<AdVKImageView>(mDevice, image, mDevice->GetSettings().surfaceFormat, VK_IMAGE_ASPECT_COLOR_BIT));
			attachments.push_back(mImageViews.back()->GetHandle());
		}
		VkFramebufferCreateInfo framebufferInfo = {
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.renderPass = mRenderPass->GetHandle(),
		.attachmentCount =static_cast<uint32_t>( mImageViews.size()),
		.pAttachments = attachments.data(),
		.width = mWidth,
		.height = mHeight,
		.layers = 1

		};
		auto ret = vkCreateFramebuffer(mDevice->GetHandle(), &framebufferInfo, nullptr, &mHandle);
		LOG_T("FrameBuffer {0}, new: {1}, width: {2}, height: {3}, view count: {4}", __FUNCTION__, (void*)mHandle, mWidth, mHeight, mImageViews.size());
		return ret == VK_SUCCESS;
	}
}
