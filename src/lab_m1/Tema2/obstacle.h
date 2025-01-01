#pragma once

#include <unordered_map>
#include "core/gpu/mesh.h"
#include <glm/glm.hpp>
#include <vector>
#include "components/simple_scene.h"

namespace m1
{
    class Obstacle
    {
    public:
        Obstacle(const std::unordered_map<std::string, Mesh*>& meshes, const std::unordered_map<std::string, Shader*>& shaders, Shader* shader, const glm::vec3& position, float scale);
        ~Obstacle();

        void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

        // Get the position and scale for collision detection
        glm::vec3 GetPosition() const { return position; }
        float GetScale() const { return scale; }

    protected:
        std::vector<std::pair<Mesh*, glm::mat4>> components; // Each component mesh and its model matrix
        std::vector<glm::vec3> componentColors;
        
    private:
        Shader* shader;
        glm::vec3 position;
        float scale;
    };
}
