//
// Created by alex on 3/13/25.
//

// Light.h
#ifndef LIGHT_H
#define LIGHT_H

#include "SpectralData.h"
#include "SamplingHelpers.h"
#include <glm/glm.hpp>

class Light {
public:
    Spectrum intensity;

    Light(const Spectrum& intensity) : intensity(intensity) {}
    virtual ~Light() = default;

    // Sample a direction from a point to this light
    // Returns direction, distance, and pdf
    virtual void sample(const glm::vec3& point, glm::vec3& direction,
                        float& distance, float& pdf) const = 0;

    // Get the direct illumination contribution
    virtual Spectrum getIllumination(const glm::vec3& point,
                                    const glm::vec3& normal) const = 0;
};

// Point light implementation
class PointLight : public Light {
public:
    glm::vec3 position;

    PointLight(const glm::vec3& position, const Spectrum& intensity)
        : Light(intensity), position(position) {}

    void sample(const glm::vec3& point, glm::vec3& direction,
               float& distance, float& pdf) const override {
        direction = position - point;
        distance = glm::length(direction);
        direction = glm::normalize(direction);
        pdf = 1.0f; // Point lights have a single sample point
    }

    Spectrum getIllumination(const glm::vec3& point,
                           const glm::vec3& normal) const override {
        glm::vec3 lightDir = position - point;
        float distanceSquared = glm::dot(lightDir, lightDir);
        lightDir = glm::normalize(lightDir);

        float cosTheta = glm::max(0.0f, glm::dot(normal, lightDir));
        return intensity * (cosTheta / distanceSquared);
    }
};

// Area light implementation
class AreaLight : public Light {
public:
    glm::vec3 corner;
    glm::vec3 u, v; // The two edge vectors
    glm::vec3 normal;
    float area;

    AreaLight(const glm::vec3& corner, const glm::vec3& u, const glm::vec3& v,
             const Spectrum& intensity)
        : Light(intensity), corner(corner), u(u), v(v) {
        normal = glm::normalize(glm::cross(u, v));
        area = glm::length(glm::cross(u, v));
    }

    void sample(const glm::vec3& point, glm::vec3& direction,
               float& distance, float& pdf) const override {
        // Sample a random point on the area light
        float u_rand = random_float();
        float v_rand = random_float();

        glm::vec3 lightPoint = corner + u * u_rand + v * v_rand;
        direction = lightPoint - point;
        distance = glm::length(direction);
        direction = glm::normalize(direction);

        // PDF is 1/area
        pdf = 1.0f / (area * distance * distance);
    }

    Spectrum getIllumination(const glm::vec3& point,
                           const glm::vec3& surfaceNormal) const override {
        // This is approximate for area lights
        glm::vec3 centerPoint = corner + u * 0.5f + v * 0.5f;
        glm::vec3 lightDir = centerPoint - point;
        float distanceSquared = glm::dot(lightDir, lightDir);
        lightDir = glm::normalize(lightDir);

        float cosTheta = glm::max(0.0f, glm::dot(surfaceNormal, lightDir));
        float lightCosTheta = glm::max(0.0f, -glm::dot(normal, lightDir));

        return intensity * (cosTheta * lightCosTheta * area / distanceSquared);
    }
};

#endif // LIGHT_H
