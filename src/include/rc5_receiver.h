/*
 * RC5 receiver
 * uses port RB13 and PORTB Input Change Interrupt
 */

#ifndef __RC5_RECEIVER__
#define __RC5_RECEIVER__

#include <sys/errno.h>

#define RC_KEY_VOLUMEDOWN 0x1011
#define RC_KEY_VOLUMEUP   0x1010
#define RC_KEY_1          0x1001
#define RC_KEY_2          0x1002
#define RC_KEY_3          0x1003
#define RC_KEY_4          0x1004

#define RC_KEY_PLAY       0x1032
#define RC_KEY_FFWD       0x1034
#define RC_KEY_STOP       0x1036
#define RC_KEY_REWIND     0x1037
#define RC_KEY_PAUSE      0x103c
#define RC_KEY_NEXT       0x1175
#define RC_KEY_PREV       0x1176

#define RC_NONE           0
//#define RC_IDLE           0x80000000


int rc5_get_keycode(unsigned *keycode);
void rc5_init(void);

#endif
