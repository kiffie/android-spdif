/*
 * terminal functions
 */

#ifndef __terminal_h__
#define __terminal_h__

#include <stdarg.h>

void term_init(void);
void term_write_char(unsigned char ch);
void term_write_string(char *string);
void term_writeln(void);
void term_write_hex(unsigned long num, unsigned int digits);
void term_hexdump8(const void *buffer, int len);

int term_vsprintf(char *buf, const char *fmt, va_list args);
int term_sprintf(char *buf, const char *fmt, ...);
int term_printf(const char *fmt, ...);

/*int term_kbhit(void);
int term_getch(void);
*/

#endif
