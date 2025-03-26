/**
 * @file l_assert.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-03-26
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef LASSERT_H_
#define LASSERT_H_



void assertion_failure(char *exp, char *file, char *base, int line);



#define assert(exp) \
        if(exp)     \
            ;       \
        else        \
            assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)

void panic(const char *fmt, ...);



#endif