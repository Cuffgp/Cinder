#include "cnpch.h"
#include "VulkanShader.h"
#include "Cinder/Core/Application.h"

namespace Cinder {

	Shader::Shader(const std::string vertPath, const std::string fragPath)
	{
		auto vertCode = readFile(vertPath);
		auto fragCode = readFile(fragPath);

		CN_CORE_INFO("VertexShader");
		createShaderModule(vertCode, &m_VertShaderModule);
		CN_CORE_INFO("FragmentShader");
		createShaderModule(fragCode, &m_FragShaderModule);

		m_ShaderStages.resize(2);

		m_ShaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_ShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		m_ShaderStages[0].module = m_VertShaderModule;
		m_ShaderStages[0].pName = "main";
		m_ShaderStages[0].flags = 0;
		m_ShaderStages[0].pNext = nullptr;
		m_ShaderStages[0].pSpecializationInfo = nullptr;

		m_ShaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_ShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		m_ShaderStages[1].module = m_FragShaderModule;
		m_ShaderStages[1].pName = "main";
		m_ShaderStages[1].flags = 0;
		m_ShaderStages[1].pNext = nullptr;
		m_ShaderStages[1].pSpecializationInfo = nullptr;
	}

	Shader::~Shader()
	{
		vkDestroyShaderModule(Application::Get().GetVulkanDevice()->device(), m_VertShaderModule, nullptr);
		vkDestroyShaderModule(Application::Get().GetVulkanDevice()->device(), m_FragShaderModule, nullptr);
	}

	std::vector<uint32_t> Shader::readFile(const std::string& filepath)
	{
		std::ifstream file{ filepath, std::ios::ate | std::ios::binary };

		if (!file.is_open())
			CN_CORE_ERROR("failed to open file: " + filepath);

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);
		std::vector<uint32_t> out(fileSize / sizeof(uint32_t));

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		memcpy(out.data(), buffer.data(), fileSize);
		return out;
	}

	void Shader::createShaderModule(const std::vector<uint32_t>& code, VkShaderModule* shaderModule)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size()*sizeof(uint32_t);
		createInfo.pCode = code.data();

		if (vkCreateShaderModule(Application::Get().GetVulkanDevice()->device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
			CN_CORE_ERROR("failed to create shader module");
	}

}