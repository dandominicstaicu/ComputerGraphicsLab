#include "lab_m1/Tema2/tree.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace m1
{
    Tree::Tree(const std::unordered_map<std::string, Mesh*>& meshes,
               const std::unordered_map<std::string, Shader*>& shaders,
               Shader* shader,
               const glm::vec3& position,
               float scale)
        : Obstacle(meshes, shaders, shader, position, scale)
    {
        // We'll keep the final transforms in trunkModel and foliageModel
        float rotationAngle = glm::radians(-90.0f);

        // --- Trunk Setup ---
        {
            Mesh* trunkMesh = meshes.at("cylinder");
            glm::mat4 trunkModel = glm::mat4(1.0f);
            trunkModel = glm::translate(trunkModel, position);
            trunkModel = glm::rotate(trunkModel, rotationAngle, glm::vec3(1.f, 0.f, 0.f));
            trunkModel = glm::scale(trunkModel, glm::vec3(scale * 0.2f, scale * 0.2f, scale * 0.38f));
            
            components.emplace_back(trunkMesh, trunkModel);
            componentColors.emplace_back(glm::vec3(0.55f, 0.27f, 0.07f)); // Brown

            // Approx local bounding box for a unit cylinder
            trunkBox.localMin = glm::vec3(-0.05f, -0.05f, -0.0f);
            trunkBox.localMax = glm::vec3( 0.05f,  0.05f,  0.25f);

            // Update to get worldMin/worldMax
            trunkBox.Update(trunkModel);
        }

        // --- Foliage Setup ---
        {
            Mesh* foliageMesh = meshes.at("cones");
            glm::mat4 foliageModel = glm::mat4(1.0f);
            foliageModel = glm::translate(foliageModel, position + glm::vec3(0.0f, scale * 0.09f, 0.0f));
            foliageModel = glm::rotate(foliageModel, rotationAngle, glm::vec3(1.f, 0.f, 0.f));
            foliageModel = glm::scale(foliageModel, glm::vec3(scale, scale, scale));

            components.emplace_back(foliageMesh, foliageModel);
            componentColors.emplace_back(glm::vec3(0.f, 0.8f, 0.f)); // Green

            // Approx local bounding box for the double-cone
            foliageBox.localMin = glm::vec3(-0.05f, -0.05f, -0.0f);
            foliageBox.localMax = glm::vec3( 0.05f,  0.05f,  0.12f);

            foliageBox.Update(foliageModel);
        }
    }

    Tree::~Tree()
    {
    }

    // Optional: a helper to visually draw the bounding boxes
    void Tree::DrawHitbox(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
    {
        // Ensure the AABBShader is available
        if (!shaders->count("AABBShader") || !shaders->at("AABBShader")->program) return;
        Shader* aabbShader = shaders->at("AABBShader");
        aabbShader->Use();

        // Set the hitbox color to red
        glUniform3f(glGetUniformLocation(aabbShader->GetProgramID(), "objectColor"), 1.0f, 0.0f, 0.0f); // Red

        // Function to draw a single hitbox
        auto DrawBox = [&](const Hitbox& box)
        {
            glm::vec3 size = box.worldMax - box.worldMin;
            glm::vec3 center = (box.worldMax + box.worldMin) * 0.5f;

            glm::mat4 modelBox = glm::mat4(1.0f);
            modelBox = glm::translate(modelBox, center);
            modelBox = glm::scale(modelBox, size);

            glUniformMatrix4fv(glGetUniformLocation(aabbShader->GetProgramID(), "model"), 1, GL_FALSE, glm::value_ptr(modelBox));
            glUniformMatrix4fv(glGetUniformLocation(aabbShader->GetProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
            glUniformMatrix4fv(glGetUniformLocation(aabbShader->GetProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

            // Enable wireframe mode for hitbox visualization
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            // Render the cube hitbox
            meshes->at("cube")->Render();

            // Restore to fill mode
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        };

        // Draw trunk and foliage hitboxes
        DrawBox(trunkBox);
        DrawBox(foliageBox);
    }
}
