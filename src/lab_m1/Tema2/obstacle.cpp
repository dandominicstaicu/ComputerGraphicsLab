#include "lab_m1/Tema2/obstacle.h"

namespace m1
{
    Obstacle::Obstacle(const std::unordered_map<std::string, Mesh*>& meshes,
                       const std::unordered_map<std::string, Shader*>& shaders,
                       Shader* shader,
                       const glm::vec3& position,
                       float scale)
        : shader(shader), position(position), scale(scale)
    {
        // My model exported from OnShape is rotated 90 degrees around the X axis
        float rotationAngle = glm::radians(-90.0f); // Convert degrees to radians

        {
            // Trunk: Cylinder (Brown Color)
            Mesh* trunkMesh = meshes.at("cylinder"); // Use "cylinder" mesh
            glm::mat4 trunkModel = glm::translate(glm::mat4(1.0f), position);
            trunkModel = glm::rotate(trunkModel, rotationAngle, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate to align with Y-axis
            trunkModel = glm::scale(trunkModel, glm::vec3(scale * 0.2f, scale * 0.2f, scale * .38f));
            components.emplace_back(trunkMesh, trunkModel);
            componentColors.emplace_back(glm::vec3(0.55f, 0.27f, 0.07f)); // Brown color

        }
    
        {
            // Foliage: Cone (Green Color)
            Mesh* foliageMesh = meshes.at("cones"); // Use "cones" mesh
            glm::mat4 foliageModel = glm::translate(glm::mat4(1.0f), position + glm::vec3(0.0f, scale * 0.09f, 0.0f));
            foliageModel = glm::rotate(foliageModel, rotationAngle, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate to align with Y-axis
            foliageModel = glm::scale(foliageModel, glm::vec3(scale, scale, scale));
            components.emplace_back(foliageMesh, foliageModel);
            componentColors.emplace_back(glm::vec3(0.0f, 0.8f, 0.0f)); // Green color
        }
    }

    Obstacle::~Obstacle()
    {
        // No dynamic memory to clean up
    }

    void Obstacle::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
    {
        shader->Use();
        for(size_t i = 0; i < components.size(); ++i)
        {
            Mesh* mesh = components[i].first;
            glm::mat4 modelMatrix = components[i].second;
            glm::vec3 color = componentColors[i];

            glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
            glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
            glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

            // Set object color
            glUniform3fv(glGetUniformLocation(shader->GetProgramID(), "objectColor"), 1, glm::value_ptr(color));

            mesh->Render();
        }
    }
}
