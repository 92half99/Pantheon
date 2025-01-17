-- App.lua
workspace "UtopiaApp"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "UtopiaApp"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

   defines { "IMGUI_DEFINE_MATH_OPERATORS" }

-- Directories
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
UtopiaNetworkingBinDir = "Utopia/Utopia-Modules/Utopia-Networking/vendor/GameNetworkingSockets/bin/%{cfg.system}/%{cfg.buildcfg}/"

include "Utopia/Build-Utopia-External.lua"

include "UtopiaApp/Build-Utopia.lua"