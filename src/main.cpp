#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "App.hpp"

static const char * TITLE = "CHIP8 Emulator";
static const int WIDTH = 832;
static const int HEIGHT = 416;

struct {
  char * file;
  int16_t offcolor;
  int16_t oncolor;
  bool flipx;
  bool flipy;
} conf;

App *chip8emul;

void keyCallback(GLFWwindow *, int key, int, int action, int);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
bool parseArgs(int argc, char * argv[]);

int main(int argc, char * argv[]) {
  // Process command line arguments
  if (!parseArgs(argc, argv))
    return 0;

  // Initialize GLFW
  glfwInit();
  // Configure desired OpenGL context
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  // Instantiate it
  GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);
  if (window == nullptr) {
    std::cerr << "FATAL ERROR: Failed to initialize OpenGL 3.3 Core context.";
    glfwTerminate();
    return 1;
  }
  // And tell OpenGL to use it
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  // Configure window callbacks
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetKeyCallback(window, keyCallback);
  // Unsheathe OpenGL API
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "FATAL ERROR: GLAD failed. OpenGL API is unavailable.";
    glfwTerminate();
    return 1;
  }
  // Configure OpenGL viewport
  // Remember OpenGL works with pixels, GLFW with screen coordinates
  int px_w, px_h;
  glfwGetFramebufferSize(window, &px_w, &px_h);
  glViewport(0, 0, px_w, px_h);

  chip8emul = new App();
  /*
  chip8emul.setOnColor(conf.oncolor);
  chip8emul.setOffColor(conf.offcolor);
  chip8emul.setFlipX(conf.flipx);
  chip8emul.setFlipY(conf.flipy);
   */
  try {
    chip8emul->LoadFromFile(conf.file);
  } catch (std::exception & e) {
    std::cerr << "Initialization error: " << e.what() << std::endl;
  }

  // Game loop
  double last_tick;
  while (!glfwWindowShouldClose(window)) {
    last_tick = glfwGetTime();

    glfwPollEvents();
    //ProcessInput()

    chip8emul->Update();

    glClearColor(0, 1, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    chip8emul->Render();

    glfwSwapBuffers(window);

    double now = glfwGetTime();
    chip8emul->emul.subDT(60 * (now - last_tick));
    chip8emul->emul.subSND(60* (now - last_tick));
    last_tick = now;
  }

  return 0;
}

void keyCallback(GLFWwindow *, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS)
    chip8emul->keys[key] = ++chip8emul->total_pressed;
  else
    chip8emul->keys[key] = 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

bool parseArgs(int argc, char * argv[]) {
  if (argc == 1) {
    std::cout <<
    "CHIP8 emulator @ hikarideai\n"
    "Usage:\n" << argv[0] << " <file> [options]\n\n"
    "Available options"
    "-offcolor x -- Set the color of an inactive pixel (Hex format)\n"
    "-oncolor x  -- Set the color of an active pixel (Hex format)\n"
    "-flipy      -- Flip the render texture along x-axis (i.e., let (0,0) coord"
    "inate\n"
    "               be at the bottom of the screen, rather than on the top)\n"
    "-flipx      -- flip the render texture along y-axis (i.e., let (0,0) coord"
    "inate\n"
    "               be at the right of the screen, rather than on the left)\n";
    return false;
  }

  conf.file = argv[1];

  return true;
}
