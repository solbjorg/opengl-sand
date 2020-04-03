#pragma once

#include <cstring>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

class Mesh {
public:
  std::string name;
  std::vector<glm::vec4> vertices;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> textureCoordinates;
  std::vector<unsigned int> indices;

  Mesh(std::string vname = "<missing>") : name(vname) {
    // Trading memory usage for speed
    vertices.reserve(200000);
    normals.reserve(200000);
    indices.reserve(200000);
  }

  bool hasNormals;
  bool hasTexture;

  unsigned long faceCount() { return (this->indices.size() / 3); }
};
