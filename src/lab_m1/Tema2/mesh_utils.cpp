// mesh_utils.cpp
#include "lab_m1/Tema2/mesh_utils.h"
#include <glm/glm.hpp>

namespace m1 {
    Mesh* CreateCube(const std::string& name)
    {
        // A simple unit cube [-0.5..0.5]^3 for each axis
        std::vector<VertexFormat> vertices =
        {
            // Position            // Normal      // Texture
            VertexFormat(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0, 0, 1)), 
            VertexFormat(glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0, 0, 1)), 
            VertexFormat(glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0, 0, 1)), 
            VertexFormat(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0, 0, 1)), 

            VertexFormat(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0, 0, -1)), 
            VertexFormat(glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0, 0, -1)), 
            VertexFormat(glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0, 0, -1)), 
            VertexFormat(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0, 0, -1))  
        };

        // 12 triangles (36 indices), or as quads grouped by faces
        std::vector<unsigned int> indices =
        {
            // front
            0, 1, 2,   2, 3, 0,
            // right
            1, 5, 6,   6, 2, 1,
            // back
            7, 6, 5,   5, 4, 7,
            // left
            4, 0, 3,   3, 7, 4,
            // top
            3, 2, 6,   6, 7, 3,
            // bottom
            4, 5, 1,   1, 0, 4
        };

        Mesh* cube = new Mesh(name);
        cube->InitFromData(vertices, indices);
        return cube;
    }
}
