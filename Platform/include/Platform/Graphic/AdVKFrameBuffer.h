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
		AdVKFramebuffer(AdVKDevice* device, AdVKRenderPass* renderPass, const std::vector<std::shared_ptr<AdVKImage>>& images, uint32_t width, uint32_t height);
		~AdVKFramebuffer();

		bool ReCreate(const std::vector<std::shared_ptr<AdVKImage>>& images, uint32_t width, uint32_t height);

		uint32_t GetWidth()const { return mWidth; }
		uint32_t GetHeight()const { return mHeight; }
		VkFramebuffer GetHandle()const { return mHandle; }
	private:
		VkFramebuffer mHandle=VK_NULL_HANDLE;
		AdVKDevice* mDevice;
		AdVKRenderPass* mRenderPass;

		uint32_t mWidth;
		uint32_t mHeight;
		std::vector<std::shared_ptr<AdVKImage>> mImage; 
		std::vector<std::shared_ptr<AdVKImageView>> mImageViews; 
	};
} // namespace ade
