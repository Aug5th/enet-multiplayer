@echo off
setlocal

echo [INFO] Generating Visual Studio projects...

REM Create output directories
mkdir prj/prj1_scissors-paper-rock/enet_client
mkdir prj/prj1_scissors-paper-rock/enet_server

REM Build enet_client
echo [INFO] Generating enet_client.sln...
cmake -S src/scissors-paper-rock/enet_client -B prj/prj1_scissors-paper-rock/enet_client -G "Visual Studio 17 2022"

REM Build enet_server
echo [INFO] Generating enet_server.sln...
cmake -S src/scissors-paper-rock/enet_server -B prj/prj1_scissors-paper-rock/enet_server -G "Visual Studio 17 2022"

echo Projects generated successfully.
pause