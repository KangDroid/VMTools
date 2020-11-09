cmake_minimum_required(VERSION 3.15)
project(VM_TOOLS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

include_directories(${VM_TOOLS_SOURCE_DIR}/include)

add_executable(VM_TOOLS src/main.cpp src/VMInstance.cpp src/Logger.cpp)