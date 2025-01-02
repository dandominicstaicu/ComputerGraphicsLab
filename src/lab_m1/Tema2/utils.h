#ifndef _UTILS_H_
#define _UTILS_H_

#include <glm/glm.hpp>
#include "components/simple_scene.h"

inline bool CheckSphereAABB(const glm::vec3& sphereCenter, float sphereRadius,
                            const glm::vec3& boxMin, const glm::vec3& boxMax)
{
    // For each axis X, Y, Z, find the point on the box’s face closest to the sphere center.
    float distSquared = 0.0f;

    // X-axis
    float x = glm::max(boxMin.x, glm::min(sphereCenter.x, boxMax.x));
    distSquared += (x - sphereCenter.x) * (x - sphereCenter.x);

    // Y-axis
    float y = glm::max(boxMin.y, glm::min(sphereCenter.y, boxMax.y));
    distSquared += (y - sphereCenter.y) * (y - sphereCenter.y);

    // Z-axis
    float z = glm::max(boxMin.z, glm::min(sphereCenter.z, boxMax.z));
    distSquared += (z - sphereCenter.z) * (z - sphereCenter.z);

    return distSquared <= (sphereRadius * sphereRadius);
}

#endif