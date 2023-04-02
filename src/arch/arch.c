#include "arch.h"
#include <stdlib.h>
#include <string.h>

extern const arch_info arch_stestarch;

const arch_info* arch_get(const char* name) {
  if(strcmp(name, "stestarch") == 0) return &arch_stestarch;
  return NULL;
}