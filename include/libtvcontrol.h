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

#define MFG_NAME "Gizmite Designs"
#define GIZMITE_ADVANCED_BOARD "Apple TV Serial/DFU Control"

#define ADVANCED_MODE_GPIO 1

/************************************************************************/
// Copied from CyUSBSerial.h to allow independability from including it.
#ifndef _INCLUDED_CYUSBSERIAL_H_

#ifndef UINT32
    typedef unsigned int UINT32;
#endif
#ifndef UINT8
    typedef unsigned char UINT8;
#endif
#ifndef UINT16
    typedef unsigned short UINT16;
#endif

typedef struct _CY_FIRMWARE_VERSION {
    UINT8 majorVersion;                 /*Major version of the Firmware*/
    UINT8 minorVersion;                 /*Minor version of the Firmware*/
    UINT16 patchNumber;                 /*Patch Number of the Firmware*/  
    UINT32 buildNumber;                 /*Build Number of the Firmware*/
} CY_FIRMWARE_VERSION, *PCY_FIRMWARE_VERSION;

typedef void* CY_HANDLE;
#endif
/************************************************************************/

typedef enum {
    LIBTVCTL_E_OK = 0,
    LIBTVCTL_E_MALLOC_FAIL,
    LIBTVCTL_E_CYLIB_ERR,
    LIBTVCTL_E_USB_ERR,
    LIBTVCTL_E_GPIO_FAIL,
    LIBTVCTL_E_DFU_TIMEOUT,
    LIBTVCTL_E_NOT_SUPPORTED,
    LIBTVCTL_E_NO_DEVICE,
    LIBTVCTL_E_INVALID_ARGUMENTS
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

#ifdef __cplusplus
extern "C" {
#endif
tvcErr_t tvctrl_find_device(tvcontrol_t **tvcDevice);
tvcErr_t tvctrl_release_device(tvcontrol_t **tvcDevice);
#ifdef __cplusplus
}
#endif

#endif /* libtvcontrol_h */
