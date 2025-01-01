#include "lab_m1/Tema2/terrain.h"
#include <iostream>

Terrain::Terrain() : terrainMesh(nullptr) {}

Terrain::~Terrain() {
    if (terrainMesh) {
        delete terrainMesh;
    }
}

void Terrain::GenerateGrid(int width, int height) {
    vertices.clear();
    indices.clear();

    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;

    // Generate vertices
    for (int z = 0; z <= height; ++z) {
        for (int x = 0; x <= width; ++x) {
            vertices.emplace_back(
                glm::vec3(x - halfWidth, 0.0f, z - halfHeight),  // Center the grid
                glm::vec3(0, 1, 0),                              // Normal
                glm::vec3(0.5f, 0.5f, 0.5f));                   // Color
        }
    }

    // Generate indices
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            int topLeft = z * (width + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = topLeft + (width + 1);
            int bottomRight = bottomLeft + 1;

            // Triangle 1
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // Triangle 2
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    // Create the mesh
    terrainMesh = new Mesh("terrain");
    terrainMesh->InitFromData(vertices, indices);
}


Mesh* Terrain::GetMesh() {
    return terrainMesh;
}
