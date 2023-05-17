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

enum ir_instruction_collapsed_type {
    ir_instruction_collapsed_type_mov,
    ir_instruction_collapsed_type_add,
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

struct ir_instruction_collapsed {
    enum ir_instruction_collapsed_type type;
    union {
        struct ir_instruction_collapsed_data_mov {
            struct ir_instruction_collapsed_location output;
            struct ir_instruction_collapsed_location input;
        } mov;
        struct ir_instruction_collapsed_data_add {
            struct ir_instruction_collapsed_location inputa;
            struct ir_instruction_collapsed_location inputb;
            struct ir_instruction_collapsed_location output;
        } add;
    } data;
};

bool ir_instruction_compare_locations_collapsed(struct ir_instruction_collapsed_location location_a, struct ir_instruction_collapsed_location location_b);

struct ir_instruction_list* ir_instruction_add_instruction_collapsed(struct ir_instruction_list* list, uint64_t instruction_count, struct ir_instruction_collapsed instr);
struct ir_instruction_collapsed* ir_instruction_alloc_collapsed(struct ir_instruction_collapsed data);
