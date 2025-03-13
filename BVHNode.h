//
// Created by alex on 3/13/25.
//

// BVHNode.h
#ifndef BVHNODE_H
#define BVHNODE_H

#include "Entity.h"
#include "AABB.h"
#include <vector>
#include <memory>
#include <algorithm>

class BVHNode : public Entity {
public:
    AABB box;
    std::shared_ptr<Entity> left;
    std::shared_ptr<Entity> right;

    // Constructors
    BVHNode() {}

    BVHNode(const std::vector<std::shared_ptr<Entity>>& entities, size_t start, size_t end) {
        // Random axis - can be refined later
        int axis = rand() % 3;

        // Sort the entities based on their position along the chosen axis
        auto comparator = [axis](const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b) {
            AABB boxA = a->getBounds();
            AABB boxB = b->getBounds();

            // Compare centers of bounding boxes
            return (boxA.min[axis] + boxA.max[axis]) < (boxB.min[axis] + boxB.max[axis]);
        };

        size_t objectSpan = end - start;

        if (objectSpan == 1) {
            // Only one entity, make it both children (a leaf node)
            left = right = entities[start];
        } else if (objectSpan == 2) {
            // Two entities, sort them
            if (comparator(entities[start], entities[start+1])) {
                left = entities[start];
                right = entities[start+1];
            } else {
                left = entities[start+1];
                right = entities[start];
            }
        } else {
            // More than two objects, sort and split
            std::vector<std::shared_ptr<Entity>> sortedEntities(entities.begin() + start, entities.begin() + end);
            std::sort(sortedEntities.begin(), sortedEntities.end(), comparator);

            size_t mid = objectSpan / 2;
            left = std::make_shared<BVHNode>(sortedEntities, 0, mid);
            right = std::make_shared<BVHNode>(sortedEntities, mid, objectSpan);
        }

        AABB boxLeft = left->getBounds();
        AABB boxRight = right->getBounds();
        box = AABB::surrounding(boxLeft, boxRight);
    }

    // Build from a vector of entities
    BVHNode(std::vector<std::shared_ptr<Entity>> entities) : BVHNode(entities, 0, entities.size()) {}

    // Implement Entity interface
    bool intersect(const glm::vec3& origin, const glm::vec3& dir, HitRecord& rec) const override {
        float tMin, tMax;
        if (!box.intersect(origin, dir, tMin, tMax))
            return false;

        bool hitLeft = left->intersect(origin, dir, rec);
        HitRecord rightRec;
        bool hitRight = right->intersect(origin, dir, rightRec);

        if (hitLeft && hitRight) {
            if (rightRec.t < rec.t) rec = rightRec;
            return true;
        }

        return hitLeft || hitRight;
    }

    AABB getBounds() const override {
        return box;
    }

    // These need implementation based on your Entity class
    Spectrum getEmission() const override { return Spectrum(0.0f); }
    bool isEmissive() const override { return false; }
    BSDF* getBSDF() const override { return nullptr; }
};

#endif // BVHNODE_H
