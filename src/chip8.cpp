//
// Created by isabelle on 7/30/19 2:31 PM.
//

#include <cstring>
#include <chip8.hpp>
#include <cmath>

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
  dt = snd = 0;
  ddt = dsnd = 0;
}

int16_t CHIP8::getInstruction() {
  int16_t opcode = mem[pc] << 8;
  opcode |= mem[pc + 1];
  return opcode;
}

void CHIP8::setDT(unsigned char v) {
  dt = v;
  ddt = 0.0;
  return;
  ddt -= std::floor(ddt);
  if (ddt < 0.0)
    ddt = 0.0;
}

void CHIP8::setSND(unsigned char v) {
  snd = v;
  dsnd = 0.0;
  return;
  dsnd -= std::floor(dsnd);
  if (dsnd < 0.0)
    dsnd = 0.0;
}

unsigned char CHIP8::getDT() {
  return dt ? dt - std::floor(ddt) : 0;
}

unsigned char CHIP8::getSND() {
  return snd ? snd - std::floor(dsnd) : 0;
}

void CHIP8::subDT(double t) {
  ddt += t;
  if (dt - std::floor(ddt) <= 0.0) {
    dt = 0;
    ddt = 0.0;
  }
}

void CHIP8::subSND(double t) {
  dsnd += t;
  if (snd - std::floor(dsnd) <= 0.0) {
    snd = 0;
    dsnd = 0.0;
  }
}
