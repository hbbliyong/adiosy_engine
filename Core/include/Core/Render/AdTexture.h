#pragma  once

#include "Graphic/AdVKCommon.h"

namespace ade
{
	class AdVKImage;
	class AdVKImageView;
	class AdVKBuffer;
	class AdTexture
	{
	public:
		AdTexture(const std::string& filePath);
		~AdTexture();

		uint32_t GetWidth() const { return mWidth; }
		uint32_t GetHeight() const { return mHeight; }
		AdVKImage* GetImage() const { return mImage.get(); }
		AdVKImageView* GetImageView() const { return mImageView.get(); }

	private:
		uint32_t mWidth;
		uint32_t mHeight;
		VkFormat mFormat;
		std::shared_ptr<AdVKImage> mImage;
		std::shared_ptr<AdVKImageView> mImageView;
	};
}