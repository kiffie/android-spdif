// Created by the Microchip USBConfig Utility

#include "GenericTypeDefs.h"
#include "USB/usb.h"
#include "USB/usb_host_android.h"
#include "USB/usb_host_audio_v1.h"

// *****************************************************************************
// Client Driver Function Pointer Table for the USB Embedded Host foundation
// *****************************************************************************

CLIENT_DRIVER_TABLE usbClientDrvTable[NUM_CLIENT_DRIVER_ENTRIES] =
{                                        
    {
        AndroidAppInitialize,
        AndroidAppEventHandler,
        AndroidAppDataEventHandler,
        0
    },
    {
        AndroidAppInitialize,
        AndroidAppEventHandler,
        AndroidAppDataEventHandler,
        ANDROID_INIT_FLAG_BYPASS_PROTOCOL
    },
    {
        USBHostAudioV1Initialize,
        USBHostAudioV1EventHandler,
        USBHostAudioV1DataEventHandler,
        0
    }
};

// *****************************************************************************
// USB Embedded Host Targeted Peripheral List (TPL)
// *****************************************************************************
USB_TPL usbTPL[NUM_TPL_ENTRIES] =
{
      /*[1] Device identification information
        [2] Initial USB configuration to use
        [3] Client driver table entry
        [4] Flags (HNP supported, client driver entry, SetConfiguration() commands allowed)
    ---------------------------------------------------------------------
                [1]                      [2][3] [4]
    ---------------------------------------------------------------------*/
    { INIT_CL_SC_P( 0xFFul, 0xFFul, 0xFFul ),   0, 0, {TPL_CLASS_DRV | TPL_IGNORE_SUBCLASS | TPL_IGNORE_PROTOCOL} }, // Android accessory
    { INIT_CL_SC_P( 0x01ul, 0x01ul, 0x00ul ),   0, 2, {TPL_CLASS_DRV | TPL_IGNORE_PROTOCOL} }, // Audio class
    { INIT_CL_SC_P( 0x01ul, 0x02ul, 0x00ul ),   0, 2, {TPL_CLASS_DRV | TPL_IGNORE_PROTOCOL} }, // Audio class
    
    { INIT_VID_PID( 0x18D1ul, 0x2D00ul ), 0, 1, {TPL_EP0_ONLY_CUSTOM_DRIVER} }, // Enumerates everything
    { INIT_VID_PID( 0x18D1ul, 0x2D01ul ), 0, 1, {TPL_EP0_ONLY_CUSTOM_DRIVER} }, // Enumerates everything
    { INIT_VID_PID( 0x18D1ul, 0x2D02ul ), 0, 1, {TPL_EP0_ONLY_CUSTOM_DRIVER} }, // Enumerates everything
    { INIT_VID_PID( 0x18D1ul, 0x2D03ul ), 0, 1, {TPL_EP0_ONLY_CUSTOM_DRIVER} }, // Enumerates everything
    { INIT_VID_PID( 0x18D1ul, 0x2D04ul ), 0, 1, {TPL_EP0_ONLY_CUSTOM_DRIVER} }, // Enumerates everything
    { INIT_VID_PID( 0x18D1ul, 0x2D05ul ), 0, 1, {TPL_EP0_ONLY_CUSTOM_DRIVER} }, // Enumerates everything
};

