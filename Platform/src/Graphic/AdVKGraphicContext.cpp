#include "Graphic/AdVKGraphicContext.h"
#include "Window/AdGLFWwindow.h"
namespace ade
{
	const DeviceFeature requestedLayers[] = {
	{ "VK_LAYER_KHRONOS_validation", true },
	};
	const DeviceFeature requestedExtensions[] = {
		{ VK_KHR_SURFACE_EXTENSION_NAME, true },
#ifdef AD_ENGINE_PLATFORM_WIN32
			{ VK_KHR_WIN32_SURFACE_EXTENSION_NAME, true },
#elif AD_ENGINE_PLATFORM_MACOS
			{ VK_MVK_MACOS_SURFACE_EXTENSION_NAME, true },
#elif AD_ENGINE_PLATFORM_LINUX
			{ VK_KHR_XCB_SURFACE_EXTENSION_NAME, true },
#endif
		{ VK_EXT_DEBUG_REPORT_EXTENSION_NAME, true},
	};


	static VkBool32  VkDebugReportCallback(
		VkDebugReportFlagsEXT                       flags,
		VkDebugReportObjectTypeEXT                  objectType,
		uint64_t                                    object,
		size_t                                      location,
		int32_t                                     messageCode,
		const char* pLayerPrefix,
		const char* pMessage,
		void* pUserData)
	{
		if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
		{
			LOG_E("{0}", pMessage);
		}
		if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT || flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
		{
			LOG_W("{0}", pMessage);
		}
		return VK_TRUE;
	}

	AdVKGraphicContext::AdVKGraphicContext(AdWindow* window)
	{
		CreateInstance();
		CreateSurface(window);
		SelectPhyDevice();
	}
	AdVKGraphicContext:: ~AdVKGraphicContext()
	{
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyInstance(m_Instance, nullptr);
	}
	void  AdVKGraphicContext::CreateInstance()
	{
		//1.构建Layer
		uint32_t availableLayerCount;
		CALL_VK(vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr));
		std::vector<VkLayerProperties> availableLayers(availableLayerCount);
		CALL_VK(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data()));

		uint32_t enableLayerCount = 0;
		const char* enableLayers[32];
		if (m_bShouldValidate)
		{
			if (!checkDeviceFeatures("Instance Layers", false, availableLayerCount, availableLayers.data(),
				ARRAY_SIZE(requestedLayers), requestedLayers, &enableLayerCount, enableLayers))
			{
				return;
			}
		}

		//2.构建extension
		uint32_t availableExtensionCount;
		CALL_VK(vkEnumerateInstanceExtensionProperties("", &availableExtensionCount, nullptr));
		std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
		CALL_VK(vkEnumerateInstanceExtensionProperties("", &availableExtensionCount, availableExtensions.data()));

		//glfw extension
		uint32_t glfwRequestedExtensionCount;
		const char** glfwRequestedExtensions = glfwGetRequiredInstanceExtensions(&glfwRequestedExtensionCount);
		std::unordered_set<std::string> allRequestedExtensionSet;
		std::vector<DeviceFeature> allRequestedExtensions;
		for (const auto& item : requestedExtensions)
		{
			if (allRequestedExtensionSet.find(item.name) == allRequestedExtensionSet.end())
			{
				allRequestedExtensionSet.insert(item.name);
				allRequestedExtensions.push_back(item);
			}
		}
		for (int i = 0; i < glfwRequestedExtensionCount; i++)
		{
			const char* extensionName = glfwRequestedExtensions[i];
			if (allRequestedExtensionSet.find(extensionName) == allRequestedExtensionSet.end())
			{
				allRequestedExtensionSet.insert(extensionName);
				allRequestedExtensions.push_back({ extensionName, true });
			}
		}

		uint32_t enableExtensionCount;
		const char* enableExtensions[32];
		if (!checkDeviceFeatures("Instance Extension", true, availableExtensionCount, availableExtensions.data(),
			ARRAY_SIZE(requestedExtensions), requestedExtensions, &enableExtensionCount, enableExtensions))
		{
			return;
		}


		//3.create instance

		VkApplicationInfo applicationInfo = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = "Adiosy_Engine",
			.applicationVersion = VK_MAKE_VERSION(1,0,0),
			.pEngineName = "None",
			.engineVersion = VK_MAKE_VERSION(1,0,0),
			.apiVersion = VK_API_VERSION_1_4
		};

		VkDebugReportCallbackCreateInfoEXT debugReportCallbackInfoExt = {
				.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT,
				.pNext = nullptr,
				.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT
						| VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT
						| VK_DEBUG_REPORT_DEBUG_BIT_EXT,
				.pfnCallback = VkDebugReportCallback
		};

		VkInstanceCreateInfo instanceInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = m_bShouldValidate ? &debugReportCallbackInfoExt : nullptr,
			.flags = 0,
			.pApplicationInfo = &applicationInfo,
			.enabledLayerCount = enableLayerCount,
			.ppEnabledLayerNames = enableLayerCount > 0 ? enableLayers : nullptr,
			.enabledExtensionCount = enableExtensionCount,
			.ppEnabledExtensionNames = enableExtensionCount > 0 ? enableExtensions : nullptr
		};

		CALL_VK(vkCreateInstance(&instanceInfo, nullptr, &m_Instance));
		LOG_T("{0} : instance : {1}", __FUNCTION__, (void*)m_Instance);
	}

	void AdVKGraphicContext::CreateSurface(AdWindow* window)
	{
		if (!window)
		{
			LOG_E("window is not exists.");
			return;
		}

		auto* glfWwindow = dynamic_cast<AdGLFWwindow*>(window);
		if (!glfWwindow)
		{
			LOG_E("this window is not a glfw window.");
			return;
		}
		GLFWwindow* implWindowPointer = static_cast<GLFWwindow*>(glfWwindow->GetImplWindowPointer());
		CALL_VK(glfwCreateWindowSurface(m_Instance, implWindowPointer, nullptr, &m_Surface));
		LOG_T("{0} : instance : {1}", __FUNCTION__, (void*)m_Instance);
	}
	void AdVKGraphicContext::SelectPhyDevice()
	{
		uint32_t phyDeviceCount;
		CALL_VK(vkEnumeratePhysicalDevices(m_Instance, &phyDeviceCount, nullptr));
		std::vector<VkPhysicalDevice> phyDevices(phyDeviceCount);
		CALL_VK(vkEnumeratePhysicalDevices(m_Instance, &phyDeviceCount, phyDevices.data()));

		uint32_t maxScore = 0;
		int32_t maxScorePhyDeviceIndex = -1;
		for (int i = 0; i < phyDeviceCount; i++)
		{
			VkPhysicalDeviceProperties props;

			vkGetPhysicalDeviceProperties(phyDevices[i], &props);
			PrintPhyDeviceInfo(props);

			uint32_t score = GetPhyDeviceScore(props);
			uint32_t formatCount;
			CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(phyDevices[i], m_Surface, &formatCount, nullptr));
			std::vector<VkSurfaceFormatKHR> formats(formatCount);
			CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(phyDevices[i], m_Surface, &formatCount, formats.data()));
			for (int j = 0; j < formatCount; j++)
			{
				//这两个参数通常一起用于 ​交换链（Swap Chain）的配置​：
				  //​理想选择​：
					//开发者优先选择 VK_FORMAT_B8G8R8A8_UNORM + VK_COLORSPACE_SRGB_NONLINEAR_KHR 的组合，因其兼顾色彩准确性和硬件兼容
				if (formats[j].format == VK_FORMAT_B8G8R8A8_UNORM //像素格式,8位bgra 无符号归一
					&& formats[j].format == VK_COLORSPACE_SRGB_NONLINEAR_KHR/*颜色空间*/)
				{
					score += 10;
					break;
				}
			}
			if (score < maxScore)
			{
				continue;
			}

			//query queue
			uint32_t queueFamilyCount;
			vkGetPhysicalDeviceQueueFamilyProperties(phyDevices[i], &queueFamilyCount,nullptr);
			std::vector<VkQueueFamilyProperties> queueFamilys(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(phyDevices[i], &queueFamilyCount, queueFamilys.data());
			for (int j = 0; j < queueFamilyCount; j++)
			{
				if (queueFamilys[j].queueCount == 0)
				{
					continue;
				}
				//1.graphic family
				if (queueFamilys[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					m_GraphicQueueFamily.queueCount = queueFamilys[j].queueCount;
					m_GraphicQueueFamily.queueFamilyIndex = j;
				}

				if (m_GraphicQueueFamily.queueFamilyIndex >= 0 && m_PresentQueueFamily.queueFamilyIndex >= 0
					&& m_GraphicQueueFamily.queueFamilyIndex != m_PresentQueueFamily.queueFamilyIndex)
				{
					break;
				}

				//2.present family
				VkBool32 bSupportSurface;
				vkGetPhysicalDeviceSurfaceSupportKHR(phyDevices[i], j, m_Surface, &bSupportSurface);
				if (bSupportSurface)
				{
					m_PresentQueueFamily.queueFamilyIndex = j;
					m_PresentQueueFamily.queueCount = queueFamilys[j].queueCount;
				}

				if (m_GraphicQueueFamily.queueCount > 0 && m_PresentQueueFamily.queueCount > 0)
				{
					maxScore = score;
					maxScorePhyDeviceIndex = i;
				}
			}	
		}
		if (maxScorePhyDeviceIndex < 0)
		{
			maxScorePhyDeviceIndex = 0;
			LOG_W("Maybe can not find suitable physical device,will 0.");
		}
		m_PhyDevice = phyDevices[maxScorePhyDeviceIndex];

		vkGetPhysicalDeviceMemoryProperties(m_PhyDevice, &m_PhyDeviceMemProperties);
		LOG_T("{0} : physical device:{1}, score:{2}, graphic queue: {3} : {4}, present queue: {5} : {6}", __FUNCTION__, maxScorePhyDeviceIndex, maxScore,
			m_GraphicQueueFamily.queueFamilyIndex, m_GraphicQueueFamily.queueCount,
			m_PresentQueueFamily.queueFamilyIndex, m_PresentQueueFamily.queueCount);
	}
	void AdVKGraphicContext::PrintPhyDeviceInfo(VkPhysicalDeviceProperties& props)
	{
		const char* deviceType = props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ? "integrated gpu" :
			props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? "discrete gpu" :
			props.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU ? "virtual gpu" :
			props.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU ? "cpu" : "others";

		uint32_t driverVersionMajor = VK_VERSION_MAJOR(props.driverVersion);
		uint32_t driverVersionMinor = VK_VERSION_MINOR(props.driverVersion);
		uint32_t driverVersionPatch = VK_VERSION_PATCH(props.driverVersion);

		uint32_t apiVersionMajor = VK_VERSION_MAJOR(props.apiVersion);
		uint32_t apiVersionMinor = VK_VERSION_MINOR(props.apiVersion);
		uint32_t apiVersionPatch = VK_VERSION_PATCH(props.apiVersion);

		LOG_D("-----------------------------");
		LOG_D("deviceName       : {0}", props.deviceName);
		LOG_D("deviceType       : {0}", deviceType);
		LOG_D("vendorID         : {0}", props.vendorID);
		LOG_D("deviceID         : {0}", props.deviceID);
		LOG_D("driverVersion    : {0}.{1}.{2}", driverVersionMajor, driverVersionMinor, driverVersionPatch);
		LOG_D("apiVersion       : {0}.{1}.{2}", apiVersionMajor, apiVersionMinor, apiVersionPatch);
	}

	uint32_t AdVKGraphicContext::GetPhyDeviceScore(VkPhysicalDeviceProperties& props)
	{
		VkPhysicalDeviceType deviceType = props.deviceType;
		uint32_t score = 0;
		switch (deviceType)
		{
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
			break;
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			score += 40;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			score += 30;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			score += 20;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			score += 10;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
			break;
		}
		return score;
	}
}