#pragma once

#include "Graphic/AdVKCommon.h"

namespace ade
{
	class AdVKDevice;
	class AdVKRenderPass;
	class AdVKImageView;
	class AdVKImage;
	class AdVKFramebuffer
	{
	public:
		AdVKFramebuffer(AdVKDevice* device, AdVKRenderPass* renderPass, const std::vector<VkImage>& images, uint32_t width, uint32_t height);
		~AdVKFramebuffer();

		bool ReCreate(const std::vector<VkImage>& images, uint32_t width, uint32_t height);
	private:
		VkFramebuffer mHandle=VK_NULL_HANDLE;
		AdVKDevice* mDevice;
		AdVKRenderPass* mRenderPass;

		uint32_t mWidth;
		uint32_t mHeight;
		std::vector<std::shared_ptr<AdVKImageView>> mImageViews;
	};
} // namespace ade
