#include <stdlib.h>
#include <stdio.h>
#include "byte_reader.h"

uint8_t read_byte(uint8_t* bytes, int* index, int byte_count) {
  if(*index + 1 > byte_count) {
    fprintf(stderr, "Cannot read byte from index %d\n", *index);
    exit(1);
  }
  return bytes[(*index)++];
}

uint16_t read_word(uint8_t* bytes, int* index, int byte_count, bool le) {
  if(*index + 2 > byte_count) {
    fprintf(stderr, "Cannot read word from index %d\n", *index);
    exit(1);
  }
  uint16_t data = *(uint16_t*) (bytes + *index);
  (*index) += 2;
  return le ? le16toh(data) : be16toh(data);
}


uint32_t read_int(uint8_t* bytes, int* index, int byte_count, bool le) {
  if(*index + 4 > byte_count) {
    fprintf(stderr, "Cannot read int from index %d\n", *index);
    exit(1);
  }
  uint32_t data = *(uint32_t*) (bytes + *index);
  (*index) += 4;
  return le ? le32toh(data) : be32toh(data);
}

uint64_t read_long(uint8_t* bytes, int* index, int byte_count, bool le) {
  if(*index + 8 > byte_count) {
    fprintf(stderr, "Cannot read long from index %d\n", *index);
    exit(1);
  }
  uint64_t data = *(uint64_t*) (bytes + *index);
  (*index) += 8;
  return le ? le64toh(data) : be64toh(data);
}


float read_float(uint8_t* bytes, int* index, int byte_count, bool le) {
  uint32_t x = read_int(bytes, index, byte_count, le);
  return *(float*) &x;
}

double read_double(uint8_t* bytes, int* index, int byte_count, bool le) {
  uint64_t x = read_long(bytes, index, byte_count, le);
  return *(double*) &x;
}