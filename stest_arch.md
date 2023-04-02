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

Currently the only prefixes are the `memory` and `jump` prefix.

### `memory`

Prefix: `0b0010xxyy`

This prefix is used for [`readptr`](#readptr) and [`writeptr`](#writeptr).

`xx` is the size of the pointer size

`yy` is the size of the data size

### `jump`

Prefix: `0b0011xx0y`

This prefix is used for all jump instructions.

`xx` is the size of the offset/location operand.

`y` is indicating if the jump is an offset or a specific location, where `0` means its an offset.

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
- [`jump`](#jump) [`jmp`](#jmp) (`0bxx010000`)
- [`jump`](#jump) [`je`](#je) (`0bxx010001`)
- [`jump`](#jump) [`jne`](#jne) (`0bxx010010`)
- [`jump`](#jump) [`jle`](#jle) (`0bxx010011`)
- [`jump`](#jump) [`jlt`](#jlt) (`0bxx010100`)
- [`jump`](#jump) [`jbe`](#jbe) (`0bxx010101`)
- [`jump`](#jump) [`jbt`](#jbt) (`0bxx010110`)
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

Description:

Takes two numbers from the stack, ANDs them together and puts the result on the stack.

Stack: `b, a -> c`

`pop()` and `push()` are set by the size from the instruction format.

Operation:
```
a = pop()
b = pop()
v = a & b
push(v)
```

#### `not`

Opcode: `0bxx001100`

Description:

Takes a number from the stack, inverts all bits and puts the result on the stack.

Stack: `a -> b`

`pop()` and `push()` are set by the size from the instruction format.

Operation:
```
a = pop()
v = ~a
push(v)
```

#### `or`

Opcode: `0bxx001101`

Description:

Takes two numbers from the stack, ORs them together and puts the result on the stack.

Stack: `b, a -> c`

`pop()` and `push()` are set by the size from the instruction format.

Operation:
```
a = pop()
b = pop()
v = a | b
push(v)
```

#### `xor`

Opcode: `0bxx001110`

Description:

Takes two numbers from the stack, XORs them together and puts the result on the stack.

Stack: `b, a -> c`

`pop()` and `push()` are set by the size from the instruction format.

Operation:
```
a = pop()
b = pop()
v = a ^ b
push(v)
```

#### `tob`

Opcode: `0bxx001111`

Description:

Takes a number from the stack, and (unsigned) casts it to the size in the instruction.

Stack: `a -> b`

`pop()` is set by the size from the instruction format.

Operation:
```
a = pop()
v = a & 0xff
push(v)
```

#### `jmp`

Opcode: `0bxx010000`

This instruction requires the [`jump`](#jump) prefix.

Encoding:
`0bxx010000 imm` - where `imm` is the size of the offset (signed)/location from the jump prefix

Description:

Jump by some offset or to some specific location

Operation:
```
if jump.isoffset -> pc += imm
else pc = imm
```

#### `je`

Opcode: `0bxx010001`

This instruction requires the [`jump`](#jump) prefix.

Encoding:
`0bxx010000 imm` - where `imm` is the size of the offset (signed)/location from the jump prefix

Description:

Jump by some offset or to some specific location if the value is equal

`pop()` is set by the size from the instruction format.

Stack: `b, a -> `

Operation:
```
a = pop()
b = pop()
if a == b -> {
    if jump.isoffset -> pc += imm
    else pc = imm
}
```

#### `jne`

Opcode: `0bxx010010`

This instruction requires the [`jump`](#jump) prefix.

Encoding:
`0bxx010000 imm` - where `imm` is the size of the offset (signed)/location from the jump prefix

Description:

Jump by some offset or to some specific location if the value is not equal

`pop()` is set by the size from the instruction format.

Stack: `b, a -> `

Operation:
```
a = pop()
b = pop()
if a != b -> {
    if jump.isoffset -> pc += imm
    else pc = imm
}
```

#### `jle`

Opcode: `0bxx010011`

This instruction requires the [`jump`](#jump) prefix.

Encoding:
`0bxx010000 imm` - where `imm` is the size of the offset (signed)/location from the jump prefix

Description:

Jump by some offset or to some specific location if the value is less or equal than another (signed)

`pop()` is set by the size from the instruction format.

Stack: `b, a -> `

Operation:
```
signed a = pop()
signed b = pop()
if a <= b -> {
    if jump.isoffset -> pc += imm
    else pc = imm
}
```

#### `jlt`

Opcode: `0bxx010100`

This instruction requires the [`jump`](#jump) prefix.

Encoding:
`0bxx010000 imm` - where `imm` is the size of the offset (signed)/location from the jump prefix

Description:

Jump by some offset or to some specific location if the value is less than another (signed)

`pop()` is set by the size from the instruction format.

Stack: `b, a -> `

Operation:
```
signed a = pop()
signed b = pop()
if a < b -> {
    if jump.isoffset -> pc += imm
    else pc = imm
}
```

#### `jbe`

Opcode: `0bxx010101`

This instruction requires the [`jump`](#jump) prefix.

Encoding:
`0bxx010000 imm` - where `imm` is the size of the offset (signed)/location from the jump prefix

Description:

Jump by some offset or to some specific location if the value is below or equal than another (unsigned)

`pop()` is set by the size from the instruction format.

Stack: `b, a -> `

Operation:
```
unsigned a = pop()
unsigned b = pop()
if a <= b -> {
    if jump.isoffset -> pc += imm
    else pc = imm
}
```

#### `jbt`

Opcode: `0bxx010110`

This instruction requires the [`jump`](#jump) prefix.

Encoding:
`0bxx010000 imm` - where `imm` is the size of the offset (signed)/location from the jump prefix

Description:

Jump by some offset or to some specific location if the value is below than another (unsigned)

`pop()` is set by the size from the instruction format.

Stack: `b, a -> `

Operation:
```
unsigned a = pop()
unsigned b = pop()
if a < b -> {
    if jump.isoffset -> pc += imm
    else pc = imm
}
```

#### `tow`

Opcode: `0bxx010111`

Description:

Takes a number from the stack, and (unsigned) casts it to the size in the instruction.

Stack: `a -> b`

`pop()` is set by the size from the instruction format.

Operation:
```
a = pop()
v = a & 0xffff
push(v)
```

#### `tod`

Opcode: `0bxx011000`

Description:

Takes a number from the stack, and (unsigned) casts it to the size in the instruction.

Stack: `a -> b`

`pop()` is set by the size from the instruction format.

Operation:
```
a = pop()
v = a & 0xffffffff
push(v)
```

#### `toq`

Opcode: `0bxx011001`

Description:

Takes a number from the stack, and (unsigned) casts it to the size in the instruction.

Stack: `a -> b`

`pop()` is set by the size from the instruction format.

Operation:
```
a = pop()
v = a & 0xffffffffffffffff
push(v)
```

#### `shl`

Opcode: `0bxx011010`

Description:

Takes two numbers from the stack, and shifts a number to the left by some amount.

The shift count is a 1 byte value.

Stack: `b, a -> c`

`pop()` and `push()` are set by the size from the instruction format.

Operation:
```
a = pop()
b = pop(1)
v = a << b
push(v)
```

#### `shr`

Opcode: `0bxx011011`

Description:

Takes two numbers from the stack, and shifts a number to the right by some amount.

The shift count is a 1 byte value.

Stack: `b, a -> c`

`pop()` and `push()` are set by the size from the instruction format.

Operation:
```
a = pop()
b = pop(1)
v = a >> b
push(v)
```

#### `neg`

Opcode: `0bxx011100`

Description:

Takes a number from the stack, negates the numbers and puts it back on the stack.

Stack: `a -> b`

`pop()` and `push()` are set by the size from the instruction format.

Operation:
```
a = pop()
v = -a
push(v)
```

