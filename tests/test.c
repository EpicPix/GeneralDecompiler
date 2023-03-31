#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct section {
  int start;
  int end;
} section;

#define TEST_OK 1
#define TEST_FAIL 0

static inline int hex_get_num(char v) {
  if(v >= '0' && v <= '9') return v - '0';
  if(v >= 'a' && v <= 'f') return v - 'a' + 0xa;
  return (v - 'A' + 0xa) & 0xf;
}

int test_run_file(char* file) {
  int fd = open(file, O_RDONLY);
  struct stat stat;
  fstat(fd, &stat);
  char* data = malloc(stat.st_size);
  read(fd, data, stat.st_size);
  
  section name = { 0 };
  section input = { 0 };
  section result = { 0, .end = stat.st_size };
  int current_section = 0;
  
  for(int i = 0; i<stat.st_size; i++) {
    if(i < stat.st_size - 3) {
      if(data[i] == '-' && data[i + 1] == '-' && data[i + 2] == '-') {
        if(current_section == 0) name.end = i - 1;
        else if(current_section == 1) input.end = i - 1;
        else if(current_section == 2) result.end = i - 1;
        
        i += 3;
        current_section++;
        
        if(current_section == 1) input.start = i + 1;
        else if(current_section == 2) result.start = i + 1;
        else break;
        continue;
      }
    }
  }
  {
    char* name_str = alloca(name.end - name.start + 1);
    memcpy(name_str, data + name.start, name.end - name.start);
    name_str[name.end - name.start] = '\0';
    printf("Running '%s': ", name_str);
  }
  if((input.end - input.start) % 2 != 0) {
    printf("invalid data\n");
    return TEST_FAIL;
  }
  
  int in_length = (input.end - input.start) / 2;
  uint8_t* in = alloca(in_length);
  for(int i = 0; i<in_length; i++) {
    in[i] = (hex_get_num(data[i * 2 + input.start]) << 4) | hex_get_num(data[i * 2 + input.start + 1]);
  }
  int tmp = open("./tests/test.tmp", O_RDWR | O_CREAT, 0700);
  write(tmp, in, in_length);
  
  char* args[3] = {"./out/decompiler", "./tests/test.tmp", NULL};
  
  int pipefd[2];
  pipe(pipefd);
  if(fork() == 0) {
    close(pipefd[0]);
    dup2(pipefd[1], STDOUT_FILENO);
    execvp("./out/decompiler", args);
    close(pipefd[1]);
    exit(1);
  }
  close(pipefd[1]);
  int total_read_count = 0;
  char rd[128];
  int read_count = 0;
  while((read_count = read(pipefd[0], rd, 128)) != 0) {
    if(result.end - result.start == total_read_count + read_count) {
      if(memcmp(data + result.start + total_read_count, rd, read_count) != 0) {
        goto fail;
      }
      printf("SUCCESS\n");
      total_read_count += read_count;
      break;
    }else if(result.end - result.start < total_read_count + read_count) {
    fail:
      printf("FAILED\n");
      close(pipefd[0]);
      close(tmp);
      unlink("./tests/test.tmp"); 
      return TEST_FAIL;
    }else if(memcmp(data + result.start + total_read_count, rd, read_count) != 0) {
      goto fail;
    }
    total_read_count += read_count;
  }
  printf("%d %d\n", total_read_count, result.end - result.start);
  if(total_read_count != result.end - result.start) {
    goto fail;
  }
  close(pipefd[0]);
  
  close(tmp);
  unlink("./tests/test.tmp"); 
  return TEST_OK;
}

int main(int argc, char** argv) {
  int test_count = argc-1;
  printf("----------------------\n");
  printf("Test Count: %d\n", test_count);
  int total_correct = 0;
  for(int i = 0; i<test_count; i++) {
    total_correct += test_run_file(argv[i + 1]);
  }
  printf("----------------------\n");
  printf("Results: %d/%d tests passed\n", total_correct, test_count);
  return total_correct != test_count;
}