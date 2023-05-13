#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "IR_type.h"

enum ir_instruction_low_location_type {
    ir_instruction_low_location_type_immediate,
    ir_instruction_low_location_type_register,
    ir_instruction_low_location_type_address,
};

enum ir_instruction_low_type {
    ir_instruction_low_type_mov,
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

struct ir_instruction_low {
    enum ir_instruction_low_type type;
    union {
        struct ir_instruction_low_data_oii {
            struct ir_instruction_low_location output;
            struct ir_instruction_low_location inputa;
            struct ir_instruction_low_location inputb;
        } oii;
        struct ir_instruction_low_data_oi {
            struct ir_instruction_low_location output;
            struct ir_instruction_low_location input;
        } oi;
    } data;
};

struct ir_instruction_list* ir_instruction_add_instruction_low(struct ir_instruction_list* list, uint64_t instruction_count, struct ir_instruction_low instr);