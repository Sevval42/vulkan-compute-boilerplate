cmake_minimum_required(VERSION 3.10)

project(vulkan_compute_boilerplate)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

set(SOURCE_FILES 
    src/main.cpp 
    src/vulkan_base/vulkan_device.cpp
)

include(FetchContent)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)

FetchContent_MakeAvailable(stb)

find_package(Vulkan REQUIRED)

if (UNIX)
add_custom_target(build_shaders ALL
    COMMAND "${PROJECT_SOURCE_DIR}/shaders/compile.sh"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/shaders"
    )
endif(UNIX)

add_executable(vulkan_compute_boilerplate ${SOURCE_FILES})

target_include_directories(vulkan_compute_boilerplate PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(vulkan_compute_boilerplate PUBLIC ${Vulkan_LIBRARIES})

target_include_directories(vulkan_compute_boilerplate PRIVATE ${stb_SOURCE_DIR})
