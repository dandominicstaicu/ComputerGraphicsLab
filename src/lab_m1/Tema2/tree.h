#pragma once

#include "lab_m1/Tema2/obstacle.h"
#include "lab_m1/Tema2/hitbox.h"


namespace m1
{
    class Tree : public Obstacle
    {
    public:
        Tree(const std::unordered_map<std::string, Mesh*>& meshes,
             const std::unordered_map<std::string, Shader*>& shaders,
             Shader* shader,
             const glm::vec3& position,
             float scale);
        ~Tree();


        // We’ll store two bounding boxes:
        Hitbox trunkBox;
        Hitbox foliageBox;

        // For debugging/drawing
        void DrawHitbox(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);


    };

    
}