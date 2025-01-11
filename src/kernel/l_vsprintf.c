/*
    @ return : Number of characters written if successful (not including the terminating null character) or a negative value if an error occurred.
    + vsprintf 
    + sprintf
*/
#include "l_stdio.h"

/*
    copy from linux/lib/vsprintf.v and onix/src/lib/vsprintf.c
*/
#define SIGN	1		/* unsigned/signed, must be 1 */
#define LEFT	2		/* left justified */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define ZEROPAD	16		/* pad with zero, must be 16 == '0' - ' ' */
#define SMALL	32		/* use lowercase in hex (must be 32 == 0x20) */
#define SPECIAL	64		/* prefix hex with "0x", octal with "0" */


#define is_digit(c) ((c) >= '0' && (c) <= '9')

/// @brief 将字符串转为数字， 并移动指针
/// @param s const 保证不修改 **s 的内容， ** 可以对指针进行移动:（*s)++
/// @return 
static int skip_atoi(const char **s)
{
    int i = 0;
    while(is_digit(**s))
    {
        i = i * 10 + *((*s)++) - '0';
    }
    return i;
}

/// @brief 将整数转为指定格式的字符串
/// @param str 输出指针
/// @param num 带转换整数
/// @param base 进制
/// @param size 字符串长度
/// @param precision 精度
/// @param flags flag 标志位
/// @return 返回新的str指针
static char *number(char *str, uint32_t num, int base, int size, int precision, int flags)
{
    char c;
    char sign;
    char tmp[36];
    const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";                //  字符集 十进制只用0-9， 不同进制不同

    int i;
    int index;
    // char *ptr = str;

    if (flags & SMALL)
        digits = "0123456789abcdefghijklmnopqrstuvwxyz";                        //  小写

    if (flags & LEFT)
        flags &= ~ZEROPAD;                                                      //  

    if (base < 2 || base > 36)
        return 0;
        
    c = (flags & ZEROPAD) ? '0' : ' ';                                          //  
    
    if (flags & SIGN && num < 0)
    {
        sign = '-';
        num = -num;
    }
    else
    {
        sign = (flags & PLUS) ? '+' : ((flags & SPACE) ? ' ' : 0);
    }
    if (sign)
        size--;

    if (flags & SPECIAL)
    {
        if (base == 16)
            size -= 2;
        else if (base == 8)
            size--;
    }
    
    i = 0;
                                                    //  如果数值 num 为 0，则临时字符串='0'；否则根据给定的基数将数值 num 转换成字符形式
    if (num == 0)
    {
        tmp[i++] = '0';
    }
    else
    {
        while (num != 0)
        {
            index = num % base;                     //  开始转换 并写进tmp
            num /= base;
            tmp[i++] = digits[index];
        }
    }
        

    if (i > precision)
        precision = i;
                                                    // 宽度值 size 减去用于存放数值字符的个数
    size -= precision;

    if (!(flags & (ZEROPAD + LEFT)))
    {
        while (size-- > 0)
        {
            *str++ = ' ';
        }
    }

    if (sign)
        *str++ = sign;
    
    if (flags & SPECIAL)
    {
        
        if (base == 8)
            *str++ = '0';
        
        else if (base == 16)
        {
            *str++ = '0';
            *str++ = digits[33];
        }
    }
    
    if (!(flags & LEFT))
        while (size-- > 0)
            *str++ = c;
    
    
    while (i < precision--)
        *str++ = '0';
    
    while (i-- > 0)
        *str++ = tmp[i];                            //  从大到小赋值

    while (size-- > 0)
        *str++ = ' ';
    
    return str;                                     //  返回新的指针
}

/// @brief 从 va_list 中不断匹配 fmt 中的 "%*" 格式化字符，写到 buf 中
/// @param buf 
/// @param fmt 
/// @param args 
/// @return 
int vsprintf(char *buf, const char *fmt, va_list args)
{
    
    int len;

    int i;

    char *str;                          //  当前正在处理的字符

    char *s;                            //  存放%s 字符串

    int *ip;

    int flags;                          //  符号标记位，传递给 number 函数

    int field_width;

    int precision;                      //  精度部分对不同的specifiers 效果不同 对 %d %x是制定了最小的长度

    int qualifier;

    for(str = buf; *fmt; ++fmt)
    {
                                        
        if(*fmt != '%')
        {                               //  没有找到 % 时，字符写进 buf
            *str++ = *fmt;
            continue;
        }

        flags = 0;                      //  开始找flags

    repeat:

        ++fmt;                          //  向前移动一位。跳过 % 或switch 到的字符

        switch(*fmt)
        {

            case '-':
                flags |= LEFT;
                goto repeat;

            case '+':
                flags |= PLUS;
                goto repeat;
            case ' ':
                flags |= SPACE;
                goto repeat;
            case '#':
                flags |= SPECIAL;
                goto repeat;
            case '0':
                flags |= ZEROPAD;
                goto repeat;
        }

        field_width = -1;                       //  开始找字符宽度

        if(is_digit(*fmt))
        {                                       //  如果第一个就是数字，那么就被当作宽度值
            field_width = skip_atoi(&fmt);      //  得到数字
        }
        else if(*fmt == '*')
        {
            return 0;                           //  Todo
        }
        

        precision = -1;

        if(*fmt == '.')
        {                                       //  开始匹配精度
    
            ++fmt;

            if(is_digit(*fmt))
            {
                precision = skip_atoi(&fmt);
            }
            else if(*fmt == '*')
            {
                return 0;                       //  Todo
            }

            if(precision < 0)
            {                  
                precision = 0;                  //  精度小于零 ignore
            }
        }


        qualifier = -1;

        if(*fmt == 'h' || *fmt == 'l' || *fmt == 'L')
        {                                           //  暂时只支持1个 qualifier
            qualifier = *fmt;
            ++fmt;
        }   

        switch (*fmt)
        {
        case 'c':
            if(!(flags & LEFT))
            {                                       //  不是左对齐
                while(--field_width > 0)
                {                                   //  插入 field_width - 1个空格
                    *str++ = ' ';
                }
            }

            *str++ = (uint8_t)va_arg(args, int);                    //  字符赋值
            
            while(--field_width > 0)
            {                                       //  左对齐部分
                    *str++ = ' ';
            }

            break;

        case 's':

            s = va_arg(args, char *);
            len = string_len(s);

            if(!(flags & LEFT))
            {                                    //  不是左对齐    
                while(--field_width > len)
                {                         //  插入 field_width - 1 - len个空格
                    *str++ = ' ';
                }
            }

            for(i = 0; i< len; ++i)
            {
                *str++ = *s++;
            }

            while(--field_width > 0)
            {                               //  左对齐部分
                    *str++ = ' ';
            }

            break;
        case 'x':
            str = number(str, va_arg(args, uint32_t), 16, field_width, precision, flags);
            break;

        case 'd':
            flags |= SIGN;

        case 'u':
            str = number(str, va_arg(args, uint32_t), 10, field_width, precision, flags);
            break;

        default:
            break;
        }
    }
    *str = '\0';

    return str - buf;
}


/// @brief 依赖于 vsprintf， 把格式化的字符串写进buf
/// @param buf 
/// @param fmt 
/// @param  
/// @return 
int sprintf(char *buf, const char *fmt, ...)
{
    va_list args;
    int i;
    
    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    va_end(args);
    return i;
}