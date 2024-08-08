#include "../include/emulator.hpp"
#include "../../useful-programs/Random.h"
#include <cstdio>
#include <cstring>

typedef std::uint8_t BYTE;
typedef std::uint16_t WORD;

void Emulator::CPUReset(char * file) {
  m_addressI = 0;
  m_programCounter = 0x200;
  memset(m_register, 0, sizeof(m_register));

  // load in the game
  FILE *in;
  in = fopen(file, "rb");
  fread(&m_gameMemory[0x200], 0xfff, 1, in);
  fclose(in);
}

Emulator::Emulator(char * file) { CPUReset(file); }

WORD Emulator::getNextOpcode() {
  WORD res = 0;
  res = m_gameMemory[m_programCounter];
  res <<= 8;
  res |= m_gameMemory[m_programCounter + 1];
  m_programCounter += 2;
  return res;
}

void Emulator::decodeOpcode() {
  WORD opcode = getNextOpcode();

  switch (opcode & 0xF000) {
  case 0x0000:
    switch (opcode & 0x000F) {
    case 0x0000:
      opcode00E0(opcode);
      break;
    case 0x000E:
      opcode00EE(opcode);
      break;
    }
  case 0x1000:
    opcode1NNN(opcode);
    break;
  case 0x2000:
    opcode2NNN(opcode);
    break;
  case 0x3000:
    opcode3XNN(opcode);
    break;
  case 0x4000:
    opcode4XNN(opcode);
    break;
  case 0x5000:
    opcode5XY0(opcode);
    break;
  case 0x6000:
    opcode6XNN(opcode);
    break;
  case 0x7000:
    opcode7XNN(opcode);
    break;
  case 0x8000:
    switch (opcode & 0x000F) {
    case 0x0000:
      opcode8XY0(opcode);
      break;
    case 0x0001:
      opcode8XY1(opcode);
      break;
    case 0x0002:
      opcode8XY2(opcode);
      break;
    case 0x0003:
      opcode8XY3(opcode);
      break;
    case 0x0004:
      opcode8XY4(opcode);
      break;
    case 0x0005:
      opcode8XY5(opcode);
      break;
    case 0x0006:
      opcode8XY6(opcode);
      break;
    case 0x0007:
      opcode8XY7(opcode);
      break;
    case 0x000E:
      opcode8XYE(opcode);
      break;
    }
  case 0x9000:
    opcode9XY0(opcode);
    break;
  case 0xA000:
    opcodeANNN(opcode);
    break;
  case 0xB000:
    opcodeBNNN(opcode);
    break;
  case 0xC000:
    opcodeCXNN(opcode);
    break;
  case 0xD000:
    opcodeDXYN(opcode);
    break;
  case 0xE000:
    switch (opcode & 0x000F) {
    case 0x000E:
      opcodeEX9E(opcode);
      break;
    case 0x0001:
      opcodeEXA1(opcode);
      break;
    }
  case 0xF000:
    switch (opcode & 0x00FF) {
    case 0x0015:
      opcodeFX15(opcode);
      break;
    case 0x0018:
      opcodeFX18(opcode);
      break;
    case 0x001E:
      opcodeFX1E(opcode);
      break;
    case 0x0029:
      opcodeFX29(opcode);
      break;
    case 0x0033:
      opcodeFX33(opcode);
      break;
    case 0x0055:
      opcodeFX55(opcode);
      break;
    case 0x0065:
      opcodeFX65(opcode);
    }

  default:
    break;
  }
}

// clears the screen
void Emulator::opcode00E0(WORD opcode) {

}

// returns from a subroutine
void Emulator::opcode00EE(WORD opcode) {
  m_programCounter = m_stack.back();
  m_stack.pop_back();
}

// jumps to address NNN
void Emulator::opcode1NNN(WORD opcode) {
  m_programCounter = opcode & 0x0FFF;
}

// calls a subroutine at NNN
void Emulator::opcode2NNN(WORD opcode) {
  m_stack.push_back(m_programCounter);
  m_programCounter = opcode & 0x0FFF;
}

// Skips the next instruction if VX equals NN
// (usually the next instruction is a jump to skip a code block).
void Emulator::opcode3XNN(WORD opcode) {
  int regx = opcode & 0x0F00;
  regx >>= 8;
  int nn = opcode & 0x00FF;
  if (m_register[regx] == nn) {
    m_programCounter += 2;
  }
}

// Skips the next instruction if VX does not equal NN
// (usually the next instruction is a jump to skip a code block)
void Emulator::opcode4XNN(WORD opcode) {
  int regx = opcode & 0x0F00;
  regx >>= 8;
  int nn = opcode & 0x00FF;
  if (m_register[regx] != nn) {
    m_programCounter += 2;
  }
}

// Skips the next instruction if VX equals VY
// (usually the next instruction is a jump to skip a code block)
void Emulator::opcode5XY0(WORD opcode) {
  int regx = opcode & 0x0F00;
  regx >>= 8;
  int regy = opcode & 0x00F0;
  regy >>= 4;
  if (m_register[regx] == m_register[regy]) {
    m_programCounter += 2;
  }
}

// Vx == NN
void Emulator::opcode6XNN(WORD opcode) {
  int regx = opcode & 0x0F00;
  regx >>= 8;
  int nn = opcode & 0x00FF;
  m_register[regx] = nn;
}

// Vx += NN
void Emulator::opcode7XNN(WORD opcode) {
  int regx = opcode & 0x0F00;
  regx >>= 8;
  int nn = opcode & 0x00FF;
  m_register[regx] += nn;
}

// Vx = Vy
void Emulator::opcode8XY0(WORD opcode) {
  int regx = opcode & 0x0F00;
  regx >>= 8;
  int regy = opcode & 0x00F0;
  regx >>= 4;
  m_register[regx] = m_register[regy];
}

// Vx |= Vy
void Emulator::opcode8XY1(WORD opcode) {
  int regx = opcode & 0x0F00;
  regx >>= 8;
  int regy = opcode & 0x00F0;
  regx >>= 4;
  m_register[regx] |= m_register[regy];
}

// Vx &= Vy
void Emulator::opcode8XY2(WORD opcode) {
  int regx = opcode & 0x0F00;
  regx >>= 8;
  int regy = opcode & 0x00F0;
  regx >>= 4;
  m_register[regx] &= m_register[regy];
}

// Vx ^= Vy
void Emulator::opcode8XY3(WORD opcode) {
  int regx = opcode & 0x0F00;
  regx >>= 8;
  int regy = opcode & 0x00F0;
  regx >>= 4;
  m_register[regx] ^= m_register[regy];
}

// Vx += Vy (sets overflow)
void Emulator::opcode8XY4(WORD opcode) {
  m_register[0xF] = 1; // signifies wrapping did not occur
  int regx = opcode & 0x0F00;
  regx >>= 8;
  int regy = opcode & 0x00F0;
  regy >>= 4;
  int xval = m_register[regx];
  int yval = m_register[regy];
  if (xval + yval > 255) {
    m_register[0xF] = 0; // signifies wrapping did occur
  }
  m_register[regx] = xval + yval;
}

// Vx -= Vy (sets overflow)
void Emulator::opcode8XY5(WORD opcode) {
  m_register[0xF] = 1; // signifies wrapping did not occur
  int regx = opcode & 0x0F00;
  regx >>= 8;
  int regy = opcode & 0x00F0;
  regy >>= 4;
  int xval = m_register[regx];
  int yval = m_register[regy];
  if (yval > xval) {
    m_register[0xF] = 0; // signifies wrapping did occur
  }
  m_register[regx] = xval - yval;
}

// Vx >>= 1 (least significant bit to overflow)
void Emulator::opcode8XY6(WORD opcode) {
  int regx = opcode & 0x0F00;
  regx >>= 8;
  m_register[0xF] = m_register[regx] & 0x1;
  m_register[regx] >>= 1;
}

// Vx = Vy - Vx (sets overflow)
void Emulator::opcode8XY7(WORD opcode) {
  m_register[0xF] = 1; // signifies wrapping did not occur
  int regx = opcode & 0x0F00;
  regx >>= 8;
  int regy = opcode & 0x00F0;
  regy >>= 4;
  int xval = m_register[regx];
  int yval = m_register[regy];
  if (yval < xval) {
    m_register[0xF] = 0; // signifies wrapping did occur
  }
  m_register[regx] = yval - xval;
}

// Vx <<= 1 (most significant bit to overflow)
void Emulator::opcode8XYE(WORD opcode) {
  int regx = opcode & 0x0F00;
  regx >>= 8;
  m_register[0xF] = m_register[regx] & 0x80;
  m_register[regx] <<= 1;
}

// Skips the next instruction if VX does not equal VY.
// (Usually the next instruction is a jump to skip a code block)
void Emulator::opcode9XY0(WORD opcode) {
  int regx = opcode & 0x0F00;
  regx >>= 8;
  int regy = opcode & 0x00F0;
  regy >>= 4;
  if (m_register[regx] != m_register[regy]) {
    m_programCounter += 2;
  }
}

// I = NNN
void Emulator::opcodeANNN(WORD opcode) {
  int nnn = opcode & 0x0FFF;
  m_addressI = nnn;
}

// Jumps to the address NNN plus V0
void Emulator::opcodeBNNN(WORD opcode) {
  int nnn = opcode & 0x0FFF;
  m_programCounter = nnn + m_register[0x0];
}

// Sets VX to the result of a bitwise and operation on a random number
// (Typically: 0 to 255) and NN.
void Emulator::opcodeCXNN(WORD opcode) {
  int regx = opcode & 0x0F00;
  regx >>= 8;
  int nn = opcode & 0x00FF;
  m_register[regx] = Random::get(0, 255) & nn;
}

// Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a
// height of N pixels. Each row of 8 pixels is read as bit-coded starting from
// memory location I; I value does not change after the execution of this
// instruction. As described above, VF is set to 1 if any screen pixels are
// flipped from set to unset when the sprite is drawn, and to 0 if that does not
// happen
void Emulator::opcodeDXYN(WORD opcode) {
  int regx = opcode & 0x0F00;
  regx >>= 8;
  int regy = opcode & 0x00F0;
  regy >>= 4;

  int height = opcode & 0x000F;
  int coordx = m_register[regx];
  int coordy = m_register[regy];

  m_register[0xF] = 0;

  for (int yline{0}; yline < height; yline++) {
    BYTE data = m_gameMemory[m_addressI + yline];
    int xpixelinv = 7;
    int xpixel = 0;
    for (; xpixel < 8; xpixel++, xpixelinv--) {
      int mask = 1 << xpixelinv;
      if (data & mask) {
        int x = coordx + xpixel;
        int y = coordy + yline;
        if (m_screenData[x][y] == 1) {
          m_register[0xF] = 1; // collision
        }
        m_screenData[x][y] ^= 1;
      }
    }
  }
}

/*// Skips the next instruction if the key stored in VX is pressed (usually the*/
/*// next instruction is a jump to skip a code block)*/
/*void Emulator::opcodeEX9E(WORD opcode);*/
/**/
/*// Skips the next instruction if the key stored in VX is not pressed (usually*/
/*// the next instruction is a jump to skip a code block)*/
/*void Emulator::opcodeEXA1(WORD opcode);*/
/**/
/*// Sets VX to the value of the delay timer*/
/*void Emulator::opcodeFX07(WORD opcode);*/
/**/
/*// A key press is awaited, and then stored in VX (blocking operation, all*/
/*// instruction halted until next key event)*/
/*void Emulator::opcodeFX0A(WORD opcode);*/
/**/
/*// Sets the delay timer to VX*/
/*void Emulator::opcodeFX15(WORD opcode);*/
/**/
/*// Sets the sound timer to VX*/
/*void Emulator::opcodeFX18(WORD opcode);*/
/**/
/*// I += Vx*/
/*void Emulator::opcodeFX1E(WORD opcode);*/
/**/
/*// Sets I to the location of the sprite for the character in VX. Characters 0-F*/
/*// (in hexadecimal) are represented by a 4x5 font*/
/*void Emulator::opcodeFX29(WORD opcode);*/

// Stores the binary-coded decimal representation of VX, with the hundreds digit
// in memory at location in I, the tens digit at location I+1, and the ones
// digit at location I+2
void Emulator::opcodeFX33(WORD opcode) {
  int regx = opcode & 0x0F00;
  regx >>= 8;

  int value = m_register[regx];

  int hundreds = value / 100;
  int tens = (value / 10) % 10;
  int units = value % 10;

  m_gameMemory[m_addressI] = hundreds;
  m_gameMemory[m_addressI + 1] = tens;
  m_gameMemory[m_addressI + 2] = units;
}

// Stores from V0 to VX (including VX) in memory, starting at address I. The
// offset from I is increased by 1 for each value written, but I itself is left
// unmodified
void Emulator::opcodeFX55(WORD opcode) {
  int regx = opcode & 0x0F00;
  regx >>= 8;
  for (int i{0}; i <= regx; i++) {
    m_gameMemory[m_addressI + i] = m_register[i];
  }
  m_addressI = m_addressI + regx + 1;
}

// Fills from V0 to VX (including VX) with values from memory, starting at
// address I. The offset from I is increased by 1 for each value read, but I
// itself is left unmodified
void Emulator::opcodeFX65(WORD opcode) {
  int regx = opcode & 0x0F00;
  regx >>= 8;
  for (int i{0}; i <= regx; i++) {
    m_register[m_addressI + i] = m_gameMemory[i];
  }
  m_addressI = m_addressI + regx + 1;
}
