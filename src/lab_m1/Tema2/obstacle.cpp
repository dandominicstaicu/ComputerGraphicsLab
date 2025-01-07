#include "lab_m1/Tema2/obstacle.h"
#include <glm/gtc/type_ptr.hpp>

namespace m1
{
    Obstacle::Obstacle(const std::unordered_map<std::string, Mesh*>& meshes,
                       const std::unordered_map<std::string, Shader*>& shaders,
                       Shader* shader,
                       const glm::vec3& position,
                       float scale)
        : meshes(&meshes)  
        , shaders(&shaders)
        , shader(shader)  
        , position(position)   
        , scale(scale)
    {

    }

    Obstacle::~Obstacle()
    {

    }

    void Obstacle::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
    {
        shader->Use();

        for (size_t i = 0; i < components.size(); i++)
        {
            Mesh* mesh = components[i].first;
            const glm::mat4& modelMatrix = components[i].second;
            const glm::vec3& color = componentColors[i];

            glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
            glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
            glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

            // Set object color
            glUniform3fv(glGetUniformLocation(shader->GetProgramID(), "objectColor"), 1, glm::value_ptr(color));

            mesh->Render();
        }
    }
}