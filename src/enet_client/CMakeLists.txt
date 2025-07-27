cmake_minimum_required(VERSION 3.16)
project(enet_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

get_filename_component(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
set(ENET_DIR "${ROOT_DIR}/enet-1.3.18")

add_subdirectory(${ENET_DIR} ${CMAKE_BINARY_DIR}/enet_build)

add_executable(enet_client client.cpp)

target_include_directories(enet_client PRIVATE ${ENET_DIR}/include)
target_link_libraries(enet_client PRIVATE enet ws2_32 winmm)