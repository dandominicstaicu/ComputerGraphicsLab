// checkpoint.h
#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include "components/simple_scene.h"
#include "hitbox.h"

namespace m1
{
    class Tema2; // Forward declaration

    class Checkpoint
    {
    public:
        Checkpoint(glm::vec3 position, glm::vec3 color);
        ~Checkpoint();

        void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, std::unordered_map<std::string, Mesh*>& meshes, Shader* shader);

        // Getters
        glm::vec3 GetPosition() const { return position; }
        glm::vec3 GetColor() const { return color; }

        // Setters
        void SetColor(const glm::vec3& newColor) { color = newColor; }
        void SetPosition(const glm::vec3& newPosition) { position = newPosition; }

    private:
        glm::vec3 position;
        glm::vec3 color;

        // Optional: If you plan to add collision detection later
        Hitbox hitbox;
    };
}
