# (Stack) Test Arch

This architecture is little endian, any immediate or operand that is multi-byte will be written in ltitle-endian encoding.

## Instruction Format

Bit sized operations will have the operand size stored in the top 2 bits of the opcode. There are 4 different sizes:
- 8 bit (`0b00xxxxxxx`)
- 16 bit (`0b01xxxxxxx`)
- 32 bit (`0b10xxxxxxx`)
- 64 bit (`0b11xxxxxxx`)

## Instructions

There will be some basic instructions stored in the first 5 bits of the opcode, which makes the maximum count of instruction types only 32, which is enough for this architecture.

These will be the instructions:
- not defined (`0bxxx00000`)
- [`add`](#add) (`0bxxx00001`)
- [`sub`](#sub) (`0bxxx00010`)
- [`mul`](#mul) (`0bxxx00011`)
- not defined (`0bxxx00100`)
- not defined (`0bxxx00101`)
- [`push`](#push) (`0bxxx00110`)
- [`drop`](#drop) (`0bxxx00111`)
- [`dup`](#dup) (`0bxxx01000`)
- [`readptr`](#readptr) (`0bxxx01001`)
- [`writeptr`](#writeptr) (`0bxxx01010`)
- [`and`](#and) (`0bxxx01011`)
- [`not`](#not) (`0bxxx01100`)
- [`or`](#or) (`0bxxx01101`)
- [`xor`](#xor) (`0bxxx01110`)
- [`tob`](#tob) (`0bxxx01111`)
- [`jmp`](#jmp) (`0bxxx10000`)
- [`je`](#je) (`0bxxx10001`)
- [`jne`](#jne) (`0bxxx10010`)
- [`jle`](#jle) (`0bxxx10011`)
- [`jlt`](#jlt) (`0bxxx10100`)
- [`jbe`](#jbe) (`0bxxx10101`)
- [`jbt`](#jbt) (`0bxxx10110`)
- [`tow`](#tow) (`0bxxx10111`)
- [`tod`](#tod) (`0bxxx11000`)
- [`toq`](#toq) (`0bxxx11001`)
- [`shl`](#shl) (`0bxxx11010`)
- [`shr`](#shr) (`0bxxx11011`)
- [`neg`](#neg) (`0bxxx11100`)
- not defined (`0bxxx11101`)
- not defined (`0bxxx11110`)
- not defined (`0bxxx11111`)

### Instruction Operations

#### `add`

Opcode: `0bxxx00001`

Description:

Takes two numbers from the stack, adds them together and puts the result on the stack.

Stack: `a, b -> c`

`pop()` and `push()` are set by the size from the instruction format.

Operation:
```
a = pop()
b = pop()
v = a + b
push(v)
```

#### `sub`

Opcode: `0bxxx00010`

Description:

Takes two numbers from the stack, subtracts them and puts the result on the stack.

Stack: `a, b -> c`

`pop()` and `push()` are set by the size from the instruction format.

Operation:
```
a = pop()
b = pop()
v = a - b
push(v)
```

#### `mul`

Opcode: `0bxxx00011`

Description:

Takes two numbers from the stack, multiplies them together and puts the result on the stack.

Stack: `a, b -> c`

`pop()` and `push()` are set by the size from the instruction format.

Operation:
```
a = pop()
b = pop()
v = a * b
push(v)
```

#### `push`

Opcode: `0bxxx00110`

Encoding:
- `0b00x00110 imm8` - Push a 1 byte number from instruction immediate
- `0b01x00110 imm16` - Push a 2 byte number from instruction immediate
- `0b10x00110 imm32` - Push a 4 byte number from instruction immediate
- `0b11x00110 imm64` - Push a 8 byte number from instruction immediate

Description:

Gets the immediate from the instruction, then pushes it on the stack.

Stack: ` -> imm`

`push()` is set by the size from the instruction format.

Operation:
```
push(imm)
```

#### `drop`

Opcode: `0bxxx00111`

Description:

Takes a number from the stack and discards it.

Stack: `a -> `

`pop()` is set by the size from the instruction format.

Operation:
```
pop()
```

#### `dup`

Opcode: `0bxxx01000`

Description:

Takes a number from the stack and pushes the same value twice.

Stack: `a -> a, a`

`pop()` and `push()` are set by the size from the instruction format.

Operation:
```
a = pop()
push(a)
push(a)
```

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

