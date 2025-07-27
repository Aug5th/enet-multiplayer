cmake_minimum_required(VERSION 3.16)
project(enet_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Get path to enet folder (one level up from "src")
get_filename_component(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
set(ENET_DIR "${ROOT_DIR}/enet-1.3.18")

add_subdirectory(${ENET_DIR} ${CMAKE_BINARY_DIR}/enet_build)

add_executable(enet_server server.cpp)

target_include_directories(enet_server PRIVATE ${ENET_DIR}/include)
target_link_libraries(enet_server PRIVATE enet ws2_32 winmm)