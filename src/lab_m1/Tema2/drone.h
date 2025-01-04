// drone.h
#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include "components/simple_scene.h"
#include "lab_m1/Tema2/hitbox.h"

namespace m1
{
    class Tema2; // Forward declaration

    class Drone
    {
    public:
        Drone();
        ~Drone();

        // Updated Init method to accept Tema2* parent
        void Init(std::unordered_map<std::string, Mesh*>& meshes,
                  std::unordered_map<std::string, Shader*>& shaders,
                  Shader* shader,
                  Tema2* parent);
        void Update(float deltaTimeSeconds);
        void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

        // Input handling methods
        void MoveForward(float distance);
        void MoveBackward(float distance);
        void MoveLeft(float distance);
        void MoveRight(float distance);
        void MoveUp(float distance);
        void MoveDown(float distance);
        void RotateLeft(float angle);
        void RotateRight(float angle);

        // Getters and Setters
        glm::vec3 GetPosition() const;
        void SetPosition(const glm::vec3& position);

        // Get the forward direction of the drone
        glm::vec3 GetForwardVector() const;

        // The bounding box that we'll check for collisions
        Hitbox droneBox;


        // draw hitbox
        void DrawHitbox(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

    private:
        // Drone properties
        float rotorAngle;
        glm::vec3 position;
        glm::vec3 scale;
        float rotationY;

        // Meshes and Shaders
        std::unordered_map<std::string, Mesh*> meshes;
        std::unordered_map<std::string, Shader*> shaders;
        Shader* shader;

        // Pointer to parent Tema2
        Tema2* parent;

        // Helper methods
        void RenderBody(const glm::mat4& modelMatrix);
        void RenderPropellers(const glm::mat4& modelMatrix);
        void RenderAxes(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
    
    
    };
} // namespace m1