# Test Arch

This architecture is stack based.

## Instruction Format

Bit sized operations will have the operand size stored in the top 2 bits of the opcode. There are 4 different sizes:
- 8 bit (`0b00xxxxxxx`)
- 16 bit (`0b01xxxxxxx`)
- 32 bit (`0b10xxxxxxx`)
- 64 bit (`0b11xxxxxxx`)

## Instructions

There will be some basic instructions stored in the first 5 bits of the opcode, which makes the maximum count of instruction types only 32, which is enough for this architecture.

These will be the instructions:
- `add` (`0bxxx00001`)
- `sub` (`0bxxx00010`)
- `mul` (`0bxxx00011`)
- `div` (`0bxxx00100`)
- `mod` (`0bxxx00101`)
- `push` (`0bxxx00110`)
- `drop` (`0bxxx00111`)
- `dup` (`0bxxx01000`)


