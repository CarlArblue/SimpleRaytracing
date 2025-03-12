//
// Created by alex on 3/10/25.
//

// Renderer.cpp
#include "SpectralData.h"
#include "Renderer.h"
#include "Scene.h"
#include "SamplingHelpers.h"
#include <cmath>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <limits>

// Global constants.
static constexpr int maxDepth = 2;              // Reflection recursion depth.
static constexpr int samplesPerPixel = 16;      // Increase for less noise.
static constexpr float shadowBias = 1e-4f;      // To avoid self-intersection.
static const Spectrum backgroundSpectrum = Spectrum::fromRGB(glm::vec3(0.0f, 0.0f, 0.0f)); // Change the background color to a spectrum

static constexpr float fov = M_PI / 3.0f; // 60° field of view.
static const float scale = std::tan(fov / 2.0f);

// raceRaySpectral function to use Spectral rendering
Spectrum traceRaySpectral(const glm::vec3& rayOrigin,
                   const glm::vec3& rayDir,
                   int depth,
                   const Scene& scene,
                   const glm::vec3& lightDir) {
    HitRecord closestHit;
    closestHit.t = std::numeric_limits<float>::infinity();
    bool hitSomething = false;

    // Test each entity in the scene.
    for (const auto& entity : scene.entities) {
        HitRecord rec;
        if (entity->intersect(rayOrigin, rayDir, rec)) {
            if (rec.t < closestHit.t) {
                closestHit = rec;
                hitSomething = true;
            }
        }
    }

    if (!hitSomething)
        return backgroundSpectrum;

    // Direct illumination using a fixed light direction.
    float diffuse = std::max(0.0f, glm::dot(closestHit.normal, lightDir));

    // Shadow check.
    glm::vec3 shadowOrigin = closestHit.hitPoint + closestHit.normal * shadowBias;
    bool inShadow = false;
    for (const auto& entity : scene.entities) {
        HitRecord shadowRec;
        if (entity->intersect(shadowOrigin, lightDir, shadowRec)) {
            inShadow = true;
            break;  // This break is important for efficiency
        }
    }
    if (inShadow)
        diffuse *= 0.3f;  // Darken if in shadow. Should be dependent on the surface of the entity.

    Spectrum localColor = closestHit.color * diffuse;

    // Monte Carlo diffuse bounce: randomly sample a direction in the hemisphere.
    if (depth < maxDepth) {
        glm::vec3 randomDir = random_in_hemisphere(closestHit.normal);
        glm::vec3 newOrigin = closestHit.hitPoint + closestHit.normal * shadowBias;
        Spectrum indirect = traceRaySpectral(newOrigin, randomDir, depth + 1, scene, lightDir);
        localColor += indirect * 0.5f;  // Weight the contribution.
    }
    return localColor;
}

// Update renderImage to use spectral rendering
void Renderer::renderImage(uint32_t* pixels,
                           const Scene& scene,
                           const glm::vec3& camPos,
                           const glm::vec3& forward,
                           const glm::vec3& right,
                           const glm::vec3& up) {

    float aspectRatio = static_cast<float>(WIDTH) / HEIGHT;
    glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -1.0f, -1.0f)); // Does not have a color??

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            Spectrum pixelSpectrum;
            // Average multiple samples.
            for (int s = 0; s < samplesPerPixel; s++) {
                // Jitter the ray within the pixel.
                float offsetX = random_float();
                float offsetY = random_float();
                float imageX = (2.0f * ((x + offsetX) / (float)WIDTH) - 1.0f) * aspectRatio * scale;
                float imageY = (1.0f - 2.0f * ((y + offsetY) / (float)HEIGHT)) * scale;
                glm::vec3 rayDir = glm::normalize(forward + right * imageX + up * imageY);
                pixelSpectrum += traceRaySpectral(camPos, rayDir, 0, scene, lightDir);
            }
            pixelSpectrum *= (1.0f / samplesPerPixel);

            // Convert spectrum to RGB for display
            glm::vec3 rgbColor = pixelSpectrum.toRGB();

            // Pack the color into a pixel (assuming ARGB format).
            pixels[y * WIDTH + x] = (255 << 24) |
                                    (static_cast<int>(glm::clamp(rgbColor.r, 0.0f, 1.0f) * 255) << 16) |
                                    (static_cast<int>(glm::clamp(rgbColor.g, 0.0f, 1.0f) * 255) << 8) |
                                    static_cast<int>(glm::clamp(rgbColor.b, 0.0f, 1.0f) * 255);
        }
    }
}