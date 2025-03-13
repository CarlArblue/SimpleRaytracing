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

Spectrum traceRaySpectral(const glm::vec3& rayOrigin,
                           const glm::vec3& rayDir,
                           int depth,
                           const Scene& scene) {
    HitRecord closestHit;
    closestHit.t = std::numeric_limits<float>::infinity();
    bool hitSomething = false;

    if (scene.bvh) {
        hitSomething = scene.bvh->intersect(rayOrigin, rayDir, closestHit);
    } else {
        // Fallback to the existing loop if BVH not built
        for (const auto& entity : scene.entities) {
            HitRecord rec;
            if (entity->intersect(rayOrigin, rayDir, rec)) {
                if (rec.t < closestHit.t) {
                    closestHit = rec;
                    closestHit.hitEntity = entity;
                    hitSomething = true;
                }
            }
        }
    }

    if (!hitSomething)
        return backgroundSpectrum;

    // Direct hit on an emissive surface.
    if (closestHit.isEmissive)
        return closestHit.emission;

    // Start with ambient light.
    Spectrum localColor = closestHit.color * Spectrum(0.1f);  // Ambient term

    // Process emissive entities (lights) as before...
    for (const auto& entity : scene.entities) {
        if (!entity->isEmissive())
            continue;

        glm::vec3 samplePoint, lightNormal;
        float pdf;
        entity->sampleLight(closestHit.hitPoint, samplePoint, lightNormal, pdf);

        glm::vec3 lightDir = samplePoint - closestHit.hitPoint;
        float distance = glm::length(lightDir);
        lightDir = glm::normalize(lightDir);

        glm::vec3 shadowOrigin = closestHit.hitPoint + closestHit.normal * shadowBias;
        bool inShadow = false;
        for (const auto& other : scene.entities) {
            HitRecord shadowRec;
            if (other->intersect(shadowOrigin, lightDir, shadowRec) && shadowRec.t < distance) {
                inShadow = true;
                break;
            }
        }

        if (!inShadow) {
            float cosTheta = std::max(0.0f, glm::dot(closestHit.normal, lightDir));
            float distanceSquared = distance * distance;
            Spectrum lightEmission = entity->getEmission();
            localColor += closestHit.color * lightEmission * cosTheta / (pdf * distanceSquared);
        }
    }

    // Use BSDF for the indirect bounce.
    if (depth < maxDepth) {
        BSDF* bsdf = nullptr;
        if (closestHit.hitEntity) {
            bsdf = closestHit.hitEntity->getBSDF();
        }

        if (bsdf) {
            float bsdfPdf;
            glm::vec3 newDir = bsdf->sample(-rayDir, closestHit.normal, bsdfPdf);
            glm::vec3 newOrigin = closestHit.hitPoint + closestHit.normal * shadowBias;

            if (bsdfPdf > 0.0f) {
                Spectrum indirect = traceRaySpectral(newOrigin, newDir, depth + 1, scene);
                Spectrum bsdfVal = bsdf->evaluate(-rayDir, newDir, closestHit.normal);

                // Apply proper weighting with the PDF
                float cosTheta = std::max(0.0f, glm::dot(closestHit.normal, newDir));
                localColor += indirect * bsdfVal * cosTheta / bsdfPdf;
            }
        } else {
            // Fallback: cosine-weighted hemisphere sampling.
            glm::vec3 randomDir = random_in_hemisphere(closestHit.normal);
            glm::vec3 newOrigin = closestHit.hitPoint + closestHit.normal * shadowBias;
            Spectrum indirect = traceRaySpectral(newOrigin, randomDir, depth + 1, scene);
            localColor += indirect * closestHit.color * 0.5f;
        }
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

    // Declare the variables before the parallel region
    int x, y;                // Loop indices
    Spectrum pixelSpectrum;  // To store the color value per pixel
    float offsetX, offsetY;  // Jitter offsets for each sample
    float imageX, imageY;    // Image coordinates for ray casting
    glm::vec3 rayDir;        // Ray direction
    glm::vec3 rgbColor;       // Convert from spectrum to RGB

    #pragma omp parallel for collapse(2) schedule(dynamic) default(none) \
    shared(scale, aspectRatio, pixels, scene, camPos, forward, right, up, samplesPerPixel) \
    private(x, y, pixelSpectrum, offsetX, offsetY, imageX, imageY, rayDir, rgbColor)
        for (y = 0; y < HEIGHT; y++) {
            for (x = 0; x < WIDTH; x++) {
                // Average multiple samples.
                for (int s = 0; s < samplesPerPixel; s++) {
                    // Jitter the ray within the pixel.
                    offsetX = random_float();
                    offsetY = random_float();
                    imageX = (2.0f * ((x + offsetX) / (float)WIDTH) - 1.0f) * aspectRatio * scale;
                    imageY = (1.0f - 2.0f * ((y + offsetY) / (float)HEIGHT)) * scale;
                    rayDir = glm::normalize(forward + right * imageX + up * imageY);
                    pixelSpectrum += traceRaySpectral(camPos, rayDir, 0, scene);
                }
                pixelSpectrum *= (1.0f / samplesPerPixel);

                // Convert spectrum to RGB for display
                rgbColor = pixelSpectrum.toRGB();

                // Pack the color into a pixel (assuming ARGB format).
                pixels[y * WIDTH + x] = (255 << 24) |
                                        (static_cast<int>(glm::clamp(rgbColor.r, 0.0f, 1.0f) * 255) << 16) |
                                        (static_cast<int>(glm::clamp(rgbColor.g, 0.0f, 1.0f) * 255) << 8) |
                                        static_cast<int>(glm::clamp(rgbColor.b, 0.0f, 1.0f) * 255);
            }
        }
}