project "Pantheon-Server"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"

   
   files { "Source/**.h", "Source/**.hpp", "Source/**.cpp" }

   includedirs
   {
      "../Pantheon-Common/Source",

      "../Utopia/vendor/glm",

      "../Utopia/Utopia/Source",
      "../Utopia/Utopia/Platform/Headless",

      "../Utopia/vendor/spdlog/include",
      "../Utopia/vendor/yaml-cpp/include",

      -- Utopia-Networking
      "../Utopia/Utopia-Modules/Utopia-Networking/Source",
      "../Utopia/Utopia-Modules/Utopia-Networking/vendor/GameNetworkingSockets/include"
   }

   links
   {
       "Pantheon-Common-Headless",
       "Utopia-Headless",
       "Utopia-Networking",

       "yaml-cpp",
   }

   	defines
	{
		"YAML_CPP_STATIC_DEFINE"
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

   filter "system:linux"
      libdirs { "../Utopia/Utopia-Networking/vendor/GameNetworkingSockets/bin/Linux" }
      links { "GameNetworkingSockets" }

       defines { "UT_HEADLESS" }

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
      defines { "UT_DIST" }
      runtime "Release"
      optimize "On"
      symbols "Off"