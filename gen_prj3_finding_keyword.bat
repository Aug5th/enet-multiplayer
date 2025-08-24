@echo off
setlocal

echo [INFO] Generating Visual Studio projects...

REM Create output directories
mkdir prj/prj3_finding-keyword/enet_client
mkdir prj/prj3_finding-keyword/enet_server

REM Build enet_client
echo [INFO] Generating enet_client.sln...
cmake -S src/finding-keyword/enet_client -B prj/prj3_finding-keyword/enet_client -G "Visual Studio 17 2022"

REM Copy server from src to prj
echo [INFO] Copying enet_server source files...
xcopy /E /I src/finding-keyword/enet_server prj/prj3_finding-keyword/enet_server

echo Projects generated successfully.
pause