#pragma once
#include "Vulkan.h"
#include "Cinder/Renderer/BufferLayout.h"

namespace Cinder {

	class Shader
	{
	public:
		Shader(const std::string vertPath, const std::string fragPath);
		~Shader();

		const std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages(){ return m_ShaderStages; }
		const VertexBufferLayout& getVertexBufferLayout() { return m_Layout; }
	private:
		std::vector<uint32_t> readFile(const std::string& filepath);
		void createShaderModule(const std::vector<uint32_t>& code, VkShaderModule* shaderModule);
		void Reflection(const std::vector<uint32_t>& code);
	private:
		VkShaderModule m_VertShaderModule;
		VkShaderModule m_FragShaderModule;
		std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
		VertexBufferLayout m_Layout;
	};
}