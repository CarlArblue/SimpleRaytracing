#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Sphere.h"
#include "Triangle.h"
#include "Scene.h"
#include "Renderer.h"
#include "SpectralData.h"

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Init failed: " << SDL_GetError() << "\n";
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Animated Raytracer",
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

    // Create a scene with spectral entities
    Scene scene;
    // Red sphere
    scene.addEntity(std::make_shared<Sphere>(
        glm::vec3(0.0f, 0.0f, -5.0f),
        1.0f,
        Spectrum::fromRGB(glm::vec3(1.0f, 0.0f, 0.0f))  // Use fromRGB
    ));

    // Green triangle
    scene.addEntity(std::make_shared<Triangle>(
        glm::vec3(-2.0f, -1.0f, -4.0f),  // v0
        glm::vec3( 2.0f, -1.0f, -4.0f),  // v1
        glm::vec3( 0.0f, -1.0f, -8.0f),  // v2
        Spectrum::fromRGB(glm::vec3(0.0f, 1.0f, 0.0f))  // Use fromRGB
    ));
    // You can add more entities dynamically here.

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT)
                running = false;

        float time = SDL_GetTicks64() / 1000.0f;
        float orbitRadius = 4.0f;
        glm::vec3 camPos(orbitRadius * std::cos(time),
                         2.0f,
                         orbitRadius * std::sin(time) - 5.0f);
        glm::vec3 target(0.0f, 0.0f, -5.0f); // For simplicity, look at the sphere.
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
