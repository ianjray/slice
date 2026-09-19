#include "slice.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void test_slice_make(void)
{
    struct slice s;
    const char buf[] = "foo";

    s = slice_make(NULL, 0);
    assert(NULL == s.buf);
    assert(0 == s.len);

    s = slice_make(buf, sizeof(buf));
    assert(buf == s.buf);
    assert(sizeof(buf) == s.len);

    // Expected: error: read-only variable is not assignable
    // s.buf[0] = 'A';
}

static void test_slice_mutable_make(void)
{
    struct slice_mutable m;
    char buf[4];

    m = slice_mutable_make(NULL, 0);
    assert(NULL == m.buf);
    assert(0 == m.len);

    m = slice_mutable_make(buf, sizeof(buf));
    assert(buf == m.buf);
    assert(sizeof(buf) == m.len);

    m.buf[0] = 'A';
}

static void test_slice_from_string(void)
{
    struct slice s;

    s = slice_from_string(NULL);
    assert(NULL == s.buf);
    assert(0 == s.len);

    s = slice_from_string("abc");
    assert('a' == s.buf[0]);
    assert(3 == s.len);
}

static void test_slice_from_mutable(void)
{
    struct slice_mutable m;
    struct slice s;
    char buf[4];

    m = slice_mutable_make(NULL, 0);
    s = slice_from_mutable(m);
    assert(s.buf == m.buf);
    assert(s.len == m.len);

    m = slice_mutable_make(buf, sizeof(buf));
    m.buf[0] = 'A';

    s = slice_from_mutable(m);
    assert(s.buf == m.buf);
    assert(s.len == m.len);
}

static void test_slice_begin(void)
{
    struct slice s;

    s = slice_make(NULL, 0);
    assert(NULL == slice_begin(s));

    s = slice_from_string("abcdef");
    assert('a' == *slice_begin(s));
    assert('b' == slice_begin(s)[1]);
}

static void test_slice_end(void)
{
    struct slice s;

    s = slice_make(NULL, 0);
    assert(NULL == slice_end(s));

    s = slice_from_string("abcdef");
    assert('f' == slice_end(s)[-1]);
    assert('e' == slice_end(s)[-2]);
}

static void test_slice_mutable_begin(void)
{
    struct slice_mutable m;
    char buf[4];

    m = slice_mutable_make(NULL, 0);
    assert(NULL == slice_mutable_begin(m));

    m = slice_mutable_make(buf, sizeof(buf));
    assert(buf == slice_mutable_begin(m));
}

static void test_slice_mutable_end(void)
{
    struct slice_mutable m;
    char buf[4];

    m = slice_mutable_make(NULL, 0);
    assert(NULL == slice_mutable_end(m));

    m = slice_mutable_make(buf, sizeof(buf));
    assert(&buf[4] == slice_mutable_end(m));
}

static void test_slice_offset(void)
{
    struct slice outer;
    struct slice inner;
    const char buf[] = "0123456789";

    /* NULL slices. */
    outer = slice_make(NULL, 0);
    inner = slice_make(NULL, 0);
    assert(-1 == slice_offset(outer, inner));

    outer = slice_make(buf, sizeof(buf) - 1);
    inner = slice_make(NULL, 0);
    assert(-1 == slice_offset(outer, inner));

    outer = slice_make(NULL, 0);
    inner = slice_make(buf, 1);
    assert(-1 == slice_offset(outer, inner));

    outer = slice_make(buf, sizeof(buf) - 1);

    /* Beginning. */
    inner = slice_make(buf, 0);
    assert(0 == slice_offset(outer, inner));

    /* Empty slice in the middle. */
    inner = slice_make(buf + 5, 0);
    assert(5 == slice_offset(outer, inner));

    /* End. */
    inner = slice_make(buf + 10, 0);
    assert(10 == slice_offset(outer, inner));

    /* Partial slice. */
    inner = slice_make(buf + 2, 4);
    assert(2 == slice_offset(outer, inner));

    /* Entire slice. */
    inner = outer;
    assert(0 == slice_offset(outer, inner));

    /* Inner slice extends beyond outer. */
    inner = slice_make(buf + 8, 3);
    assert(-1 == slice_offset(outer, inner));

    /* Inner slice starts before outer. */
    outer = slice_make(buf + 2, 6);
    inner = slice_make(buf, 3);
    assert(-1 == slice_offset(outer, inner));

    /* Inner slice ends exactly at outer. */
    inner = slice_make(buf + 6, 2);
    assert(4 == slice_offset(outer, inner));
}

static void test_slice_find(void)
{
    struct slice haystack;
    struct slice needle;

    /* NULL empty slices. */
    haystack = slice_make(NULL, 0);
    needle = slice_make(NULL, 0);
    assert(-1 == slice_find(haystack, needle));

    /* NULL haystack. */
    haystack = slice_make(NULL, 0);
    needle = slice_from_string("a");
    assert(-1 == slice_find(haystack, needle));

    /* NULL needle. */
    haystack = slice_from_string("a");
    needle = slice_make(NULL, 0);
    assert(-1 == slice_find(haystack, needle));

    /* Haystack is empty. */
    haystack = slice_make("", 0);
    needle = slice_from_string("");
    assert(0 == slice_find(haystack, needle));

    needle = slice_from_string("a");
    assert(-1 == slice_find(haystack, needle));

    /* Empty needle. */
    haystack = slice_from_string("anything");
    needle = slice_make("abc", 0);
    assert(0 == slice_find(haystack, needle));

    /* Needle is absent. */
    needle = slice_from_string("xyz");
    assert(-1 == slice_find(haystack, needle));

    /* Needle at beginning. */
    haystack = slice_from_string("a_ab_abc");
    needle = slice_from_string("a");
    assert(0 == slice_find(haystack, needle));

    needle = slice_from_string("a_");
    assert(0 == slice_find(haystack, needle));

    /* Needle in middle. */
    needle = slice_from_string("ab");
    assert(2 == slice_find(haystack, needle));

    needle = slice_from_string("_ab_");
    assert(1 == slice_find(haystack, needle));

    /* Needle at end. */
    needle = slice_from_string("abc");
    assert(5 == slice_find(haystack, needle));

    needle = slice_from_string("_abc");
    assert(4 == slice_find(haystack, needle));

    /* Needle is the entire haystack. */
    needle = slice_from_string("a_ab_abc");
    assert(0 == slice_find(haystack, needle));

    /* Needle longer than haystack. */
    needle = slice_from_string("a_ab_abcd");
    assert(-1 == slice_find(haystack, needle));

    /* Embedded NUL. */
    {
        const char haystack_buf[] = { 'a', '\0', 'b', 'c' };
        const char needle_buf[] = { '\0', 'b' };

        haystack = slice_make(haystack_buf, sizeof(haystack_buf));
        needle = slice_make(needle_buf, sizeof(needle_buf));
        assert(1 == slice_find(haystack, needle));
    }
}

static void test_slice_compare(void)
{
    struct slice a;
    struct slice b;

    a = slice_from_string("abc");
    b = slice_from_string("abc");
    assert(0 == slice_compare(a, b));

    a = slice_from_string("abc");
    b = slice_from_string("abd");
    assert(slice_compare(a, b) < 0);
    assert(slice_compare(b, a) > 0);

    a = slice_from_string("abc");
    b = slice_from_string("abcd");
    assert(slice_compare(a, b) < 0);
    assert(slice_compare(b, a) > 0);

    {
        const char a_buf[] = { 'a', '\0', 'b' };
        const char b_buf[] = { 'a', '\0', 'c' };

        a = slice_make(a_buf, sizeof(a_buf));
        b = slice_make(b_buf, sizeof(b_buf));

        assert(slice_compare(a, b) < 0);
        assert(slice_compare(b, a) > 0);
    }
}

static void test_slice_equal(void)
{
    struct slice a;
    struct slice b;
    char buf[] = "abc";

    a = slice_from_string("abc");
    b = slice_from_string(buf);
    assert(slice_equal(a, b));

    buf[0] = 'A';
    assert(!slice_equal(a, b));

    a = slice_from_string("Abc");
    assert(slice_equal(a, b));
}

static void test_slice_strcpy(void)
{
    struct slice s;
    char buf[4];

    s = slice_from_string("foo");
    assert(NULL == slice_strcpy(NULL, sizeof(buf), s));

    s = slice_from_string("foo");
    assert(NULL == slice_strcpy(buf, sizeof(buf) - 1, s));

    s = slice_make(NULL, 0);
    assert(NULL == slice_strcpy(buf, sizeof(buf), s));

    s = slice_from_string("");
    assert(buf == slice_strcpy(buf, sizeof(buf), s));
    assert('\0' == buf[0]);

    s = slice_from_string("foo");
    assert(buf == slice_strcpy(buf, sizeof(buf), s));
    assert(0 == strcmp(buf, "foo"));

    /* Embedded NUL. */
    {
        const char src[] = { 'a', '\0', 'b' };

        s = slice_make(src, sizeof(src));

        assert(buf == slice_strcpy(buf, sizeof(buf), s));
        assert(0 == memcmp(buf, src, sizeof(src)));
        assert('\0' == buf[sizeof(src)]);
    }
}

static void test_slice_strdup(void)
{
    struct slice s;
    char *str;

    s = slice_from_string(NULL);
    assert(NULL == slice_strdup(s));

    s = slice_from_string("");
    str = slice_strdup(s);
    assert(str);
    assert('\0' == str[0]);
    free(str);

    s = slice_from_string("foo");
    str = slice_strdup(s);
    assert(str);
    assert(0 == strcmp(str, "foo"));
    free(str);

    /* Embedded NUL. */
    {
        const char src[] = { 'a', '\0', 'b' };

        s = slice_make(src, sizeof(src));
        str = slice_strdup(s);

        assert(str);
        assert(0 == memcmp(str, src, sizeof(src)));
        assert('\0' == str[sizeof(src)]);

        free(str);
    }
}

int main(void)
{
    test_slice_make();
    test_slice_mutable_make();
    test_slice_from_string();
    test_slice_from_mutable();

    test_slice_begin();
    test_slice_end();
    test_slice_mutable_begin();
    test_slice_mutable_end();

    test_slice_offset();
    test_slice_find();
    test_slice_compare();
    test_slice_equal();

    test_slice_strcpy();
    test_slice_strdup();
}
