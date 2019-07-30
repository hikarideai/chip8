//
// Created by isabelle on 7/30/19 2:31 PM.
//

#include <cstring>
#include <chip8.hpp>

#include "chip8.hpp"

CHIP8::CHIP8() {
  init();
}

void CHIP8::init() {
  // Reset memory
  memset(mem, 0, MEM_CAPACITY);
  // Reset stack
  memset(stack, 0, STACK_DEPTH);
  stack_end = 0;
  // Reset registers
  memset(V, 0, sizeof V);
  I = 0;
  pc = START_POS;
  // Reset timers
  dt = 0;
  snd = 0;
}

int16_t CHIP8::getInstruction() {
  int16_t opcode = mem[pc] << 8;
  opcode |= mem[pc + 1];
  return opcode;
}
