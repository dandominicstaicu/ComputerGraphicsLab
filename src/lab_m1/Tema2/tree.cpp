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
        // Model from OnShape is rotated 90 degrees around the X axis
        float rotationAngle = glm::radians(-90.0f);

        // Trunk: Cylinder (brown)
        {
            Mesh* trunkMesh = meshes.at("cylinder");
            glm::mat4 trunkModel = glm::translate(glm::mat4(1.0f), position);
            trunkModel = glm::rotate(trunkModel, rotationAngle, glm::vec3(1.f, 0.f, 0.f));
            trunkModel = glm::scale(trunkModel, glm::vec3(scale * 0.2f, scale * 0.2f, scale * 0.38f));
            components.emplace_back(trunkMesh, trunkModel);
            componentColors.emplace_back(glm::vec3(0.55f, 0.27f, 0.07f)); // Brown
        }

        // Foliage: Cone (green)
        {
            Mesh* foliageMesh = meshes.at("cones");
            glm::mat4 foliageModel = glm::translate(glm::mat4(1.0f), position + glm::vec3(0.0f, scale * 0.09f, 0.0f));
            foliageModel = glm::rotate(foliageModel, rotationAngle, glm::vec3(1.f, 0.f, 0.f));
            foliageModel = glm::scale(foliageModel, glm::vec3(scale, scale, scale));
            components.emplace_back(foliageMesh, foliageModel);
            componentColors.emplace_back(glm::vec3(0.f, 0.8f, 0.f)); // Green
        }
    }

    Tree::~Tree()
    {
    }
}
