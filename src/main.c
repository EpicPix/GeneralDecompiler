#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "arch/arch.h"
#include "elf/elf.h"

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

  if(strcmp(argv[1], "elf") == 0) {
    elf_read(data, stat.st_size);
  }else {
    fprintf(stderr, "Unknown format: %s\n", argv[1]);
  }

  free(data);
  close(fd);

  return 0;
}