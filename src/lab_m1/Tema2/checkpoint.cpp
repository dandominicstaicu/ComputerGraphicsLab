// checkpoint.cpp
#include "checkpoint.h"
#include "tema2.h" // To access Tema2's RenderMesh method
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace m1
{
    Checkpoint::Checkpoint(glm::vec3 pos, glm::vec3 col)
        : position(pos), color(col)
    {
        // Define the size of the checkpoint
        glm::vec3 size = glm::vec3(7.0f, 1.0f, 7.0f); // Width, Height, Depth

        // Initialize Hitbox's localMin and localMax relative to model origin
        hitbox.localMin = glm::vec3(-0.04f, -0.18f, -0.18f);
        hitbox.localMax = glm::vec3( 0.04f,  0.18f,  0.18f);

        // Initially, worldMin and worldMax are same as local (will be updated)
        hitbox.worldMin = hitbox.localMin;
        hitbox.worldMax = hitbox.localMax;

    }

    Checkpoint::~Checkpoint()
    {
        // Cleanup if necessary
    }

    void Checkpoint::DrawHitbox(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const std::unordered_map<std::string, Mesh*>& meshes, Shader* shader)
    {
        // Ensure the AABBShader and hitbox mesh are available
        if (!shader || !shader->program)
            return;

        // Calculate size and center of the hitbox
        glm::vec3 size = hitbox.worldMax - hitbox.worldMin;
        glm::vec3 center = (hitbox.worldMin + hitbox.worldMax) * 0.5f;

        // Create the model matrix for the hitbox
        glm::mat4 modelBox = glm::mat4(1.0f);
        modelBox = glm::translate(modelBox, center);
        modelBox = glm::scale(modelBox, size);

        // Set shader uniforms
        shader->Use();
        glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "model"), 1, GL_FALSE, glm::value_ptr(modelBox));
        glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        // Set the hitbox color to red
        glUniform3f(glGetUniformLocation(shader->GetProgramID(), "objectColor"), 1.0f, 0.0f, 0.0f); // Red

        // Enable wireframe mode for hitbox visualization
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // Render the cube hitbox
        if (meshes.find("cube") != meshes.end())
        {
            meshes.at("cube")->Render();
        }

        // Restore to fill mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}
