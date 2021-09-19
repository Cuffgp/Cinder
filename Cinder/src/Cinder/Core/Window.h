#pragma once

#include "Cinder/Core/Base.h"
#include "Cinder/Events/Event.h"
#include <GLFW/glfw3.h>

namespace Cinder {

	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(std::string title = "Cinder Window",
			uint32_t width = 1280, uint32_t height = 720) :
			Title(title), Width(width), Height(height)
		{}
	};

	class Window
	{

	public:
		using EventCallbackFn = std::function<void(Event&)>;

		Window(const WindowProps& props);
		~Window();

		void OnUpdate();

		unsigned int GetWidth() const { return m_Data.Width; }
		unsigned int GetHeight() const { return m_Data.Height; }
		float GetAspect() const { return (float)m_Data.Width / (float)m_Data.Height; }
		void Maximize() { glfwMaximizeWindow(m_Window); }

		// Window attributes
		void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }

		GLFWwindow* GetNativeWindow() const { return m_Window; }

		static Ref<Window> Create(const WindowProps& props = WindowProps());

	private:
		void Init(const WindowProps& props);
		void Shutdown();
	private:
		GLFWwindow* m_Window;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};

}