#include "slice.h"

#include <stdlib.h>
#include <string.h>

struct slice slice_make(const char *buf, size_t len)
{
    struct slice slice;
    slice.buf = buf;
    slice.len = len;
    return slice;
}

struct slice_mutable slice_mutable_make(char *buf, size_t len)
{
    struct slice_mutable slice;
    slice.buf = buf;
    slice.len = len;
    return slice;
}

struct slice slice_from_string(const char *str)
{
    return slice_make(str, str ? strlen(str) : 0);
}

struct slice slice_from_mutable(const struct slice_mutable m)
{
    return slice_make(m.buf, m.len);
}

const char *slice_begin(const struct slice slice)
{
    return slice.buf;
}

const char *slice_end(const struct slice slice)
{
    return slice.buf ? slice.buf + slice.len : slice.buf;
}

char *slice_mutable_begin(const struct slice_mutable slice)
{
    return slice.buf;
}

char *slice_mutable_end(const struct slice_mutable slice)
{
    return slice.buf ? slice.buf + slice.len : slice.buf;
}

ssize_t slice_offset(const struct slice outer, const struct slice inner)
{
    if (!outer.buf || !inner.buf || inner.buf < outer.buf || inner.buf + inner.len > outer.buf + outer.len) {
        return -1;
    }

    return inner.buf - outer.buf;
}

ssize_t slice_find(const struct slice haystack, const struct slice needle)
{
    if (!haystack.buf || !needle.buf) {
        return -1;
    }

    if (needle.len <= haystack.len) {
        size_t pos = 0;
        size_t end = haystack.len - needle.len;

        for (; pos <= end; ++pos) {
            if (memcmp(haystack.buf + pos, needle.buf, needle.len) == 0) {
                return (ssize_t)pos;
            }
        }
    }

    return -1;
}

int slice_compare(const struct slice slice, const struct slice other)
{
    size_t n;
    int r;

    n = slice.len < other.len ? slice.len : other.len;
    if (n) {
        r = memcmp(slice.buf, other.buf, n);
        if (r) {
            return r;
        }
    }

    return (slice.len > other.len) - (slice.len < other.len);
}

bool slice_equal(const struct slice slice, const struct slice other)
{
    return slice_compare(slice, other) == 0;
}

char *slice_strcpy(char *str, size_t size, const struct slice slice)
{
    if (!str || size <= slice.len || !slice.buf) {
        return NULL;
    }

    if (slice.len) {
        memcpy(str, slice.buf, slice.len);
    }

    str[slice.len] = '\0';
    return str;
}

char *slice_strdup(const struct slice slice)
{
    char *str;

    if (!slice.buf) {
        return NULL;
    }

    if (slice.len == (size_t)-1) {
        /* Overflow. */
        return NULL; // UNREACHABLE
    }

    str = malloc(slice.len + 1);
    if (!str) {
        return NULL; // UNREACHABLE
    }

    return slice_strcpy(str, slice.len + 1, slice);
}
