//
// Created by isabelle on 9/22/19 8:01 PM.
//

#include <random>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>


#include "App.hpp"
#include "ResourceManager.hpp"

#include "GLFW/glfw3.h"

App::App() {
  // Configure keyboard input
  total_pressed = 0;
  waiting_key_th = 0;
  keymap[0] = GLFW_KEY_KP_0;
  keymap[1] = GLFW_KEY_KP_1;
  keymap[2] = GLFW_KEY_KP_2;
  keymap[3] = GLFW_KEY_KP_3;
  keymap[4] = GLFW_KEY_KP_4;
  keymap[5] = GLFW_KEY_KP_5;
  keymap[6] = GLFW_KEY_KP_6;
  keymap[7] = GLFW_KEY_KP_7;
  keymap[8] = GLFW_KEY_KP_8;
  keymap[9] = GLFW_KEY_KP_9;
  keymap[10] = GLFW_KEY_KP_DECIMAL;
  keymap[11] = GLFW_KEY_KP_DIVIDE;
  keymap[12] = GLFW_KEY_KP_MULTIPLY;
  keymap[13] = GLFW_KEY_KP_SUBTRACT;
  keymap[14] = GLFW_KEY_KP_ADD;
  keymap[15] = GLFW_KEY_KP_ENTER;
  // Initialize random engine seed
  seed = (int)time(nullptr);
  // Configure error handling
  panic = false;
  msg = "";
  // Initialize render texture
  memset(pixels, 0, sizeof pixels);
  background.filter_max_ = GL_NEAREST;
  background.Sync();
  needs_refresh = true;
  // Load shaders
  ResourceManager::LoadShader("shaders/display.vs", "shaders/display.fs",
                              nullptr, "fill_screen_wtex");
  ResourceManager::GetShader("fill_screen_wtex").SetInteger("img", 0);
  // Prepare sprite
  float data[] = {
          // x   y  s  t
          -1, 1, 0, 0,
          1, 1, 1, 0,
          1, -1, 1, 1,
          -1, 1, 0, 0,
          -1, -1, 0, 1,
          1, -1, 1, 1
  };
  // Generate a buffer and upload the vertex data to it
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof data, data, GL_STATIC_DRAW);
  // Tell where to get the data for the shader's input
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *) (2 * sizeof(float)));
  glBindVertexArray(0);
  // Configure built-in font
  unsigned char font[] = { 0xF0, 0x90, 0x90, 0x90, 0xF0, 0x20, 0x60, 0x20, 0x20,
                           0x70, 0xF0, 0x10, 0xF0, 0x80, 0xF0, 0xF0, 0x10, 0xF0,
                           0x10, 0xF0, 0x90, 0x90, 0xF0, 0x10, 0x10, 0xF0, 0x80,
                           0xF0, 0x10, 0xF0, 0xF0, 0x80, 0xF0, 0x90, 0xF0, 0xF0,
                           0x10, 0x20, 0x40, 0x40, 0xF0, 0x90, 0xF0, 0x90, 0xF0,
                           0xF0, 0x90, 0xF0, 0x10, 0xF0, 0xF0, 0x90, 0xF0, 0x90,
                           0x90, 0xE0, 0x90, 0xE0, 0x90, 0xE0, 0xF0, 0x80, 0x80,
                           0x80, 0xF0, 0xE0, 0x90, 0x90, 0x90, 0xE0, 0xF0, 0x80,
                           0xF0, 0x80, 0xF0, 0xF0, 0x80, 0xF0, 0x80, 0x80 };
  memcpy(emul.mem, font, sizeof font);
}

void App::Render() {
  glBindVertexArray(vao);
  ResourceManager::GetShader("fill_screen_wtex").Use();
  background.Bind();
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void App::Update() {
  if (panic)
    return;

  for (int i = 0; i < CPS; ++i) {
    nextInstruction();
    if (panic) {
      std::cerr << "Panic at " << std::hex << emul.pc << ", instruction:"
                << emul.getInstruction() << ", description: " << msg << std::endl;
      break;
    }
  }

  if (needs_refresh) {
    background.Generate(width, height, pixels);
    needs_refresh = false;
  }
}

void App::nextInstruction() {
  if (emul.pc >= CHIP8::MEM_CAPACITY) {
    panic = true;
    msg = "PC points beyond available memory.";
    return;
  }

  uint16_t op = emul.getInstruction();

  int a = (op & 0x0F00) >> 2 * 4;
  int b = (op & 0x00F0) >> 1 * 4;

  // Random engine
  std::mt19937 rd(seed);
  std::uniform_int_distribution<unsigned char> dstr(0, 255);

  // Here goes nothing.
  switch ((op & 0xF000) >> 3 * 4) {
    case 0x0:
      switch (op & 0x00FF) {
        case 0xE0:
          // CLEAR
          memset(pixels, 0, 3U * width * height);
          needs_refresh = true;
          break;
        case 0xEE:
          // RET
          if (emul.stack_end == 0) {
            panic = true;
            msg = "return called with an empty stack.";
          } else {
            emul.pc = emul.stack[emul.stack_end-- - 1] - 2;
          }
          break;
        default:
          panic = true;
          msg = "unknown opcode.";
      }
      break;
    case 0x1:
      // GOTO
      emul.pc = (op & 0x0FFF) - 2;
      break;
    case 0x2:
      // CALL
      if (emul.stack_end == CHIP8::STACK_DEPTH) {
        panic = true;
        msg = "recursion depth limit exceeded.";
      } else {
        emul.stack[emul.stack_end++] = emul.pc;
        emul.pc = (op & 0x0FFF) - 2;
      }
      break;
    case 0x3:
      // EQ
      if (emul.V[a] == (op & 0x00FF))
        emul.pc += 2;
      break;
    case 0x4:
      // NEQ
      if (emul.V[a] == (op & 0x00FF))
        emul.pc += 2;
      break;
    case 0x5:
      // Register EQ
      if (op & 0x000F) {
        panic = true;
        msg = "unknown opcode.";
      } else {
        if (emul.V[a] == emul.V[b])
          emul.pc += 2;
      }
      break;
    case 0x6:
      // MOVI
      emul.V[a] = op & 0x00FF;
      break;
    case 0x7:
      // ADDI
      emul.V[a] += op & 0x00FF;
      break;
    case 0x8:
      switch (op & 0x000F) {
        case 0x0:
          // MOV
          emul.V[a] = emul.V[b];
          break;
        case 0x1:
          // OR
          emul.V[a] = emul.V[a] | emul.V[b];
          break;
        case 0x2:
          // AND
          emul.V[a] = emul.V[a] & emul.V[b];
          break;
        case 0x3:
          // XOR
          emul.V[a] = emul.V[a] ^ emul.V[b];
          break;
        case 0x4:
          // ADD
          emul.V[0xF] = (int16_t)emul.V[a] + emul.V[b] >= 0x100;
          emul.V[a] += emul.V[b];
          break;
        case 0x5:
          // SUB
          emul.V[0xF] = emul.V[a] >= emul.V[b];
          emul.V[a] -= emul.V[b];
          break;
        case 0x6:
          // Shift right
          emul.V[0xF] = emul.V[a] & 0x1; // The least significant bit
          emul.V[a] >>= 1;
          break;
        case 0x7:
          // Inverted SUB
          emul.V[0xF] = emul.V[b] >= emul.V[a];
          emul.V[a] = emul.V[b] - emul.V[a];
          break;
        case 0xE:
          // Shift left
          emul.V[0xF] = emul.V[a] & 0x80; // The most significant bit
          emul.V[a] <<= 1;
          break;
        default:
          panic = true;
          msg = "unknown opcode.";
      }
      break;
    case 0x9:
      // Register NEQ
      if (op & 0x000F) {
        panic = true;
        msg = "unknown opcode.";
      } else {
        if (emul.V[a] != emul.V[b])
          emul.pc += 2;
      }
      break;
    case 0xA:
      // LD
      emul.I = op & 0x0FFF;
      break;
    case 0xB:
      // GOTO with offset
      emul.pc = emul.V[0] + (op & 0x0FFF) - 2;
      break;
    case 0xC:
      // RAND
      emul.V[a] = dstr(rd) & (op & 0x00FF);
      break;
    case 0xD:
      // DRAW
      emul.V[0xF] = 0;
      for (int row = 0; row < (op & 0x000F); ++row) {
        if (emul.V[b] + row >= height)
          break;

        unsigned char bits = emul.mem[emul.I + row];
        for (int px = 0; px < 8; ++px) {
          if (emul.V[a] + px >= width)
            break;

          if (bits & (0x80 >> px)) {
            size_t idx = 3 * (emul.V[a] + px + (emul.V[b] + row) * width);
            // XOR pixels
            if (pixels[idx] == 0xFF) {
              for (int j = 0; j < 3; ++j)
                pixels[idx + j] = 0;
              emul.V[0xF] = 1;
            } else {
              for (int j = 0; j < 3; ++j)
                pixels[idx + j] = 0xFF;
            }
            needs_refresh = true;
          }
        }
      }
      break;
    case 0xE:
      // KEY STUFF
      switch (op & 0x00FF) {
        case 0x9E:
          // KEY PRESSED
          if (keys[keymap[emul.V[a]]] > 0)
            emul.pc += 2;
          break;
        case 0xA1:
          // KEY RELEASED
          if (keys[keymap[emul.V[a]]] == 0)
            emul.pc += 2;
          break;
        default:
          panic = true;
          msg = "unknown opcode.";
      }
      break;
    case 0xF:
      switch (op & 0x00FF) {
        case 0x07:
          // DTGET
          emul.V[a] = emul.dt;
          break;
        case 0x0A:
          // WAITKEY
          // If it's the first encounter, state that we want the next pressed
          // key.
          if (!waiting_key_th) {
            waiting_key_th = total_pressed + 1;
            std::cout << "Please, press a key... ";
            std::cout.flush();
          }
          // Then check weather it was pressed
          for (int i = 0; i < keymap_size; ++i) {
            if (keys[keymap[i]] >= waiting_key_th) {
              waiting_key_th = 0; // Satisfied
              emul.V[a] = i;
              break;
            }
          }
          // If the request wasn't satisfied, don't move pc
          if (waiting_key_th) {
            emul.pc -= 2;
          } else {
            std::cout << std::hex << (int)emul.V[a] << std::endl;
          }
          break;
        case 0x15:
          // DTSET
          emul.dt = emul.V[a];
          break;
        case 0x18:
          // SNDSET
          emul.snd = emul.V[a];
          break;
        case 0x1E:
          // IADD
          emul.I += emul.V[a];
          break;
        case 0x29:
          // GLYPH
          emul.I = emul.V[a] * 5;
          break;
        case 0x33:
          // BCD (Binary-Coded Decimal)
          if (emul.I + 2 >= CHIP8::MEM_CAPACITY) {
            panic = true;
            msg = "out of range memory access.";
          } else {
            emul.mem[emul.I] = emul.V[a] / 100;
            emul.mem[emul.I + 1] = emul.V[a] / 10 % 10;
            emul.mem[emul.I + 2] = emul.V[a] % 10;
          }
          break;
        case 0x55:
          // Register Dump
          if (emul.I + a >= CHIP8::MEM_CAPACITY) {
            panic = true;
            msg = "out of range memory access.";
          } else {
            for (int i = 0; i <= a; ++i)
              emul.mem[emul.I + i] = emul.V[i];
          }
          break;
        case 0x65:
          // Register Load (undump)
          if (emul.I + a >= CHIP8::MEM_CAPACITY) {
            panic = true;
            msg = "out of range memory access.";
          } else {
            for (int i = 0; i <= a; ++i)
              emul.V[i] = emul.mem[emul.I + i];
          }
          break;
        default:
          panic = true;
          msg = "unknown opcode.";
      }
      break;
    default:
      panic = true;
      msg = "unknown opcode.";
  }

  emul.pc += 2;
}

void App::LoadFromFile(const char *filename) {
  std::ifstream fs(filename);
  if (!fs.is_open())
    throw std::runtime_error(std::string("Failed to open ") + filename);

  int i = CHIP8::START_POS;
  while (!fs.eof() && i < CHIP8::MEM_CAPACITY) {
    emul.mem[i++] = fs.get();
  }

  if (!fs.eof())
    throw std::runtime_error("Provided CHIP8 binary exceeds the available "
                             "memory's size.");

  fs.close();
}
