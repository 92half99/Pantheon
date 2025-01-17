-- Utopia.lua
project "UtopiaApp"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"

   files { "Source/**.h", "Source/**.hpp", "Source/**.cpp" }

   includedirs
   {
      "../Utopia/vendor/imgui",
      "../Utopia/vendor/glfw/include",
      "../Utopia/vendor/glm",
      "../Utopia/vendor/spdlog/include",

      "../Utopia/Utopia/Source",
      "../Utopia/Utopia/Platform/GUI",

      "%{IncludeDir.VulkanSDK}",
      "%{IncludeDir.glm}",

      -- Utopia-Networking
      "../Utopia/Utopia-Modules/Utopia-Networking/Source",
      "../Utopia/Utopia-Modules/Utopia-Networking/vendor/GameNetworkingSockets/include"
   }

    links
    {
      "Utopia"
    }

   targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

   filter "system:windows"
      systemversion "latest"
      defines { "UT_PLATFORM_WINDOWS" }
      buildoptions { "/utf-8" }

      postbuildcommands 
      {
        '{COPY} "../%{UtopiaNetworkingBinDir}/GameNetworkingSockets.dll" "%{cfg.targetdir}"',
        '{COPY} "../%{UtopiaNetworkingBinDir}/libcrypto-3-x64.dll" "%{cfg.targetdir}"',
        '{COPY} "../%{UtopiaNetworkingBinDir}/libprotobufd.dll" "%{cfg.targetdir}"',
      }

   filter "configurations:Debug"
      defines { "UT_DEBUG" }
      runtime "Debug"
      symbols "On"

   filter "configurations:Release"
      defines { "UT_RELEASE" }
      runtime "Release"
      optimize "On"
      symbols "On"

   filter "configurations:Dist"
      kind "WindowedApp"
      defines { "UT_DIST" }
      runtime "Release"
      optimize "On"
      symbols "Off"