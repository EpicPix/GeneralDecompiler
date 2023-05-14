#pragma once

#include "../IR.h"

struct ir_optimize_location_mapping {
    struct ir_instruction_low_location_with_offset loc;
    uint64_t reg;
};

struct ir_optimize_location_mappings {
    struct ir_optimize_location_mapping mapping;
    struct ir_optimize_location_mappings* next;
};

struct ir_optimize_register_usage_data {
    struct ir_instruction_low_location assigned_data;
    struct ir_instruction_low_location reg;
    uint64_t count;
    struct ir_optimize_register_usage_data* next;
    bool has_assigned_data;
};

struct ir_optimize_data {
    struct ir_optimize_location_mappings* mappings;
    uint64_t mapping_count;
    struct ir_optimize_register_usage_data* register_usage;
};

typedef struct ir_instruction_list* (*ir_optimize_step_func_t)(struct ir_instruction_list* output, uint64_t current_location, struct ir_instruction_list* input_instructions, struct ir_optimize_data* data);

struct ir_optimize_location_mapping* ir_optimize_get_or_create_mapping(struct ir_instruction_low_location_with_offset loc, struct ir_optimize_data* data);
bool ir_optimize_remove_mapping(struct ir_instruction_low_location_with_offset loc, struct ir_optimize_data* data);
struct ir_optimize_register_usage_data* ir_optimize_increment_register_usage_mapping(struct ir_instruction_low_location* reg, struct ir_instruction_low_location* loc, struct ir_optimize_data* data);


struct ir_instruction_list* ir_optimize_step1(struct ir_instruction_list* output, uint64_t current_location, struct ir_instruction_list* input_instructions, struct ir_optimize_data* data);
