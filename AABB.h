//
// Created by alex on 3/13/25.
//

// AABB.h
#ifndef AABB_H
#define AABB_H

#include <glm/glm.hpp>
#include <algorithm>

class AABB {
public:
    glm::vec3 min, max;

    AABB() : min(INFINITY), max(-INFINITY) {}

    AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

    // Check if a ray intersects this AABB
    bool intersect(const glm::vec3& origin, const glm::vec3& dir, float& tMin, float& tMax) const {
        tMin = -INFINITY;
        tMax = INFINITY;

        // Check for division by zero - important!
        for (int i = 0; i < 3; i++) {
            if (std::abs(dir[i]) < 1e-8) {
                // Ray is parallel to this slab, check if origin is within bounds
                if (origin[i] < min[i] || origin[i] > max[i])
                    return false;
            } else {
                float invD = 1.0f / dir[i];
                float t0 = (min[i] - origin[i]) * invD;
                float t1 = (max[i] - origin[i]) * invD;

                // Swap if necessary
                if (invD < 0.0f) std::swap(t0, t1);

                // Update interval
                tMin = std::max(t0, tMin);
                tMax = std::min(t1, tMax);

                // Early termination
                if (tMax < tMin) return false;
            }
        }

        return true;
    }

    // Combine two AABBs
    static AABB surrounding(const AABB& box1, const AABB& box2) {
        glm::vec3 small(
            std::min(box1.min.x, box2.min.x),
            std::min(box1.min.y, box2.min.y),
            std::min(box1.min.z, box2.min.z)
        );

        glm::vec3 big(
            std::max(box1.max.x, box2.max.x),
            std::max(box1.max.y, box2.max.y),
            std::max(box1.max.z, box2.max.z)
        );

        return AABB(small, big);
    }
};

#endif // AABB_H
