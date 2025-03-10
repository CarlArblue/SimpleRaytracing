//
// Created by alex on 3/10/25.
//

#ifndef RENDERER_H
#define RENDERER_H

#include <cstdint>
#include <glm/glm.hpp>

#include "Scene.h"

// Renderer class encapsulating the raytracing function.
class Renderer {
public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    // Renders the scene to the pixel buffer.
    // camPos, forward, right, and up define the camera coordinate system.
    static void renderImage(uint32_t* pixels,
                            const Scene& scene,
                            const glm::vec3& camPos,
                            const glm::vec3& forward,
                            const glm::vec3& right,
                            const glm::vec3& up);
};

#endif // RENDERER_H


