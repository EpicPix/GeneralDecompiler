#include "IR_optimizer.h"
#include "../../utils.h"
#include <stdlib.h>
#include <stdio.h>

static struct ir_instruction_list* ir_optimize_step1_put_instruction(struct ir_instruction_list* output, struct ir_optimize_data* data, struct ir_instruction_low instr) {
  if(instr.type == ir_instruction_low_type_mov) {
    ir_optimize_increment_register_usage_mapping(&instr.data.mov.input, NULL, data);
    ir_optimize_increment_register_usage_mapping(&instr.data.mov.output, &instr.data.mov.input, data);
  }
  if(instr.type == ir_instruction_low_type_add) {
    ir_optimize_increment_register_usage_mapping(&instr.data.add.inputa, NULL, data);
    ir_optimize_increment_register_usage_mapping(&instr.data.add.inputb, NULL, data);
    ir_optimize_increment_register_usage_mapping(&instr.data.add.output, NULL, data);
  }
  if(instr.type == ir_instruction_low_type_mul) {
    ir_optimize_increment_register_usage_mapping(&instr.data.add.inputa, NULL, data);
    ir_optimize_increment_register_usage_mapping(&instr.data.add.inputb, NULL, data);
    ir_optimize_increment_register_usage_mapping(&instr.data.add.output, NULL, data);
  }
  return ir_instruction_add_instruction_low(output, 1024, instr);
}


struct ir_instruction_list* ir_optimize_step1(struct ir_instruction_list* output, uint64_t current_location, struct ir_instruction_list* input_instructions, struct ir_optimize_data* data) {
  struct ir_instruction_low* instr = ir_get_instruction_low(input_instructions, current_location);
  if(instr->type == ir_instruction_low_type_movoo) {
    struct ir_optimize_location_mapping* mapping = ir_optimize_get_or_create_mapping(instr->data.movoo.output, data);
    return ir_optimize_step1_put_instruction(output, data, (struct ir_instruction_low) {
      .type = ir_instruction_low_type_mov,
      .data = {
        .mov = {
          .input = instr->data.movoo.input,
          .output = (struct ir_instruction_low_location) { .type = mapping->loc.loc.type, .location_type = ir_instruction_low_location_type_register, .data = { .reg = mapping->reg } }
        }
      }
    });
  }else if(instr->type == ir_instruction_low_type_movoi) {
    struct ir_optimize_location_mapping* mapping = ir_optimize_get_or_create_mapping(instr->data.movoi.input, data);
    return ir_optimize_step1_put_instruction(output, data, (struct ir_instruction_low) {
      .type = ir_instruction_low_type_mov,
      .data = {
        .mov = {
          .input = (struct ir_instruction_low_location) { .type = mapping->loc.loc.type, .location_type = ir_instruction_low_location_type_register, .data = { .reg = mapping->reg } },
          .output = instr->data.movoi.output
        }
      }
    });
  }else if(instr->type == ir_instruction_low_type_movoio) {
    struct ir_optimize_location_mapping* mappingin = ir_optimize_get_or_create_mapping(instr->data.movoio.input, data);
    struct ir_optimize_location_mapping* mappingout = ir_optimize_get_or_create_mapping(instr->data.movoio.output, data);
    return ir_optimize_step1_put_instruction(output, data, (struct ir_instruction_low) {
      .type = ir_instruction_low_type_mov,
      .data = {
        .mov = {
          .input = (struct ir_instruction_low_location) { .type = mappingin->loc.loc.type, .location_type = ir_instruction_low_location_type_register, .data = { .reg = mappingin->reg } },
          .output = (struct ir_instruction_low_location) { .type = mappingout->loc.loc.type, .location_type = ir_instruction_low_location_type_register, .data = { .reg = mappingout->reg } },
        }
      }
    });
  }else if(instr->type == ir_instruction_low_type_norelso) {
    bool mapping_removed = ir_optimize_remove_mapping(instr->data.norelso.input, data);
    if(mapping_removed) {
      return output;
    }
  }
  return ir_optimize_step1_put_instruction(output, data, *instr);
}