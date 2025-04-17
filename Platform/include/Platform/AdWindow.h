#pragma once

#include "AdEngine.h"

namespace ade
{
	class AdWindow
	{
	public:
		AdWindow(const AdWindow&) = delete;
		AdWindow& operator&=(const AdWindow&) = delete;
		virtual ~AdWindow() = default;

		static std::unique_ptr<AdWindow> Create(uint32_t width, uint32_t height, const char* title);

	
	virtual bool ShouldClose()=0;
	virtual void PollEvent()=0;
	virtual	void SwapBuffer()=0;

	virtual void* GetImplWindowPointer() const =0;
	protected:
		AdWindow() = default;
	};
}