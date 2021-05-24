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
LibraryDir["Vulkan"] = "C:/VulkanSDK/1.2.170.0/Lib/vulkan-1.lib"


group "Dependencies"
	include "Cinder/vendor/"

project "Cinder"
	location "Cinder"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

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
		"%{LibraryDir.Vulkan}"
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		runtime "Release"
		optimize "On"