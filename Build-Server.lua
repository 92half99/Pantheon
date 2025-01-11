workspace "Pantheon-Server"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Pantheon-Server"

   -- Workspace-wide defines
   defines
   {
       "UT_HEADLESS"
   }

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

-- Directories
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
UtopiaNetworkingBinDir = "Utopia/Utopia-Modules/Utopia-Networking/vendor/GameNetworkingSockets/bin/%{cfg.system}/%{cfg.buildcfg}/"

include "Utopia/Build-Utopia-Headless-External.lua"

group "App"
    include "Pantheon-Common/Build-Pantheon-Common-Headless.lua"
    include "Pantheon-Server/Build-Pantheon-Server-Headless.lua"
group ""