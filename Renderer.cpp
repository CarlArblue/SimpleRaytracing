//
// Created by alex on 3/10/25.
//

// Renderer.cpp
#include "Constants.h"
#include "SpectralData.h"
#include "Renderer.h"
#include "Scene.h"
#include "SamplingHelpers.h"
#include <cmath>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <limits>

// In Renderer.cpp, modify the traceRaySpectral function to handle emissive surfaces:
Spectrum traceRaySpectral(const glm::vec3& rayOrigin,
                   const glm::vec3& rayDir,
                   int depth,
                   const Scene& scene) {
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

    // If we hit an emissive surface, return its emission
    if (closestHit.isEmissive) {
        return closestHit.emission;
    }

    // Start with ambient light
    Spectrum localColor = closestHit.color * Spectrum(0.1f);  // Ambient term

    // Add contribution from each light
    for (const auto& light : scene.lights) {
        glm::vec3 lightDir;
        float distance;
        float pdf;

        // Sample a direction to the light
        light->sample(closestHit.hitPoint, lightDir, distance, pdf);

        // Shadow check
        glm::vec3 shadowOrigin = closestHit.hitPoint + closestHit.normal * shadowBias;
        bool inShadow = false;
        for (const auto& entity : scene.entities) {
            HitRecord shadowRec;
            if (entity->intersect(shadowOrigin, lightDir, shadowRec) && shadowRec.t < distance) {
                inShadow = true;
                break;
            }
        }

        if (!inShadow) {
            float cosTheta = std::max(0.0f, glm::dot(closestHit.normal, lightDir));
            // Add distance attenuation
            float distanceSquared = distance * distance;
            localColor += closestHit.color * light->intensity * cosTheta / (pdf * distanceSquared);
        }
    }

    // Monte Carlo diffuse bounce
    if (depth < maxDepth) {
        glm::vec3 randomDir = random_in_hemisphere(closestHit.normal);
        glm::vec3 newOrigin = closestHit.hitPoint + closestHit.normal * shadowBias;
        Spectrum indirect = traceRaySpectral(newOrigin, randomDir, depth + 1, scene);
        localColor += indirect * closestHit.color * 0.5f;  // Weight the contribution
    }

    return localColor;
}

void Renderer::renderImage(uint32_t* pixels,
                           const Scene& scene,
                           const glm::vec3& camPos,
                           const glm::vec3& forward,
                           const glm::vec3& right,
                           const glm::vec3& up) {

    float aspectRatio = static_cast<float>(WIDTH) / HEIGHT;

        #pragma omp parallel for collapse(2) schedule(dynamic)
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
                    pixelSpectrum += traceRaySpectral(camPos, rayDir, 0, scene);
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