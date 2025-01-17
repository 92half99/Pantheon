project "Pantheon-Common"
   kind "StaticLib"
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

      "../Utopia/Utopia/Source",
      "../Utopia-Networking/Source",

      "%{IncludeDir.VulkanSDK}",
      "../Utopia/vendor/spdlog/include",

      "../Utopia-Networking/vendor/GameNetworkingSockets/include"
   }

   links
   {
       "Utopia",
       "Utopia-Networking",
   }

   targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

   filter "system:windows"
      systemversion "latest"
      defines { "UT_PLATFORM_WINDOWS" }

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