#pragma once

#include "lab_m1/Tema2/obstacle.h"

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
    };
}
