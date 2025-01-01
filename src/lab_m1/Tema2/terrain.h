#pragma once

#include "core/gpu/mesh.h"
#include <vector>
#include <glm/glm.hpp>

class Terrain {
public:
    Terrain();
    ~Terrain();

    // Initializes the grid with specified resolution
    void GenerateGrid(int width, int height);

    // Returns the mesh object
    Mesh* GetMesh();

private:
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;
    Mesh* terrainMesh;
};


