#pragma once

#include "lab_m1/Tema2/obstacle.h"
#include "lab_m1/Tema2/utils.h"    // for CheckSphereAABB

namespace m1
{
    class Building : public Obstacle
    {
    public:
        Building(const std::unordered_map<std::string, Mesh*>& meshes,
                 const std::unordered_map<std::string, Shader*>& shaders,
                 Shader* shader,
                 const glm::vec3& position,
                 float scale);
        ~Building();

        bool CheckCollisionWithSphere(const glm::vec3& sphereCenter, float sphereRadius) override;
    
        // AABB Retrieval
        std::vector<std::pair<glm::vec3, glm::vec3>> GetAABBs() const override;
    
    };
}
