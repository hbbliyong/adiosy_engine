#pragma  once

#include "AdVKCommon.h"
namespace ade
{
	class AdVKDevice;
	class AdVKDescriptorSetLayout
	{
	public:
		AdVKDescriptorSetLayout(AdVKDevice* device, const std::vector<VkDescriptorSetLayoutBinding>& bindings);
		~AdVKDescriptorSetLayout();

		VkDescriptorSetLayout GetHandle() const { return mHandle; }
	private:
		VkDescriptorSetLayout mHandle = VK_NULL_HANDLE;

		AdVKDevice* mDevice;

	};
	class AdVKDescriptorPool
	{
	public:
		AdVKDescriptorPool(AdVKDevice* device, uint32_t maxSets, const std::vector<VkDescriptorPoolSize>& poolSizes);
		~AdVKDescriptorPool();

		std::vector<VkDescriptorSet> AllocateDescriptorSet(AdVKDescriptorSetLayout* setLayout, uint32_t count);
	private:
		VkDescriptorPool mHandle = VK_NULL_HANDLE;

		AdVKDevice* mDevice;
	};
}