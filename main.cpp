#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Sphere.h"
#include "Triangle.h"
#include "Scene.h"
#include "Renderer.h"
#include "SpectralData.h"

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
        Spectrum::fromRGB(glm::vec3(0.8f, 0.8f, 0.8f))
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(halfSize, -halfSize, -halfSize - roomSize),   // back right
        glm::vec3(halfSize, -halfSize, -halfSize),              // front right
        glm::vec3(-halfSize, -halfSize, -halfSize),             // front left
        Spectrum::fromRGB(glm::vec3(0.8f, 0.8f, 0.8f))
    ));

    // Ceiling (white)
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(-halfSize, halfSize, -halfSize - roomSize),   // back left
        glm::vec3(-halfSize, halfSize, -halfSize),              // front left
        glm::vec3(halfSize, halfSize, -halfSize - roomSize),    // back right
        Spectrum::fromRGB(glm::vec3(0.8f, 0.8f, 0.8f))
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(halfSize, halfSize, -halfSize - roomSize),    // back right
        glm::vec3(-halfSize, halfSize, -halfSize),              // front left
        glm::vec3(halfSize, halfSize, -halfSize),               // front right
        Spectrum::fromRGB(glm::vec3(0.8f, 0.8f, 0.8f))
    ));

    // Back wall (white)
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(-halfSize, -halfSize, -halfSize - roomSize),  // bottom left
        glm::vec3(-halfSize, halfSize, -halfSize - roomSize),   // top left
        glm::vec3(halfSize, halfSize, -halfSize - roomSize),    // top right
        Spectrum::fromRGB(glm::vec3(0.8f, 0.8f, 0.8f))
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(-halfSize, -halfSize, -halfSize - roomSize),  // bottom left
        glm::vec3(halfSize, halfSize, -halfSize - roomSize),    // top right
        glm::vec3(halfSize, -halfSize, -halfSize - roomSize),   // bottom right
        Spectrum::fromRGB(glm::vec3(0.8f, 0.8f, 0.8f))
    ));

    // Left wall (red)
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(-halfSize, -halfSize, -halfSize),             // front bottom
        glm::vec3(-halfSize, halfSize, -halfSize),              // front top
        glm::vec3(-halfSize, halfSize, -halfSize - roomSize),   // back top
        Spectrum::fromRGB(glm::vec3(1.0f, 0.0f, 0.0f))
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(-halfSize, -halfSize, -halfSize),             // front bottom
        glm::vec3(-halfSize, halfSize, -halfSize - roomSize),   // back top
        glm::vec3(-halfSize, -halfSize, -halfSize - roomSize),  // back bottom
        Spectrum::fromRGB(glm::vec3(1.0f, 0.0f, 0.0f))
    ));

    // Right wall (green)
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(halfSize, -halfSize, -halfSize),              // front bottom
        glm::vec3(halfSize, -halfSize, -halfSize - roomSize),   // back bottom
        glm::vec3(halfSize, halfSize, -halfSize - roomSize),    // back top
        Spectrum::fromRGB(glm::vec3(0.0f, 1.0f, 0.0f))
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(halfSize, -halfSize, -halfSize),              // front bottom
        glm::vec3(halfSize, halfSize, -halfSize - roomSize),    // back top
        glm::vec3(halfSize, halfSize, -halfSize),               // front top
        Spectrum::fromRGB(glm::vec3(0.0f, 1.0f, 0.0f))
    ));

    // Light source (bright white/yellow) - now using the emissive triangle method
    float lightSize = 3.0f;
    float lightY = halfSize - 0.1f;  // Slightly below ceiling

    scene.addEmissiveTriangle(
        glm::vec3(-lightSize/2, lightY, -halfSize - roomSize/2 - lightSize/2),
        glm::vec3(-lightSize/2, lightY, -halfSize - roomSize/2 + lightSize/2),
        glm::vec3(lightSize/2, lightY, -halfSize - roomSize/2 - lightSize/2),
        Spectrum::fromRGB(glm::vec3(0.8f, 0.8f, 0.8f)),  // Surface color
        Spectrum::fromRGB(glm::vec3(0.1f, 0.1f, 0.1f))  // Emission
    );

    scene.addEmissiveTriangle(
        glm::vec3(lightSize/2, lightY, -halfSize - roomSize/2 - lightSize/2),
        glm::vec3(-lightSize/2, lightY, -halfSize - roomSize/2 + lightSize/2),
        glm::vec3(lightSize/2, lightY, -halfSize - roomSize/2 + lightSize/2),
        Spectrum::fromRGB(glm::vec3(0.8f, 0.8f, 0.8f)),  // Surface color
        Spectrum::fromRGB(glm::vec3(0.1f, 0.1f, 0.1f))  // Emission
    );

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
        Spectrum::fromRGB(tallBoxColor)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMax.x, tallBoxMin.y, tallBoxMin.z),
        glm::vec3(tallBoxMax.x, tallBoxMin.y, tallBoxMax.z),
        glm::vec3(tallBoxMin.x, tallBoxMin.y, tallBoxMax.z),
        Spectrum::fromRGB(tallBoxColor)
    ));

    // Tall box - Top face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMin.x, tallBoxMax.y, tallBoxMin.z),
        glm::vec3(tallBoxMin.x, tallBoxMax.y, tallBoxMax.z),
        glm::vec3(tallBoxMax.x, tallBoxMax.y, tallBoxMin.z),
        Spectrum::fromRGB(tallBoxColor)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMax.x, tallBoxMax.y, tallBoxMin.z),
        glm::vec3(tallBoxMin.x, tallBoxMax.y, tallBoxMax.z),
        glm::vec3(tallBoxMax.x, tallBoxMax.y, tallBoxMax.z),
        Spectrum::fromRGB(tallBoxColor)
    ));

    // Tall box - Front face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMin.x, tallBoxMin.y, tallBoxMax.z),
        glm::vec3(tallBoxMax.x, tallBoxMin.y, tallBoxMax.z),
        glm::vec3(tallBoxMin.x, tallBoxMax.y, tallBoxMax.z),
        Spectrum::fromRGB(tallBoxColor)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMax.x, tallBoxMin.y, tallBoxMax.z),
        glm::vec3(tallBoxMax.x, tallBoxMax.y, tallBoxMax.z),
        glm::vec3(tallBoxMin.x, tallBoxMax.y, tallBoxMax.z),
        Spectrum::fromRGB(tallBoxColor)
    ));

    // Tall box - Back face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMin.x, tallBoxMin.y, tallBoxMin.z),
        glm::vec3(tallBoxMin.x, tallBoxMax.y, tallBoxMin.z),
        glm::vec3(tallBoxMax.x, tallBoxMin.y, tallBoxMin.z),
        Spectrum::fromRGB(tallBoxColor)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMax.x, tallBoxMin.y, tallBoxMin.z),
        glm::vec3(tallBoxMin.x, tallBoxMax.y, tallBoxMin.z),
        glm::vec3(tallBoxMax.x, tallBoxMax.y, tallBoxMin.z),
        Spectrum::fromRGB(tallBoxColor)
    ));

    // Tall box - Left face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMin.x, tallBoxMin.y, tallBoxMin.z),
        glm::vec3(tallBoxMin.x, tallBoxMin.y, tallBoxMax.z),
        glm::vec3(tallBoxMin.x, tallBoxMax.y, tallBoxMin.z),
        Spectrum::fromRGB(tallBoxColor)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMin.x, tallBoxMin.y, tallBoxMax.z),
        glm::vec3(tallBoxMin.x, tallBoxMax.y, tallBoxMax.z),
        glm::vec3(tallBoxMin.x, tallBoxMax.y, tallBoxMin.z),
        Spectrum::fromRGB(tallBoxColor)
    ));

    // Tall box - Right face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMax.x, tallBoxMin.y, tallBoxMin.z),
        glm::vec3(tallBoxMax.x, tallBoxMax.y, tallBoxMin.z),
        glm::vec3(tallBoxMax.x, tallBoxMin.y, tallBoxMax.z),
        Spectrum::fromRGB(tallBoxColor)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(tallBoxMax.x, tallBoxMin.y, tallBoxMax.z),
        glm::vec3(tallBoxMax.x, tallBoxMax.y, tallBoxMin.z),
        glm::vec3(tallBoxMax.x, tallBoxMax.y, tallBoxMax.z),
        Spectrum::fromRGB(tallBoxColor)
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
        Spectrum::fromRGB(shortBoxColor)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMax.x, shortBoxMin.y, shortBoxMin.z),
        glm::vec3(shortBoxMax.x, shortBoxMin.y, shortBoxMax.z),
        glm::vec3(shortBoxMin.x, shortBoxMin.y, shortBoxMax.z),
        Spectrum::fromRGB(shortBoxColor)
    ));

    // Short box - Top face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMin.x, shortBoxMax.y, shortBoxMin.z),
        glm::vec3(shortBoxMin.x, shortBoxMax.y, shortBoxMax.z),
        glm::vec3(shortBoxMax.x, shortBoxMax.y, shortBoxMin.z),
        Spectrum::fromRGB(shortBoxColor)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMax.x, shortBoxMax.y, shortBoxMin.z),
        glm::vec3(shortBoxMin.x, shortBoxMax.y, shortBoxMax.z),
        glm::vec3(shortBoxMax.x, shortBoxMax.y, shortBoxMax.z),
        Spectrum::fromRGB(shortBoxColor)
    ));

    // Short box - Front face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMin.x, shortBoxMin.y, shortBoxMax.z),
        glm::vec3(shortBoxMax.x, shortBoxMin.y, shortBoxMax.z),
        glm::vec3(shortBoxMin.x, shortBoxMax.y, shortBoxMax.z),
        Spectrum::fromRGB(shortBoxColor)
    ));
    // Short box - Front face (continued)
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMax.x, shortBoxMin.y, shortBoxMax.z),
        glm::vec3(shortBoxMax.x, shortBoxMax.y, shortBoxMax.z),
        glm::vec3(shortBoxMin.x, shortBoxMax.y, shortBoxMax.z),
        Spectrum::fromRGB(shortBoxColor)
    ));

    // Short box - Back face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMin.x, shortBoxMin.y, shortBoxMin.z),
        glm::vec3(shortBoxMin.x, shortBoxMax.y, shortBoxMin.z),
        glm::vec3(shortBoxMax.x, shortBoxMin.y, shortBoxMin.z),
        Spectrum::fromRGB(shortBoxColor)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMax.x, shortBoxMin.y, shortBoxMin.z),
        glm::vec3(shortBoxMin.x, shortBoxMax.y, shortBoxMin.z),
        glm::vec3(shortBoxMax.x, shortBoxMax.y, shortBoxMin.z),
        Spectrum::fromRGB(shortBoxColor)
    ));

    // Short box - Left face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMin.x, shortBoxMin.y, shortBoxMin.z),
        glm::vec3(shortBoxMin.x, shortBoxMin.y, shortBoxMax.z),
        glm::vec3(shortBoxMin.x, shortBoxMax.y, shortBoxMin.z),
        Spectrum::fromRGB(shortBoxColor)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMin.x, shortBoxMin.y, shortBoxMax.z),
        glm::vec3(shortBoxMin.x, shortBoxMax.y, shortBoxMax.z),
        glm::vec3(shortBoxMin.x, shortBoxMax.y, shortBoxMin.z),
        Spectrum::fromRGB(shortBoxColor)
    ));

    // Short box - Right face
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMax.x, shortBoxMin.y, shortBoxMin.z),
        glm::vec3(shortBoxMax.x, shortBoxMax.y, shortBoxMin.z),
        glm::vec3(shortBoxMax.x, shortBoxMin.y, shortBoxMax.z),
        Spectrum::fromRGB(shortBoxColor)
    ));
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(shortBoxMax.x, shortBoxMin.y, shortBoxMax.z),
        glm::vec3(shortBoxMax.x, shortBoxMax.y, shortBoxMin.z),
        glm::vec3(shortBoxMax.x, shortBoxMax.y, shortBoxMax.z),
        Spectrum::fromRGB(shortBoxColor)
    ));

    return scene;
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Init failed: " << SDL_GetError() << "\n";
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Monte Carlo Path tracing Spectral Rendering",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          Renderer::WIDTH, Renderer::HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    std::vector<uint32_t> pixels(Renderer::WIDTH * Renderer::HEIGHT);

    // In main(), replace the scene creation with:
    Scene scene = createCornellBox();

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

        Renderer::renderImage(pixels.data(), scene, camPos, forward, right, up);

        SDL_LockSurface(surface);
        memcpy(surface->pixels, pixels.data(), Renderer::WIDTH * Renderer::HEIGHT * sizeof(uint32_t));
        SDL_UnlockSurface(surface);
        SDL_UpdateWindowSurface(window);

        SDL_Delay(16); // ~60 FPS.
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

