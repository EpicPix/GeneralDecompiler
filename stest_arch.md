# (Stack) Test Arch

## Instruction Format

Bit sized operations will have the operand size stored in the top 2 bits of the opcode. There are 4 different sizes:
- 8 bit (`0b00xxxxxxx`)
- 16 bit (`0b01xxxxxxx`)
- 32 bit (`0b10xxxxxxx`)
- 64 bit (`0b11xxxxxxx`)

## Instructions

There will be some basic instructions stored in the first 5 bits of the opcode, which makes the maximum count of instruction types only 32, which is enough for this architecture.

These will be the instructions:
- (`add`)[#add] (`0bxxx00001`)
- (`sub`)[#sub] (`0bxxx00010`)
- (`mul`)[#mul] (`0bxxx00011`)
- (`div`)[#div] (`0bxxx00100`)
- (`mod`)[#mod] (`0bxxx00101`)
- (`push`)[#push] (`0bxxx00110`)
- (`drop`)[#drop] (`0bxxx00111`)
- (`dup`)[#dup] (`0bxxx01000`)
- (`readptr`)[#readptr] (`0bxxx01001`)
- (`writeptr`)[#writeptr] (`0bxxx01010`)
- (`and`)[#and] (`0bxxx01011`)
- (`not`)[#not] (`0bxxx01100`)
- (`or`)[#or] (`0bxxx01101`)
- (`xor`)[#xor] (`0bxxx01110`)
- (`tob`)[#tob] (`0bxxx01111`)
- (`jmp`)[#jmp] (`0bxxx10000`)
- (`je`)[#je] (`0bxxx10001`)
- (`jne`)[#jne] (`0bxxx10010`)
- (`jle`)[#jle] (`0bxxx10011`)
- (`jlt`)[#jlt] (`0bxxx10100`)
- (`jbe`)[#jbe] (`0bxxx10101`)
- (`jbt`)[#jbt] (`0bxxx10110`)
- (`tow`)[#tow] (`0bxxx10111`)
- (`tod`)[#tod] (`0bxxx11000`)
- (`toq`)[#toq] (`0bxxx11001`)
- (`shl`)[#shl] (`0bxxx11010`)
- (`shr`)[#shr] (`0bxxx11011`)
- (`neg`)[#neg] (`0bxxx11100`)
- not defined (`0bxxx11101`)
- not defined (`0bxxx11110`)
- not defined (`0bxxx11111`)

### Instruction Operations

#### `add`

Opcode: `0bxxx00001`

#### `sub`

Opcode: `0bxxx00010`

#### `mul`

Opcode: `0bxxx00011`

#### `div`

Opcode: `0bxxx00100`

#### `mod`

Opcode: `0bxxx00101`

#### `push`

Opcode: `0bxxx00110`

#### `drop`

Opcode: `0bxxx00111`

#### `dup`

Opcode: `0bxxx01000`

#### `readptr`

Opcode: `0bxxx01001`

#### `writeptr`

Opcode: `0bxxx01010`

#### `and`

Opcode: `0bxxx01011`

#### `not`

Opcode: `0bxxx01100`

#### `or`

Opcode: `0bxxx01101`

#### `xor`

Opcode: `0bxxx01110`

#### `tob`

Opcode: `0bxxx01111`

#### `jmp`

Opcode: `0bxxx10000`

#### `je`

Opcode: `0bxxx10001`

#### `jne`

Opcode: `0bxxx10010`

#### `jle`

Opcode: `0bxxx10011`

#### `jlt`

Opcode: `0bxxx10100`

#### `jbe`

Opcode: `0bxxx10101`

#### `jbt`

Opcode: `0bxxx10110`

#### `tow`

Opcode: `0bxxx10111`

#### `tod`

Opcode: `0bxxx11000`

#### `toq`

Opcode: `0bxxx11001`

#### `shl`

Opcode: `0bxxx11010`

#### `shr`

Opcode: `0bxxx11011`

#### `neg`

Opcode: `0bxxx11100`

