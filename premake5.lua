workspace "debugmenu"
	configurations { "Release", "Debug" }
	location "build"

	files { "src/*.*" }

project "debugmenu_III"
	kind "SharedLib"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"
	targetextension ".dll"
	characterset ("MBCS")
	defines { "GTA3" }
	includedirs { os.getenv("RWSDK33") }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
		debugdir "C:/Users/aap/games/gta3"
		debugcommand "C:/Users/aap/games/gta3/gta3.exe"
		postbuildcommands "copy /y \"$(TargetPath)\" \"C:\\Users\\aap\\games\\gta3\\debugmenu.dll\""

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
		flags { "StaticRuntime" }
		debugdir "C:/Users/aap/games/gta3"
		debugcommand "C:/Users/aap/games/gta3/gta3.exe"
		postbuildcommands "copy /y \"$(TargetPath)\" \"C:\\Users\\aap\\games\\gta3\\debugmenu.dll\""

project "debugmenu_VC"
	kind "SharedLib"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"
	targetextension ".dll"
	characterset ("MBCS")
	defines { "GTAVC" }
	includedirs { os.getenv("RWSDK34") }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
		debugdir "C:/Users/aap/games/gtavc"
		debugcommand "C:/Users/aap/games/gtavc/gta_vc.exe"
		postbuildcommands "copy /y \"$(TargetPath)\" \"C:\\Users\\aap\\games\\gtavc\\debugmenu.dll\""

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
		flags { "StaticRuntime" }
		debugdir "C:/Users/aap/games/gtavc"
		debugcommand "C:/Users/aap/games/gtavc/gta_vc.exe"
		postbuildcommands "copy /y \"$(TargetPath)\" \"C:\\Users\\aap\\games\\gtavc\\debugmenu.dll\""

project "debugmenu_SA"
	kind "SharedLib"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"
	targetextension ".dll"
	characterset ("MBCS")
	defines { "GTASA" }
	includedirs { os.getenv("RWSDK36") }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
		debugdir "C:/Users/aap/games/gtasa"
		debugcommand "C:/Users/aap/games/gtasa/gta_sa.exe"
		postbuildcommands "copy /y \"$(TargetPath)\" \"C:\\Users\\aap\\games\\gtasa\\debugmenu.dll\""

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
		flags { "StaticRuntime" }
		debugdir "C:/Users/aap/games/gtasa"
		debugcommand "C:/Users/aap/games/gtasa/gta_sa.exe"
		postbuildcommands "copy /y \"$(TargetPath)\" \"C:\\Users\\aap\\games\\gtasa\\debugmenu.dll\""
