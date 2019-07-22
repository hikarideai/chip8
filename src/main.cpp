#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "App.hpp"

static const char * TITLE = "CHIP8 Emulator";
static const int WIDTH = 800;
static const int HEIGHT = 400;

App *chip8emul;

void keyCallback(GLFWwindow *, int key, int, int action, int);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

int main() {
  // Initialize GLFW
  glfwInit();
  // Configure desired OpenGL context
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  // Instantiate it
  GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);
  if (window == nullptr) {
    std::cerr << "FATAL ERROR: Failed to initialize OpenGL 3.3 Core context.";
    glfwTerminate();
    return 1;
  }
  // And tell OpenGL to use it
  glfwMakeContextCurrent(window);
  // Configure window callbacks
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  // Unsheathe OpenGL API
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "FATAL ERROR: GLAD failed. OpenGL API is unavailable.";
    glfwTerminate();
    return 1;
  }
  // Configure OpenGL
  glViewport(0, 0, WIDTH, HEIGHT);

  chip8emul = new App(WIDTH, HEIGHT);

  // Game loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    // ProcessInput()

    chip8emul->Update();

    glClearColor(0, 1, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    chip8emul->Render();

    glfwSwapBuffers(window);
  }

  return 0;
}

void keyCallback(GLFWwindow *, int key, int scancode, int action, int mods) {
  chip8emul->keys[key] = action == GLFW_PRESS;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}
