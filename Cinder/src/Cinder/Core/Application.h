#pragma once

#include "Cinder/Core/Base.h"

#include "Cinder/Core/Window.h"
#include "Cinder/Core/LayerStack.h"
#include "Cinder/Events/Event.h"
#include "Cinder/Events/ApplicationEvent.h"

#include "Cinder/Core/Timestep.h"

#include "Cinder/Vulkan/VulkanDevice.h"
#include "Cinder/Vulkan/VulkanPipeline.h"
#include "Cinder/Vulkan/VulkanModel.h"
#include "Cinder/Vulkan/VulkanRenderer.h"

namespace Cinder {

	class Application 
	{
	public:
		Application();
		~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }

	private:

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		Ref<Window> m_Window;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;

		Ref<VulkanDevice> m_Device;
		Ref<VulkanPipeline> m_Pipeline;
		Ref<VulkanRenderer> m_Renderer;

		Scope<VulkanModel> m_Model;

		VkPipelineLayout pipelineLayout;

		void loadModels();
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);
		//void recordCommandBuffer(int imageIndex);
		void renderGameObjects(VkCommandBuffer commandBuffer);
	};
}