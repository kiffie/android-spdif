/*
 * RC5 receiver
 * uses port RB13 and PORTB Input Change Interrupt
 */

#include <rc5_receiver.h>
#include <timer.h>
#include <xc.h>
#include <sys/attribs.h>
#include <mips_irq.h>


#include <logger.h>
#ifndef LOGLEVEL_RC5
    #define LOGLEVEL_RC5 LOG_DEBUG
#endif
#define LOGLEVEL LOGLEVEL_RC5
#define LOG_PREFIX "SPDIF_OUT"

/*** IR RC receiver ***/

#define RC_MAX_SHORT 1200
#define RC_MIN_SILENCE 10000
#define RC_TOGGLE_BIT 0x0800
#define RC_IDLE_TIME 1000000

typedef struct rc5_context {
    unsigned keycode;
    timer_time_t keycode_time;
    unsigned keycode_mbox;

    /* rc5 decoder state */
    timer_time_t last_slope;
    unsigned bit_ctr;
    unsigned value;

} rc5_context_t;

static rc5_context_t rc5;

int rc5_get_keycode(unsigned *keycode){
    int r;
    unsigned irqstate = mips_di();
    if(rc5.keycode_mbox != RC_NONE){
	*keycode = rc5.keycode_mbox;
	rc5.keycode_mbox = RC_NONE;
	r = 0;
    }else{
	r = -EAGAIN;
    }
    mips_restore_irq(irqstate);
    return r;
}

unsigned rc5_decode(unsigned delay_us, int pin){

    unsigned result= 0;
    if( delay_us >= RC_MIN_SILENCE ){
	rc5.bit_ctr= 1;
	rc5.value= 0;
    }else {
	if( delay_us < RC_MAX_SHORT ){  /* delay is one half bit */
	    if( !(rc5.bit_ctr & 0x01) ){ /* data slope */
		rc5.value<<=1;
		if( pin ) rc5.value|=1;
	    }
	    rc5.bit_ctr++;
	}else{ /* delay is two half bits */
	    if( rc5.bit_ctr & 0x01 ){ /* data slope */
		rc5.value<<=1;
		if( pin ) rc5.value|=1;
	    }
	    rc5.bit_ctr+= 2;
	}
    }
    if( rc5.bit_ctr == 27 ){
	result= rc5.value;
	rc5.bit_ctr= 0;
    }
    return result;
}


void __ISR(_CHANGE_NOTICE_VECTOR, IPL6SOFT) rc5_isr(void) {
    
    timer_time_t now = timer_now();
    timer_time_t delay = now - rc5.last_slope;
    rc5.last_slope = now;
    
    int pin = !PORTBbits.RB13;
    
    unsigned code= rc5_decode(delay/TIMER_TICKS_PER_US, pin);

    if( code != 0 ){
	rc5.keycode_time = now;
	if( code != rc5.keycode ){
	    rc5.keycode_mbox = code & ~RC_TOGGLE_BIT;
	    rc5.keycode = code;
	    uint8_t cmd= code & 0x3f;
	    uint8_t dev= (code >> 6) & 0x1f;
	    log_verbose("code=%04x, dev=%x, cmd=%x\n", code, dev, cmd);
	}      
    }else{
	if( now >= rc5.keycode_time + RC_IDLE_TIME * TIMER_TICKS_PER_US ){
	    rc5.keycode = RC_NONE;
	}
    }
    IFS1CLR =  _IFS1_CNBIF_MASK;
}


void rc5_init(void){

    rc5.last_slope = 0;
    
    /* set up port */
    ANSELBCLR = _ANSELB_ANSB13_MASK;
    TRISBSET = _TRISB_TRISB13_MASK;

    /* enable interrupt */    
    CNENB = _CNENB_CNIEB13_MASK;
    CNCONBSET = _CNCONB_ON_MASK;

    IFS1CLR =  _IFS1_CNBIF_MASK;
    IPC8bits.CNIP = 6;
    IEC1SET =  _IEC1_CNBIE_MASK;

    log_debug("RC5 receiver initialized.\n");

}


/*** end IR RC receiver ***/

