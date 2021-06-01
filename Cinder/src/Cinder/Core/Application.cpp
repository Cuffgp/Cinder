#include "cnpch.h"
#include "Cinder/Core/Application.h"
#include "Cinder/Vulkan/VulkanAllocator.h"

#include "GLFW/glfw3.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/ext.hpp>

namespace Cinder {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		CN_CORE_ASSERT(!s_Instance, "Application already exsists!")
		s_Instance = this;

		m_Window = Window::Create();
		m_Window->SetEventCallback(CN_BIND_EVENT_FN(OnEvent));
		m_Window->Maximize();

		m_Device = CreateRef<VulkanDevice>(m_Window->GetNativeWindow());

		VulkanAllocator::Init();

		m_Renderer = CreateRef<VulkanRenderer>();
		m_Shader = CreateRef<Shader>("assets/shaders/Uniform.vert.spv", "assets/shaders/Uniform.frag.spv");
		m_Shader2 = CreateRef<Shader>("assets/shaders/UniformTexture.vert.spv", "assets/shaders/UniformTexture.frag.spv");

		m_Model = CreateRef<Model>("assets/objects/Spot.obj");

		createPipelineLayout();
		createPipeline(m_Renderer->getSwapChainRenderPass());

		m_ImGuiLayer = new ImGuiLayer(m_Renderer, m_Window);
		PushOverlay(m_ImGuiLayer);

		float vertexData[] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f
		};

		uint32_t indexData[] = {
			0, 1, 2, 2, 3, 0
		};

		m_Vertex = CreateRef<VertexBuffer>(vertexData, sizeof(vertexData));
		m_Index = CreateRef<IndexBuffer>(indexData, sizeof(indexData));

	}

	Application::~Application()
	{

	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(CN_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(CN_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}
	}

	void Application::Run()
	{
		while (m_Running)
		{
			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);

				m_Window->OnUpdate();

				auto commandBuffer = m_Renderer->beginFrame();
				m_Renderer->beginSwapChainRenderPass(commandBuffer);

				updateUniformBuffer();
				renderGameObjects(commandBuffer);
				//drawIndexed(commandBuffer);
				m_ImGuiLayer->Begin();
				m_ImGuiLayer->End(commandBuffer);

				m_Renderer->endSwapChainRenderPass(commandBuffer);
				m_Renderer->endFrame();

			}
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Renderer->OnWindowResize(e.GetWidth(), e.GetHeight());

		m_Minimized = false;

		return false;
	}

	void Application::createPipelineLayout()
	{
		//VkPushConstantRange pushConstantRange{};
		//pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		//pushConstantRange.offset = 0;
		//pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &m_Renderer->getDescriptorLayout();
		//pipelineLayoutInfo.setLayoutCount = 0;
		//pipelineLayoutInfo.pSetLayouts = nullptr;
		//pipelineLayoutInfo.pushConstantRangeCount = 1;
		//pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(m_Device->device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void Application::createPipeline(VkRenderPass renderPass)
	{
		CN_ASSERT(pipelineLayout, "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		VulkanPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		m_Pipeline = std::make_unique<VulkanPipeline>(
			m_Shader,
			pipelineConfig);
	}

	void Application::renderGameObjects(VkCommandBuffer commandBuffer)
	{
		m_Pipeline->bind(commandBuffer);
		m_Renderer->bindDescriptorSet(pipelineLayout);
		m_Model->bind(commandBuffer);
		m_Model->draw(commandBuffer);
	}

	void Application::drawIndexed(VkCommandBuffer commandBuffer)
	{
		m_Pipeline->bind(commandBuffer);

		VkBuffer vertexBuffers[] = { m_Vertex->GetVulkanBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, m_Index->GetVulkanBuffer(), 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(commandBuffer, m_Index->GetIndexCount(), 1, 0, 0, 0);

	}

	void Application::updateUniformBuffer()
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.view = glm::lookAt(glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), (float)m_Window->GetWidth() / (float)m_Window->GetHeight(), 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		m_Renderer->updateUniformBuffer(ubo);
	}

	
}
