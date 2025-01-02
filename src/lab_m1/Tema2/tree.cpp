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

    bool Tree::CheckCollisionWithSphere(const glm::vec3& sphereCenter, float sphereRadius)
    {
        // 1) TRUNK bounding box
        float trunkHalfX = (scale * 0.2f) * 0.07f;   // x half-extent
        float trunkHalfZ = (scale * 0.2f) * 0.07;   // z half-extent
        float trunkHeight = (scale * 0.38f) * 0.25f;        // total height
        // We place the trunk so that position.y is the bottom, then top is position.y + trunkHeight:
        glm::vec3 trunkMin = glm::vec3(
            position.x - trunkHalfX,
            position.y,
            position.z - trunkHalfZ
        );
        glm::vec3 trunkMax = glm::vec3(
            position.x + trunkHalfX,
            position.y + trunkHeight, 
            position.z + trunkHalfZ
        );

        // 2) FOLIAGE bounding box 
        float foliageHalf = scale * 0.5f;  // rough half-extent in X, Z
        float foliageHeight = scale;       // total height in Y
        float foliageOffset = scale * 0.09f;

        glm::vec3 foliageMin = glm::vec3(
            position.x - foliageHalf,
            position.y + foliageOffset,  // offset upward
            position.z - foliageHalf
        );
        glm::vec3 foliageMax = glm::vec3(
            position.x + foliageHalf,
            position.y + foliageOffset + foliageHeight,
            position.z + foliageHalf
        );

        // Now we check collision with trunk box AND foliage box
        bool collTrunk = CheckSphereAABB(sphereCenter, sphereRadius, trunkMin, trunkMax);
        bool collFoliage = CheckSphereAABB(sphereCenter, sphereRadius, foliageMin, foliageMax);

        return (collTrunk || collFoliage);
    }

    std::vector<std::pair<glm::vec3, glm::vec3>> Tree::GetAABBs() const
    {
        std::vector<std::pair<glm::vec3, glm::vec3>> aabbs;

        // TRUNK bounding box
        float trunkHalfX = (scale * 0.2f) * 0.07f;   // x half-extent
        float trunkHalfZ = (scale * 0.2f) * 0.07f;   // z half-extent
        float trunkHeight = (scale * 0.38f) * 0.25f; // total height

        glm::vec3 trunkMin = glm::vec3(
            position.x - trunkHalfX,
            position.y,
            position.z - trunkHalfZ
        );
        glm::vec3 trunkMax = glm::vec3(
            position.x + trunkHalfX,
            position.y + trunkHeight,
            position.z + trunkHalfZ
        );

        aabbs.emplace_back(trunkMin, trunkMax);

        // FOLIAGE bounding box
        float foliageHalf = scale * 0.05f;  // half-extent in X, Z
        float foliageHeight = scale * 0.115;       // height in Y
        float foliageOffset = scale * 0.09f;

        glm::vec3 foliageMin = glm::vec3(
            position.x - foliageHalf,
            position.y + foliageOffset,
            position.z - foliageHalf
        );
        glm::vec3 foliageMax = glm::vec3(
            position.x + foliageHalf,
            position.y + foliageOffset + foliageHeight,
            position.z + foliageHalf
        );

        aabbs.emplace_back(foliageMin, foliageMax);

        return aabbs;
    }
}
