/*
 *  libtvcontrol.h
 *  TV Control library header
 *
 *  Copyright © 2020 Steven Barker. All rights reserved.
 */

#ifndef libtvcontrol_h
#define libtvcontrol_h

#include <stdio.h>
#include <stdbool.h>
#include "CyUSBSerial.h"

#define MFG_NAME "Gizmite Designs"
#define GIZMITE_ADVANCED_BOARD "Apple TV Serial/DFU Control"

#define ADVANCED_MODE_GPIO 1

typedef enum {
    E_OK = 0,
    E_MALLOC_FAIL,
    E_CYLIB_ERR,
    E_USB_ERR,
    E_GPIO_FAIL,
    E_DFU_TIMEOUT,
    E_NOT_SUPPORTED,
    E_NO_DEVICE,
    E_INVALID_ARGUMENTS
} tvcErr_t;

typedef enum {
    NORMAL_MODE = 0,
    DFU_BOOT
} tvcUSBMode_t;

struct tvcontrol;
typedef tvcErr_t (*setUSBMode_prototype)(struct tvcontrol *, tvcUSBMode_t);
typedef tvcErr_t (*toggleUSBMode_prototype)(struct tvcontrol *);
typedef tvcErr_t (*rebootDevice_prototype)(struct tvcontrol *);

struct tvcontrol {
    bool isDevDetected;
    CY_HANDLE handle;
    CY_FIRMWARE_VERSION fwVers;
    tvcUSBMode_t mode;
    uint8_t modeGPIO;
    uint8_t resetGPIO;
    setUSBMode_prototype setUSBMode;
    toggleUSBMode_prototype toggleUSBMode;
    rebootDevice_prototype rebootDev;
};

typedef struct tvcontrol tvcontrol_t;

extern tvcErr_t tvctrl_find_device(tvcontrol_t **tvcDevice);
extern tvcErr_t tvctrl_release_device(tvcontrol_t **tvcDevice);

#endif /* libtvcontrol_h */
