#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "ir/IR.h"
#include "arch/arch.h"
#include "elf/elf.h"
#include "utils.h"

int main(int argc, char** argv) {
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
  DEBUG_LOG("Using arch: %s", argv[1]);

  if(arch == NULL) {
    fprintf(stderr, "Unknown architecture/format: %s\n", argv[1]);
    free(data);
    close(fd);
    return 1;
  }

  DEBUG_LOG("Loading data into '%s' arch...", argv[1]);
  void* loaded = arch->load_data(data, stat.st_size);
  DEBUG_LOG("Loaded data at address %p, preparing data...", loaded);
  void* prepared = arch->prepare_data(loaded);
  DEBUG_LOG("Prepared data at address %p, generating IR...", prepared);
  struct ir_data* ir_gen = arch->generate_ir(prepared);
  DEBUG_LOG("Generated IR at address %p, optimizing IR...", ir_gen);
  struct ir_data* ir = ir_optimize(ir_gen);
#ifdef DEBUG
  DEBUG_LOG("%s", "Printing optimized IR");
  ir_print_instructions(ir);
#endif
  DEBUG_LOG("Optimized IR at address %p, printing decompiled code...", ir);
  ir_print_decompiled(ir);

  free(data);
  close(fd);

  return 0;
}