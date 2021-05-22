#pragma once
#include "Vulkan.h"

namespace Cinder {

	class Shader
	{
	public:
		Shader(const std::string vertPath, const std::string fragPath);
		~Shader();

		const std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages(){ return m_ShaderStages; }

	private:
		std::vector<uint32_t> readFile(const std::string& filepath);
		void createShaderModule(const std::vector<uint32_t>& code, VkShaderModule* shaderModule);
	private:
		VkShaderModule m_VertShaderModule;
		VkShaderModule m_FragShaderModule;
		std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
	};
}