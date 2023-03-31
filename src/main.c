#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char** argv) {
  if(argc < 2) {
    fprintf(stderr, "Usage: %s <decompile_file>\n", argv[0]);
    return 1;
  }
  int fd = open(argv[1], O_RDONLY);
  if(fd == -1) {
    fprintf(stderr, "Could not open file\n");
    return 1;
  }
  struct stat stat;
  fstat(fd, &stat);
  char* data = malloc(stat.st_size + 1);
  read(fd, data, stat.st_size);
  data[stat.st_size] = '\0';
  printf("%s", data);
  return 0;
}