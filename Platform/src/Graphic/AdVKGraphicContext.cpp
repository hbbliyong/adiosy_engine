#include "Graphic/AdVKGraphicContext.h"
#include "Window/AdGLFWwindow.h"
namespace ade
{
	// 扩展和layer要求
	const DeviceFeature requestedLayers[] = {
		{ "VK_LAYER_KHRONOS_validation", true },
	};

	const DeviceFeature requestedExtensions[] = {
		{ VK_KHR_SURFACE_EXTENSION_NAME, true },
	#ifdef AD_ENGINE_PLATFORM_WIN32
		{ VK_KHR_WIN32_SURFACE_EXTENSION_NAME, true },
	#elif defined(AD_ENGINE_PLATFORM_MACOS)
		{ VK_MVK_MACOS_SURFACE_EXTENSION_NAME, true },
	#elif defined(AD_ENGINE_PLATFORM_LINUX)
		{ VK_KHR_XCB_SURFACE_EXTENSION_NAME, true },
	#endif
		{ VK_EXT_DEBUG_UTILS_EXTENSION_NAME, true },
	};

	// 声明外部回调函数
	static VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugUtilsMessengerCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			LOG_E("{0}", pCallbackData->pMessage);
		}
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			LOG_W("{0}", pCallbackData->pMessage);
		}
		else
		{
			LOG_T("{0}", pCallbackData->pMessage);
		}
		return VK_FALSE;
	}

	AdVKGraphicContext::AdVKGraphicContext(AdWindow* window)
	{
		CreateInstance();
		SetupDebugMessenger();
		CreateSurface(window);
		SelectPhyDevice();
	}
	AdVKGraphicContext:: ~AdVKGraphicContext()
	{
		if (m_DebugMessenger != VK_NULL_HANDLE)
		{
			auto destroyFunc = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
			if (destroyFunc)
			{
				destroyFunc(m_Instance, m_DebugMessenger, nullptr);
			}
		}
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyInstance(m_Instance, nullptr);
	}
	void  AdVKGraphicContext::CreateInstance()
	{
	

		// 查询可用的layer
		uint32_t availableLayerCount;
		CALL_VK(vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr));
		std::vector<VkLayerProperties> availableLayers(availableLayerCount);
		CALL_VK(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data()));

		// 查询可用的extension
		uint32_t availableExtensionCount;
		CALL_VK(vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr));
		std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
		CALL_VK(vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data()));

		// glfw要求的extension
		uint32_t glfwExtensionCount;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::unordered_set<std::string_view> requestedExtensionSet;
		std::vector<DeviceFeature> allRequestedExtensions(std::begin(requestedExtensions), std::end(requestedExtensions));
		for (uint32_t i = 0; i < glfwExtensionCount; ++i)
		{
			allRequestedExtensions.push_back({ glfwExtensions[i], true });
		}

		uint32_t enableLayerCount = 0;
		const char* enableLayers[32] = {};
		if (m_bShouldValidate)
		{
			if (!checkDeviceFeatures("Instance Layers", false, availableLayerCount, availableLayers.data(),
				ARRAY_SIZE(requestedLayers), requestedLayers, &enableLayerCount, enableLayers))
			{
				return;
			}
		}

		uint32_t enableExtensionCount = 0;
		const char* enableExtensions[64] = {};
		if (!checkDeviceFeatures("Instance Extensions", true, availableExtensionCount, availableExtensions.data(),
			allRequestedExtensions.size(), allRequestedExtensions.data(), &enableExtensionCount, enableExtensions))
		{
			return;
		}

		uint32_t instanceVersion = 0;
		vkEnumerateInstanceVersion(&instanceVersion);
		LOG_I("Driver supports Vulkan version: {0}.{1}.{2}",
			VK_VERSION_MAJOR(instanceVersion),
			VK_VERSION_MINOR(instanceVersion),
			VK_VERSION_PATCH(instanceVersion));

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Adiosy_Engine";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "None";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_1;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledLayerCount = enableLayerCount;
		createInfo.ppEnabledLayerNames = enableLayerCount > 0 ? enableLayers : nullptr;
		createInfo.enabledExtensionCount = enableExtensionCount;
		createInfo.ppEnabledExtensionNames = enableExtensionCount > 0 ? enableExtensions : nullptr;

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		if (m_bShouldValidate)
		{
			PopulateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = &debugCreateInfo;
		}

		CALL_VK(vkCreateInstance(&createInfo, nullptr, &m_Instance));
		LOG_T("{0}: Created Vulkan Instance {1}", __FUNCTION__, (void*)m_Instance);
	}
	void AdVKGraphicContext::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = VkDebugUtilsMessengerCallback;
	}

	void AdVKGraphicContext::SetupDebugMessenger()
	{
		if (!m_bShouldValidate) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		PopulateDebugMessengerCreateInfo(createInfo);

		auto createFunc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
		if (createFunc != nullptr)
		{
			CALL_VK(createFunc(m_Instance, &createInfo, nullptr, &m_DebugMessenger));
		}
		else
		{
			LOG_W("Could not load vkCreateDebugUtilsMessengerEXT!");
		}
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
			vkGetPhysicalDeviceQueueFamilyProperties(phyDevices[i], &queueFamilyCount, nullptr);
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