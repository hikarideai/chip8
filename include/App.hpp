//
// Created by isabelle on 7/22/19 7:46 PM.
//

#ifndef APP_HPP
#define APP_HPP

#include <iostream>

#include "Texture.hpp"
#include "chip8.hpp"

class App {
public:
  // Keyboard state
  static const int keymap_size = 16;
  int keys[1024];
  int keymap[keymap_size];
  int total_pressed;
  int waiting_key_th;
  // Render texture
  static const int width = 64;
  static const int height = 32;
  unsigned vao, vbo;
  Texture2D background;
  unsigned char pixels[width * height * 3];
  // Random engine
  int seed;
  // Emulated hardware
  static const int CPS = 400;
  CHIP8 emul;
  bool panic;
  std::string msg;

  // Loads resources and sets up the stage
  App();
  // App Loop
  void Update();
  void Render();
  // State control
  void LoadFromFile(const char * filename);

private:
  bool needs_refresh;
  void nextInstruction();
};


#endif //BREAKOUT_APP_HPP
