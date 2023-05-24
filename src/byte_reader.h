#pragma once

#include <stdint.h>
#include <stdbool.h>

uint8_t read_byte(uint8_t* bytes, int* index, int byte_count);
uint16_t read_word(uint8_t* bytes, int* index, int byte_count, bool le);
uint32_t read_int(uint8_t* bytes, int* index, int byte_count, bool le);
uint64_t read_long(uint8_t* bytes, int* index, int byte_count, bool le);

float read_float(uint8_t* bytes, int* index, int byte_count, bool le);
double read_double(uint8_t* bytes, int* index, int byte_count, bool le);

#define read_word_be(bytes, index, byte_count) read_word(bytes, index, byte_count, false)
#define read_int_be(bytes, index, byte_count) read_int(bytes, index, byte_count, false)
#define read_long_be(bytes, index, byte_count) read_long(bytes, index, byte_count, false)
#define read_float_be(bytes, index, byte_count) read_float(bytes, index, byte_count, false)
#define read_double_be(bytes, index, byte_count) read_double(bytes, index, byte_count, false)

#define read_word_le(bytes, index, byte_count) read_word(bytes, index, byte_count, true)
#define read_int_le(bytes, index, byte_count) read_int(bytes, index, byte_count, true)
#define read_long_le(bytes, index, byte_count) read_long(bytes, index, byte_count, true)
#define read_float_le(bytes, index, byte_count) read_float(bytes, index, byte_count, true)
#define read_double_le(bytes, index, byte_count) read_double(bytes, index, byte_count, true)