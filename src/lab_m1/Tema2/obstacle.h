#pragma once

#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include "core/gpu/mesh.h"
#include "components/simple_scene.h"

namespace m1
{
    class Obstacle
    {
    public:
        Obstacle(const std::unordered_map<std::string, Mesh*>& meshes,
                 const std::unordered_map<std::string, Shader*>& shaders,
                 Shader* shader,
                 const glm::vec3& position,
                 float scale);
        virtual ~Obstacle();

        virtual void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

        glm::vec3 GetPosition() const { return position; }
        float GetScale() const { return scale; }

        // virtual bool CheckCollisionWithSphere(const glm::vec3& sphereCenter, float sphereRadius) = 0;

        // TODO remove
        // NEW: for collision detection
        virtual bool CheckCollisionWithSphere(const glm::vec3& sphereCenter, float sphereRadius) = 0;
        
        // NEW: to retrieve AABBs for visualization
        virtual std::vector<std::pair<glm::vec3, glm::vec3>> GetAABBs() const = 0;


    protected:
        // Inheriting classes will populate these with their own geometry
        std::vector<std::pair<Mesh*, glm::mat4>> components;
        std::vector<glm::vec3> componentColors;


        // Common fields
        Shader* shader;
        glm::vec3 position;
        float scale;
    };
}
