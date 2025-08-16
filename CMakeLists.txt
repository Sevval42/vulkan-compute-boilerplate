cmake_minimum_required(VERSION 3.10)

project(vulkan_compute_boilerplate)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

set(SOURCE_FILES 
    src/main.cpp 
    src/vulkan_base/vulkan_device.cpp
    src/vulkan_base/vulkan_pipeline.cpp
    src/vulkan_base/vulkan_descriptor_set.cpp
    src/vulkan_base/vulkan_buffer.cpp
    src/vulkan_base/vulkan_image.cpp
    src/vulkan_base/vulkan_helper.cpp
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
elseif(WIN32)
    add_custom_target(build_shaders ALL
        COMMAND "${PROJECT_SOURCE_DIR}/shaders/compile.bat"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/shaders"
    )
endif()

add_executable(vulkan_compute_boilerplate ${SOURCE_FILES})

target_link_libraries(vulkan_compute_boilerplate PUBLIC Vulkan::Vulkan)

target_include_directories(vulkan_compute_boilerplate PRIVATE ${stb_SOURCE_DIR})
