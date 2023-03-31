#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "arch/arch.h"

int main(int argc, char** argv) {
  if(argc < 3) {
    fprintf(stderr, "Usage: %s <arch> <decompile_file>\n", argv[0]);
    return 1;
  }
  const arch_info* arch = arch_get_info(argv[1]);
  if(!arch) {
    fprintf(stderr, "Unknown arch\n");
    return 1;
  }
  int fd = open(argv[2], O_RDONLY);
  if(fd == -1) {
    fprintf(stderr, "Could not open file\n");
    return 1;
  }
  struct stat stat;
  fstat(fd, &stat);
  char* data = malloc(stat.st_size);
  read(fd, data, stat.st_size);

  return 0;
}