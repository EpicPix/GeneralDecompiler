#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "IR_type.h"

enum ir_instruction_collapsed_location_type {
    ir_instruction_collapsed_location_type_immediate,
    ir_instruction_collapsed_location_type_register,
    ir_instruction_collapsed_location_type_register_address,
    ir_instruction_collapsed_location_type_address,
    ir_instruction_collapsed_location_type_inlined,
    ir_instruction_collapsed_location_type_inherit,
};

struct ir_instruction_collapsed_location {
    ir_type_t type;
    enum ir_instruction_collapsed_location_type location_type;
    union {
        uint64_t imm;
        uint64_t reg;
        uint64_t addr;
        struct ir_instruction_collapsed* instr;
    } data;
};

#include "generated/instructions_ir_level_collapsed_include.h"

bool ir_instruction_compare_locations_collapsed(struct ir_instruction_collapsed_location location_a, struct ir_instruction_collapsed_location location_b);

struct ir_instruction_list* ir_instruction_add_instruction_collapsed(struct ir_instruction_list* list, uint64_t instruction_count, struct ir_instruction_collapsed instr);
struct ir_instruction_collapsed* ir_instruction_alloc_collapsed(struct ir_instruction_collapsed data);
