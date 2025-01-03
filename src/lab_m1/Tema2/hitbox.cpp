#include "hitbox.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

static glm::vec3 TransformPoint(const glm::mat4& mat, const glm::vec3& pt)
{
    glm::vec4 transformed = mat * glm::vec4(pt, 1.0f);
    return glm::vec3(transformed);
}

// Update the worldMin / worldMax by transforming all 8 corners
// and computing the new min / max
void Hitbox::Update(const glm::mat4& modelMatrix)
{
    // All 8 corners in local space
    std::vector<glm::vec3> corners = {
        { localMin.x, localMin.y, localMin.z },
        { localMin.x, localMin.y, localMax.z },
        { localMin.x, localMax.y, localMin.z },
        { localMin.x, localMax.y, localMax.z },
        { localMax.x, localMin.y, localMin.z },
        { localMax.x, localMin.y, localMax.z },
        { localMax.x, localMax.y, localMin.z },
        { localMax.x, localMax.y, localMax.z },
    };

    // Transform each corner to world space
    for (int i = 0; i < 8; i++)
    {
        corners[i] = TransformPoint(modelMatrix, corners[i]);
    }

    // Compute new min / max
    glm::vec3 minP = corners[0];
    glm::vec3 maxP = corners[0];
    for (int i = 1; i < 8; i++)
    {
        minP.x = std::min(minP.x, corners[i].x);
        minP.y = std::min(minP.y, corners[i].y);
        minP.z = std::min(minP.z, corners[i].z);

        maxP.x = std::max(maxP.x, corners[i].x);
        maxP.y = std::max(maxP.y, corners[i].y);
        maxP.z = std::max(maxP.z, corners[i].z);
    }
    worldMin = minP;
    worldMax = maxP;
}

std::vector<glm::vec3> Hitbox::GetCorners() const
{
    return {
        worldMin,
        { worldMin.x, worldMin.y, worldMax.z },
        { worldMin.x, worldMax.y, worldMin.z },
        { worldMin.x, worldMax.y, worldMax.z },
        { worldMax.x, worldMin.y, worldMin.z },
        { worldMax.x, worldMin.y, worldMax.z },
        { worldMax.x, worldMax.y, worldMin.z },
        worldMax
    };
}

// A simple AABB-vs-AABB collision check in *world* space
bool CheckCollision(const Hitbox& boxA, const Hitbox& boxB)
{
    // Check overlap on X, Y, Z axes
    return (boxA.worldMin.x <= boxB.worldMax.x && boxA.worldMax.x >= boxB.worldMin.x) &&
           (boxA.worldMin.y <= boxB.worldMax.y && boxA.worldMax.y >= boxB.worldMin.y) &&
           (boxA.worldMin.z <= boxB.worldMax.z && boxA.worldMax.z >= boxB.worldMin.z);
}
