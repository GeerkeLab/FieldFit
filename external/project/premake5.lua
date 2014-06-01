local root      = "../../"

solution "fieldfit"

	location( root )
	objdir( root .. "bin/obj/" )
	debugdir( root .. "bin/" )
	
	configurations { "Debug", "Release" }

	platforms { "x64", "x32" }

	vectorextensions "SSE2"

	floatingpoint "Fast"	

	warnings "Extra"

	flags "Unicode"	

    configuration "x32"
		targetdir( root .. "bin/x32/" )
		architecture "x32"

    configuration "x64"
		targetdir( root .. "bin/x64/" )
		architecture "x64"
		
	configuration "Debug"
		targetsuffix "d"
		defines "DEBUG"
		flags "Symbols"
		optimize "Off"

	configuration "Release"		
		flags "LinkTimeOptimization"
		optimize "Speed"

	configuration "gmake"
		buildoptions "-std=c++11"
		
	configuration {}

	project "fieldfit"
		targetname "fieldfit"	 
		kind "ConsoleApp"

		includedirs {
			}	
			
		files { 
			root .. "src/*.cpp",
			root .. "src/*.h",
			root .. "src/**.cpp",
			root .. "src/**.h",
			}