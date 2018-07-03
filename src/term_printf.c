/*
 * term_printf
 *
 */

#include "terminal.h"
#include <stdio.h>

int term_printf(const char *fmt, ...)
{
    char printf_buf[1024];
    va_list args;
    int printed;

    va_start(args, fmt);
    printed = vsprintf(printf_buf, fmt, args);
    va_end(args);

    term_write_string(printf_buf);

    return printed;
}
