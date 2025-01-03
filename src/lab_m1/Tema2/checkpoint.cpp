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
        // Initialize the hitbox if needed later
        // For now, it can remain empty or be set to match the ring's dimensions
    }

    Checkpoint::~Checkpoint()
    {
        // Cleanup if necessary
    }

    void Checkpoint::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, std::unordered_map<std::string, Mesh*>& meshes, Shader* shader)
    {
        if (!shader || !shader->program)
            return;

        shader->Use();

        // Set the model matrix for the checkpoint
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        // Optionally, scale the checkpoint if needed
        // modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f));

        // Set the color uniform
        glUniform3fv(glGetUniformLocation(shader->GetProgramID(), "object_color"), 1, glm::value_ptr(color));

        // Render the ring mesh
        if (meshes.find("ring") != meshes.end())
        {
            // Assuming you have a shader that uses "object_color" to set the fragment color
            Tema2* parent = nullptr; // If you need access to parent, modify accordingly
            // parent->RenderMesh(meshes["ring"], shader, modelMatrix);
            // Since we don't have access to the parent here, we'll assume RenderMesh is accessible
            // Alternatively, pass the RenderMesh function as a parameter or make it accessible globally
        }
    }
}
