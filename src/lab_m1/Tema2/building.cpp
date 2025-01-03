#include "lab_m1/Tema2/building.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace m1
{
    Building::Building(const std::unordered_map<std::string, Mesh*>& meshes,
                       const std::unordered_map<std::string, Shader*>& shaders,
                       Shader* shader,
                       const glm::vec3& position,
                       float scale)
        : Obstacle(meshes, shaders, shader, position, scale)
    {
        Mesh* boxMesh = meshes.at("box");

        glm::mat4 buildingModel = glm::mat4(1.0f);
        buildingModel = glm::translate(buildingModel, position);
        buildingModel = glm::scale(buildingModel, glm::vec3(scale * 0.35f));

        components.emplace_back(boxMesh, buildingModel);
        componentColors.emplace_back(glm::vec3(0.5f, 0.5f, 0.5f));

        // Local bounding box for a 1x1x1 “box.obj”
        buildingBox.localMin = glm::vec3(-0.5f);
        buildingBox.localMax = glm::vec3( 0.5f);

        // Now reflect the model transform
        buildingBox.Update(buildingModel);
    }

    Building::~Building()
    {
    }

    void Building::DrawHitbox(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
    {
        // Ensure the AABBShader is available
        if (!shaders->count("AABBShader") || !shaders->at("AABBShader")->program) return;
        Shader* aabbShader = shaders->at("AABBShader");
        aabbShader->Use();

        // Set the hitbox color to red
        glUniform3f(glGetUniformLocation(aabbShader->GetProgramID(), "objectColor"), 1.0f, 0.0f, 0.0f); // Red

        // Calculate size and center of the hitbox
        glm::vec3 size = buildingBox.worldMax - buildingBox.worldMin;
        glm::vec3 center = (buildingBox.worldMax + buildingBox.worldMin) * 0.5f;

        // Create the model matrix for the hitbox
        glm::mat4 modelBox = glm::mat4(1.0f);
        modelBox = glm::translate(modelBox, center);
        modelBox = glm::scale(modelBox, size);

        // Set the uniforms
        glUniformMatrix4fv(glGetUniformLocation(aabbShader->GetProgramID(), "model"), 1, GL_FALSE, glm::value_ptr(modelBox));
        glUniformMatrix4fv(glGetUniformLocation(aabbShader->GetProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(aabbShader->GetProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        // Enable wireframe mode for hitbox visualization
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // Render the cube hitbox
        meshes->at("cube")->Render();

        // Restore to fill mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}
