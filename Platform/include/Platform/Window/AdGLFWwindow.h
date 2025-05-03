#pragma once

#include "AdWindow.h"
#include "GLFW/glfw3.h"
namespace ade
{
	class AdGLFWwindow :public AdWindow
	{
	public:
		AdGLFWwindow() = delete;
		AdGLFWwindow(uint32_t width, uint32_t height, const char* title);
		virtual ~AdGLFWwindow() override;

		// Í¨¹ý AdWindow ¼Ì³Ð
		virtual bool ShouldClose() override;
		virtual void PollEvent() override;
		virtual void SwapBuffer() override;

		void* GetImplWindowPointer() const override { return mGLFWwindow; };
		void GetMousePos(glm::vec2& mousePos) const override;
		bool IsMouseDown(MouseButton mouseButton) const override;
		bool IsMouseUp(MouseButton mouseButton) const override;
		bool IsKeyDown(Key key) const override;
		bool IsKeyUp(Key key) const override;
	private:
		GLFWwindow* mGLFWwindow;

		void SetupWindowCallbacks();
	};
}