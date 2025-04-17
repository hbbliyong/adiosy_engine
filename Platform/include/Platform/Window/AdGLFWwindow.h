#pragma once

#include "AdWindow.h"
#include "glfw/glfw3.h"
namespace ade
{
	class AdGLFWwindow :public AdWindow
	{
	public:
		AdGLFWwindow() = delete;
		AdGLFWwindow(uint32_t width, uint32_t height, const char* title);
		virtual ~AdGLFWwindow() override;

		void* GetImplWindowPointer() const override { return m_GLFWwindow; };
	private:
		GLFWwindow* m_GLFWwindow;

		// Í¨¹ý AdWindow ¼Ì³Ð
		virtual bool ShouldClose() override;
		virtual void PollEvent() override;
		virtual void SwapBuffer() override;
	};
}