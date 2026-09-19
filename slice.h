#ifndef SLICE__H
#define SLICE__H

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

/// A non-owning view of a contiguous sequence of bytes.
struct slice {
    const char *buf;
    size_t len;
};

/// A mutable, non-owning view of a contiguous sequence of bytes.
struct slice_mutable {
    char *buf;
    size_t len;
};

/// Factory function.
/// Create slice from given buffer and length.
struct slice slice_make(const char *, size_t);

/// Factory function.
/// Create mutable slice from given buffer and length.
struct slice_mutable slice_mutable_make(char *, size_t);

/// Factory function.
/// Create slice from given string.
/// The slice length does not include the terminating NUL.
struct slice slice_from_string(const char *);

/// Convert mutable slice to regular immutable slice.
struct slice slice_from_mutable(struct slice_mutable);

/// @return An iterator pointing to the first element in the slice.
const char *slice_begin(struct slice);

/// @return An iterator referring to the past-the-end element in the slice.
const char *slice_end(struct slice);

/// @return An iterator pointing to the first element in the slice.
char *slice_mutable_begin(struct slice_mutable);

/// @return An iterator referring to the past-the-end element in the slice.
char *slice_mutable_end(struct slice_mutable);

/// Return the offset of slice @c inner within another slice @c outer.
/// @return Zero-based byte offset, or -1 if the slice is not contained.
/// @note The inner slice must be wholly contained within the outer.
ssize_t slice_offset(struct slice outer, struct slice inner);

/// Find @c needle in @c haystack.
/// @return Position of @c needle, or -1 if not found.
ssize_t slice_find(struct slice haystack, struct slice needle);

/// Compare slice with another.
/// @return An integer less than, equal to, or greater than 0 according as the slice compares less than, equal to, or greater than the other slice.
int slice_compare(struct slice, struct slice);

/// Test equality.
/// @return True if the slices contain the same byte sequence.
bool slice_equal(struct slice, struct slice);

/// Copy slice to NUL terminated string.
/// @param str The destination.
/// @param size Size of the destination buffer, including the NUL terminator.
/// @return @c str on success, NULL otherwise.
char *slice_strcpy(char *str, size_t size, struct slice);

/// Create an allocated, NUL-terminated string from the given slice.
/// @return Pointer to allocated string, or NULL on failure.
/// @note The string may contain embedded NULs.
/// @note Memory ownership: Caller must free() the returned pointer.
char *slice_strdup(struct slice);

#endif
