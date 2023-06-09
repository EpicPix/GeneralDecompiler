#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "ir/IR.h"
#include "arch/arch.h"
#include "utils.h"

int main(int argc, char** argv) {
  init_app_time();
  if(argc < 3) {
    fprintf(stderr, "Usage: %s <format> <file>\n", argv[0]);
    return 1;
  }
  int fd = open(argv[2], O_RDONLY);
  if(fd == -1) {
    fprintf(stderr, "Could not open file: %s\n", argv[2]);
    return 1;
  }
  struct stat stat;
  fstat(fd, &stat);
  uint8_t* data = malloc(stat.st_size);
  read(fd, data, stat.st_size);

  const arch_info* arch = arch_get(argv[1]);
  DEBUG_LOG("main", "Using arch: %s", argv[1]);

  if(arch == NULL) {
    fprintf(stderr, "Unknown architecture/format: %s\n", argv[1]);
    free(data);
    close(fd);
    return 1;
  }

  DEBUG_LOG("main", "Loading data into '%s' arch...", argv[1]);
  void* loaded = arch->load_data(data, stat.st_size);
  DEBUG_LOG("main", "Loaded data at address %p, preparing data...", loaded);
  void* prepared = arch->prepare_data(loaded);
  DEBUG_LOG("main", "Prepared data at address %p, generating IR...", prepared);
  struct ir_data ir_data_high = arch->generate_ir(prepared);
  DEBUG_LOG("main", "Generated IR. Symbol table at %p", ir_data_high.symbol_table);
#ifdef DEBUG
  DEBUG_LOG("memory_pages", "%s", "Printing memory pages");
  struct ir_memory_page* memory_page = ir_data_high.memory_page_start;
  while(memory_page != NULL) {
    DEBUG_LOG("memory_pages", "Address: %016lx  Length: %016lx", memory_page->start_address, memory_page->length);
    memory_page = memory_page->next_page;
  }
  DEBUG_LOG("main", "%s", "Printing unoptimized IR");
  ir_print_instructions(ir_data_high);
#endif
  DEBUG_LOG("main", "%s", "Lowering IR...");
  struct ir_data ir_data_low = ir_lower_level(ir_data_high);
#ifdef DEBUG
  DEBUG_LOG("main", "%s", "Printing lowered IR");
  ir_print_instructions(ir_data_low);
#endif
  DEBUG_LOG("main", "%s", "Optmizing IR...");
  struct ir_data ir_data_optimized = ir_optimize(ir_data_low);
#ifdef DEBUG
  DEBUG_LOG("main", "%s", "Printing optimized IR");
  ir_print_instructions(ir_data_optimized);
#endif
  DEBUG_LOG("main", "%s", "Collapsing IR...");
  struct ir_data ir_data = ir_collapse(ir_data_optimized);
#ifdef DEBUG
  DEBUG_LOG("main", "%s", "Printing collapsed IR");
  ir_print_instructions(ir_data);
#endif
  DEBUG_LOG("main", "Optimized IR. Symbol table at %p, printing decompiled code...", ir_data.symbol_table);
  ir_decompiled_print(ir_data);

  free(data);
  close(fd);

  return 0;
}