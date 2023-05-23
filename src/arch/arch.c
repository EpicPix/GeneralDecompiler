#include "arch.h"
#include <stdlib.h>
#include <string.h>

const arch_info* arch_get(const char* name) {
  if(strcmp(name, "jvm") == 0) {
    return &arch_jvm;
  }
  if(strcmp(name, "elf") == 0) {
    return &arch_elf;
  }
  return NULL;
}