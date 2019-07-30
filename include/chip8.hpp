//
// Created by isabelle on 7/29/19 9:39 AM.
//

#ifndef CHIP8EMUL_CHIP8_HPP
#define CHIP8EMUL_CHIP8_HPP


#include <cstddef>
#include <cstdint>

struct CHIP8 {
  static const size_t MEM_CAPACITY = 0x1000;
  static const size_t START_POS = 0x200;
  static const size_t STACK_DEPTH = 24;

  unsigned char mem[MEM_CAPACITY];
  unsigned char V[0x10];
  int16_t I;
  int16_t stack[STACK_DEPTH];
  size_t stack_end;
  unsigned char dt;
  unsigned char snd;
  int16_t pc;

  CHIP8();
  void init();
  int16_t getInstruction();
};

#endif //CHIP8EMUL_CHIP8_HPP
