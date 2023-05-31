#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "IR_type.h"

enum ir_instruction_low_special_registers {
    ir_instruction_low_special_registers_stack = -1,
    ir_instruction_low_special_registers_temp_start = -2,
    ir_instruction_low_special_registers_mappings_start = -1000000,
};

enum ir_instruction_low_location_type {
    ir_instruction_low_location_type_immediate,
    ir_instruction_low_location_type_register,
    ir_instruction_low_location_type_register_address,
    ir_instruction_low_location_type_address,
};

struct ir_instruction_low_location {
    ir_type_t type;
    enum ir_instruction_low_location_type location_type;
    union {
        uint64_t imm;
        uint64_t reg;
        uint64_t addr;
    } data;
};

struct ir_instruction_low_location_with_offset {
    struct ir_instruction_low_location loc;
    struct ir_instruction_low_location offset;
};

#include "generated/instructions_ir_level_low_include.h"

bool ir_instruction_compare_locations_low(struct ir_instruction_low_location location_a, struct ir_instruction_low_location location_b);
bool ir_instruction_compare_locations_with_offset_low(struct ir_instruction_low_location_with_offset location_a, struct ir_instruction_low_location_with_offset location_b);

struct ir_instruction_list* ir_instruction_add_instruction_low(struct ir_instruction_list* list, uint64_t instruction_count, struct ir_instruction_low instr);
struct ir_instruction_low* ir_get_instruction_low(struct ir_instruction_list* instructions, uint64_t instruction);