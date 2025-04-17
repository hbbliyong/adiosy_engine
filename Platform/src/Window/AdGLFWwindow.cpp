#pragma once

#include "Window/AdGLFWWindow.h"
#include "AdLog.h"
#include "glfw/glfw3native.h"

namespace ade
{
	AdGLFWwindow::AdGLFWwindow(uint32_t width, uint32_t height, const char* title)
	{
		if (!glfwInit())
		{
			LOG_E("Failed to init glfw.");
			return;
		}
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
		m_GLFWwindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
		if (!m_GLFWwindow)
		{
			LOG_E("Failed to create glfw window.");
			return;
		}
		//…Ë÷√¥∞ÃÂæ”÷–
		GLFWmonitor* primaryMonitro = glfwGetPrimaryMonitor();
		if (primaryMonitro)
		{
			int xPos, yPos, workWidth, workHeight;
			glfwGetMonitorWorkarea(primaryMonitro, &xPos, &yPos, &workWidth, &workHeight);
			glfwSetWindowPos(m_GLFWwindow,workWidth / 2 - width / 2, workHeight / 2 - height / 2);
		}
		glfwMakeContextCurrent(m_GLFWwindow);

		glfwShowWindow(m_GLFWwindow);
	}

	AdGLFWwindow::~AdGLFWwindow()
	{
		glfwDestroyWindow(m_GLFWwindow);
		glfwTerminate();
		LOG_I("The application runing end.");
	}
	bool AdGLFWwindow::ShouldClose()
	{
		return glfwWindowShouldClose(m_GLFWwindow);
	}
	void AdGLFWwindow::PollEvent()
	{
		glfwPollEvents();
	}
	void AdGLFWwindow::SwapBuffer()
	{
		glfwSwapBuffers(m_GLFWwindow);
	}
}