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

        filter "configurations:Debug"
            defines { "DEBUG" }
            symbols "On"
        filter {}

        filter "configurations:Release"
            defines { "NDEBUG" }
            optimize "On"
        filter {}

        conan_setup()
        linkoptions { "/IGNORE:4099" }
