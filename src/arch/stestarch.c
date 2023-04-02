#include "arch.h"
#include <stdio.h>
#include <stdint.h>

#define PREFIX_BIT 0b00100000

#define IS_PREFIX_MEMORY(n) (((n)>>4) == 0b0010)
#define IS_PREFIX_JUMP(n) (((n)>>4) == 0b0011)

#define INSTR_ADD 0b00001
#define INSTR_SUB 0b00010
#define INSTR_MUL 0b00011
#define INSTR_PUSH 0b00110
#define INSTR_DROP 0b00111
#define INSTR_DUP 0b01000
#define INSTR_READPTR 0b01001
#define INSTR_WRITEPTR 0b01010
#define INSTR_AND 0b01011
#define INSTR_NOT 0b01100
#define INSTR_OR 0b01101
#define INSTR_XOR 0b01110
#define INSTR_TOB 0b01111
#define INSTR_JMP 0b10000
#define INSTR_JE 0b10001
#define INSTR_JNE 0b10010
#define INSTR_JLE 0b10011
#define INSTR_JLT 0b10100
#define INSTR_JBE 0b10101
#define INSTR_JBT 0b10110
#define INSTR_TOW 0b10111
#define INSTR_TOD 0b11000
#define INSTR_TOQ 0b11001
#define INSTR_SHL 0b11010
#define INSTR_SHR 0b11011
#define INSTR_NEG 0b11100

static inline int get_bit_size(int e) {
  switch(e) {
    case 0: return 8;
    case 1: return 16;
    case 2: return 32;
    case 4: return 64;
  }
  return 0;
}

static inline uint64_t read_sized_unsigned(uint8_t* code, int* x, int size) {
  uint64_t v = 0;
  int i = *x;
  if(size == 1) v = code[i+1];
  else if(size == 2) v = code[i+1] | (code[i+2] << 8);
  else if(size == 4) v = code[i+1] | (code[i+2] << 8) | (code[i+3] << 16) | (code[i+4] << 24);
  else if(size == 8) v = code[i+1] | (code[i+2] << 8) | (code[i+3] << 16) | (code[i+4] << 24) | ((uint64_t)code[i+5] << 32) | ((uint64_t)code[i+6] << 40) | ((uint64_t)code[i+7] << 48) | ((uint64_t)code[i+8] << 54);
  *x += size;
  return v;
}

static inline int64_t read_sized_signed(uint8_t* code, int* x, int size) {
  int64_t v = 0;
  int i = *x;
  if(size == 1) v = (int64_t) (char) code[i+1];
  else if(size == 2) v = (int64_t) (int16_t) (code[i+1] | (code[i+2] << 8));
  else if(size == 4) v = (int64_t) (int32_t) (code[i+1] | (code[i+2] << 8) | (code[i+3] << 16) | (code[i+4] << 24));
  else if(size == 8) v = (int64_t) (int64_t) (code[i+1] | (code[i+2] << 8) | (code[i+3] << 16) | (code[i+4] << 24) | ((uint64_t)code[i+5] << 32) | ((uint64_t)code[i+6] << 40) | ((uint64_t)code[i+7] << 48) | ((uint64_t)code[i+8] << 54));
  *x += size;
  return v;
}

void arch_stestarch_disassemble(uint8_t* code, int code_length) {
  int jmp_size = 0;
  for(int i = 0; i<code_length; i++) {
    uint8_t opcode = code[i];
    if(opcode & PREFIX_BIT) {
      if(IS_PREFIX_MEMORY(opcode)) {
        printf("memory[pointer=%d, data=%d] ", get_bit_size((opcode >> 2) & 0x3), get_bit_size(opcode & 0x3));
      }else if(IS_PREFIX_JUMP(opcode)) {
        jmp_size = get_bit_size((opcode >> 2) & 0x3);
        printf("jump[offloc=%d, %s] ", jmp_size, opcode & 1 ? "location" : "offset");
      }else {
        printf("invalid prefix\n");
        break;
      }
    }else {
      uint8_t size = get_bit_size(opcode >> 6) >> 3;
      uint8_t instr = opcode & 0b00011111;

      switch(instr) {
        case INSTR_ADD:
          printf("add\n");
          break;
        case INSTR_SUB:
          printf("sub\n");
          break;
        case INSTR_MUL:
          printf("mul\n");
          break;
        case INSTR_PUSH:
          printf("push %lu\n", read_sized_unsigned(code, &i, size));
          break;
        case INSTR_DROP:
          printf("drop\n");
          break;
        case INSTR_DUP:
          printf("dup\n");
          break;
        case INSTR_READPTR:
          printf("readptr\n");
          break;
        case INSTR_WRITEPTR:
          printf("writeptr\n");
          break;
        case INSTR_AND:
          printf("and\n");
          break;
        case INSTR_NOT:
          printf("not\n");
          break;
        case INSTR_OR:
          printf("or\n");
          break;
        case INSTR_XOR:
          printf("xor\n");
          break;
        case INSTR_TOB:
          printf("tob\n");
          break;
        case INSTR_JMP:
          printf("jmp %ld\n", read_sized_signed(code, &i, jmp_size >> 3));
          break;
        case INSTR_JE:
          printf("je %ld\n", read_sized_signed(code, &i, jmp_size >> 3));
          break;
        case INSTR_JNE:
          printf("jne %ld\n", read_sized_signed(code, &i, jmp_size >> 3));
          break;
        case INSTR_JLE:
          printf("jle %ld\n", read_sized_signed(code, &i, jmp_size >> 3));
          break;
        case INSTR_JLT:
          printf("jlt %ld\n", read_sized_signed(code, &i, jmp_size >> 3));
          break;
        case INSTR_JBE:
          printf("jbe %ld\n", read_sized_signed(code, &i, jmp_size >> 3));
          break;
        case INSTR_JBT:
          printf("jbt %ld\n", read_sized_signed(code, &i, jmp_size >> 3));
          break;
        case INSTR_TOW:
          printf("tow\n");
          break;
        case INSTR_TOD:
          printf("tod\n");
          break;
        case INSTR_TOQ:
          printf("toq\n");
          break;
        case INSTR_SHL:
          printf("shl\n");
          break;
        case INSTR_SHR:
          printf("shr\n");
          break;
        case INSTR_NEG:
          printf("neg\n");
          break;
        default:
          printf("Invalid instruction\n");
      }
    }
  }
}

const arch_info arch_stestarch = {
    .disassemble = arch_stestarch_disassemble
};