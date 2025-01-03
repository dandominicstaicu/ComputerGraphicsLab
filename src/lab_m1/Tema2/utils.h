#ifndef _UTILS_H_
#define _UTILS_H_

#include <glm/glm.hpp>
#include "components/simple_scene.h"

inline bool CheckAABBAABB(const glm::vec3& minA, const glm::vec3& maxA,
                          const glm::vec3& minB, const glm::vec3& maxB)
{
    // Two AABBs overlap if and only if
    // they overlap in X, overlap in Y, and overlap in Z
    return (minA.x <= maxB.x && maxA.x >= minB.x) &&
           (minA.y <= maxB.y && maxA.y >= minB.y) &&
           (minA.z <= maxB.z && maxA.z >= minB.z);
}

#endif