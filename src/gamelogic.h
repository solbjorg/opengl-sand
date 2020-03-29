#pragma once

#include <utilities/window.hpp>
#include <GLFW/glfw3.h>
#include "sceneGraph.hpp"

void updateNodeTransformations(SceneNode* node, glm::mat4 transformationThusFar);
void initGame(GLFWwindow* window, CommandLineOptions options);
void updateFrame(GLFWwindow* window);
void renderFrame(GLFWwindow* window);

void keyboardCallback(GLFWwindow* window, int key, int scancode,
                      int action, int mods);
void mouseButtonCallback(GLFWwindow* window, int button, int action,
                         int mods);
void mouseCallback(GLFWwindow* window, double x, double y);
void processInput(GLFWwindow* window);
