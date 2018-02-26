  workspace "Envy CS"
    configurations {"Debug x86", "Release x86"}
    location "build"
    filter "configurations:*32"
      architecture "x86"
    configuration "vs*"
      defines { "_CRT_SECURE_NO_WARNINGS" }
    filter "configurations:Debug x86"
      targetdir "bin/x86/Debug"
      defines { "DEBUG" }
      symbols "On"
    filter "configurations:Release x86"
      targetdir "bin/x86/Release"
      defines { "NDEBUG" }
      optimize "On"
project "Envy Injector"
  kind "ConsoleApp"
  language "C++"
  files { "injector/include/**.h", "injector/include/**.hpp", "injector/src/**.cpp", "injector/src/**.c", "shared/include/**.h", "shared/include/**.hpp", "shared/src/**.cpp" }
  libdirs {
    "injector/lib",
    "injector/lib/**"
  }
  includedirs {
    "injector/include/",
    "injector/include/**",
    "shared/include/",
    "shared/include/**"
  }
  configuration "vs*"
    characterset "MBCS"
project "Envy Module"
  kind "SharedLib"
  language "C++"
  files { 
    "module/include/**.h", 
    "module/include/**.hpp",
    "module/src/**.cpp", 
    "module/sourcesdk/include/**.h", 
    "module/sourcesdk/include/**.hpp", 
    "module/sourcesdk/src/**.cpp", 
    "shared/include/**.h", 
    "shared/include/**.hpp", 
    "shared/src/**.cpp",
    "external/**.h",
    "external/**.cpp"
  }
  includedirs { 
    "injector/include/", 
    "injector/include/**",
    "module/include/", 
    "module/include/**", 
    "shared/include/",
    "shared/include/**",
    "module/sourcesdk/",
    "module/sourcesdk/**",
    "external/",
    "external/**",
    "F:\\Microsoft DirectX SDK (June 2010)\\Include"
  }
  libdirs {
    "F:\\Microsoft DirectX SDK (June 2010)\\Lib\\x86"
  }
  configuration "vs*"
    characterset "MBCS"