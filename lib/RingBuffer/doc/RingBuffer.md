# `RingBuffer` Library

This library provides a toolbox of different ring buffer structures. Because
this environment lacks any form of run-time memory allocation, each class type
is specifically tailored to its use case. However, I have made them as generic
as possible, so adjusting specific classes for use is a minimum of effort.

This should really be a generic library, but I don't know how to do that. I
tried to make an abstract base class, but that doesn't permit type variance.

So, all ring buffers conform to the same API, but by choice and trust, not by
force.

## Structure

Ring buffers consist of a flat storage array of their specific size and type.
They also possess two pointers, `head` and `tail`, and a state flag.

## Behavior

When writing to the buffer, the new value is stored in `tail`'s target (if the
buffer is self-overwriting OR non-full). When reading from the buffer, the given
storage variable is populated if the buffer is non-empty. Reading from an
empty buffer returns `-1`; a successful read returns `0`.

(This feature is not yet implemented; it currently returns a default value for
failure.)

## API

Specific classes of `RingBuffer` are suffixed with a `_TYPE`, where TYPE is most
of a type name.

Established classes:

- `RingBuffer_us` -- Used for the Ultrasonic sensors. Each has 6 storage.

###### Constructor

```cpp
RingBuffer_TYPE::RingBuffer_TYPE(bool state = false);
```

The state flag sets default overwriting behavior. `head` and `tail` are set to
the `[0]` cell and ready for writing.

```cpp
//  Non-overwriting
RingBuffer_uint8 example;  //  [ht][][][]...[][][][]]
```

###### Writer

```cpp
void RingBuffer_TYPE::write(TYPE val);
```

This (probably) stores the given value in the buffer. At present, if it fails,
no signal is returned. It will write if the buffer is set to overwrite or if it
is not full.

```cpp
//  example.is_empty() => true
example.write(1);  //  [h1]t[][][]...[][][][]
//  example.is_empty() => false
example.write(2);  //  [h1][2][t][]...[][][][]
```

###### Reader

```cpp
TYPE RingBuffer_TYPE::read(void);
```

Returns the value in the `head` slot, if the buffer is non-empty. Returns a
default value if empty, so as not to return garbage.

```cpp
uint8_t t;
t = example.read();  //  [1][h2][t][]...[][][][]
//  t == 1
//  example.is_empty() => false
t = example.read();  //  [1][2][ht][]...[][][][]
//  t == 2
//  example.is_empty() => true
```

###### Global Reader

```cpp
TYPE RingBuffer_TYPE::read_all(char op);
```

This function is defined on some buffer types and performs an arithmetic or
logical operation on the buffer's contents in sequence. It also sets `head` and
`tail` to the beginning, effectively resetting the buffer for single read/write.

**Implementations**

- `RingBuffer_bool` performs the following logical operations:
	- OR (`|`)
	- AND (`&`)
	- XOR (`^`)

```cpp
RingBuffer_us example;
example.write(true); example.write(false);
bool tmp;
tmp = example.read_all('|');  //=> true
tmp = example.read_all('&');  //=> false
tmp = example.read_all('^');  //=> true
```

###### Override Mode

```cpp
void RingBuffer_TYPE::set_overwrite(bool state = true);
```

Sets the overwrite status. When `true`, the buffer behaves as an infinite-size
storage, with finite recall. Single reads from an overwrite-enabled buffer
are not guaranteed to work properly. Infinite buffers should be used for
`read_all` operations.

```cpp
example.set_overwrite(true);  //  infinite buffer
example.set_overwrite(false);  //  proper queue
```

###### PROTECTED METHODS

```cpp
bool RingBuffer_TYPE::is_full(void);
```

Returns whether or not the buffer is full. Meaningless on infinite buffers.

```cpp
bool RingBuffer_TYPE::is_empty(void);
```

Returns whether or not the buffer is empty. Meaningless on infinite buffers.
