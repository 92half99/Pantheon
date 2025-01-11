workspace "Pantheon-Client"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Pantheon-Client"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

   defines { "IMGUI_DEFINE_MATH_OPERATORS" }

-- Directories
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
UtopiaNetworkingBinDir = "Utopia/Utopia-Modules/Utopia-Networking/vendor/GameNetworkingSockets/bin/%{cfg.system}/%{cfg.buildcfg}/"

include "Utopia/Build-Utopia-External.lua"

group "App"
    include "Pantheon-Common/Build-Pantheon-Common.lua"
    include "Pantheon-Client/Build-Pantheon-Client.lua"
group ""