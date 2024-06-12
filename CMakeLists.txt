# Example usage:
#  mkdir build
#  cd build
#  cmake -g "Visual Studio 14 2015" ..

cmake_minimum_required(VERSION 3.28)
project(KBUI C CXX)


if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_PROTOTYPES")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")

# GLFW
set(GLFW_DIR ../glfw) # Set this to point to an up-to-date GLFW repo
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
add_subdirectory(${GLFW_DIR} binary_dir EXCLUDE_FROM_ALL)
include_directories(${GLFW_DIR}/include)

# Libraries
find_package(Vulkan REQUIRED)
set(LIBRARIES "glfw;Vulkan::Vulkan")

# Use vulkan headers from glfw:
include_directories(${GLFW_DIR}/deps)

set(SOURCE_LIST
        src/private/source/VulkanWindow.cpp
)

set(PRIVATE_HEADERS
)

set(PUBLIC_HEADERS
        src/public/headers/VulkanWindow.h
        src/public/headers/Properties/WindowProperties.h
        src/public/headers/ImVecMath.h
        src/public/headers/WindowWithoutBorders.cpp
        src/public/headers/WindowWithoutBorders.h
        src/public/headers/MainWindow.cpp
        src/public/headers/MainWindow.h
)

# Dear ImGui
set(IMGUI_DIR vendor/imgui)
add_library(
        ${PROJECT_NAME}
        STATIC
        ${sources}
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${SOURCE_LIST}
        ${PRIVATE_HEADERS}
        ${PUBLIC_HEADERS}
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        src/public/headers)

target_link_libraries(${PROJECT_NAME}
        ${LIBRARIES})

target_compile_definitions(${PROJECT_NAME}
        PUBLIC
        -DImTextureID=ImU64)

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        src/include)


set(EXE_NAME ${PROJECT_NAME}-EXE)
add_executable(${EXE_NAME}
    src/main.cpp
)

target_link_libraries(
        ${EXE_NAME}
        ${PROJECT_NAME}
)
