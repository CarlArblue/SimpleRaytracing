cmake_minimum_required(VERSION 3.10)
project(SimpleRaytracing)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable Release optimizations
set(CMAKE_BUILD_TYPE Release)

# Optimization flags for Release mode
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -ffast-math -fno-trapping-math -DNDEBUG")


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

# Include Vulkan headers
target_include_directories(SimpleRaytracing PRIVATE ${Vulkan_INCLUDE_DIRS})

# Link against Vulkan
target_link_libraries(SimpleRaytracing PRIVATE Vulkan::Vulkan)
