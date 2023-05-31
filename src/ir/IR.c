#include "IR.h"
#include "../utils.h"
#include <stdlib.h>
#include <stdio.h>

struct ir_level_translation_data {
    int64_t stack_data;
    uint64_t current_temp_register;
};

static struct ir_instruction_list* ir_lower_level_unpack_instruction(struct ir_instruction_high* high_instruction, struct ir_instruction_list* output_instructions, struct ir_type_table* type_table, struct ir_level_translation_data* translation_data);

static struct ir_instruction_low_location ir_lower_level_unpack_location(struct ir_instruction_high_location high_location, struct ir_instruction_list** output_instructions, struct ir_type_table* type_table, struct ir_level_translation_data* translation_data) {
  if(high_location.location_type == ir_instruction_high_location_type_immediate)
    return (struct ir_instruction_low_location){ .type = high_location.type, .location_type = ir_instruction_low_location_type_immediate, .data = { .imm = high_location.data.imm } };
  if(high_location.location_type == ir_instruction_high_location_type_register)
    return (struct ir_instruction_low_location){ .type = high_location.type, .location_type = ir_instruction_low_location_type_register, .data = { .reg = high_location.data.reg } };
  if(high_location.location_type == ir_instruction_high_location_type_address)
    return (struct ir_instruction_low_location){ .type = high_location.type, .location_type = ir_instruction_low_location_type_address, .data = { .addr = high_location.data.addr } };
  if(high_location.location_type == ir_instruction_high_location_type_inherited)
    return (struct ir_instruction_low_location){ .type = high_location.type, .location_type = ir_instruction_low_location_type_register, .data = { .reg = translation_data->current_temp_register } };

  // high_location.location_type is ir_instruction_high_location_type_instruction
  *output_instructions = ir_lower_level_unpack_instruction(high_location.data.instr, *output_instructions, type_table, translation_data);
  return (struct ir_instruction_low_location){ .type = high_location.type, .location_type = ir_instruction_low_location_type_register, .data = { .reg = translation_data->current_temp_register-- } };
}

static struct ir_instruction_list* ir_lower_level_unpack_instruction(struct ir_instruction_high* high_instruction, struct ir_instruction_list* output_instructions, struct ir_type_table* type_table, struct ir_level_translation_data* translation_data) {
  if(high_instruction->type == ir_instruction_high_type_add) {
    struct ir_instruction_high_data_oii data = high_instruction->data.oii;
    struct ir_instruction_list* temporary_instructions = ir_instruction_create_temp_list(ir_instruction_level_low);
    struct ir_instruction_low_location inputb = ir_lower_level_unpack_location(data.inputb, &output_instructions, type_table, translation_data);
    struct ir_instruction_low_location inputa = ir_lower_level_unpack_location(data.inputa, &output_instructions, type_table, translation_data);
    output_instructions = ir_instruction_add_instruction_low(output_instructions, 1024, (struct ir_instruction_low)
      {
        .type = ir_instruction_low_type_add,
        .data = {
          .add = {
            .inputa = inputa,
            .inputb = inputb,
            .output = ir_lower_level_unpack_location(data.output, &temporary_instructions, type_table, translation_data)
          }
        }
      });
    output_instructions = ir_instruction_move_and_destroy_list(output_instructions, temporary_instructions, ir_instruction_level_low);
  }else if(high_instruction->type == ir_instruction_high_type_sub) {
    struct ir_instruction_high_data_oii data = high_instruction->data.oii;
    struct ir_instruction_list* temporary_instructions = ir_instruction_create_temp_list(ir_instruction_level_low);
    struct ir_instruction_low_location inputb = ir_lower_level_unpack_location(data.inputb, &output_instructions, type_table, translation_data);
    struct ir_instruction_low_location inputa = ir_lower_level_unpack_location(data.inputa, &output_instructions, type_table, translation_data);
    output_instructions = ir_instruction_add_instruction_low(output_instructions, 1024, (struct ir_instruction_low)
      {
        .type = ir_instruction_low_type_sub,
        .data = {
          .add = {
            .inputa = inputa,
            .inputb = inputb,
            .output = ir_lower_level_unpack_location(data.output, &temporary_instructions, type_table, translation_data)
          }
        }
      });
    output_instructions = ir_instruction_move_and_destroy_list(output_instructions, temporary_instructions, ir_instruction_level_low);
  }else if(high_instruction->type == ir_instruction_high_type_mul) {
    struct ir_instruction_high_data_oii data = high_instruction->data.oii;
    struct ir_instruction_list* temporary_instructions = ir_instruction_create_temp_list(ir_instruction_level_low);
    struct ir_instruction_low_location inputb = ir_lower_level_unpack_location(data.inputb, &output_instructions, type_table, translation_data);
    struct ir_instruction_low_location inputa = ir_lower_level_unpack_location(data.inputa, &output_instructions, type_table, translation_data);
    output_instructions = ir_instruction_add_instruction_low(output_instructions, 1024, (struct ir_instruction_low)
      {
        .type = ir_instruction_low_type_mul,
        .data = {
          .mul = {
            .inputa = inputa,
            .inputb = inputb,
            .output = ir_lower_level_unpack_location(data.output, &temporary_instructions, type_table, translation_data)
          }
        }
      });
    output_instructions = ir_instruction_move_and_destroy_list(output_instructions, temporary_instructions, ir_instruction_level_low);
  }else if(high_instruction->type == ir_instruction_high_type_pop) {
    struct ir_instruction_high_data_i data = high_instruction->data.i;
    struct ir_instruction_low_location_with_offset lo =
      {
        .loc = { .type = data.input.type, .location_type = ir_instruction_low_location_type_register_address, .data = { .reg = ir_instruction_low_special_registers_stack } },
        .offset = { .type = data.input.type, .location_type = ir_instruction_low_location_type_immediate, .data = { .imm = translation_data->stack_data } }
      };
    output_instructions = ir_instruction_add_instruction_low(output_instructions, 1024, (struct ir_instruction_low)
      {
        .type = ir_instruction_low_type_movoi,
        .data = {
          .movoi = {
            .output = ir_lower_level_unpack_location(data.input, &output_instructions, type_table, translation_data),
            .input = lo
          }
        }
      });
    output_instructions = ir_instruction_add_instruction_low(output_instructions, 1024, (struct ir_instruction_low)
      {
        .type = ir_instruction_low_type_norelso,
        .data = { .norelso = { .input = lo } }
      });
    translation_data->stack_data += (ir_type_bit_size(data.input.type, type_table) + 7) >> 3;
  }else if(high_instruction->type == ir_instruction_high_type_push) {
    struct ir_instruction_high_data_i data = high_instruction->data.i;
    translation_data->stack_data -= (ir_type_bit_size(data.input.type, type_table) + 7) >> 3;
    output_instructions = ir_instruction_add_instruction_low(output_instructions, 1024, (struct ir_instruction_low)
      {
        .type = ir_instruction_low_type_movoo,
        .data = {
          .movoo = {
            .output = {
              .loc = { .type = data.input.type, .location_type = ir_instruction_low_location_type_register_address, .data = { .reg = ir_instruction_low_special_registers_stack } },
              .offset = { .type = data.input.type, .location_type = ir_instruction_low_location_type_immediate, .data = { .imm = translation_data->stack_data } }
            },
            .input = ir_lower_level_unpack_location(data.input, &output_instructions, type_table, translation_data)
          }
        }
      });
  }else if(high_instruction->type == ir_instruction_high_type_mov) {
    struct ir_instruction_high_data_oi data = high_instruction->data.oi;
    struct ir_instruction_list* temporary_instructions = ir_instruction_create_temp_list(ir_instruction_level_low);
    output_instructions = ir_instruction_add_instruction_low(output_instructions, 1024, (struct ir_instruction_low)
      {
        .type = ir_instruction_low_type_mov,
        .data = {
          .mov = {
            .input = ir_lower_level_unpack_location(data.input, &output_instructions, type_table, translation_data),
            .output = ir_lower_level_unpack_location(data.output, &temporary_instructions, type_table, translation_data)
          }
        }
      });
    output_instructions = ir_instruction_move_and_destroy_list(output_instructions, temporary_instructions, ir_instruction_level_low);
  }else if(high_instruction->type == ir_instruction_high_type_intrinsic) {
  }else if(high_instruction->type == ir_instruction_high_type_intrinsic_typed) {
    if(high_instruction->data.intrinsic_typed.intrinsic == ir_instruction_high_type_intrinsic_jvm_dup) {
      ir_type_t type = high_instruction->data.intrinsic_typed.type;
      uint64_t off = (ir_type_bit_size(type, type_table) + 7) >> 3;
      output_instructions = ir_instruction_add_instruction_low(output_instructions, 1024, (struct ir_instruction_low)
        {
          .type = ir_instruction_low_type_movoio,
          .data = {
            .movoio = {
              .output = {
                .loc = { .type = type, .location_type = ir_instruction_low_location_type_register_address, .data = { .reg = ir_instruction_low_special_registers_stack } },
                .offset = { .type = type, .location_type = ir_instruction_low_location_type_immediate, .data = { .imm = translation_data->stack_data - off } }
              },
              .input = {
                .loc = { .type = type, .location_type = ir_instruction_low_location_type_register_address, .data = { .reg = ir_instruction_low_special_registers_stack } },
                .offset = { .type = type, .location_type = ir_instruction_low_location_type_immediate, .data = { .imm = translation_data->stack_data } }
              }
            }
          }
        });
      translation_data->stack_data -= off;
    }
  }
  return output_instructions;
}

struct ir_data ir_lower_level(struct ir_data data) {
  if(data.instruction_level != ir_instruction_level_high) return data;

  struct ir_symbol_table* symbol_table = ir_symbol_create_table(NULL);
  struct ir_instruction_list* instructions_start = ir_instruction_create_list(NULL, 0x10000, 1024, ir_instruction_level_low);
  struct ir_instruction_list* instructions_current = instructions_start;
  struct ir_instruction_list* instructions_high = data.instructions;

  struct ir_level_translation_data translation_data = { .stack_data = 0, .current_temp_register = ir_instruction_low_special_registers_temp_start };
  while(instructions_high) {
    for(uint64_t i = 0; i<instructions_high->instruction_count; i++) {
      instructions_current = ir_lower_level_unpack_instruction(&instructions_high->instructions.high_level[i], instructions_current, data.type_table, &translation_data);
    }
    instructions_high = instructions_high->next;
  }

  return (struct ir_data) {
    .instruction_level = ir_instruction_level_low,
    .instructions = instructions_start,
    .memory_page_start = data.memory_page_start,
    .symbol_table = symbol_table,
    .type_table = data.type_table
  };
}

struct ir_instruction_list* ir_instruction_create_temp_list(enum ir_instruction_level level) {
  return ir_instruction_create_list(NULL, 0x0, 16, level);
}

struct ir_instruction_list* ir_instruction_create_list(struct ir_instruction_list* prev, uint64_t start_address, uint64_t instruction_count, enum ir_instruction_level level) {
  struct ir_instruction_list* list = malloc(sizeof(struct ir_instruction_list));
  if(level == ir_instruction_level_high) {
    list->instructions.high_level = malloc(instruction_count * sizeof(struct ir_instruction_high));
  }else if(level == ir_instruction_level_low) {
    list->instructions.low_level = malloc(instruction_count * sizeof(struct ir_instruction_low));
  }else if(level == ir_instruction_level_collapsed) {
    list->instructions.collapsed_level = malloc(instruction_count * sizeof(struct ir_instruction_collapsed));
  }
  list->allocated_count = instruction_count;
  list->instruction_count = 0;
  if(prev) prev->next = list;
  list->start_address = start_address;
  return list;
}

void ir_instruction_destroy_list(struct ir_instruction_list* instructions, enum ir_instruction_level level) {
  while(instructions) {
    if(level == ir_instruction_level_high) {
      free(instructions->instructions.high_level);
    }else if(level == ir_instruction_level_low) {
      free(instructions->instructions.low_level);
    }else if(level == ir_instruction_level_collapsed) {
      free(instructions->instructions.collapsed_level);
    }
    struct ir_instruction_list* current = instructions;
    instructions = instructions->next;
    free(current);
  }
}

struct ir_instruction_list* ir_instruction_move_list(struct ir_instruction_list* new_list, struct ir_instruction_list* old_list, enum ir_instruction_level level) {
  if(level == ir_instruction_level_high) {
    while(old_list) {
      for(uint64_t i = 0; i<old_list->instruction_count; i++) {
        ir_instruction_add_instruction_high(new_list, 1024, old_list->instructions.high_level[i]);
      }
      old_list = old_list->next;
    }
  }else if(level == ir_instruction_level_low) {
    while(old_list) {
      for(uint64_t i = 0; i<old_list->instruction_count; i++) {
        ir_instruction_add_instruction_low(new_list, 1024, old_list->instructions.low_level[i]);
      }
      old_list = old_list->next;
    }
  }else if(level == ir_instruction_level_collapsed) {
    while(old_list) {
      for(uint64_t i = 0; i<old_list->instruction_count; i++) {
        ir_instruction_add_instruction_collapsed(new_list, 1024, old_list->instructions.collapsed_level[i]);
      }
      old_list = old_list->next;
    }
  }
  return new_list;
}

struct ir_instruction_list* ir_instruction_move_and_destroy_list(struct ir_instruction_list* new_list, struct ir_instruction_list* old_list, enum ir_instruction_level level) {
  struct ir_instruction_list* list = ir_instruction_move_list(new_list, old_list, level);
  ir_instruction_destroy_list(old_list, level);
  return list;
}

bool ir_instruction_compare_locations_low(struct ir_instruction_low_location location_a, struct ir_instruction_low_location location_b) {
  if(location_a.location_type != location_b.location_type) return false;
  if(!ir_equal_types(location_a.type, location_b.type)) return false;

  if(location_a.location_type == ir_instruction_low_location_type_register) return location_a.data.reg == location_b.data.reg;
  if(location_a.location_type == ir_instruction_low_location_type_register_address) return location_a.data.reg == location_b.data.reg;
  if(location_a.location_type == ir_instruction_low_location_type_immediate) return location_a.data.imm == location_b.data.imm;
  if(location_a.location_type == ir_instruction_low_location_type_address) return location_a.data.addr == location_b.data.addr;
  return false;
}

bool ir_instruction_compare_locations_with_offset_low(struct ir_instruction_low_location_with_offset location_a, struct ir_instruction_low_location_with_offset location_b) {
  return ir_instruction_compare_locations_low(location_a.loc, location_b.loc) && ir_instruction_compare_locations_low(location_a.offset, location_b.offset);
}

struct ir_instruction_list* ir_instruction_add_instruction_low(struct ir_instruction_list* list, uint64_t instruction_count, struct ir_instruction_low instr) {
  if(list->instruction_count >= list->allocated_count) {
    list = ir_instruction_create_list(list, list->start_address + list->allocated_count, instruction_count, ir_instruction_level_low);
  }
  list->instructions.low_level[list->instruction_count] = instr;
  list->instruction_count++;
  return list;
}

struct ir_instruction_low* ir_get_instruction_low(struct ir_instruction_list* instructions, uint64_t instruction) {
  while(instructions) {
    if(instructions->start_address <= instruction && instructions->start_address + instructions->allocated_count > instruction) {
      if(instructions->start_address + instructions->instruction_count <= instruction) {
        return NULL;
      }
      return &instructions->instructions.low_level[instruction - instructions->start_address];
    }
    instructions = instructions->next;
  }
  return NULL;
}

struct ir_instruction_list* ir_instruction_add_instruction_high(struct ir_instruction_list* list, uint64_t instruction_count, struct ir_instruction_high instr) {
  if(list->instruction_count >= list->allocated_count) {
    list = ir_instruction_create_list(list, list->start_address + list->allocated_count, instruction_count, ir_instruction_level_high);
  }
  list->instructions.high_level[list->instruction_count] = instr;
  list->instruction_count++;
  return list;
}

struct ir_type_table* ir_type_create_table(struct ir_type_table* prev) {
  int start = prev ? (prev->start_index + prev->allocated_count) : 0;
  int alloc_count = 256;

  struct ir_type_table* type_table = calloc(sizeof(struct ir_type_table) + alloc_count * sizeof(struct ir_type_composed*), 1);
  type_table->start_index = start;
  type_table->allocated_count = alloc_count;
  if(prev) prev->next = type_table;
  return type_table;
}

struct ir_type_composed* ir_type_get_composed(uint32_t composed_type_id, struct ir_type_table* type_table) {
  while(type_table) {
    if(type_table->start_index <= composed_type_id && type_table->start_index + type_table->entry_count < composed_type_id) {
      return type_table->types[composed_type_id - type_table->start_index];
    }
    type_table = type_table->next;
  }
  return NULL;
}

uint64_t ir_type_bit_size(ir_type_t type, struct ir_type_table* table) {
  if(type.is_builtin) return type.built_in.size;
  struct ir_type_composed* composed = ir_type_get_composed(type.composed_type_index, table);
  if(composed) {
    if(composed->composed_type == ir_type_composed_type_struct) {
      uint64_t total_size = 0;
      for(int i = 0; i<composed->type_count; i++) {
        total_size += ir_type_bit_size(composed->contained_types[i], table);
      }
      return total_size;
    }else if(composed->composed_type == ir_type_composed_type_union) {
      uint64_t biggest_size = 0;
      for(int i = 0; i<composed->type_count; i++) {
        uint64_t size = ir_type_bit_size(composed->contained_types[i], table);
        if(size > biggest_size) {
          biggest_size = size;
        }
      }
      return biggest_size;
    }
  }
  return 0;
}

struct ir_symbol_table* ir_symbol_create_table(struct ir_symbol_table* prev) {
  int alloc_count = sizeof(prev->elements) / sizeof(struct ir_symbol_table_element);
  int start = prev ? (prev->start_index + alloc_count) : 0;

  struct ir_symbol_table* symbol_table = calloc(sizeof(struct ir_symbol_table), 1);
  symbol_table->start_index = start;
  if(prev) prev->next = symbol_table;
  return symbol_table;
}

struct ir_instruction_high* ir_instruction_alloc_high(struct ir_instruction_high data) {
  struct ir_instruction_high* allocated = malloc(sizeof(struct ir_instruction_high));
  allocated->type = data.type;
  allocated->data = data.data;
  return allocated;
}