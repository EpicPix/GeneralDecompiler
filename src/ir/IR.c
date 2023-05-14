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
    struct ir_instruction_list* temporary_instructions = ir_instruction_create_temp_list(false);
    output_instructions = ir_instruction_add_instruction_low(output_instructions, 1024, (struct ir_instruction_low)
      {
        .type = ir_instruction_low_type_add,
        .data = {
          .add = {
            .inputa = ir_lower_level_unpack_location(data.inputa, &output_instructions, type_table, translation_data),
            .inputb = ir_lower_level_unpack_location(data.inputb, &output_instructions, type_table, translation_data),
            .output = ir_lower_level_unpack_location(data.output, &temporary_instructions, type_table, translation_data)
          }
        }
      });
    output_instructions = ir_instruction_move_and_destroy_list(output_instructions, temporary_instructions, false);
  }else if(high_instruction->type == ir_instruction_high_type_pop) {
    struct ir_instruction_high_data_i data = high_instruction->data.i;
    struct ir_instruction_low_location_with_offset lo =
      {
        .loc = { .type = data.input.type, .location_type = ir_instruction_low_location_type_register_address, .data = { .reg = ir_instruction_low_special_registers_stack } },
        .offset = { .type = data.input.type, .location_type = ir_instruction_low_location_type_immediate, .data = { .imm = translation_data->stack_data } }
      };
    output_instructions = ir_instruction_add_instruction_low(output_instructions, 1024, (struct ir_instruction_low)
      {
        .type = ir_instruction_low_type_mov_offsetin,
        .data = {
          .movoin = {
            .output = ir_lower_level_unpack_location(data.input, &output_instructions, type_table, translation_data),
            .input = lo
          }
        }
      });
    output_instructions = ir_instruction_add_instruction_low(output_instructions, 1024, (struct ir_instruction_low)
      {
        .type = ir_instruction_low_type_norelso,
        .data = { .norel = { .loc = lo } }
      });
    translation_data->stack_data += (ir_type_bit_size(data.input.type, type_table) + 7) >> 3;
  }else if(high_instruction->type == ir_instruction_high_type_push) {
    struct ir_instruction_high_data_i data = high_instruction->data.i;
    translation_data->stack_data -= (ir_type_bit_size(data.input.type, type_table) + 7) >> 3;
    output_instructions = ir_instruction_add_instruction_low(output_instructions, 1024, (struct ir_instruction_low)
      {
        .type = ir_instruction_low_type_mov_offsetout,
        .data = {
          .movoout = {
            .output = {
              .loc = { .type = data.input.type, .location_type = ir_instruction_low_location_type_register_address, .data = { .reg = ir_instruction_low_special_registers_stack } },
              .offset = { .type = data.input.type, .location_type = ir_instruction_low_location_type_immediate, .data = { .imm = translation_data->stack_data } }
            },
            .input = ir_lower_level_unpack_location(data.input, &output_instructions, type_table, translation_data)
          }
        }
      });
  }
  return output_instructions;
}

struct ir_data ir_lower_level(struct ir_data data) {
  if(!data.is_high_level) return data;

  struct ir_symbol_table* symbol_table = ir_symbol_create_table(NULL);
  struct ir_instruction_list* instructions_start = ir_instruction_create_list(NULL, 0x10000, 1024, false);
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
    .is_high_level = false,
    .instructions = instructions_start,
    .memory_page_start = data.memory_page_start,
    .symbol_table = symbol_table,
    .type_table = data.type_table
  };
}

static void ir_print_instruction_print_type(ir_type_t type) {
  printf("{:");
  if(type.is_builtin) {
    if(type.built_in.type == ir_type_definition_type_void) {
      printf("v");
    }else if(type.built_in.type == ir_type_definition_type_int_signed) {
      printf("s%d", type.built_in.size);
    }else if(type.built_in.type == ir_type_definition_type_int_unsigned) {
      printf("u%d", type.built_in.size);
    }else if(type.built_in.type == ir_type_definition_type_float) {
      printf("f%d", type.built_in.size);
    }else {
      printf("%d-%d", type.built_in.type, type.built_in.size);
    }
  }else {
    printf("%d", type.composed_type_index);
  }
  printf("}");
}

static void ir_print_instruction_high_inl(struct ir_instruction_high* instr);
static void ir_print_instruction_high_location(struct ir_instruction_high_location* location) {
  ir_print_instruction_print_type(location->type);
  printf(" ");
  if(location->location_type == ir_instruction_high_location_type_immediate) {
    printf("%ld", location->data.imm);
  }else if(location->location_type == ir_instruction_high_location_type_address) {
    printf("*0x%016lx", location->data.addr);
  }else if(location->location_type == ir_instruction_high_location_type_register) {
    printf("@%ld", location->data.reg);
  }else if(location->location_type == ir_instruction_high_location_type_instruction) {
    printf("<");
    ir_print_instruction_high_inl(location->data.instr);
    printf(">");
  }else if(location->location_type == ir_instruction_high_location_type_inherited) {
    printf("@^");
  }else {
    printf("?");
  }
}

static void ir_print_instruction_high_inl(struct ir_instruction_high* instr) {
  if(instr->type == ir_instruction_high_type_push) {
    printf("push ");
    ir_print_instruction_high_location(&instr->data.i.input);
  }else if(instr->type == ir_instruction_high_type_pop) {
    printf("pop ");
    ir_print_instruction_high_location(&instr->data.o.output);
  }else if(instr->type == ir_instruction_high_type_add) {
    printf("add ");
    ir_print_instruction_high_location(&instr->data.oii.output);
    printf(", ");
    ir_print_instruction_high_location(&instr->data.oii.inputa);
    printf(", ");
    ir_print_instruction_high_location(&instr->data.oii.inputb);
  }
}

static void ir_print_instruction_high(uint64_t addr, struct ir_instruction_high* instr) {
  printf("0x%016lx: ", addr);
  ir_print_instruction_high_inl(instr);
  printf("\n");
}

static void ir_print_instruction_low_location(struct ir_instruction_low_location* location) {
  ir_print_instruction_print_type(location->type);
  printf(" ");
  if(location->location_type == ir_instruction_low_location_type_immediate) {
    printf("%ld", location->data.imm);
  }else if(location->location_type == ir_instruction_low_location_type_address) {
    printf("*0x%016lx", location->data.addr);
  }else if(location->location_type == ir_instruction_low_location_type_register) {
    printf("@%ld", location->data.reg);
  }else if(location->location_type == ir_instruction_low_location_type_register_address) {
    printf("*@%ld", location->data.reg);
  }else {
    printf("?");
  }
}

static void ir_print_instruction_low_inl(struct ir_instruction_low* instr) {
  if(instr->type == ir_instruction_low_type_mov) {
    printf("mov ");
    ir_print_instruction_low_location(&instr->data.mov.output);
    printf(", ");
    ir_print_instruction_low_location(&instr->data.mov.input);
  }else if(instr->type == ir_instruction_low_type_mov_offsetout) {
    printf("movoo (");
    ir_print_instruction_low_location(&instr->data.movoout.output.loc);
    printf(" + ");
    ir_print_instruction_low_location(&instr->data.movoout.output.offset);
    printf("), ");
    ir_print_instruction_low_location(&instr->data.movoout.input);
  }else if(instr->type == ir_instruction_low_type_mov_offsetin) {
    printf("movoi ");
    ir_print_instruction_low_location(&instr->data.movoin.output);
    printf(", (");
    ir_print_instruction_low_location(&instr->data.movoin.input.loc);
    printf(" + ");
    ir_print_instruction_low_location(&instr->data.movoin.input.offset);
    printf(")");
  }else if(instr->type == ir_instruction_low_type_add) {
    printf("add ");
    ir_print_instruction_low_location(&instr->data.add.output);
    printf(", ");
    ir_print_instruction_low_location(&instr->data.add.inputa);
    printf(", ");
    ir_print_instruction_low_location(&instr->data.add.inputb);
  }else if(instr->type == ir_instruction_low_type_norelso) {
    printf("norel (");
    ir_print_instruction_low_location(&instr->data.norel.loc.loc);
    printf(" + ");
    ir_print_instruction_low_location(&instr->data.norel.loc.offset);
    printf(")");
  }
}

static void ir_print_instruction_low(uint64_t addr, struct ir_instruction_low* instr) {
  printf("0x%016lx: ", addr);
  ir_print_instruction_low_inl(instr);
  printf("\n");
}

void ir_print_instructions(struct ir_data data) {
  struct ir_instruction_list* list = data.instructions;
  if(data.is_high_level) {
    while(list != NULL) {
      for(uint64_t i = 0; i<list->instruction_count; i++) {
        ir_print_instruction_high(list->start_address + i, &list->instructions.high_level[i]);
      }
      list = list->next;
    }
  }else {
    while(list != NULL) {
      for(uint64_t i = 0; i<list->instruction_count; i++) {
        ir_print_instruction_low(list->start_address + i, &list->instructions.low_level[i]);
      }
      list = list->next;
    }
  }
}

void ir_print_decompiled(struct ir_data data) {

}

struct ir_instruction_list* ir_instruction_create_temp_list(bool high_level) {
  return ir_instruction_create_list(NULL, 0x0, 16, high_level);
}

struct ir_instruction_list* ir_instruction_create_list(struct ir_instruction_list* prev, uint64_t start_address, uint64_t instruction_count, bool high_level) {
  struct ir_instruction_list* list = malloc(sizeof(struct ir_instruction_list));
  if(high_level) {
    list->instructions.high_level = malloc(instruction_count * sizeof(struct ir_instruction_high));
  }else {
    list->instructions.low_level = malloc(instruction_count * sizeof(struct ir_instruction_low));
  }
  list->allocated_count = instruction_count;
  list->instruction_count = 0;
  if(prev) prev->next = list;
  list->start_address = start_address;
  return list;
}

void ir_instruction_destroy_list(struct ir_instruction_list* instructions, bool high_level) {
  while(instructions) {
    if (high_level) {
      free(instructions->instructions.high_level);
    } else {
      free(instructions->instructions.low_level);
    }
    struct ir_instruction_list* current = instructions;
    instructions = instructions->next;
    free(current);
  }
}

struct ir_instruction_list* ir_instruction_move_list(struct ir_instruction_list* new_list, struct ir_instruction_list* old_list, bool high_level) {
  if(high_level) {
    while(old_list) {
      for(uint64_t i = 0; i<old_list->instruction_count; i++) {
        ir_instruction_add_instruction_high(new_list, 1024, old_list->instructions.high_level[i]);
      }
      old_list = old_list->next;
    }
  }else {
    while(old_list) {
      for(uint64_t i = 0; i<old_list->instruction_count; i++) {
        ir_instruction_add_instruction_low(new_list, 1024, old_list->instructions.low_level[i]);
      }
      old_list = old_list->next;
    }
  }
  return new_list;
}

struct ir_instruction_list* ir_instruction_move_and_destroy_list(struct ir_instruction_list* new_list, struct ir_instruction_list* old_list, bool high_level) {
  struct ir_instruction_list* list = ir_instruction_move_list(new_list, old_list, high_level);
  ir_instruction_destroy_list(old_list, high_level);
  return list;
}

struct ir_instruction_list* ir_instruction_add_instruction_low(struct ir_instruction_list* list, uint64_t instruction_count, struct ir_instruction_low instr) {
  if(list->instruction_count >= list->allocated_count) {
    list = ir_instruction_create_list(list, list->start_address + list->allocated_count, instruction_count, false);
  }
  list->instructions.low_level[list->instruction_count] = instr;
  list->instruction_count++;
  return list;
}

struct ir_instruction_list* ir_instruction_add_instruction_high(struct ir_instruction_list* list, uint64_t instruction_count, struct ir_instruction_high instr) {
  if(list->instruction_count >= list->allocated_count) {
    list = ir_instruction_create_list(list, list->start_address + list->allocated_count, instruction_count, true);
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