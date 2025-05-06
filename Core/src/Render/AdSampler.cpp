#include "Core/Render/AdSampler.h"
#include "Core/Render/AdRenderContext.h"
#include "Core/AdApplication.h"
#include "Platform/Graphic/AdVKDevice.h"
namespace ade
{
	AdSampler::AdSampler(VkFilter filter /*= VK_FILTER_LINEAR*/, VkSamplerAddressMode addressMode /*= VK_SAMPLER_ADDRESS_MODE_REPEAT*/)
		:mFilter(filter), mAddressMode(addressMode)
	{
		ade::AdRenderContext* renderCxt = AdApplication::GetAppContext()->renderCxt;
		ade::AdVKDevice* device = renderCxt->GetDevice();
		
		CALL_VK(device->CreateSimpleSampler(filter, mAddressMode, &mHandle));
	}

	AdSampler::~AdSampler()
	{
		ade::AdRenderContext* renderCxt = AdApplication::GetAppContext()->renderCxt;
		ade::AdVKDevice* device = renderCxt->GetDevice();
		VK_D(Sampler, device->GetHandle(), mHandle);
	}
}