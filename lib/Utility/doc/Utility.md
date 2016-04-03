# `Utility` module

This module provides a small set of utility types and functions for use in the
rest of the environment. It can be `#include`d in any file, and has no stateful
code.

## Contents

### Types

###### `gpio_t`

This is an alias for `uint8_t` and helps distinguish port numbers, which are
numeric literals on Arduino, from regular integral data.

```cpp
gpio_t example_pin = 40;
```

###### `iopair_t`

This is a `struct` containing two `gpio_t` elements. It is designed to simplify
keeping track of port assignments for a peripheral that uses two ports for
duplex communication. The two elements are named `tx` and `rx`, and indicate the
signal direction relative to the Arduino. `tx` is outbound, `rx` is inbound.

```cpp
iopair_t example_pair = { .tx = 2, .rx = 3 };
```

### Functions

###### `void pinsMode(iopair_t pair)`

I disagree with the naming scheme used throughout the Arduino libraries, but use
it here to emphasize that this is simply a wrapper around standard Arduino
functionality. This takes an `iopair_t` struct and sets the appropriate
`pinMode` for each.

```cpp
iopairMode(example_pair);
//  pinMode(example_pair.tx, OUTPUT);
//  pinMode(example_pair.rx, INPUT);
```

###### `int16_t uint8x2_to_int16(uint8_t l, uint8_t h)`

Transforms two `uint8_t` numbers into a SIGNED `int16_t` number. Parameters are
in little-endian order: `l` is the low half, `h` is the high half.

```cpp
uint8x2_to_int16(0xCD/*205*/, 0xAB/*171*/);
//  Returns 0xABCD (-11_213)
```

###### `uint8_t parse_hex(char c)`

Parses an alphanumeric character into the number it represents. NOTE: This only
accepts the ASCII characters '0' through '9', 'A' through 'F', and 'a' through
'f'. Any other characters return 0. There is no error signal; input checking is
on the user.

```cpp
parse_hex('6');
//  Returns 0x06 (6)
parse_hex('A');
//  Returns 0x0A (10)
parse_hex('d');
//  Returns 0x0D (13)
parse_hex('~');
//  Returns 0x00 (0)
```

###### `uint8_t parse_hex(char l, char h)`

Parses two alphanumeric characters into the number they represent. This has the
same behavior as above: only 0-9, A-F, and a-f are permitted, and errors return
0 in their nibble with no signal. Parameters are in little-endian order.

```cpp
parse_hex('c', '4');
//  Returns 0x4C
parse_hex('7', 'O'); // Note that is upper-case Oscar, not zero
//  Returns 0x07
```

### Macros

###### `DEBUG_SIZE_1(x)`

Prints the `sizeof` its argument. Requires `Serial` to have been initialized.
Conditional upon `#ifdef DEVEL`.

```cpp
#define DEVEL
DEBUG_SIZE_1(gpio_t);
//  Prints '1'
DEBUG_SIZE_1(iopair_t);
//  Prints '2'
#undef  DEVEL
```

###### `DEBUG_SIZE_2(x,y)`

Prints the difference between the `sizeof` x and `sizeof` y. Useful for checking
the actual size of a wrapper class without the object it is wrapping. Requires
`Serial` to have been initialized. Conditional upon `#ifdef DEVEL`.

```cpp
#define DEVEL
DEBUG_SIZE_2(iopair_t, gpio_t);
//  Prints '1'
#undef  DEVEL
```
