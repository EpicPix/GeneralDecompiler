#include "arch.h"
#include <stdlib.h>
#include <string.h>

extern const arch_info arch_testing;

const arch_info* arch_get(const char* name) {
  if(strcmp(name, "testing") == 0) return &arch_testing;
  return NULL;
}