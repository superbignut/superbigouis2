#ifndef LTYPES_H_
#define LTYPES_H_

#define EOF -1
#define NULL ((void*)0)
#define EOS '\0'

#define bool _Bool
#define True 1
#define False 0

#define _packed __attribute__((packed))
#define _ofp __attribute__((optimize("omit-frame-pointer"))) // 函数不使用栈帧

typedef unsigned int size_t;

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

#endif