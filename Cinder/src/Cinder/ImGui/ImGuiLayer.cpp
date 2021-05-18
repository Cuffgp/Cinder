#include "cnpch.h"
#include "ImGuiLayer.h"

#include "Cinder/Vulkan/Vulkan.h"

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

namespace Cinder {

	ImGuiLayer::ImGuiLayer(Ref<VulkanRenderer> renderer, Ref<Window> window)
		: Layer("ImGuiLayer"), m_Renderer(renderer), m_Device(renderer->getDevice()), m_Window(window)
	{

	}

	void ImGuiLayer::OnAttach()
	{
		// create descriptor pool for IMGUI
		// the size of the pool is very oversize, but it's copied from imgui demo itself.
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = std::size(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		VkDescriptorPool imguiPool;
		VK_CHECK_RESULT(vkCreateDescriptorPool(m_Device->device(), &pool_info, nullptr, &imguiPool));

		// create the render pass for imgui

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForVulkan(m_Window->GetNativeWindow(), true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = m_Device->getInstance();
		init_info.PhysicalDevice = m_Device->getPhysicalDevice();
		init_info.Device = m_Device->device();
		init_info.QueueFamily = 0; // g_QueueFamily
		init_info.Queue = m_Device->graphicsQueue();
		init_info.PipelineCache = nullptr;
		init_info.DescriptorPool = imguiPool;
		init_info.Allocator = nullptr;
		init_info.MinImageCount = 2;
		init_info.ImageCount = m_Renderer->getSwapChain()->imageCount();
		init_info.CheckVkResultFn = VulkanCheckResult;
		ImGui_ImplVulkan_Init(&init_info, m_Renderer->getSwapChainRenderPass());

		VkCommandBuffer command_buffer = m_Device->beginSingleTimeCommands();
		ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
		m_Device->endSingleTimeCommands(command_buffer);
	}

	void ImGuiLayer::OnDetach()
	{

	}

	void ImGuiLayer::OnEvent(Event& e)
	{

	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow();
	}

	void ImGuiLayer::End()
	{
		ImGui::Render();

		VkCommandBuffer drawCommandBuffer = m_Renderer->getCurrentCommandBuffer();

		VkClearValue clearValues[2];
		clearValues[0].color = { {0.1f, 0.1f,0.1f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };

		uint32_t width = m_Renderer->getSwapChain()->width();
		uint32_t height = m_Renderer->getSwapChain()->height();

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = nullptr;
		renderPassBeginInfo.renderPass = m_Renderer->getSwapChainRenderPass();
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent.width = width;
		renderPassBeginInfo.renderArea.extent.height = height;
		renderPassBeginInfo.clearValueCount = 2; // Color + depth
		renderPassBeginInfo.pClearValues = clearValues;
		renderPassBeginInfo.framebuffer = swapChain.GetCurrentFramebuffer();
	}

}