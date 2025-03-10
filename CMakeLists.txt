cmake_minimum_required(VERSION 3.10)
project(SimpleRaytracing)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find SDL2
find_package(SDL2 REQUIRED)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Gather all source files
set(SOURCES
        main.cpp
        Triangle.cpp
        Sphere.cpp
        Renderer.cpp
        SamplingHelpers.cpp
)

# Create the executable
add_executable(SimpleRaytracing ${SOURCES})

# Include SDL2 headers
target_include_directories(SimpleRaytracing PRIVATE ${SDL2_INCLUDE_DIRS})

# Link against SDL2
target_link_libraries(SimpleRaytracing PRIVATE ${SDL2_LIBRARIES})