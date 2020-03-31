// TODO: I made a mess of submoduling imgui...fix tomorrow.

#include "gamelogic.h"
#include "mesh.hpp"
#include "sceneGraph.hpp"
#include <GLFW/glfw3.h>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <chrono>
#include <fmt/format.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <utilities/glutils.h>
#include <utilities/shader.hpp>
#include <utilities/timeutils.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include "imgui.hpp"

#include "OBJLoader.hpp"
#include "utilities/camera.hpp"
#include "utilities/imageLoader.hpp"
#include "utilities/texture.hpp"

enum KeyFrameAction { BOTTOM, TOP };

#include "timestamps.h"

bool escapeMouse = false;

unsigned int currentKeyFrame = 0;
unsigned int previousKeyFrame = 0;

float deltaTime;
float lastFrame;

SceneNode *rootNode;
SceneNode *terrainNode;

// These are heap allocated, because they should not be initialised at the start
// of the program
sf::SoundBuffer *buffer;
Gloom::Shader *shader;
sf::Sound *sound;

Camera camera(glm::vec3(0.0f, 1.0f, 0.0f));

glm::mat4 VP;

CommandLineOptions options;

bool hasStarted = false;
bool hasLost = false;
bool jumpedToNextFrame = false;
bool isPaused = false;

bool mouseLeftPressed = false;
bool mouseLeftReleased = false;
bool mouseRightPressed = false;
bool mouseRightReleased = false;

const float debug_startTime = 0;
double totalElapsedTime = debug_startTime;
double gameElapsedTime = debug_startTime;

unsigned int heightmap;
unsigned int normalShallowX;
unsigned int normalShallowZ;
unsigned int normalSteepX;
unsigned int normalSteepZ;

// frag shader settings
float N_y = 0.3f;
bool specular = true;
bool show_normal_map = false;
int shininess = 32;
int dot_degree = 4;

Gui *gui;

void initGame(GLFWwindow *window, CommandLineOptions gameOptions) {
  options = gameOptions;
  gui = new Gui(window);

  glfwSetCursorPosCallback(window, mouseCallback);
  glfwSetKeyCallback(window, keyboardCallback);

  shader = new Gloom::Shader();
  shader->makeBasicShader("../res/shaders/simple.vert",
                          "../res/shaders/simple.frag");
  shader->activate();

  // Create meshes
  Mesh terrain = loadTerrainMesh("../res/models/sanddunes.obj");
  unsigned int terrainVAO = generateBuffer(terrain);
  PNGImage sand_texture = loadPNGFile("../res/textures/sand2.png");
  unsigned int sand_texture_id = getTexture(sand_texture, true, false);

  PNGImage heightmapImg = loadPNGFile("../res/textures/HeightMap.png");
  heightmap = getTexture(heightmapImg, true);
  PNGImage heightmapShallowXImg =
      loadPNGFile("../res/textures/normalShallowX_2.png");
  normalShallowX = getTexture(heightmapShallowXImg, true);
  PNGImage heightmapShallowZImg =
      loadPNGFile("../res/textures/normalShallowZ_2.png");
  normalShallowZ = getTexture(heightmapShallowZImg, true);
  PNGImage heightmapSteepXImg =
      loadPNGFile("../res/textures/normalSteepX_2.png");
  normalSteepX = getTexture(heightmapSteepXImg, true);
  PNGImage heightmapSteepZImg =
      loadPNGFile("../res/textures/normalSteepZ_2.png");
  normalSteepZ = getTexture(heightmapSteepZImg, true);
  // Construct scene
  rootNode = createSceneNode();
  terrainNode = createSceneNode();

  rootNode->children.push_back(terrainNode);

  terrainNode->vertexArrayObjectID = terrainVAO;
  terrainNode->VAOIndexCount = terrain.indices.size();
  terrainNode->textureID = sand_texture_id;

  getTimeDeltaSeconds();

  std::cout << fmt::format("Initialized scene with {} SceneNodes.",
                           totalChildren(rootNode))
            << std::endl;

  std::cout << "Ready. Click to start!" << std::endl;
}

void updateFrame(GLFWwindow *window) {
  glm::mat4 projection =
      glm::perspective(glm::radians(80.0f),
                       float(windowWidth) / float(windowHeight), 0.1f, 350.f);

  glm::mat4 cameraTransform = camera.GetViewMatrix();

  VP = projection * cameraTransform;

  updateNodeTransformations(rootNode, glm::mat4(1.0f));
}

void updateNodeTransformations(SceneNode *node,
                               glm::mat4 transformationThusFar) {
  glm::mat4 transformationMatrix =
      glm::translate(node->position) * glm::translate(node->referencePoint) *
      glm::rotate(node->rotation.y, glm::vec3(0, 1, 0)) *
      glm::rotate(node->rotation.x, glm::vec3(1, 0, 0)) *
      glm::rotate(node->rotation.z, glm::vec3(0, 0, 1)) *
      glm::scale(node->scale) * glm::translate(-node->referencePoint);

  node->currentTransformationMatrix =
      transformationThusFar * transformationMatrix;

  switch (node->nodeType) {
  case GEOMETRY:
    break;
  case POINT_LIGHT:
    break;
  case SPOT_LIGHT:
    break;
  }

  for (SceneNode *child : node->children) {
    updateNodeTransformations(child, node->currentTransformationMatrix);
  }
}

void renderNode(SceneNode *node) {
  glUniform3fv(2, 1, glm::value_ptr(camera.Position));
  glUniformMatrix4fv(3, 1, GL_FALSE,
                     glm::value_ptr(node->currentTransformationMatrix));
  glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(VP));
  glUniform1f(5, N_y);
  glUniform1i(6, specular);
  glUniform1i(7, shininess);
  glUniform1i(8, dot_degree);
  glUniform1i(9, show_normal_map);

  switch (node->nodeType) {
  case GEOMETRY:
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, node->textureID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalShallowX);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, normalShallowZ);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, normalSteepX);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, normalSteepZ);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, heightmap);
    if (node->vertexArrayObjectID != -1) {
      glBindVertexArray(node->vertexArrayObjectID);
      glDrawElements(GL_TRIANGLES, node->VAOIndexCount, GL_UNSIGNED_INT,
                     nullptr);
    }
    break;
  case POINT_LIGHT:
    break;
  case SPOT_LIGHT:
    break;
  }

  for (SceneNode *child : node->children) {
    renderNode(child);
  }
}

void renderFrame(GLFWwindow *window) {
  int windowWidth, windowHeight;
  glfwGetWindowSize(window, &windowWidth, &windowHeight);
  glViewport(0, 0, windowWidth, windowHeight);

  GLfloat currentFrame = glfwGetTime();
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;

  // imgui stuff
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    if (escapeMouse) {
      ImGui::Begin("Shader");
      if (ImGui::Button("Reload shader")) {
        shader->reload();
      }
      ImGui::Text("Frag");
      ImGui::Checkbox("Show normal map?", &show_normal_map);
      ImGui::Text("Diffuse");
      ImGui::SliderFloat("N.y modifier", &N_y, 0.0f, 1.0f);
      ImGui::SliderInt("Dot degree", &dot_degree, 1, 6);
      ImGui::Text("Specular");
      ImGui::Checkbox("Specular on?", &specular);
      ImGui::SliderInt("Shininess", &shininess, 0, 256);
      ImGui::End();
    }
    ImGui::Render();
  }

  renderNode(rootNode);
  camera.updateCamera(deltaTime);
}

void mouseCallback(GLFWwindow *window, double x, double y) {
  if (!gui->io->WantCaptureMouse && !escapeMouse)
    camera.ProcessMouseMovement(x, y);
}

void keyboardCallback(GLFWwindow *window, int key, int scancode, int action,
                      int mods) {
  // intuitively, if we release a button it should not keep moving
  if (!gui->io->WantCaptureKeyboard || action == GLFW_RELEASE)
    camera.handleKeyboardInputs(key, action);
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    escapeMouse = !escapeMouse;
    if (escapeMouse) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
  }
}
