#pragma once

#include "Cinder/Core/Layer.h"

#include "Cinder/Events/Event.h"
#include "Cinder/Vulkan/VulkanDevice.h"
#include "Cinder/Vulkan/VulkanRenderer.h"
#include "Cinder/Core/Window.h"

namespace Cinder {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer(Ref<VulkanRenderer> renderer, Ref<Window> window);
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End(VkCommandBuffer imGuiCommandBuffer);

	private:
		Ref<VulkanRenderer> m_Renderer;
		Ref<VulkanDevice> m_Device;
		Ref<Window> m_Window;
	};

}