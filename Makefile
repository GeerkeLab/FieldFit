extern/premake5:
	wget "https://github.com/premake/premake-core/releases/download/v5.0.0-alpha11/premake-5.0.0-alpha11-linux.tar.gz" -O premake-5.0.0-alpha11-linux.tar.gz
	tar -xvf premake-5.0.0-alpha11-linux.tar.gz
	premake5 extern/
