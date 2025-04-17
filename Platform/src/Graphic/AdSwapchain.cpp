#include "Graphic/AdSwapchain.h"
#include "Graphic/AdVKDevice.h"
namespace ade
{
    AdSwapchain::AdSwapchain(AdVKGraphicContext* context, AdVKDevice* device):
        m_Context(context),m_Device(device)
    {
    }
    AdSwapchain::~AdSwapchain()
    {
    }
    bool AdSwapchain::ReCreate()
    {
        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        CALL_VK(vkCreateSwapchainKHR(m_Device->GetHandle(), &swapchainCreateInfo, nullptr, &m_Handle));
        return false;
    }
} // namespace ade
