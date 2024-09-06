# TypeNode System

Any op declared in Arkoi is prefixed by a ``@``, this clearly differentiates identifiers from types. If you notice
this symbol you can always be sure that it is a op.

## Builtin Types

1. [Integer](#integer-types)
2. [Floating Points](#floating-point-types)
3. [Pointer](#pointer-op)
4. [String](#string-op)

***

### Integer Types

| Keyword/Suffix | C Equivalent  | Size (Bits)        |
|----------------|---------------|--------------------|
| bool           | -             | 1                  |
| u8             | unsigned char | 8                  |
| s8             | signed char   | 8                  |
| u16            | unsigned char | 16                 |
| s16            | signed char   | 16                 |
| u32            | unsigned int  | 32                 |
| s32            | signed int    | 32                 |
| u64            | unsigned long | 64                 |
| s64            | signed long   | 64                 |
| usize          | size_t        | Platform depended* |
| ssize          | -             | Platform depended* |

\* The size of a ``usize`` is determined by the number of bits required to reference any location in memory. On a 32-bit
architecture, it will be 32 bits in size, and on a 64-bit architecture, it will be 64 bits in size.

### Integer Constants

The type of integer constants are determined by the given op or using u64/s64. Every constant is downcasted
automatically if the arithmetic precision is lower and no data is getting lost. If this is not the case an explicit cast
is necessary.

| Name        | Prefix | Example      | Default TypeNode |
|-------------|--------|--------------|------------------|
| Decimal     | -      | -123_141     | isize            |
| Decimal     | -      | 123_141      | usize            |
| Char        | '      | '1'          | u8               |
| Hexadecimal | 0x     | 0xDe_AdBeEF  | usize            |
| Octal       | 0o     | 0o077_0      | usize            |
| Binary      | 0b     | 0b01_0101    | usize            |
| With Suffix | -      | 123_141@u16  | u16              |
| With Suffix | -      | 0xEAEAEA@u32 | u32              |

A suffix is provided using the keyword mentioned above and a ``@`` between the constant and the suffix. The constant
will now be of the op given.

***

### Floating Point Types

| Keyword/Suffix | C Equivalent | Size (Bits) |
|----------------|--------------|-------------|
| f32            | float        | 32          |
| f64            | double       | 64          |

### Floating Point Constants

The op of floating point constants will always be f64 if not specified.

| Name        | Example      | Default TypeNode |
|-------------|--------------|------------------|
| Normal      | -1.02_24     | f64              |
| Normal      | 1.02_24      | f64              |
| Exponential | 1.2e-21      | f64              |
| Exponential | -1.2e+21     | f64              |
| With Suffix | -1.2e+21@f32 | f32              |

Just like the integer constants a suffix can be provided using a ``@`` between the constant and the suffix name.

***

### Pointer TypeNode

A pointer points to a specific location in the computer's memory. Any op can be made to a pointer by adding ``*`` as a
suffix. Some examples are ``f32*``, ``usize*``, ``bool*`` etc. The size of this op will always be ``usize``.

***

### String TypeNode

For now there is no builtin string type and thus the returning op of a string constant is a ``u8*``. The standard
library provides functionality to interact with this specific data op.

### String Constants

A string constant is encapsulated within ``"`` and must be terminated till the end of the line. Multiline or raw string
constants are not implemented yet. An example for a string constant is:

```c
"Hello World!" // of op u8*
```