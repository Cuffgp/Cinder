#pragma once
#include "Vulkan.h"
#include "VulkanDevice.h"

#include "Cinder/Core/Base.h"
#include "VulkanVertexBuffer.h"
#include "VulkanShader.h"

namespace Cinder {

	struct PipelineConfigInfo {

		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class VulkanPipeline 
	{
	public:
		VulkanPipeline(
			Ref<Shader> shader,
			const PipelineConfigInfo& configInfo);
		~VulkanPipeline();

		VulkanPipeline(const VulkanPipeline&) = delete;
		VulkanPipeline& operator=(const VulkanPipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);

		static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

	private:
		static std::vector<char> readFile(const std::string& filepath);

		void createGraphicsPipeline(
			Ref<Shader> shader,
			const PipelineConfigInfo& configInfo);
		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		std::vector<VkVertexInputBindingDescription> getBindingDescriptions(VertexBufferLayout layout);
		std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions(VertexBufferLayout layout);

		Ref<VulkanDevice> m_VulkanDevice;
		VertexBufferLayout m_Layout;
		VkPipeline graphicsPipeline;
	};

}