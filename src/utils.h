#ifndef UTILS_H
#define UTILS_H

#include <stdarg.h>
#ifndef ASSERT
#include <assert.h>
#define ASSERT assert
#endif /* ASSERT */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__GNUC__) || defined(__clang__)
//   https://gcc.gnu.org/onlinedocs/gcc-4.7.2/gcc/Function-Attributes.html
#ifdef __MINGW_PRINTF_FORMAT
#define PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK)                            \
  __attribute__((format(__MINGW_PRINTF_FORMAT, STRING_INDEX, FIRST_TO_CHECK)))
#else
#define PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK)                            \
  __attribute__((format(printf, STRING_INDEX, FIRST_TO_CHECK)))
#endif // __MINGW_PRINTF_FORMAT
#else
//   TODO: implement PRINTF_FORMAT for MSVC
#define PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK)
#endif

#define UNUSED(value) (void)(value)

#define RANDBETWEEN(min, max) (rand() % ((max) - (min) + 1) + (min))

#define TODO(message)                                                          \
  do {                                                                         \
    fprintf(stderr, "%s:%d: TODO: %s\n", __FILE__, __LINE__, message);         \
    abort();                                                                   \
  } while (0)

#define UNREACHABLE(message)                                                   \
  do {                                                                         \
    fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message);  \
    abort();                                                                   \
  } while (0)

// Dynamic arrays
//
//  Initial capacity of a dynamic array
#ifndef DA_INIT_CAP
#define DA_INIT_CAP 256
#endif

#ifdef __cplusplus
#define DECLTYPE_CAST(T) (decltype(T))
#else
#define DECLTYPE_CAST(T)
#endif // __cplusplus

#define da_reserve(da, expected_capacity)                                      \
  do {                                                                         \
    if ((expected_capacity) > (da)->capacity) {                                \
      if ((da)->capacity == 0) {                                               \
        (da)->capacity = DA_INIT_CAP;                                          \
      }                                                                        \
      while ((expected_capacity) > (da)->capacity) {                           \
        (da)->capacity *= 2;                                                   \
      }                                                                        \
      (da)->items = DECLTYPE_CAST((da)->items)                                 \
          realloc((da)->items, (da)->capacity * sizeof(*(da)->items));         \
      ASSERT((da)->items != NULL && "Buy more RAM lol");                       \
    }                                                                          \
  } while (0)

// Append an item to a dynamic array
#define da_append(da, item)                                                    \
  do {                                                                         \
    da_reserve((da), (da)->count + 1);                                         \
    (da)->items[(da)->count++] = (item);                                       \
  } while (0)

#define da_free(da) FREE((da).items)

// Append several items to a dynamic array
#define da_append_many(da, new_items, new_items_count)                         \
  do {                                                                         \
    da_reserve((da), (da)->count + (new_items_count));                         \
    memcpy((da)->items + (da)->count, (new_items),                             \
           (new_items_count) * sizeof(*(da)->items));                          \
    (da)->count += (new_items_count);                                          \
  } while (0)

#define da_resize(da, new_size)                                                \
  do {                                                                         \
    da_reserve((da), new_size);                                                \
    (da)->count = (new_size);                                                  \
  } while (0)

#define da_last(da) (da)->items[(ASSERT((da)->count > 0), (da)->count - 1)]
#define da_remove_unordered(da, i)                                             \
  do {                                                                         \
    size_t j = (i);                                                            \
    ASSERT(j < (da)->count);                                                   \
    (da)->items[j] = (da)->items[--(da)->count];                               \
  } while (0)

// Foreach over Dynamic Arrays. Example:
// ```c
// typedef struct {
//     int *items;
//     size_t count;
//     size_t capacity;
// } Numbers;
//
// Numbers xs = {0};
//
// da_append(&xs, 69);
// da_append(&xs, 420);
// da_append(&xs, 1337);
//
// da_foreach(int, x, &xs) {
//     // `x` here is a pointer to the current element. You can get its index by
//     taking a difference
//     // between `x` and the start of the array which is `x.items`.
//     size_t index = x - xs.items;
//     log(INFO, "%zu: %d", index, *x);
// }
// ```
#define da_foreach(Type, it, da)                                               \
  for (Type *it = (da)->items; it < (da)->items + (da)->count; ++it)

typedef struct {
  char *items;
  size_t count;
  size_t capacity;
} String_Builder;

#define swap(T, a, b)                                                          \
  do {                                                                         \
    T t = a;                                                                   \
    a = b;                                                                     \
    b = t;                                                                     \
  } while (0)

bool read_entire_file(const char *path, String_Builder *sb);
int sb_appendf(String_Builder *sb, const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);
  int n = vsnprintf(NULL, 0, fmt, args);
  va_end(args);

  // NOTE: the new_capacity needs to be +1 because of the null terminator.
  // However, further below we increase sb->count by n, not n + 1.
  // This is because we don't want the sb to include the null terminator. The
  // user can always sb_append_null() if they want it
  da_reserve(sb, sb->count + n + 1);
  char *dest = sb->items + sb->count;
  va_start(args, fmt);
  vsnprintf(dest, n + 1, fmt, args);
  va_end(args);

  sb->count += n;

  return n;
}
void sb_pad_align(String_Builder *sb, size_t size);

// Append a sized buffer to a string builder
#define sb_append_buf(sb, buf, size) da_append_many(sb, buf, size)

// Append a NULL-terminated string to a string builder
#define sb_append_cstr(sb, cstr)                                               \
  do {                                                                         \
    const char *s = (cstr);                                                    \
    size_t n = strlen(s);                                                      \
    da_append_many(sb, s, n);                                                  \
  } while (0)

// Append a single NULL character at the end of a string builder. So then you
// can use it a NULL-terminated C string
#define sb_append_null(sb) da_append_many(sb, "", 1)

// Free the memory allocated by a string builder
#define sb_free(sb) FREE((sb).items)

#endif // UTILS_H
