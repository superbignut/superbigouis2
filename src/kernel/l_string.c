#include "l_string.h"
#include "l_types.h"

/// @brief Copies the null-terminated byte string pointed to by src, including the null terminator, 
///        to the character array whose first element is pointed to by dest.
/// @param dest 
/// @param src 
/// @return 
char *string_copy(char *dest, const char *src){
    char *ptr = dest;
    while(True){
        *ptr++ = *src;      // ++ suffix 的优先级最高 > * dereference
        *ptr = *src;
        if(*src++ == EOS){
            return dest;
        }
    }
}


/// @brief Copies at most count characters of the character array pointed to by src (including the terminating null character, 
///        but not any of the characters that follow the null character) to character array pointed to by dest.
/// @param dest 
/// @param sr 
/// @param count 
/// @return 
char *string_n_copy(char *dest, const char *src, size_t count){
    char *ptr = dest;
    size_t temp_cnt = 0;

    for(; temp_cnt < count; ++temp_cnt){ 
        *ptr++ = *src;
        if(*src++ == EOS){
            return dest;
        }
    }
    dest[temp_cnt - 1] = EOS;       // 强制最后一位是eos
    return dest;
}

/// @brief Appends a copy of the null-terminated byte string pointed to by src to the end of the null-terminated byte string pointed to by dest. 
///        The character src[0] replaces the null terminator at the end of dest. The resulting byte string is null-terminated.
/// @param dest 
/// @param src 
/// @return 
char *strint_cat(char *dest, const char *src){
    char *ptr = dest;
    while(*ptr != EOS){        // 找到 EOS
        ptr++;
    }         

    while(True){
        *ptr++ = *src;
        if(*src++ == EOS){
            return dest;
        }
    }
}

/// @brief Compares two null-terminated byte strings lexicographically.
/// @param lhs 
/// @param rhs 
/// @return -1 is < ; 0 is = ; 1 is > ;
int string_compare(const char* lhs, const char* rhs){
    for( ; ; lhs++, rhs++){
        if (*lhs == *rhs){
            if(*lhs == EOS){
                return 0;
            }
            continue;
        }    
        return *lhs > *rhs ? 1 : -1;
    }
}

/// @brief Finds the first occurrence of ch (after conversion to char as if by (char)ch) in the null-terminated byte string pointed to by str 
///        (each character interpreted as unsigned char). The terminating null character is considered to be a part of the string and can be found 
///        when searching for '\0'.
/// @param str 
/// @param ch 
/// @return 
char *string_find_char_first(const char* str, int ch){
    char *ptr = (char*)str;
    while(True){
        if(*ptr == ch){
            return ptr;
        }
        if(*ptr++ == EOS){
            return NULL;
        }
    }
}   

char *string_find_char_last(const char* str, int ch){

    char *ptr = (char*)str;
    char *ans = NULL;
    while(True){
        if(*ptr == ch){
            ans = ptr;
        }
        if(*ptr++ == EOS){
            return ans;
        }
    }
}

/// @brief Returns the length of the given null-terminated byte string, that is, the number of characters in a character 
///        array whose first element is pointed to by str up to and not including the first null character.
/// @param str 
/// @return 
size_t string_len(const char *str){
    char *ptr = (char*)str;

    while(*ptr != EOS){
        ++ptr;
    }
    return ptr - str;
}

/// @brief Copies the value (unsigned char)ch into each of the first count characters of the object pointed to by dest.
///        The behavior is undefined if access occurs beyond the end of the dest array. The behavior is undefined if dest is a null pointer.
/// @param dest 
/// @param ch 
/// @param count 
/// @return 
void *memory_set(void *dest, int ch, size_t count){
    char *ptr = dest;

    for(size_t i = 0; i < count; ++i){
        *ptr++ = ch;
    }
    return dest;
}

/// @brief Copies count characters from the object pointed to by src to the object pointed to by dest. 
///        Both objects are interpreted as arrays of unsigned char.
/// @param dest 
/// @param src 
/// @param count 
/// @return 
void* memory_copy(void *dest, const void *src, size_t count){
    char *ptr = dest;
    
    for(size_t i = 0; i < count; ++i){
        *ptr++ = *((char*)src++);   // (type) < ++ suffix
    }
    return dest;
}

/// @brief Compares the first count bytes of the objects pointed to by lhs and rhs. The comparison is done lexicographically.
/// @param lhs 
/// @param rhs 
/// @param count 
/// @return 
int memory_compare(const void* lhs, const void* rhs, size_t count){
    char *str1 = (char*)lhs;
    char *str2 = (char*)rhs;
    
    for(int i=0; i < count; str1++, str2++){
        if (*str1 == *str2){
            if(*str1 == EOS){
                return 0;
            }
            continue;
        }    
        return *str1> *str2 ? 1 : -1;
    }
    return 0;
}
/// @brief Finds the first occurrence of (unsigned char)ch in the initial count bytes (each interpreted as unsigned char) of the object pointed to by ptr.
/// @param ptr 
/// @param ch 
/// @param count 
/// @return Pointer to the location of the byte, or a null pointer if no such byte is found.
void* memory_find_char_first(const void* ptr, int ch, size_t count){
    char *tmp = (char*)ptr;
    size_t cnt = 0;
    for( ; cnt< count; ++cnt){
        if(*tmp == ch){
            return (void*)tmp;
        }
        if(*tmp++ == EOS){
            return NULL;
        }
    }
}
