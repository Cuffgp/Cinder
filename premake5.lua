workspace "Cinder"
	architecture "x64"
	startproject "Cinder"

	configurations 
	{
		"Debug",
		"Release"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Cinder/vendor/glfw/include"
IncludeDir["ImGui"] = "Cinder/vendor/imgui"
IncludeDir["Vulkan"] = "C:/VulkanSDK/1.2.170.0/Include"
IncludeDir["glm"] = "Cinder/vendor/glm"
IncludeDir["tinyobjloader"] = "Cinder/vendor/tinyobjloader"
IncludeDir["stb_image"] = "Cinder/vendor/stb_image"
IncludeDir["VulkanMemoryAllocator"] = "Cinder/vendor/VulkanMemoryAllocator"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "C:/VulkanSDK/1.2.170.0/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{wks.location}/Cinder/vendor/VulkanSDK/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Cross_Reflect_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-reflectd.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"
Library["SPIRV_Cross_Reflect_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-reflect.lib"


group "Dependencies"
	include "Cinder/vendor/"

project "Cinder"
	location "Cinder"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	pchheader "cnpch.h"
	pchsource "Cinder/src/cnpch.cpp"

	files 
	{ 
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.c",
		"%{prj.name}/src/**.hpp", 
		"%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/VulkanMemoryAllocator/**.h",
		"%{prj.name}/vendor/VulkanMemoryAllocator/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.Vulkan}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.tinyobjloader}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.VulkanMemoryAllocator}",
		"%{prj.name}/vendor"
	}

	links
	{
		"GLFW",
		"ImGui",
		"%{Library.Vulkan}"
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
		runtime "Debug"
		symbols "On"
		
		links
		{
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_Reflect_Debug}"
		}

	filter "configurations:Release"
		defines { "NDEBUG" }
		runtime "Release"
		optimize "On"
		
		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_Reflect_Release}"
		}