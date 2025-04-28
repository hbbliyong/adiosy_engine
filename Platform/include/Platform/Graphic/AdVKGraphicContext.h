#pragma once
#include "AdGraphicContext.h"
#include "Graphic/AdVKCommon.h"
namespace ade
{
	struct QueueFamilyInfo
	{
		int32_t queueFamilyIndex = -1;
		uint32_t queueCount;
	};

	class AdVKGraphicContext :public AdGraphicContext
	{
	public:
		AdVKGraphicContext(AdWindow* window);
		virtual ~AdVKGraphicContext() override;

		VkInstance GetInstance() const { return m_Instance; }
		VkSurfaceKHR GetSurface() const { return m_Surface; }
		VkPhysicalDevice GetPhyDevice() const { return m_PhyDevice; }
		const QueueFamilyInfo& GetGraphicQueueFamilyInfo() const { return m_GraphicQueueFamily; }
		const QueueFamilyInfo& GetPresentQueueFamilyInfo() const { return m_PresentQueueFamily; }
		VkPhysicalDeviceMemoryProperties GetPhyDeviceMemProperties() const { return m_PhyDeviceMemProperties; }
		bool IsSameGraphicPresentQueueFamily() const { return m_GraphicQueueFamily.queueFamilyIndex == m_PresentQueueFamily.queueFamilyIndex; }
	private:
		void CreateInstance();
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void SetupDebugMessenger();
		void CreateSurface(AdWindow *window);
		void SelectPhyDevice();
		void PrintPhyDeviceInfo(VkPhysicalDeviceProperties& props);
		uint32_t GetPhyDeviceScore(VkPhysicalDeviceProperties& props);
	private:
		VkInstance m_Instance;
		VkSurfaceKHR m_Surface;

		VkPhysicalDevice m_PhyDevice;
		QueueFamilyInfo m_GraphicQueueFamily;
		QueueFamilyInfo m_PresentQueueFamily;
		VkPhysicalDeviceMemoryProperties m_PhyDeviceMemProperties;
		//开启验证有性能损失，正常发布需要关闭
		bool m_bShouldValidate = true;

		VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
	};
}