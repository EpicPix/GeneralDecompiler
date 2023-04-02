# (Stack) Test Arch

This architecture is little endian, any immediate or operand that is multi-byte will be written in ltitle-endian encoding.

## Instruction Format

Bit sized operations will have the operand size stored in the top 2 bits of the opcode. There are 4 different sizes:
- 8 bit (`0b00xxxxxxx`)
- 16 bit (`0b01xxxxxxx`)
- 32 bit (`0b10xxxxxxx`)
- 64 bit (`0b11xxxxxxx`)

## Instruction Prefixes

A prefix is defined when `0bxx1xxxxx` matches.

Currently the only prefix is a memory prefix, which lets the next instruction read/write from memory at a certain pointer.

### `memory`

Prefix: `0b0010xxyy`

`xx` is the size of the pointer size

`yy` is the size of the data size

## Instructions

There will be some basic instructions stored in the first 5 bits of the opcode, which makes the maximum count of instruction types only 32, which is enough for this architecture.

These will be the instructions:
- not defined (`0bxx000000`)
- [`add`](#add) (`0bxx000001`)
- [`sub`](#sub) (`0bxx000010`)
- [`mul`](#mul) (`0bxx000011`)
- not defined (`0bxx000100`)
- not defined (`0bxx000101`)
- [`push`](#push) (`0bxx000110`)
- [`drop`](#drop) (`0bxx000111`)
- [`dup`](#dup) (`0bxx001000`)
- [`memory`](#memory) [`readptr`](#readptr) (`0bxx001001`)
- [`memory`](#memory) [`writeptr`](#writeptr) (`0bxx001010`)
- [`and`](#and) (`0bxx001011`)
- [`not`](#not) (`0bxx001100`)
- [`or`](#or) (`0bxx001101`)
- [`xor`](#xor) (`0bxx001110`)
- [`tob`](#tob) (`0bxx001111`)
- [`jmp`](#jmp) (`0bxx010000`)
- [`je`](#je) (`0bxx010001`)
- [`jne`](#jne) (`0bxx010010`)
- [`jle`](#jle) (`0bxx010011`)
- [`jlt`](#jlt) (`0bxx010100`)
- [`jbe`](#jbe) (`0bxx010101`)
- [`jbt`](#jbt) (`0bxx010110`)
- [`tow`](#tow) (`0bxx010111`)
- [`tod`](#tod) (`0bxx011000`)
- [`toq`](#toq) (`0bxx011001`)
- [`shl`](#shl) (`0bxx011010`)
- [`shr`](#shr) (`0bxx011011`)
- [`neg`](#neg) (`0bxx011100`)
- not defined (`0bxx011101`)
- not defined (`0bxx011110`)
- not defined (`0bxx011111`)

### Instruction Operations

#### `add`

Opcode: `0bxx000001`

Description:

Takes two numbers from the stack, adds them together and puts the result on the stack.

Stack: `b, a -> c`

`pop()` and `push()` are set by the size from the instruction format.

Operation:
```
a = pop()
b = pop()
v = a + b
push(v)
```

#### `sub`

Opcode: `0bxx000010`

Description:

Takes two numbers from the stack, subtracts them and puts the result on the stack.

Stack: `b, a -> c`

`pop()` and `push()` are set by the size from the instruction format.

Operation:
```
a = pop()
b = pop()
v = a - b
push(v)
```

#### `mul`

Opcode: `0bxx000011`

Description:

Takes two numbers from the stack, multiplies them together and puts the result on the stack.

Stack: `b, a -> c`

`pop()` and `push()` are set by the size from the instruction format.

Operation:
```
a = pop()
b = pop()
v = a * b
push(v)
```

#### `push`

Opcode: `0bxx000110`

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

Opcode: `0bxx000111`

Description:

Takes a number from the stack and discards it.

Stack: `a -> `

`pop()` is set by the size from the instruction format.

Operation:
```
pop()
```

#### `dup`

Opcode: `0bxx001000`

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

Opcode: `0bxx001001`

This instruction requires the [`memory`](#memory) prefix.

Description:

Takes a pointer from the stack, and gets the data from that pointer and pushes it on the stack.

Stack `ptr -> val`

Operation:
```
ptr = pop(memory_prefix.pointer_size)
val = *ptr
push(val, memory_prefix.data_size)
```

#### `writeptr`

Opcode: `0bxx001010`

This instruction requires the [`memory`](#memory) prefix.

Description:

Takes a pointer from the stack and a value, and sets the data at that pointer to the value.

Stack `val, ptr -> `

Operation:
```
ptr = pop(memory_prefix.pointer_size)
val = pop(memory_prefix.data_size)
*ptr = val
```

#### `and`

Opcode: `0bxx001011`

#### `not`

Opcode: `0bxx001100`

#### `or`

Opcode: `0bxx001101`

#### `xor`

Opcode: `0bxx001110`

#### `tob`

Opcode: `0bxx001111`

#### `jmp`

Opcode: `0bxx010000`

#### `je`

Opcode: `0bxx010001`

#### `jne`

Opcode: `0bxx010010`

#### `jle`

Opcode: `0bxx010011`

#### `jlt`

Opcode: `0bxx010100`

#### `jbe`

Opcode: `0bxx010101`

#### `jbt`

Opcode: `0bxx010110`

#### `tow`

Opcode: `0bxx010111`

#### `tod`

Opcode: `0bxx011000`

#### `toq`

Opcode: `0bxx011001`

#### `shl`

Opcode: `0bxx011010`

#### `shr`

Opcode: `0bxx011011`

#### `neg`

Opcode: `0bxx011100`

