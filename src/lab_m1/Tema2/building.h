#pragma once

#include "lab_m1/Tema2/obstacle.h"
#include "lab_m1/Tema2/hitbox.h"

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

        Hitbox buildingBox;

        // Debug
        void DrawHitbox(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

    
    };
}