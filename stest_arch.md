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
- `add` (`0bxxx00001`)
- `sub` (`0bxxx00010`)
- `mul` (`0bxxx00011`)
- `div` (`0bxxx00100`)
- `mod` (`0bxxx00101`)
- `push` (`0bxxx00110`)
- `drop` (`0bxxx00111`)
- `dup` (`0bxxx01000`)
- `readptr` (`0bxxx01001`)
- `writeptr` (`0bxxx01010`)
- `and` (`0bxxx01011`)
- `not` (`0bxxx01100`)
- `or` (`0bxxx01101`)
- `xor` (`0bxxx01110`)
- `tob` (`0bxxx01111`)
- `jmp` (`0bxxx10000`)
- `je` (`0bxxx10001`)
- `jne` (`0bxxx10010`)
- `jle` (`0bxxx10011`)
- `jlt` (`0bxxx10100`)
- `jbe` (`0bxxx10101`)
- `jbt` (`0bxxx10110`)
- `tow` (`0bxxx10111`)
- `tod` (`0bxxx11000`)
- `toq` (`0bxxx11001`)
- `shl` (`0bxxx11010`)
- `shr` (`0bxxx11011`)
- `neg` (`0bxxx11100`)
- not defined (`0bxxx11101`)
- not defined (`0bxxx11110`)
- not defined (`0bxxx11111`)

### Instruction Operations

#### `add`

#### `sub`

#### `mul`

#### `div`

#### `mod`

#### `push`

#### `drop`

#### `readptr`

#### `writeptr`

#### `and`

#### `not`

#### `or`

#### `xor`

#### `tob`

#### `jmp`

#### `je`

#### `jne`

#### `jle`

#### `jlt`

#### `jbe`

#### `jbt`

#### `tow`

#### `tod`

#### `toq`

#### `shl`

#### `shr`

#### `neg`

