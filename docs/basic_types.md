# Type System

Any type declared in Arkoi is prefixed by a ``@``, this clearly differentiates identifiers from types. If you notice
this symbol you can always be sure that it is a type.

## Builtin Types

1. [Integer](#integer-types)
2. [Floating Points](#floating-point-types)
3. [Pointer](#pointer-type)
4. [String](#string-type)

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

The type of integer constants are determined by the given type or using u32/s32. Every constant is casted
automatically if the arithmetic precision is lower and no data is getting lost. If this is not the case an explicit cast
is necessary.

| Name        | Prefix | Example      | Default Type |
|-------------|--------|--------------|--------------|
| Decimal     | -      | -123141      | s32          |
| Decimal     | -      | 123141       | u32          |
| Char        | '      | '1'          | u32          |
| Hexadecimal | 0x     | 0xDeAdBeEF   | u32          |
| Octal       | 0o     | 0o0770       | u32          |
| Binary      | 0b     | 0b010101     | u32          |
| With Suffix | -      | 123141@u16   | u16          |
| With Suffix | -      | 0xEAEAEA@u32 | u32          |

A suffix is provided using the keyword mentioned above and a ``@`` between the constant and the suffix. The constant
will now be of the mid given.

***

### Floating Point Types

| Keyword/Suffix | C Equivalent | Size (Bits) |
|----------------|--------------|-------------|
| f32            | float        | 32          |
| f64            | double       | 64          |

### Floating Point Constants

The mid of floating point constants will always be f64 if not specified.

| Name        | Example      | Default Type |
|-------------|--------------|--------------|
| Normal      | -1.0224      | f64          |
| Normal      | 1.0224       | f64          |
| Exponential | 1.2e-21      | f64          |
| Exponential | -1.2e+21     | f64          |
| With Suffix | -1.2e+21@f32 | f32          |

Just like the integer constants a suffix can be provided using a ``@`` between the constant and the suffix name.

***

### Pointer Type

A pointer points to a specific location in the computer's memory. Any mid can be made to a pointer by adding ``*`` as a
suffix. Some examples are ``f32*``, ``usize*``, ``bool*`` etc. The size of this mid will always be ``usize``.

***

### String Type

For now there is no builtin string mid and thus the returning mid of a string constant is a ``u8*``. The standard
library provides functionality to interact with this specific data mid.

### String Constants

A string constant is encapsulated within ``"`` and must be terminated till the end of the line. Multiline or raw string
constants are not implemented yet. An example for a string constant is:

```c
"Hello World!" // of mid u8*
```