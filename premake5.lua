
--[[ @cond ___LICENSE___
-- Copyright (c) 2016 Koen Visscher, Paul Visscher and individual contributors.
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
-- THE SOFTWARE.
--
-- @endcond
--]]

workspace "FieldFit"

    local config = { "Debug", "Release", "OptimisedDebug" }
    configurations( config )

--  only support 64 bit
    platforms { "x86_64" }

    startproject( "FieldFit" )
    location( "." )
    objdir "bin/obj/"

    vectorextensions "SSE"
    warnings "Extra"
	
    flags {  "C++11" }
    characterset "Unicode"

    filter "system:not windows"
        configurations { "Coverage" }
    
    filter "platforms:x86"
        targetdir "bin/x86/"
        debugdir "bin/x86/"
        architecture "x86"
    
    filter "platforms:x86_64"
        targetdir "bin/x86_64/"
        debugdir "bin/x86_64/"
        architecture "x86_64"
    
    filter "*Release"
        optimize "Speed"
        defines "NDEBUG"
    
    filter "*Debug"
        targetsuffix "d"
        defines "DEBUG"
        symbols "On"
        optimize "Off"
        
    filter "*OptimisedDebug"
        targetsuffix "od"
        flags "LinkTimeOptimization"
        symbols "On"
        optimize "Speed"

    filter "Coverage" 
        targetsuffix "cd"
        links "gcov"
        buildoptions "-coverage" 

        objdir "!./"
        targetdir "!./"
        
    filter {}

    project( "FieldFit" )
    
        targetname( "FieldFit" )
        kind "ConsoleApp"
        flags "WinMain"
        
	   	links { "lapack", "blas" }
	    buildoptions "-std=c++11"
        
        defines {
                "ARMA_DONT_PRINT_CXX11_WARNING",
                "ARMA_USE_CXX11",
                "ARMA_USE_SUPERLU",
                "ARMA_USE_BLAS",
                "ARMA_USE_ARPACK"
            }
        
        filter "*Release"
            defines "ARMA_NO_DEBUG"
        filter{}        
                    
        
        includedirs {
                "include/",
                "extern/armadillo-7.600.2/include/",
                "extern/tclap-1.2.1/include/"
            }
    
        files { 
                "include/**.hpp",
                "include/**.h"
            }
              
        files { 
                "source/**.cpp"
              }
              
        filter {}
            
    workspace()
