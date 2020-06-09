/*
 * libtvcontrol.h
 * TV Control library header
 *
 * MIT License
 *
 * Copyright © 2020 Steven Barker. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef libtvcontrol_h
#define libtvcontrol_h

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
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
