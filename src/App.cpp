//
// Created by isabelle on 7/22/19 8:01 PM.
//

#include <random>
#include <thread>
#include <cstdlib>

#include "App.hpp"
#include "ResourceManager.hpp"

App::App(int width, int height) {
  // Set up basic properties
  this->width = width;
  this->height = height;
  // Load textures
  ResourceManager::LoadTexture("textures/sakura.jpg", GL_FALSE,
                               "dummy_background");
  // Load shaders
  ResourceManager::LoadShader("shaders/display.vs", "shaders/display.fs",
                              nullptr, "fill_screen_wtex");
  ResourceManager::GetShader("fill_screen_wtex").SetInteger("img", 0);
  // Prepare sprite
  float data[] = {
          -1,  1, 0, 0,
           1,  1, 1, 0,
           1, -1, 1, 1,
          -1,  1, 0, 0,
          -1, -1, 0, 1,
           1, -1, 1, 1
  };

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof data, data, GL_STATIC_DRAW);
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glBindVertexArray(0);
}

void App::Render() {
  glBindVertexArray(vao);
  ResourceManager::GetShader("fill_screen_wtex").Use();
  background.Bind();
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

std::mt19937 rd(time(0));
std::uniform_int_distribution<unsigned char> dist(0, 255);

void fill_static(unsigned char * data, int start, int count) {
  for (int i = 0; i < count; ++i) {
    auto grayish = dist(rd);
    for (int j = 0; j < 3; ++j)
      data[3 * (start + i) + j] = dist(rd);//grayish++;
  }
}

void App::Update() {
  auto *pixels = (unsigned  char *)malloc(3 * width * height * 1U);
  std::vector<std::thread> threads;

  const int block = 1000;
  for (int i = 0; i < width * height / block; ++i) {
    threads.emplace_back(fill_static, pixels, i * block, block);
  }

  for (auto & th : threads)
    th.join();

  background.Generate(width, height, pixels);
}
