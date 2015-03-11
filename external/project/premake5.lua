local root      = "../../"
solution "fieldfit"
	location( root )
	objdir( root .. "bin/obj/x64/Release" )
	debugdir( root .. "bin/" )
	
	configurations { "Release" }
	platforms { "x64" }
	vectorextensions "SSE2"
	floatingpoint "Fast"	
	warnings "Extra"
	flags "Unicode"	
    configuration "x64"
		targetdir( root .. "bin/x64/" )
		architecture "x64"
		
	configuration "Release"		
		flags "LinkTimeOptimization"
		optimize "Speed"
	configuration "gmake"
		links { "lapack" , "blas" , "lapacke" }
		buildoptions "-Bstatic -std=c++11 -W"
		
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
