#include "elf.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <endian.h>
#include "../../byte_reader.h"

static void* arch_load_data(uint8_t* bytes, int byte_count) {
  int index = 0;
  if(read_byte(bytes, &index, byte_count) != 0x7F || read_byte(bytes, &index, byte_count) != 'E' || read_byte(bytes, &index, byte_count) != 'L' || read_byte(bytes, &index, byte_count) != 'F') {
    fprintf(stderr, "Invalid ELF header\n");
    exit(1);
  }
  bool is_64bit = read_byte(bytes, &index, byte_count) == 0x02;
  bool little_endian = read_byte(bytes, &index, byte_count) != 0x02;
  if(read_byte(bytes, &index, byte_count) != 0x01) {
    fprintf(stderr, "Unknown ELF version\n");
    exit(1);
  }
  uint8_t abi = read_byte(bytes, &index, byte_count);
  read_byte(bytes, &index, byte_count); // abi version
  index = 0x10;
  uint16_t type = read_word(bytes, &index, byte_count, little_endian);
  uint16_t machine = read_word(bytes, &index, byte_count, little_endian);
  if(read_int(bytes, &index, byte_count, little_endian) != 0x01) {
    fprintf(stderr, "Unknown ELF version\n");
    exit(1);
  }
  uint64_t entry = is_64bit ? read_long(bytes, &index, byte_count, little_endian) : read_int(bytes, &index, byte_count, little_endian);
  uint64_t ph_start = is_64bit ? read_long(bytes, &index, byte_count, little_endian) : read_int(bytes, &index, byte_count, little_endian);
  uint64_t sh_start = is_64bit ? read_long(bytes, &index, byte_count, little_endian) : read_int(bytes, &index, byte_count, little_endian);
  uint32_t flags = read_int(bytes, &index, byte_count, little_endian);
  uint16_t header_size = read_word(bytes, &index, byte_count, little_endian);
  uint16_t ph_size = read_word(bytes, &index, byte_count, little_endian);
  uint16_t phe_count = read_word(bytes, &index, byte_count, little_endian);
  uint16_t sh_size = read_word(bytes, &index, byte_count, little_endian);
  uint16_t she_count = read_word(bytes, &index, byte_count, little_endian);
  uint16_t shstr_index = read_word(bytes, &index, byte_count, little_endian);
  printf("%d %d %02x %04x %04x %p %p %p\n", is_64bit, little_endian, abi, type, machine, (void*)entry, (void*) ph_start, (void*) sh_start);
  printf("%08x %d %d %d %d %d %d\n", flags, header_size, ph_size, phe_count, sh_size, she_count, shstr_index);
  if(index != header_size) {
    fprintf(stderr, "Invalid read count\n");
    exit(1);
  }

  printf("------\n");

  index = ph_start;
  for(int i = 0; i<phe_count; i++) {
    uint32_t type = read_int(bytes, &index, byte_count, little_endian);
    uint64_t flags;
    if(is_64bit) flags = read_int(bytes, &index, byte_count, little_endian);
    uint64_t offset = is_64bit ? read_long(bytes, &index, byte_count, little_endian) : read_int(bytes, &index, byte_count, little_endian);
    uint64_t vaddr = is_64bit ? read_long(bytes, &index, byte_count, little_endian) : read_int(bytes, &index, byte_count, little_endian);
    uint64_t paddr = is_64bit ? read_long(bytes, &index, byte_count, little_endian) : read_int(bytes, &index, byte_count, little_endian);
    uint64_t size = is_64bit ? read_long(bytes, &index, byte_count, little_endian) : read_int(bytes, &index, byte_count, little_endian);
    uint64_t memsize = is_64bit ? read_long(bytes, &index, byte_count, little_endian) : read_int(bytes, &index, byte_count, little_endian);
    if(!is_64bit) flags = read_int(bytes, &index, byte_count, little_endian);
    uint64_t align = is_64bit ? read_long(bytes, &index, byte_count, little_endian) : read_int(bytes, &index, byte_count, little_endian);
    printf("%08x %04lx 0x%016lx 0x%016lx 0x%016lx %ld %ld %ld\n", type, flags, offset, vaddr, paddr, size, memsize, align);
  }
  
  printf("------\n");

  index = sh_start;
  for(int i = 0; i<she_count; i++) {
    uint32_t name = read_int(bytes, &index, byte_count, little_endian);
    uint32_t type = read_int(bytes, &index, byte_count, little_endian);
    uint64_t flags = is_64bit ? read_long(bytes, &index, byte_count, little_endian) : read_int(bytes, &index, byte_count, little_endian);
    uint64_t addr = is_64bit ? read_long(bytes, &index, byte_count, little_endian) : read_int(bytes, &index, byte_count, little_endian);
    uint64_t offset = is_64bit ? read_long(bytes, &index, byte_count, little_endian) : read_int(bytes, &index, byte_count, little_endian);
    uint64_t size = is_64bit ? read_long(bytes, &index, byte_count, little_endian) : read_int(bytes, &index, byte_count, little_endian);
    uint32_t link = read_int(bytes, &index, byte_count, little_endian);
    uint32_t info = read_int(bytes, &index, byte_count, little_endian);
    uint64_t addralign = is_64bit ? read_long(bytes, &index, byte_count, little_endian) : read_int(bytes, &index, byte_count, little_endian);
    uint64_t entsize = is_64bit ? read_long(bytes, &index, byte_count, little_endian) : read_int(bytes, &index, byte_count, little_endian);
    printf("% 3d %08x %04lx 0x%016lx 0x%016lx % 3ld %d %08x % 3ld %ld\n", name, type, flags, addr, offset, size, link, info, addralign, entsize);
    if(shstr_index == i) {
      uint64_t end = offset+size;
      for(uint64_t si = offset; si<end; si++) {
        if(bytes[si] == '\0') {
          printf("\n");
        }else {
          printf("%c", bytes[si]);
        }
      }
    }
  }
  return NULL;
}

static void* arch_prepare_data(void* loaded_data) {
  return NULL;
}

static struct ir_data arch_generate_ir(void* prepared_data) {
  return (struct ir_data) {
    .instruction_level = ir_instruction_level_high,
    .instructions = ir_instruction_create_list(NULL, 0x10000, 1024, ir_instruction_level_high),
    .symbol_table = ir_symbol_create_table(NULL),
    .type_table = NULL,
    .memory_page_start = NULL
  };
}

const arch_info arch_elf = (const arch_info){
  .load_data = arch_load_data,
  .prepare_data = arch_prepare_data,
  .generate_ir = arch_generate_ir,
};