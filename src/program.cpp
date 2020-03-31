// Local headers
#include "program.hpp"
#include "gamelogic.h"
#include "utilities/window.hpp"
#include <glm/glm.hpp>
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include <SFML/Audio.hpp>
#include <SFML/System/Time.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <utilities/glutils.h>
#include <utilities/shader.hpp>
#include <utilities/timeutils.h>

void runProgram(GLFWwindow *window, CommandLineOptions options) {
  // Enable depth (Z) buffer (accept "closest" fragment)
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // Configure miscellaneous OpenGL settings
  glEnable(GL_CULL_FACE);

  // Disable built-in dithering
  glDisable(GL_DITHER);

  // Enable transparency
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Set default colour after clearing the colour buffer
  glClearColor(0.657f, 0.64f, 0.59f, 1.0f);

  initGame(window, options);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Rendering Loop
  while (!glfwWindowShouldClose(window)) {
    // Clear colour and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    updateFrame(window);
    renderFrame(window);

    // Handle other events
    glfwPollEvents();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    // Flip buffers
    glfwSwapBuffers(window);
  }
}
