#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Hitbox {
    // Axis-aligned bounding box in *local* space
    glm::vec3 localMin;
    glm::vec3 localMax;

    // Axis-aligned bounding box in *world* space (computed after transform)
    glm::vec3 worldMin;
    glm::vec3 worldMax;

    // Helper function to update worldMin / worldMax given a modelMatrix
    void Update(const glm::mat4& modelMatrix);

    // Debugging: returns the 8 corners of the box in world space
    std::vector<glm::vec3> GetCorners() const;
};

// Checks if two world-space axis-aligned bounding boxes overlap
bool CheckCollision(const Hitbox& boxA, const Hitbox& boxB);
