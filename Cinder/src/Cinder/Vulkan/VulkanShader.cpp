#include "cnpch.h"
#include "VulkanShader.h"
#include "Cinder/Core/Application.h"

#include "spirv_cross/spirv_cross.hpp"

namespace Cinder {

	static ShaderDataType SpirTypeToShaderType(spirv_cross::SPIRType type)
	{
		if (type.basetype == spirv_cross::SPIRType::BaseType::Float)
		{
			if (type.vecsize == 1) return ShaderDataType::Float;
			else if (type.vecsize == 2) return ShaderDataType::Float2;
			else if (type.vecsize == 3) return ShaderDataType::Float3;
			else if (type.vecsize == 4) return ShaderDataType::Float4;
		}

		if (type.basetype == spirv_cross::SPIRType::BaseType::Int)
		{
			if (type.vecsize == 1) return ShaderDataType::Int;
			else if (type.vecsize == 2) return ShaderDataType::Int2;
			else if (type.vecsize == 3) return ShaderDataType::Int3;
			else if (type.vecsize == 4) return ShaderDataType::Int4;
		}
		CN_CORE_ASSERT(false, "Could not find ShaderDataType from SPIRType");
		return ShaderDataType::None;
	}

	Shader::Shader(const std::string vertPath, const std::string fragPath)
	{
		auto vertCode = readFile(vertPath);
		auto fragCode = readFile(fragPath);

		m_Layout = reflection(vertCode);

		CN_CORE_INFO("Vertex Buffer: {}", m_Layout);

		createShaderModule(vertCode, &m_VertShaderModule);
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

	VertexBufferLayout Shader::reflection(const std::vector<uint32_t>& code)
	{
		spirv_cross::Compiler comp(std::move(code));

		spirv_cross::ShaderResources res = comp.get_shader_resources();

		std::vector<VertexBufferElement> vertex_elements;

		for (const spirv_cross::Resource& resource : res.stage_inputs)
		{
			unsigned set = comp.get_decoration(resource.id, spv::DecorationLocation);
			//unsigned binding = comp.get_decoration(resource.base_type_id, spv::DecorationBinding);
			const spirv_cross::SPIRType& base_type = comp.get_type(resource.base_type_id);
			const spirv_cross::SPIRType& type = comp.get_type(resource.type_id);
			//CN_CORE_INFO("Set {0}, Base_type {1}, Name {2}", set, base_type.basetype, resource.name);
			//CN_CORE_INFO("Width {0}, Vecsize {1}, Columns {2}", base_type.width, base_type.vecsize, base_type.columns);

			vertex_elements.push_back(VertexBufferElement(SpirTypeToShaderType(base_type), resource.name));
		}
		return VertexBufferLayout(vertex_elements);
		
	}

}