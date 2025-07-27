@echo off
setlocal

echo [INFO] Generating Visual Studio projects...

REM Create output directories
mkdir prj\enet_client
mkdir prj\enet_server

REM Build enet_client
echo [INFO] Generating enet_client.sln...
cmake -S src/enet_client -B prj/enet_client -G "Visual Studio 17 2022"

REM Build enet_server
echo [INFO] Generating enet_server.sln...
cmake -S src/enet_server -B prj/enet_server -G "Visual Studio 17 2022"

echo Projects generated successfully.
pause