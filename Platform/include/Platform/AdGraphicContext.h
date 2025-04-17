#pragma once
#include "AdEngine.h"
namespace ade
{
	class AdWindow;
	class AdGraphicContext
	{
	public:
		AdGraphicContext(const AdGraphicContext&) = delete;
		AdGraphicContext& operator&=(const AdGraphicContext&) = delete;
		virtual ~AdGraphicContext() = default;

		static std::unique_ptr<AdGraphicContext> Create(AdWindow* window);
	protected:
		AdGraphicContext() = default;
	};
}