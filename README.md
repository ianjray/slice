# slice
C library providing immutable and mutable, non-owning, slices of byte sequences.

A slice is a pointer and a length.
It does not own the memory it refers to, does not require a NUL terminator, and can represent data containing embedded NULs.

## Features

* Immutable and mutable slices
* Non-owning views of existing memory
* Explicit length; NUL termination is not required
* Supports embedded NULs
* Lexicographical slice comparison
* Searching for a slice or C string
* Copying slices to allocated or caller-provided strings

## Example

```c
#include <assert.h>
#include <libslice/slice.h>

int main(void)
{
    const char data[] = { 'a', '\0', 'b' };
    char buf[4];
    struct slice s;
    struct slice_mutable m;

    /* A slice does not copy or own its data. */
    s = slice_make(data, sizeof(data));

    assert(slice_begin(s) == data);
    assert(slice_end(s) == data + sizeof(data));
    assert(s.len == sizeof(data));

    /* Slices are length-delimited and can contain embedded NULs. */
    assert(1 == slice_find(s, slice_make("\0b", 2)));

    /* Lexicographical comparison. */
    assert(slice_compare(slice_from_string("abc"), slice_from_string("abd")) < 0);

    /* Copy a slice to a NUL-terminated string. */
    assert(buf == slice_strcpy(buf, sizeof(buf), s));
    assert(buf[0] == 'a');
    assert(buf[1] == '\0');
    assert(buf[2] == 'b');
    assert(buf[3] == '\0');

    /* Mutable slices can be converted to immutable slices. */
    m = slice_mutable_make(buf, sizeof(buf));
    s = slice_from_mutable(m);

    assert(slice_begin(s) == slice_mutable_begin(m));
    assert(s.len == m.len);
}
```

## API

### Creating slices

```c
struct slice slice_make(const char *, size_t);
struct slice_mutable slice_mutable_make(char *, size_t);

struct slice slice_from_string(const char *);
struct slice slice_from_mutable(struct slice_mutable);
```

`slice_make()` and `slice_mutable_make()` create non-owning views over existing memory.

`slice_from_string()` creates a slice from a NUL-terminated C string.
The terminating NUL is not part of the slice.

### Accessing slices

```c
const char *slice_begin(struct slice);
const char *slice_end(struct slice);

char *slice_mutable_begin(struct slice_mutable);
char *slice_mutable_end(struct slice_mutable);
```

The end pointer refers one past the last byte of the slice.

### Comparing and searching

```c
ssize_t slice_offset(struct slice outer, struct slice inner);

ssize_t slice_find(struct slice haystack, struct slice needle);

int slice_compare(struct slice, struct slice);
bool slice_equal(struct slice, struct slice);
```

`slice_offset()` returns the zero-based byte offset of `inner` within `outer`, or `-1` if `inner` is not contained.

`slice_find()` returns the zero-based position of the first occurrence, or `-1` if the needle is not found.

`slice_compare()` performs lexicographical comparison.

`slice_equal()` returns `true` if the byte sequences are identical.

### Copying

```c
char *slice_strcpy(char *, size_t, struct slice);
char *slice_strdup(struct slice);
```

`slice_strcpy()` copies a slice to a caller-provided buffer and adds a terminating NUL.

`slice_strdup()` allocates a new NUL-terminated string containing the slice.
The caller owns the returned memory and must free it.

## Installation

```sh
./configure
make
sudo make install
```

A `pkg-config` file is installed, so applications can use:

```sh
cc $(pkg-config --cflags --libs slice) ...
```

## Requirements

* C99 or later
* POSIX-compatible system

## Thread Safety

The library does not maintain global or shared mutable state.
