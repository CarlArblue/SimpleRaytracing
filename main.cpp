#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Entity.h"
#include "Scene.h"
#include "Renderer.h"
#include "SpectralData.h"
#include "LambertianBSDF.h"
#include "VulkanContext.h"

// Create a Cornell Box scene
Scene createCornellBox() {
    Scene scene;

    // Room dimensions
    float roomSize = 10.0f;
    float halfSize = roomSize / 2.0f;

    // Floor (white)
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(-halfSize, -halfSize, -halfSize - roomSize),  // back left
        glm::vec3(halfSize, -halfSize, -halfSize - roomSize),   // back right
        glm::vec3(-halfSize, -halfSize, -halfSize),             // front left
        Spectrum::fromRGB(glm::vec3(0.8f, 0.8f, 0.8f)),
        Spectrum(0.0f)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(halfSize, -halfSize, -halfSize - roomSize),   // back right
        glm::vec3(halfSize, -halfSize, -halfSize),              // front right
        glm::vec3(-halfSize, -halfSize, -halfSize),             // front left
        Spectrum::fromRGB(glm::vec3(0.8f, 0.8f, 0.8f)),
        Spectrum(0.0f)
    ));

    // Ceiling (white)
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(-halfSize, halfSize, -halfSize - roomSize),   // back left
        glm::vec3(-halfSize, halfSize, -halfSize),              // front left
        glm::vec3(halfSize, halfSize, -halfSize - roomSize),    // back right
        Spectrum::fromRGB(glm::vec3(0.8f, 0.8f, 0.8f)),
        Spectrum(0.0f)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(halfSize, halfSize, -halfSize - roomSize),    // back right
        glm::vec3(-halfSize, halfSize, -halfSize),              // front left
        glm::vec3(halfSize, halfSize, -halfSize),               // front right
        Spectrum::fromRGB(glm::vec3(0.8f, 0.8f, 0.8f)),
        Spectrum(0.0f)
    ));

    // Back wall (white)
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(-halfSize, -halfSize, -halfSize - roomSize),  // bottom left
        glm::vec3(-halfSize, halfSize, -halfSize - roomSize),   // top left
        glm::vec3(halfSize, halfSize, -halfSize - roomSize),    // top right
        Spectrum::fromRGB(glm::vec3(0.8f, 0.8f, 0.8f)),
        Spectrum(0.0f)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(-halfSize, -halfSize, -halfSize - roomSize),  // bottom left
        glm::vec3(halfSize, halfSize, -halfSize - roomSize),    // top right
        glm::vec3(halfSize, -halfSize, -halfSize - roomSize),   // bottom right
        Spectrum::fromRGB(glm::vec3(0.8f, 0.8f, 0.8f)),
        Spectrum(0.0f)
    ));

    // Left wall (red)
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(-halfSize, -halfSize, -halfSize),             // front bottom
        glm::vec3(-halfSize, halfSize, -halfSize),              // front top
        glm::vec3(-halfSize, halfSize, -halfSize - roomSize),   // back top
        Spectrum::fromRGB(glm::vec3(1.0f, 0.0f, 0.0f)),
        Spectrum(0.0f)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(-halfSize, -halfSize, -halfSize),             // front bottom
        glm::vec3(-halfSize, halfSize, -halfSize - roomSize),   // back top
        glm::vec3(-halfSize, -halfSize, -halfSize - roomSize),  // back bottom
        Spectrum::fromRGB(glm::vec3(1.0f, 0.0f, 0.0f)),
        Spectrum(0.0f)
    ));

    // Right wall (green)
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(halfSize, -halfSize, -halfSize),              // front bottom
        glm::vec3(halfSize, -halfSize, -halfSize - roomSize),   // back bottom
        glm::vec3(halfSize, halfSize, -halfSize - roomSize),    // back top
        Spectrum::fromRGB(glm::vec3(0.0f, 1.0f, 0.0f)),
        Spectrum(0.0f)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(halfSize, -halfSize, -halfSize),              // front bottom
        glm::vec3(halfSize, halfSize, -halfSize - roomSize),    // back top
        glm::vec3(halfSize, halfSize, -halfSize),               // front top
        Spectrum::fromRGB(glm::vec3(0.0f, 1.0f, 0.0f)),
        Spectrum(0.0f)
    ));

    // Light source (bright white/yellow) - now using the emissive triangle method
    float lightSize = 3.0f;
    float lightY = halfSize - 0.1f;  // Slightly below ceiling

    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(-lightSize/2, lightY, -halfSize - roomSize/2 - lightSize/2),
        glm::vec3(-lightSize/2, lightY, -halfSize - roomSize/2 + lightSize/2),
        glm::vec3(lightSize/2, lightY, -halfSize - roomSize/2 - lightSize/2),
        Spectrum::fromRGB(glm::vec3(0.8f, 0.8f, 0.8f)),
        Spectrum(20.0f)
    ));

    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(lightSize/2, lightY, -halfSize - roomSize/2 - lightSize/2),
        glm::vec3(-lightSize/2, lightY, -halfSize - roomSize/2 + lightSize/2),
        glm::vec3(lightSize/2, lightY, -halfSize - roomSize/2 + lightSize/2),
        Spectrum::fromRGB(glm::vec3(0.8f, 0.8f, 0.8f)),
        Spectrum(20.0f)
    ));

    // Left box (tall)
    float tallBoxSize = 3.0f;
    float tallBoxHeight = 6.0f;
    float tallBoxX = -halfSize/2 - tallBoxSize/2;
    float tallBoxZ = -halfSize - roomSize/2 - tallBoxSize/2;
    glm::vec3 tallBoxColor = glm::vec3(0.8f, 0.8f, 0.8f);

    // Tall box - coordinates for a box
    glm::vec3 tallBoxMin(tallBoxX, -halfSize, tallBoxZ);
    glm::vec3 tallBoxMax(tallBoxX + tallBoxSize, -halfSize + tallBoxHeight, tallBoxZ + tallBoxSize);

    // Tall box - Bottom face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMin.x, tallBoxMin.y, tallBoxMin.z),
        glm::vec3(tallBoxMax.x, tallBoxMin.y, tallBoxMin.z),
        glm::vec3(tallBoxMin.x, tallBoxMin.y, tallBoxMax.z),
        Spectrum::fromRGB(tallBoxColor),
        Spectrum(0.0f)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMax.x, tallBoxMin.y, tallBoxMin.z),
        glm::vec3(tallBoxMax.x, tallBoxMin.y, tallBoxMax.z),
        glm::vec3(tallBoxMin.x, tallBoxMin.y, tallBoxMax.z),
        Spectrum::fromRGB(tallBoxColor),
        Spectrum(0.0f)
    ));

    // Tall box - Top face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMin.x, tallBoxMax.y, tallBoxMin.z),
        glm::vec3(tallBoxMin.x, tallBoxMax.y, tallBoxMax.z),
        glm::vec3(tallBoxMax.x, tallBoxMax.y, tallBoxMin.z),
        Spectrum::fromRGB(tallBoxColor),
        Spectrum(0.0f)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMax.x, tallBoxMax.y, tallBoxMin.z),
        glm::vec3(tallBoxMin.x, tallBoxMax.y, tallBoxMax.z),
        glm::vec3(tallBoxMax.x, tallBoxMax.y, tallBoxMax.z),
        Spectrum::fromRGB(tallBoxColor),
        Spectrum(0.0f)
    ));

    // Tall box - Front face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMin.x, tallBoxMin.y, tallBoxMax.z),
        glm::vec3(tallBoxMax.x, tallBoxMin.y, tallBoxMax.z),
        glm::vec3(tallBoxMin.x, tallBoxMax.y, tallBoxMax.z),
        Spectrum::fromRGB(tallBoxColor),
        Spectrum(0.0f)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMax.x, tallBoxMin.y, tallBoxMax.z),
        glm::vec3(tallBoxMax.x, tallBoxMax.y, tallBoxMax.z),
        glm::vec3(tallBoxMin.x, tallBoxMax.y, tallBoxMax.z),
        Spectrum::fromRGB(tallBoxColor),
        Spectrum(0.0f)
    ));

    // Tall box - Back face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMin.x, tallBoxMin.y, tallBoxMin.z),
        glm::vec3(tallBoxMin.x, tallBoxMax.y, tallBoxMin.z),
        glm::vec3(tallBoxMax.x, tallBoxMin.y, tallBoxMin.z),
        Spectrum::fromRGB(tallBoxColor),
        Spectrum(0.0f)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMax.x, tallBoxMin.y, tallBoxMin.z),
        glm::vec3(tallBoxMin.x, tallBoxMax.y, tallBoxMin.z),
        glm::vec3(tallBoxMax.x, tallBoxMax.y, tallBoxMin.z),
        Spectrum::fromRGB(tallBoxColor),
        Spectrum(0.0f)
    ));

    // Tall box - Left face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMin.x, tallBoxMin.y, tallBoxMin.z),
        glm::vec3(tallBoxMin.x, tallBoxMin.y, tallBoxMax.z),
        glm::vec3(tallBoxMin.x, tallBoxMax.y, tallBoxMin.z),
        Spectrum::fromRGB(tallBoxColor),
        Spectrum(0.0f)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMin.x, tallBoxMin.y, tallBoxMax.z),
        glm::vec3(tallBoxMin.x, tallBoxMax.y, tallBoxMax.z),
        glm::vec3(tallBoxMin.x, tallBoxMax.y, tallBoxMin.z),
        Spectrum::fromRGB(tallBoxColor),
        Spectrum(0.0f)
    ));

    // Tall box - Right face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMax.x, tallBoxMin.y, tallBoxMin.z),
        glm::vec3(tallBoxMax.x, tallBoxMax.y, tallBoxMin.z),
        glm::vec3(tallBoxMax.x, tallBoxMin.y, tallBoxMax.z),
        Spectrum::fromRGB(tallBoxColor),
        Spectrum(0.0f)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMax.x, tallBoxMin.y, tallBoxMax.z),
        glm::vec3(tallBoxMax.x, tallBoxMax.y, tallBoxMin.z),
        glm::vec3(tallBoxMax.x, tallBoxMax.y, tallBoxMax.z),
        Spectrum::fromRGB(tallBoxColor),
        Spectrum(0.0f)
    ));

    // Short box (shorter cube)
    float shortBoxSize = 3.0f;
    float shortBoxHeight = 3.0f;
    float shortBoxX = halfSize/2 - shortBoxSize/2;
    float shortBoxZ = -halfSize - roomSize/2 + shortBoxSize/2;
    glm::vec3 shortBoxColor = glm::vec3(0.8f, 0.8f, 0.8f);

    // Short box - coordinates for a box
    glm::vec3 shortBoxMin(shortBoxX, -halfSize, shortBoxZ);
    glm::vec3 shortBoxMax(shortBoxX + shortBoxSize, -halfSize + shortBoxHeight, shortBoxZ + shortBoxSize);

    // Short box - Bottom face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMin.x, shortBoxMin.y, shortBoxMin.z),
        glm::vec3(shortBoxMax.x, shortBoxMin.y, shortBoxMin.z),
        glm::vec3(shortBoxMin.x, shortBoxMin.y, shortBoxMax.z),
        Spectrum::fromRGB(shortBoxColor),
        Spectrum(0.0f)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMax.x, shortBoxMin.y, shortBoxMin.z),
        glm::vec3(shortBoxMax.x, shortBoxMin.y, shortBoxMax.z),
        glm::vec3(shortBoxMin.x, shortBoxMin.y, shortBoxMax.z),
        Spectrum::fromRGB(shortBoxColor),
        Spectrum(0.0f)
    ));

    // Short box - Top face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMin.x, shortBoxMax.y, shortBoxMin.z),
        glm::vec3(shortBoxMin.x, shortBoxMax.y, shortBoxMax.z),
        glm::vec3(shortBoxMax.x, shortBoxMax.y, shortBoxMin.z),
        Spectrum::fromRGB(shortBoxColor),
        Spectrum(0.0f)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMax.x, shortBoxMax.y, shortBoxMin.z),
        glm::vec3(shortBoxMin.x, shortBoxMax.y, shortBoxMax.z),
        glm::vec3(shortBoxMax.x, shortBoxMax.y, shortBoxMax.z),
        Spectrum::fromRGB(shortBoxColor),
        Spectrum(0.0f)
    ));

    // Short box - Front face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMin.x, shortBoxMin.y, shortBoxMax.z),
        glm::vec3(shortBoxMax.x, shortBoxMin.y, shortBoxMax.z),
        glm::vec3(shortBoxMin.x, shortBoxMax.y, shortBoxMax.z),
        Spectrum::fromRGB(shortBoxColor),
        Spectrum(0.0f)
    ));
    // Short box - Front face (continued)
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMax.x, shortBoxMin.y, shortBoxMax.z),
        glm::vec3(shortBoxMax.x, shortBoxMax.y, shortBoxMax.z),
        glm::vec3(shortBoxMin.x, shortBoxMax.y, shortBoxMax.z),
        Spectrum::fromRGB(shortBoxColor),
        Spectrum(0.0f)
    ));

    // Short box - Back face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMin.x, shortBoxMin.y, shortBoxMin.z),
        glm::vec3(shortBoxMin.x, shortBoxMax.y, shortBoxMin.z),
        glm::vec3(shortBoxMax.x, shortBoxMin.y, shortBoxMin.z),
        Spectrum::fromRGB(shortBoxColor),
        Spectrum(0.0f)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMax.x, shortBoxMin.y, shortBoxMin.z),
        glm::vec3(shortBoxMin.x, shortBoxMax.y, shortBoxMin.z),
        glm::vec3(shortBoxMax.x, shortBoxMax.y, shortBoxMin.z),
        Spectrum::fromRGB(shortBoxColor),
        Spectrum(0.0f)
    ));

    // Short box - Left face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMin.x, shortBoxMin.y, shortBoxMin.z),
        glm::vec3(shortBoxMin.x, shortBoxMin.y, shortBoxMax.z),
        glm::vec3(shortBoxMin.x, shortBoxMax.y, shortBoxMin.z),
        Spectrum::fromRGB(shortBoxColor),
        Spectrum(0.0f)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMin.x, shortBoxMin.y, shortBoxMax.z),
        glm::vec3(shortBoxMin.x, shortBoxMax.y, shortBoxMax.z),
        glm::vec3(shortBoxMin.x, shortBoxMax.y, shortBoxMin.z),
        Spectrum::fromRGB(shortBoxColor),
        Spectrum(0.0f)
    ));

    // Short box - Right face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMax.x, shortBoxMin.y, shortBoxMin.z),
        glm::vec3(shortBoxMax.x, shortBoxMax.y, shortBoxMin.z),
        glm::vec3(shortBoxMax.x, shortBoxMin.y, shortBoxMax.z),
        Spectrum::fromRGB(shortBoxColor),
        Spectrum(0.0f)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMax.x, shortBoxMin.y, shortBoxMax.z),
        glm::vec3(shortBoxMax.x, shortBoxMax.y, shortBoxMin.z),
        glm::vec3(shortBoxMax.x, shortBoxMax.y, shortBoxMax.z),
        Spectrum::fromRGB(shortBoxColor),
        Spectrum(0.0f)
    ));

    // Create an instance of a Lambertian BSDF with a diffuse red color.
     BSDF* bsdf_lamb = new LambertianBSDF(Spectrum::fromRGB(glm::vec3(1.0f, 0.0f, 0.0f)));

     // Then add the sphere to the scene using the BSDF pointer.
     scene.addEntity(std::make_shared<Sphere>(
         glm::vec3(0.0f, 0.0f, -halfSize - roomSize / 2),
         1.0,
         Spectrum::fromRGB(glm::vec3(251.0f/256.0f, 198.0f/256.0f, 207.0f/256.0f)),
         Spectrum(0.0f),
         bsdf_lamb
     ));

    return scene;
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Init failed: " << SDL_GetError() << "\n";
        return -1;
    }

    // Create window with Vulkan flag
    SDL_Window* window = SDL_CreateWindow("Monte Carlo Path tracing Spectral Rendering",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          Renderer::WIDTH, Renderer::HEIGHT,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }

    // Initialize Vulkan
    VulkanContext vulkanContext;
    if (!vulkanContext.initialize(window)) {
        std::cerr << "Vulkan initialization failed\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Check if ray tracing is supported
    if (!vulkanContext.hasRayTracingSupport()) {
        std::cout << "Warning: Ray tracing extensions not supported, falling back to CPU rendering\n";
        // We'll continue with CPU rendering for now
    }

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    std::vector<uint32_t> pixels(Renderer::WIDTH * Renderer::HEIGHT);

    // Create the scene and build BVH
    Scene scene = createCornellBox();
    scene.buildBVH();

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT)
                running = false;

        // Fixed camera position to view the Cornell Box
        glm::vec3 camPos(0.0f, 0.0f, 5.0f);
        glm::vec3 target(0.0f, 0.0f, -15.0f);  // Look toward the center of the room
        glm::vec3 forward = glm::normalize(target - camPos);
        glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
        glm::vec3 up = glm::normalize(glm::cross(right, forward));

        // TODO: Replace with Vulkan rendering when ready
        Renderer::renderImage(pixels.data(), scene, camPos, forward, right, up);

        SDL_LockSurface(surface);
        memcpy(surface->pixels, pixels.data(), Renderer::WIDTH * Renderer::HEIGHT * sizeof(uint32_t));
        SDL_UnlockSurface(surface);
        SDL_UpdateWindowSurface(window);

        SDL_Delay(16); // ~60 FPS.
    }

    // Clean up Vulkan (vulkanContext destructor will handle this)

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

