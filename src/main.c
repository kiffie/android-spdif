/*
 * main function
 * 
 * This file is based on Microchip's example code
 * "USB Android Accessory basic demo with accessory in host mode"
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "USB/usb.h"
#include "USB/usb_host_android.h"
#include "USB/usb_host_audio_v1.h"

#include "spdif_out.h"

#include <mips_irq.h>
#include <terminal.h>
#include <timer.h>
#include <rc5_receiver.h>

#include <logger.h>
#ifndef LOGLEVEL_MAIN
    #define LOGLEVEL_MAIN LOG_DEBUG
#endif
#define LOGLEVEL LOGLEVEL_MAIN
#define LOG_PREFIX "MAIN"

// If a maximum current rating hasn't been defined, then define 500mA by default
#ifndef MAX_ALLOWED_CURRENT
    #define MAX_ALLOWED_CURRENT             (500)         // Maximum power we can supply in mA
#endif

/***********************************************************
Type definitions, enumerations, and constants
***********************************************************/

typedef struct
{
    USB_AUDIO_V1_DEVICE_ID* audioDeviceID;
    BOOL interfaceSet;
} AUDIO_ACCESSORY_EXAMPLE_VARS;

typedef struct __attribute__((packed))
{
    union __attribute__((packed))
    {
        struct __attribute__((packed))
        {
            unsigned play               :1;
            unsigned play_pause         :1;
            unsigned fast_forward       :1;
            unsigned rewind             :1;
            unsigned scan_next          :1;
            unsigned scan_previous      :1;
            unsigned stop               :1;
            unsigned                    :1;
        } bits;
        unsigned char value;
    } controls;
} HID_REPORT;


/***********************************************************
Local variables
***********************************************************/
static void* device_handle = NULL;
static bool device_attached = false;
static bool registered = false;
static AUDIO_ACCESSORY_EXAMPLE_VARS audioAccessoryVars =    
{
    NULL,
    FALSE
};                                                         

//static char manufacturer[] = "Microchip Technology Inc.";
//static char model[] = "Basic Accessory Demo";
static char description[] = "Android S/PDIF interface";
static char version[] = "1.0";
static char uri[] = "N/A";
static char serial[] = "N/A";

static ANDROID_ACCESSORY_INFORMATION myDeviceInfo =
{
    //Pass in NULL for the manufacturer and model string if you don't want
    //  an app to pop up when the accessory is plugged in.
    NULL,       //manufacturer,
    0,          //sizeof(manufacturer),

    NULL,       //model,
    0,          //sizeof(model),

    description,
    sizeof(description),

    version,
    sizeof(version),

    uri,
    sizeof(uri),

    serial,
    sizeof(serial),

    //ANDROID_AUDIO_MODE__NONE
    ANDROID_AUDIO_MODE__44K_16B_PCM
};

static char ReportDescriptor[] = {
    0x05, 0x0c,                    // USAGE_PAGE (Consumer Devices)
    0x09, 0x01,                    // USAGE (Consumer Control)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0xb0,                    //   USAGE (Play)
    0x09, 0xcd,                    //   USAGE (Play/Pause)
    0x09, 0xb3,                    //   USAGE (Fast Forward)
    0x09, 0xb4,                    //   USAGE (Rewind)
    0x09, 0xb5,                    //   USAGE (Scan Next Track)
    0x09, 0xb6,                    //   USAGE (Scan Previous Track)
    0x09, 0xb7,                    //   USAGE (Stop)
    0x09, 0x00,                    //   USAGE (Unassigned)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x81, 0x42,                    //   INPUT (Data,Ary,Var,Null)
    0xc0                           // END_COLLECTION
};


static HID_REPORT report;

static BYTE requestStatus = USB_SUCCESS;
static ISOCHRONOUS_DATA audioData;
static BYTE device_address = 0;
static bool reportToSend = false;
static bool reportComplete = true;


int main(void)
{
    unsigned key_code;
    timer_time_t key_relase_timer = 0;

    BMXCONCLR = _BMXCON_BMXWSDRM_MASK;
    mips_enable_mv_irq();
#ifdef __32MX470F512H__
    RPF5R = 0b0001; /* U2TX for terminal (debug) output */
    CHECON = _CHECON_DCSZ_MASK | _CHECON_PREFEN_MASK | 1;
#elif defined(__32MX270F256D__)
    TRISBCLR = _TRISB_TRISB4_MASK; /* LED */
    RPB0R = 0b0010; /* U2TX for terminal (debug) output */
#else
    TRISBCLR = _TRISB_TRISB4_MASK; /* LED */
    RPB0R= 0b0010; /* U2TX for terminal (debug) output */
#endif
    term_init();
    timer_init();
    log_info("Android Accessory\n");
#ifdef __32MX470F512H__
    log_debug("BMXCON = %08x\n", BMXCON);
    log_debug("CHECON = %08x\n", CHECON);
    /* turn 5V supply voltage for USB port on */
    TRISBCLR = 1<<5;
    LATBSET = 1<<5;
#endif
    rc5_init();
    spdif_out_init();

    USBInitialize(0);
    AndroidAppStart(&myDeviceInfo);

    while(1) {
	spdif_out_tasks();
        //Keep the USB stack running
        USBTasks();

        //If the device isn't attached yet,
        if( registered == false )
        {
            reportToSend = false;
            reportComplete = true;
            report.controls.value = 0;

            //Continue to the top of the while loop to start the check over again.
            continue;
        }
        if(!reportComplete){
            continue;
        }

	if( rc5_get_keycode(&key_code) == 0){
	    key_relase_timer = timer_now() + 500*TIMER_TICKS_PER_MS;
	    log_debug("keycode: %08x\n", key_code);
	    report.controls.value = 0;
	    reportToSend = true;
	    switch(key_code){
		case RC_KEY_PLAY:
		    report.controls.bits.play = 1;
		    break;
		case RC_KEY_PAUSE:
		    report.controls.bits.play_pause = 1;
		    break;
		case RC_KEY_FFWD:
		    report.controls.bits.fast_forward = 1;
		    break;
		case RC_KEY_REWIND:
		    report.controls.bits.rewind = 1;
		    break;
		case RC_KEY_NEXT:
		    report.controls.bits.scan_next = 1;
		    break;
		case RC_KEY_PREV:
		    report.controls.bits.scan_previous = 1;
		    break;		  
		case RC_KEY_STOP:
		    report.controls.bits.stop = 1;
		    break;
		default:
		    reportToSend = false;
	    }
	}else{
	    if((report.controls.value != 0) && (timer_now() >= key_relase_timer)){
		log_debug("release\n");
		report.controls.value = 0;
		reportToSend = true;
	    }
	}

        if(reportToSend){
	    log_debug("sending report, value == %02x\n", report.controls.value);
            if(AndroidAppHIDSendEvent(device_address,
                                      1,
                                      (BYTE*)&report,
                                      sizeof(HID_REPORT)) != USB_SUCCESS)
            {
                log_debug("error\n");
                USBHostClearEndpointErrors(device_address, 0);
            }else{
                reportToSend = false;
                reportComplete = false;
            }
        }

    } //while(1) main loop
}



BOOL USB_ApplicationDataEventHandler( BYTE address, USB_EVENT event, void *data, DWORD size )
{
    switch((int)event)
    {
        case EVENT_AUDIO_STREAM_RECEIVED:

	    //log_debug("A:%u\n", size);
	    spdif_out_tx_s16le(data, size/4);
            return TRUE;

        default:
            break;
    }

    return FALSE;
}


BOOL USB_ApplicationEventHandler( BYTE address, USB_EVENT event, void *data, DWORD size )
{
    switch( (INT)event )
    {
        case EVENT_VBUS_REQUEST_POWER:
            // The data pointer points to a byte that represents the amount of power
            // requested in mA, divided by two.  If the device wants too much power,
            // we reject it.
            if (((USB_VBUS_POWER_EVENT_DATA*)data)->current <= (MAX_ALLOWED_CURRENT / 2))
            {
                return TRUE;
            }
            else
            {
                log_error( "\r\n***** USB Error - device requires too much current *****\r\n" );
            }
            break;

        case EVENT_VBUS_RELEASE_POWER:
        case EVENT_HUB_ATTACH:
        case EVENT_UNSUPPORTED_DEVICE:
        case EVENT_CANNOT_ENUMERATE:
        case EVENT_CLIENT_INIT_ERROR:
        case EVENT_OUT_OF_MEMORY:
        case EVENT_UNSPECIFIED_ERROR:   // This should never be generated.
        case EVENT_DETACH:                   // USB cable has been detached (data: BYTE, address of device)
        case EVENT_ANDROID_DETACH:
            device_attached = false;
            registered = false;
            return TRUE;
            break;

        case EVENT_AUDIO_ATTACH:
	    log_debug("EVENT_AUDIO_ATTACH\n");
            device_address = address;
            audioAccessoryVars.audioDeviceID = data;

            requestStatus = USBHostAudioV1SetInterfaceFullBandwidth(audioAccessoryVars.audioDeviceID->deviceAddress);
                
            if( requestStatus != USB_SUCCESS )
            {
                while(1){
                    Nop();
                }
            }
            return TRUE;

        case EVENT_AUDIO_INTERFACE_SET:
            if(USBHostIsochronousBuffersCreate(&audioData, 2, 1023) == FALSE)
            {
                while(1){
                    Nop();
                }
            }
    
            USBHostIsochronousBuffersReset(&audioData, 2);

            requestStatus = USBHostAudioV1ReceiveAudioData(audioAccessoryVars.audioDeviceID->deviceAddress, &audioData );

            if(requestStatus != USB_SUCCESS)
            {
                while(1){
                    Nop();
                }
            }
            return TRUE;

        case EVENT_AUDIO_DETACH:
            USBHostIsochronousBuffersDestroy(&audioData, 2);
            memset(&audioAccessoryVars, 0, sizeof(audioAccessoryVars));
            return TRUE;

        case EVENT_ANDROID_HID_REGISTRATION_COMPLETE:
            registered = true;
            return TRUE;

        case EVENT_ANDROID_HID_SEND_EVENT_COMPLETE:
            reportComplete = true;
            return TRUE;

        // Android Specific events
        case EVENT_ANDROID_ATTACH:
            device_address = address;
            device_handle = data;
            device_attached = true;
            AndroidAppHIDRegister(device_address, 1, (BYTE*)ReportDescriptor,
                                  sizeof(ReportDescriptor));
            return TRUE;

        default :
            break;
    }
    return FALSE;
}

