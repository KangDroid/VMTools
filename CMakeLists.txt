cmake_minimum_required(VERSION 3.15)
project(VM_TOOLS)

if (APPLE)
    set(CMAKE_INSTALL_PREFIX "/usr/local/kangdroid")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "-DDEFAULT_SAMPLE_DIR=\"\\\"${CMAKE_INSTALL_PREFIX}\\\"\" -Wall -O3 -Wno-deprecated-declarations -Wno-unneeded-internal-declaration")

include_directories(${VM_TOOLS_SOURCE_DIR}/include)

add_executable(VM_TOOLS src/main.cpp src/VMInstance.cpp src/Logger.cpp src/jsoncpp.cpp)

install(TARGETS VM_TOOLS
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES assets/sample.json assets/remove.sh
        DESTINATION ${CMAKE_INSTALL_PREFIX}/assets 
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_WRITE GROUP_READ WORLD_EXECUTE WORLD_WRITE WORLD_READ)
execute_process (
    COMMAND bash -c "echo -e \"#!/bin/bash\nrm -rf ${CMAKE_INSTALL_PREFIX}/bin/VM_TOOLS\nrm -rf ${CMAKE_INSTALL_PREFIX}/assets/sample.json\nrm -rf ${CMAKE_INSTALL_PREFIX}/assets/remove.sh \" > ${PROJECT_SOURCE_DIR}/assets/remove.sh"
)