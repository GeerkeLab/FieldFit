local root      = "../"
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
    
    libdirs {"/usr/local/Cellar/lapack/3.6.0/lib/"}
	links { "lapack" , "blas" , "lapacke" }
	buildoptions " -std=c++11 "
		
	configuration {}
	project "fieldfit"
		targetname "fieldfit"	 
		kind "ConsoleApp"
		includedirs {
			}	
			
		files { 
			root .. "src/**.cpp",
			root .. "src/**.h",
			}
