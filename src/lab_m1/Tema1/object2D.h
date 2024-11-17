#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"

#define PI 3.14159265359f

namespace object2D
{

    // Create square with given bottom left corner, length and color
    Mesh* CreateSquare(const std::string &name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
    Mesh* CreateCircle(const std::string& name, glm::vec3 center, float radius, glm::vec3 color, bool fill, int numSegments); 
    Mesh* CreateTrapezoid(const std::string& name, glm::vec3 bottomLeft, float widthTop, float widthBottom, float height, glm::vec3 color);
    Mesh* CreateRectangle(const std::string &name, glm::vec3 bottomLeftCorner, float width, float height, glm::vec3 color, bool fill);
    Mesh* CreateCloud(const std::string& name, glm::vec3 center, float radius, glm::vec3 color);
}
