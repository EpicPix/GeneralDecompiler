#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

typedef struct section {
  int start;
  int end;
} section;

#define TEST_OK 1
#define TEST_FAIL 0

int test_run_file(char* file) {
  int fd = open(file, O_RDONLY);
  struct stat stat;
  fstat(fd, &stat);
  char* data = malloc(stat.st_size);
  read(fd, data, stat.st_size);
  
  section name = { 0 };
  section result = { 0, .end = stat.st_size };
  int current_section = 0;
  
  for(int i = 0; i<stat.st_size; i++) {
    if(i < stat.st_size - 3) {
      if(data[i] == '-' && data[i + 1] == '-' && data[i + 2] == '-') {
        if(current_section == 0) name.end = i - 1;
        else if(current_section == 1) result.end = i - 1;
        
        i += 3;
        current_section++;
        
        if(current_section == 1) result.start = i + 1;
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
  
  int pre_last_dot = 0;
  int last_dot = 0;
  int filename_len = strlen(file);
  for(int i = 0; i<filename_len; i++) {
    if(file[i] == '.') {
      pre_last_dot = last_dot;
      last_dot = i;
    }
  }
  char* run_file = alloca(filename_len + 5);
  memcpy(run_file, file, filename_len);
  memcpy(run_file + filename_len, ".run", 4);
  run_file[filename_len + 4] = '\0';

  char* arch_type = alloca(last_dot - pre_last_dot);
  memcpy(arch_type, file + pre_last_dot + 1, last_dot - pre_last_dot - 1);
  arch_type[last_dot - pre_last_dot - 1] = '\0';

  char* args[4] = {"./out/decompiler", arch_type, run_file, NULL};
  
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
      total_read_count += read_count;
      break;
    }else if(result.end - result.start < total_read_count + read_count) {
    fail:
      printf("FAILED\n");
      close(pipefd[0]);
      return TEST_FAIL;
    }else if(memcmp(data + result.start + total_read_count, rd, read_count) != 0) {
      goto fail;
    }
    total_read_count += read_count;
  }
  if(total_read_count != result.end - result.start) {
    goto fail;
  }else {
    if(memcmp(data + result.start + total_read_count, rd, read_count) != 0) {
      goto fail;
    }
    printf("SUCCESS\n");
  }
  close(pipefd[0]);
  
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