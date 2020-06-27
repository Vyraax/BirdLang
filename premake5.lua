workspace "BirdLang"
	architecture "x64"
	startproject "Compiler"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Compiler"
	location "Compiler"
	kind "StaticLib"
	staticruntime "on"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "Compiler/src/pch.cpp"

	files
	{
		"%{prj.name}/include/**.h",
		"%{prj.name}/include/**.hpp",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/grammar.txt"
	}

	includedirs
	{
		"%{prj.name}/include"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "DEBUG"
		buildoptions {"/MTd"}
		symbols "On"

	filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
		optimize "On"

project "Interpreter"
	location "Interpreter"

	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Compiler/include",
		"Compiler/src",
	}

	links
	{
		"Compiler"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "DEBUG"
		kind "ConsoleApp"
		symbols "On"
		
	filter "configurations:Release"
		defines "RELEASE"
		optimize "On"
		kind "ConsoleApp"


project "Tests"
	location "Tests"

	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/**.h",
		"%{prj.name}/**.cpp"
	}

	includedirs
	{
		"Compiler/src"
	}

	links
	{
		"Compiler"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "DEBUG"
		kind "ConsoleApp"
		symbols "On"
		
	filter "configurations:Release"
		defines "RELEASE"
		optimize "On"
		kind "ConsoleApp"
		entrypoint "mainCRTStartup" 