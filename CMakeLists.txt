project(VM_TOOLS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

add_executable(VM_TOOLS main.cpp VMInstance.cpp Logger.cpp)