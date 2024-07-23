#ifndef EMULATOR_H
#define EMULATOR_H

#include <cstdint>
#include <vector>

typedef std::uint8_t BYTE;
typedef std::uint16_t WORD;

class Emulator {
public:
  Emulator();
  void CPUReset();
  WORD getNextOpcode();
  void decodeOpcode();
  void opcode00E0(WORD opcode);
  void opcode00EE(WORD opcode);
  void opcode1NNN(WORD opcode);
  void opcode2NNN(WORD opcode);
  void opcode3XNN(WORD opcode);
  void opcode4XNN(WORD opcode);
  void opcode5XY0(WORD opcode);
  void opcode6XNN(WORD opcode);
  void opcode7XNN(WORD opcode);
  void opcode8XY0(WORD opcode);
  void opcode8XY1(WORD opcode);
  void opcode8XY2(WORD opcode);
  void opcode8XY3(WORD opcode);
  void opcode8XY4(WORD opcode);
  void opcode8XY5(WORD opcode);
  void opcode8XY6(WORD opcode);
  void opcode8XY7(WORD opcode);
  void opcode8XYE(WORD opcode);
  void opcode9XY0(WORD opcode);
  void opcodeANNN(WORD opcode);
  void opcodeBNNN(WORD opcode);
  void opcodeCXNN(WORD opcode);
  void opcodeDXYN(WORD opcode);
  void opcodeEX9E(WORD opcode);
  void opcodeEXA1(WORD opcode);
  void opcodeFX07(WORD opcode);
  void opcodeFX0A(WORD opcode);
  void opcodeFX15(WORD opcode);
  void opcodeFX18(WORD opcode);
  void opcodeFX1E(WORD opcode);
  void opcodeFX29(WORD opcode);
  void opcodeFX33(WORD opcode);
  void opcodeFX55(WORD opcode);
  void opcodeFX65(WORD opcode);

private:
  BYTE m_gameMemory[0xFFF];  // 0xFFF bytes of memory
  BYTE m_register[16];      // 16 registers, 1 byte each
  WORD m_addressI;           // the 16-bit address register I
  WORD m_programCounter;     // the 16-bit program counter
  std::vector<WORD> m_stack; // the 16-bit stack

  BYTE m_screenData[64][32];
};

#endif // !EMULATOR_H
