#include "arch.h"

void arch_stestarch_disassemble(uint8_t* code, int code_length) {

}

const arch_info arch_stestarch = {
    .disassemble = arch_stestarch_disassemble
};