#include "lab_m1/Tema2/building.h"

namespace m1
{
    Building::Building(const std::unordered_map<std::string, Mesh*>& meshes,
                       const std::unordered_map<std::string, Shader*>& shaders,
                       Shader* shader,
                       const glm::vec3& position,
                       float scale)
        : Obstacle(meshes, shaders, shader, position, scale)
    {
        // Use "box" to represent the building
        Mesh* boxMesh = meshes.at("box");

        glm::mat4 buildingModel = glm::translate(glm::mat4(1.0f), position);
        buildingModel = glm::scale(buildingModel, glm::vec3(scale * 0.35f));

        components.emplace_back(boxMesh, buildingModel);
        componentColors.emplace_back(glm::vec3(0.5f, 0.5f, 0.5f));
    }

    Building::~Building()
    {
    }

    bool Building::CheckCollisionWithSphere(const glm::vec3& sphereCenter, float sphereRadius)
    {
        // Half-size in each dimension of the final scaled building
        float halfSize = (scale * 0.35f) * 0.5f * 0.35f;
        
        // Build AABB
        glm::vec3 boxMin = position - glm::vec3(halfSize);
        glm::vec3 boxMax = position + glm::vec3(halfSize);

        // Check collision
        return CheckSphereAABB(sphereCenter, sphereRadius, boxMin, boxMax);
    }

    std::vector<std::pair<glm::vec3, glm::vec3>> Building::GetAABBs() const
    {
        std::vector<std::pair<glm::vec3, glm::vec3>> aabbs;

        // Building is represented by a single box
        float halfSize = (scale * 0.35f) * 0.5f;

        glm::vec3 boxMin = glm::vec3(
            position.x - halfSize,
            position.y - halfSize, // y is centered
            position.z - halfSize
        );
        glm::vec3 boxMax = glm::vec3(
            position.x + halfSize,
            position.y + halfSize,
            position.z + halfSize
        );

        aabbs.emplace_back(boxMin, boxMax);

        return aabbs;
    }


}
