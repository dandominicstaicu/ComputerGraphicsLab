// checkpoint.h
#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include "components/simple_scene.h"
#include "hitbox.h"
#include "lab_m1/Tema2/hitbox.h"

#include <iostream>

namespace m1
{
    class Tema2; // Forward declaration

    class Checkpoint
    {
    public:
        Checkpoint(glm::vec3 position, glm::vec3 color);
        ~Checkpoint();

        // Getters
        glm::vec3 GetPosition() const { return position; }
        glm::vec3 GetColor() const { return color; }
        Hitbox& GetHitbox() { return hitbox; }

        // Setters
        void SetColor(const glm::vec3& newColor) { color = newColor; }
        void SetPosition(const glm::vec3& newPosition) { position = newPosition; }

        void DrawHitbox(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const Mesh* cubeMesh, Shader* shader);

        void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, 
                    Mesh* ringMesh, Shader* obstacleShader, Shader* aabbShader, Mesh* cubeMesh);


    private:
        glm::vec3 position;
        glm::vec3 color;

        // collision detection
        Hitbox hitbox;
    };
}
