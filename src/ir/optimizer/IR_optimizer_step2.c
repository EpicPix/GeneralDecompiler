#include "IR_optimizer.h"
#include "../../utils.h"
#include <stdlib.h>
#include <stdio.h>

struct ir_optimize_register_usage_tree_result {
    struct ir_optimize_register_usage_tree* root;
    struct ir_instruction_low_location* node;
};

static struct ir_optimize_register_usage_tree_result ir_optimize_find_tree_node(struct ir_optimize_data* data, struct ir_instruction_low_location location) {
  struct ir_optimize_register_usage_tree* roots = data->register_tree;
  while(roots) {
    if(ir_instruction_compare_locations_low(location, roots->source)) {
      return (struct ir_optimize_register_usage_tree_result) { .root = roots, .node = NULL };
    }
    for(uint64_t i = 0; i<roots->entry_count; i++) {
      if(ir_instruction_compare_locations_low(location, roots->locations[i])) {
        return (struct ir_optimize_register_usage_tree_result) { .root = roots, .node = &roots->locations[i] };
      }
    }
    roots = roots->next;
  }
  return (struct ir_optimize_register_usage_tree_result) { NULL, NULL };
}

static void ir_optimize_add_node(struct ir_optimize_data* data, struct ir_instruction_low_location source, struct ir_instruction_low_location* entry) {
  if(!data->register_tree) {
    struct ir_optimize_register_usage_tree* tree = malloc(sizeof(struct ir_optimize_register_usage_tree));
    tree->next = NULL;
    tree->source = source;
    tree->entry_count = 1;
    tree->locations = malloc(sizeof(struct ir_instruction_low_location) * tree->entry_count);
    tree->locations[0] = *entry;
    data->register_tree = tree;
    return;
  }
  struct ir_optimize_register_usage_tree_result res = ir_optimize_find_tree_node(data, source);
  if(res.root) {
    res.root->entry_count++;
    res.root->locations = realloc(res.root->locations, sizeof(struct ir_instruction_low_location) * res.root->entry_count);
    res.root->locations[res.root->entry_count - 1] = *entry;
    return;
  }
  struct ir_optimize_register_usage_tree* tree = malloc(sizeof(struct ir_optimize_register_usage_tree));
  tree->next = NULL;
  tree->source = source;
  tree->entry_count = 1;
  tree->locations = malloc(sizeof(struct ir_instruction_low_location) * tree->entry_count);
  tree->locations[0] = *entry;
  struct ir_optimize_register_usage_tree* last = data->register_tree;
  while(last->next) last = last->next;
  last->next = tree;
}

void ir_optimize_prepare_step2(struct ir_optimize_data* data) {
  struct ir_optimize_register_usage_data* register_usage = data->register_usage;
  while(register_usage) {
    if(register_usage->count == 2 && register_usage->has_assigned_data && register_usage->reg.location_type == ir_instruction_low_location_type_register && register_usage->reg.data.reg >> 63) {
      ir_optimize_add_node(data, register_usage->assigned_data, &register_usage->reg);
    }
    register_usage = register_usage->next;
  }
}

struct ir_instruction_list* ir_optimize_step2(struct ir_instruction_list* output, uint64_t current_location, struct ir_instruction_list* input_instructions, struct ir_optimize_data* data) {
  struct ir_instruction_low* instr = ir_get_instruction_low(input_instructions, current_location);

  if(instr->type == ir_instruction_low_type_mov) {
    struct ir_optimize_register_usage_tree_result res_in = ir_optimize_find_tree_node(data, instr->data.mov.input);
    struct ir_optimize_register_usage_tree_result res_out = ir_optimize_find_tree_node(data, instr->data.mov.output);
    if(res_in.root && res_out.node) return output;
    return ir_instruction_add_instruction_low(output, 1024, (struct ir_instruction_low) {
      .type = ir_instruction_low_type_mov,
      .data = {
        .mov = {
          .input = res_in.root ? res_in.root->source : instr->data.mov.input,
          .output = res_out.root ? res_out.root->source : instr->data.mov.output
        }
      }
    });
  }else if(instr->type == ir_instruction_low_type_add) {
    struct ir_optimize_register_usage_tree_result res_ina = ir_optimize_find_tree_node(data, instr->data.add.inputa);
    struct ir_optimize_register_usage_tree_result res_inb = ir_optimize_find_tree_node(data, instr->data.add.inputb);
    struct ir_optimize_register_usage_tree_result res_out = ir_optimize_find_tree_node(data, instr->data.add.output);
    return ir_instruction_add_instruction_low(output, 1024, (struct ir_instruction_low) {
      .type = ir_instruction_low_type_add,
      .data = {
        .add = {
          .inputa = res_ina.root ? res_ina.root->source : instr->data.add.inputa,
          .inputb = res_inb.root ? res_inb.root->source : instr->data.add.inputb,
          .output = res_out.root ? res_out.root->source : instr->data.add.output
        }
      }
    });
  }else if(instr->type == ir_instruction_low_type_mul) {
    struct ir_optimize_register_usage_tree_result res_ina = ir_optimize_find_tree_node(data, instr->data.add.inputa);
    struct ir_optimize_register_usage_tree_result res_inb = ir_optimize_find_tree_node(data, instr->data.add.inputb);
    struct ir_optimize_register_usage_tree_result res_out = ir_optimize_find_tree_node(data, instr->data.add.output);
    return ir_instruction_add_instruction_low(output, 1024, (struct ir_instruction_low) {
      .type = ir_instruction_low_type_mul,
      .data = {
        .add = {
          .inputa = res_ina.root ? res_ina.root->source : instr->data.add.inputa,
          .inputb = res_inb.root ? res_inb.root->source : instr->data.add.inputb,
          .output = res_out.root ? res_out.root->source : instr->data.add.output
        }
      }
    });
  }

  return ir_instruction_add_instruction_low(output, 1024, *instr);
}