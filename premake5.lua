include "dependencies/conandeps.premake5.lua"

workspace "CamEngine"
    configurations { "Debug", "Release" }
    architecture "x64"

    project "CamEngine"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"

        targetdir   "build/%{cfg.buildcfg}/bin"
        objdir      "build/%{cfg.buildcfg}/obj"
        
        location "./src"

        pchheader "pch.hpp"
        pchsource "src/pch.cpp"
        includedirs {
            "%{prj.location}",
            "%{prj.location}/**"  
        }
        
        files {
            "%{prj.location}/**.h",
            "%{prj.location}/**.hpp",
            "%{prj.location}/**.cpp"
        }

        flags {
            "MultiProcessorCompile",  
            "NoMinimalRebuild"        
        }

        warnings "Extra"                   
        vectorextensions "AVX2"           
        defines {
            "_CRT_SECURE_NO_WARNINGS",     
            "NOMINMAX",
            "WIN32_LEAN_AND_MEAN"
        }

        filter "configurations:Debug"
            defines { "DEBUG" }
            symbols "On"
            runtime "Debug"
            optimize "Off"
            editandcontinue "On"

        filter "configurations:Release"
            defines { "NDEBUG" }
            optimize "Speed"
            runtime "Release"
            flags { "LinkTimeOptimization" }
            inlining "Auto"

        filter {}

        conan_setup()
        linkoptions { "/IGNORE:4099" }
