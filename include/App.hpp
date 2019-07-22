//
// Created by isabelle on 7/22/19 7:46 PM.
//

#ifndef APP_HPP
#define APP_HPP

#include "Texture.hpp"

class App {
public:
  // World dimensions
  int width, height;
  bool keys[1024];
  unsigned vao, vbo;
  Texture2D background;

  // Loads resources and sets up the stage
  App(int width, int height);
  // App Loop
  void Update();
  void Render();
};


#endif //BREAKOUT_APP_HPP
