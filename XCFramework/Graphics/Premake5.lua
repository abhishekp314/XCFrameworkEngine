project "XCGraphics"
print("Building C++ Win32 Project XCGraphics")
	kind "StaticLib"
	language "C++"
	architecture "x86"
	targetdir "bin/%{cfg.buildcfg}"
	platformVersion "10.0.10586.0"
	
	files { "**.h", "**.cpp", "**.hlsl", "**.licenseheader" }
	
	--Configuration Debug
	print("Building Debug")
	filter "configurations:Debug"
		sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir);$(ProjectDir)Graphics/;" }
		
		includedirs { "$(ProjectDir)Libs", "$(ProjectDir)Libs/flatbuffers", "$(ProjectDir)Libs/assimp/include/", "$(ProjectDir)Libs/AdobeFBX/include/" }
		libdirs { "$(ProjectDir)bin/$(Configuration)/", "$(ProjectDir)Libs", "$(ProjectDir)Libs/flatbuffers", "$(ProjectDir)Libs/Assimp", "$(ProjectDir)Libs/AdobeFBX/x86/Debug/" }

		excludes { "**/Orbis/**", "**orbis.cpp", "**/Durango/**", "**durango.cpp", "**editor.cpp", "**/Editor/**", "**Editor.cpp", "**Editor**.*", "**/DDSTextureLoader_Legacy/**", "**.pssl", "**/src/**", "**Graphics/XC_Shaders/src/**"}
		
		links { "d3d12", "d2d1", "dwrite", "d3dcompiler", "FlatBuffers", "DirectXTex", "dxguid", "dxgi", "d3d11", "glut32", "glew32" }
		
		defines { "DEBUG", "WIN_32", "WIN32", "_DEBUG", "_WINDOWS" }
		pchheader "GraphicsPrecompiledHeader.h"
		pchsource "GraphicsPrecompiledHeader.cpp"
		
		flags { "Symbols" }

		targetdir "$(ProjectDir)bin/$(Configuration)/"
		
		postbuildcommands { "PostBuildProject.bat $(Configuration)" }
		
	--Configuration Release
	print("Building Release")
	filter "configurations:Release"
		sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir);$(ProjectDir)Graphics/;" }
		
		includedirs { "$(ProjectDir)Libs", "$(ProjectDir)Libs/flatbuffers", "$(ProjectDir)Libs/assimp/include/", "$(ProjectDir)Libs/AdobeFBX/include/" }
		libdirs { "$(ProjectDir)bin/$(Configuration)/", "$(ProjectDir)Libs", "$(ProjectDir)Libs/flatbuffers", "$(ProjectDir)Libs/Assimp", "$(ProjectDir)Libs/AdobeFBX/x86/Debug/" }

		excludes { "**/Orbis/**", "**orbis.cpp", "**/Durango/**", "**durango.cpp", "**editor.cpp", "**/Editor/**", "**Editor.cpp", "**Editor**.*", "**/DDSTextureLoader_Legacy/**", "**.pssl", "**/src/**", "**/XC_Shaders/src/**"}
		
		links { "d3d12", "d2d1", "dwrite", "d3dcompiler", "FlatBuffers", "ws2_32", "DirectXTex", "dxguid", "dxgi", "d3d11", "glut32", "glew32" }
		
		defines { "NDEBUG", "WIN_32", "WIN32", "_WINDOWS" }
		pchheader "GraphicsPrecompiledHeader.h"
		pchsource "GraphicsPrecompiledHeader.cpp"
		
		flags { "Symbols" }
		
		optimize "On"
		
		targetdir "$(ProjectDir)bin/$(Configuration)/"
		
		postbuildcommands { "PostBuildProject.bat $(Configuration)" }
				
print("Project Generation done...")