#pragma once

#include "Graphic/AdVKCommon.h"

namespace ade
{
	class AdVKDevice;

	struct Attachment
	{
		VkFormat format = VK_FORMAT_UNDEFINED;
		VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkImageLayout finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
		VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	};

	struct RenderSubPass
	{
		std::vector<uint32_t> inputAttachments;
		std::vector<uint32_t> colorAttachments;
		std::vector<uint32_t> depthStencilAttachments;
		VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;
	};

	class AdVKRenderPass
	{
	public:
		AdVKRenderPass(AdVKDevice* device, const std::vector<Attachment>& attachments = {}, const std::vector<RenderSubPass>& renderSubPasses = {});
		~AdVKRenderPass();

		VkRenderPass GetHandle()const { return mHandle; }
	private:
		VkRenderPass mHandle = VK_NULL_HANDLE;
		AdVKDevice* mDevice;
		std::vector<Attachment> mAttachments;
		std::vector<RenderSubPass> mSubPasses;
	};

} // namespace ade
