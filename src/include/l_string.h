#ifndef LSTRING_H_
#define LSTRING_H_

#include "l_types.h"

char *string_copy(char *dest, const char *src);

char *string_n_copy(char *dest, const char *src, size_t count);

char *strint_cat(char *dest, const char *src);

int string_compare(const char* lhs, const char* rhs);

char *string_find_char_first(const char* str, int ch);

char *string_find_char_last(const char* str, int ch);

size_t string_len(const char *str);



void *memort_set(void *dest, int ch, size_t count);

void* memory_copy(void *dest, const void *src, size_t count);

int memory_compare(const void* lhs, const void* rhs, size_t count);

void* memory_find_char_first(const void* ptr, int ch, size_t count);

#endif
