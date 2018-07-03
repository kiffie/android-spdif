/*
 * configuration registers
 */

#if defined( __32MX250F128B__) || defined(__32MX270F256B__) || defined(__32MX270F256D__) || defined(__32MX470F512H__)

#pragma config JTAGEN = OFF             // JTAG Enable

#pragma config FUSBIDIO = OFF           // USB USID Selection (Controlled by Port Function)
#pragma config FVBUSONIO = ON           // USB VBUS ON Selection

#pragma config FNOSC    = PRIPLL        // Oscillator Selection
#pragma config POSCMOD  = XT            // Primary Oscillator
#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin
#pragma config FSOSCEN  = OFF           // Secondary Oscillator Enable

#pragma config UPLLEN   = ON            // USB PLL Enabled
#pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider

#if SYS_CLOCK == 40000000
    #pragma config FPLLMUL  = MUL_20    // PLL Multiplier
#elif SYS_CLOCK == 48000000
    #pragma config FPLLMUL  = MUL_24    // PLL Multiplier
#else
    #error "unexpected SYS_CLOCK"
#endif

#pragma config FPLLIDIV = DIV_2         // PLL Input Divider
#pragma config FPLLODIV = DIV_2         // PLL Output Divider

#pragma config FPBDIV   = DIV_1         // Peripheral Clock divisor

#pragma config FWDTEN   = OFF           // Watchdog Timer

#else
    #error Cannot define configuration bits.
#endif


