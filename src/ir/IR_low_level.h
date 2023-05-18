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

enum ir_instruction_low_type {
    ir_instruction_low_type_mov,
    ir_instruction_low_type_mov_offsetinout,
    ir_instruction_low_type_mov_offsetout,
    ir_instruction_low_type_mov_offsetin,
    ir_instruction_low_type_add,
    ir_instruction_low_type_norelso,
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

struct ir_instruction_low {
    enum ir_instruction_low_type type;
    union {
        struct ir_instruction_low_data_mov {
            struct ir_instruction_low_location output;
            struct ir_instruction_low_location input;
        } mov;
        struct ir_instruction_low_data_mov_offsetout {
            struct ir_instruction_low_location_with_offset output;
            struct ir_instruction_low_location input;
        } movoout;
        struct ir_instruction_low_data_mov_offsetin {
            struct ir_instruction_low_location output;
            struct ir_instruction_low_location_with_offset input;
        } movoin;
        struct ir_instruction_low_data_mov_offsetinout {
            struct ir_instruction_low_location_with_offset output;
            struct ir_instruction_low_location_with_offset input;
        } movoinout;
        struct ir_instruction_low_data_add {
            struct ir_instruction_low_location inputa;
            struct ir_instruction_low_location inputb;
            struct ir_instruction_low_location output;
        } add;
        struct ir_instruction_low_data_norelso {
            struct ir_instruction_low_location_with_offset loc;
        } norel;
    } data;
};

bool ir_instruction_compare_locations_low(struct ir_instruction_low_location location_a, struct ir_instruction_low_location location_b);
bool ir_instruction_compare_locations_with_offset_low(struct ir_instruction_low_location_with_offset location_a, struct ir_instruction_low_location_with_offset location_b);

struct ir_instruction_list* ir_instruction_add_instruction_low(struct ir_instruction_list* list, uint64_t instruction_count, struct ir_instruction_low instr);
struct ir_instruction_low* ir_get_instruction_low(struct ir_instruction_list* instructions, uint64_t instruction);