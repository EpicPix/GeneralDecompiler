#pragma once

#include <stdint.h>
#include <stdbool.h>

struct ir_instruction_low {

};

struct ir_instruction_list* ir_instruction_add_instruction_low(struct ir_instruction_list* list, uint64_t instruction_count, struct ir_instruction_low instr);