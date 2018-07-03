/*
 * terminal functions using UART 2
 */

#include "terminal.h"
#include <xc.h>

#ifndef TERM_PORT
#define TERM_PORT 2
#endif

#if TERM_PORT == 1
    #define UxMODE	U1MODE
    #define UxMODESET	U1MODESET
    #define UxSTA	U1STA
    #define UxTXREG	U1TXREG
    #define UxBRG	U1BRG
#elif TERM_PORT == 2
    #define UxMODE	U2MODE
    #define UxMODESET	U2MODESET
    #define UxSTA	U2STA
    #define UxTXREG	U2TXREG
    #define UxBRG	U2BRG
#else
    #error "invalid UART number"
#endif

#ifndef SYS_CLOCK
    #error "Macro SYS_CLOCK not defined (must be system clock im Hz)"
#endif


void term_write_char(unsigned char ch){
    while(UxSTA & _U1STA_UTXBF_MASK);
    UxTXREG= ch;
}

static void term_flush(void){
    while(!(UxSTA & _U1STA_TRMT_MASK));
}

#define TERM_BAUDRATE 115200

void term_init(void) {

#ifdef _OSCCON_PBDIV_POSITION
    unsigned pb_clock= SYS_CLOCK>>OSCCONbits.PBDIV;
#else
    unsigned pb_clock = SYS_CLOCK;
#endif

    UxMODE= _U1MODE_BRGH_MASK;  /* format: 8N1 */
    UxSTA= _U1STA_UTXEN_MASK |
	    (0b10 << _U1STA_UTXISEL_POSITION); /* irq when buffer becomes empty */
    
    UxBRG= pb_clock/(4*TERM_BAUDRATE)-1;
    UxMODESET = _U1MODE_ON_MASK;

}

void term_write_string(char *string) {
    while (*string) {
        if (*string == '\n')
            term_write_char('\r');
        term_write_char(*string++);
    }
    term_flush();
}

void term_writeln(void){
    term_write_char('\n');
    term_flush();
}


void term_write_hex(unsigned long num, unsigned int digits) {
    long i;
    unsigned long digit;
    for (i = (digits - 1)*4; i >= 0; i -= 4) {
        digit = (num >> i) & 0xf;
        if (digit < 10)
            term_write_char('0' + digit);
        else
            term_write_char('a' - 10 + digit);
    }
    term_flush();
}

void term_hexdump8(const void *buffer, int len){

        int ctr;
        unsigned char *buf= (unsigned char *) buffer;
        for( ctr= 0; ctr<len; ctr++ ){
                if( ctr % 16 == 0 ){
                        if( ctr!= 0 ){
                                term_writeln();
                        }
                        term_write_hex(ctr, 4);
                        term_write_string(": ");
                }
                term_write_hex(buf[ctr], 2);
                term_write_char(' ');
        }
        term_writeln();
}

