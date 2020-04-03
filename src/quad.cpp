#include "quad.hpp"
#include "utilities/window.hpp"

// modified version of cube found in shapes.cpp
Mesh quad() {
  int w = windowWidth;
  int h = windowHeight;
  float ratio = float(w) / float(h);
  glm::vec4 verts[4] = {
      {-1.0, 1.0f, 0.0f, 0.0f},
      {-1.0, -1.0f, 0.0f, 0.0f},
      {0.0, -1.0f, 0.0f, 0.0f},
      {0.0, 1.0f, 0.0f, 0.0f},
  };
  unsigned indices[6] = {0, 1, 2, 0, 2, 3};
  // gcc gang, we make 4 normals all pointing out from the quad:
  std::vector<glm::vec3> normals(4, glm::vec3(0.0f, 0.0f, 1.0f));
  glm::vec2 UVs[4] = {
      {0, 1},
      {0, 0},
      {1, 0},
      {1, 1},
  };

  Mesh m;
  m.hasNormals = true;
  m.hasTexture = true;
  m.vertices = std::vector<glm::vec4>(std::begin(verts), std::end(verts));
  m.normals = normals;
  m.textureCoordinates = std::vector<glm::vec2>(std::begin(UVs), std::end(UVs));
  m.indices = std::vector<unsigned int>(std::begin(indices), std::end(indices));

  return m;
}
