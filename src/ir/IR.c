#include "IR.h"
#include <stdlib.h>

struct ir_data ir_optimize(struct ir_data data) {
  return data;
}

void ir_print_instructions(struct ir_data data) {

}

void ir_print_decompiled(struct ir_data data) {

}


struct ir_instruction_list* ir_instruction_create_list(struct ir_instruction_list* prev, uint64_t start_address, uint64_t instruction_count, bool high_level) {
  struct ir_instruction_list* list = malloc(sizeof(struct ir_instruction_list));
  if(high_level) {
    list->instructions.high_level = malloc(instruction_count * sizeof(struct ir_instruction_high));
  }else {
    list->instructions.low_level = malloc(instruction_count * sizeof(struct ir_instruction_low));
  }
  list->instruction_count = instruction_count;
  prev->next = list;
  list->start_address = start_address;
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

struct ir_symbol_table* ir_symbol_create_table(struct ir_symbol_table* prev) {
  int alloc_count = sizeof(prev->elements) / sizeof(struct ir_symbol_table_element);
  int start = prev ? (prev->start_index + alloc_count) : 0;

  struct ir_symbol_table* symbol_table = calloc(sizeof(struct ir_symbol_table), 1);
  symbol_table->start_index = start;
  if(prev) prev->next = symbol_table;
  return symbol_table;
}