/*
 * libtvcontrol.c
 * TV Control library
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

#define _GNU_SOURCE
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#ifdef __linux__
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#endif

#include "CyUSBSerial.h"
#include "libtvcontrol.h"

int numDevices = 0;
unsigned char *deviceNumber = NULL;

bool isValidDevice (int devNum) {
    CY_HANDLE handle;
    
    int intNum = 2;
    unsigned char sig[6];
    CY_RETURN_STATUS rStatus;
    
    rStatus = CyOpen(devNum, intNum, &handle);

    if(rStatus == CY_SUCCESS){
        rStatus = CyGetSignature(handle, sig);
        if(rStatus == CY_SUCCESS){
            CyClose(handle);
            return true;
        } else {
            CyClose(handle);
            return false;
        }
    }
    
    return false;
}

void getFWVer(tvcontrol_t *tvcDevice){
    
    CY_FIRMWARE_VERSION fwVers;
    
    CyGetFirmwareVersion(tvcDevice->handle, &fwVers);
    
    tvcDevice->fwVers.majorVersion = fwVers.majorVersion;
    tvcDevice->fwVers.minorVersion = fwVers.minorVersion;
    tvcDevice->fwVers.patchNumber = fwVers.patchNumber;
    tvcDevice->fwVers.buildNumber = fwVers.buildNumber;
    
}

tvcErr_t setUSBMode(tvcontrol_t *tvcDevice, tvcUSBMode_t mode) {
    CY_RETURN_STATUS cyStatus;
    uint8_t val;

    if (tvcDevice == NULL)
        return LIBTVCTL_E_INVALID_ARGUMENTS;
    
    if(LIBTVCTL_E_NO_DEVICE == tvctrl_find_device(&tvcDevice))
        return LIBTVCTL_E_NO_DEVICE;
    
    cyStatus = CySetGpioValue(tvcDevice->handle, tvcDevice->modeGPIO, (uint8_t)mode);
    if(cyStatus != CY_SUCCESS)
        return LIBTVCTL_E_GPIO_FAIL;
    
    cyStatus = CyGetGpioValue(tvcDevice->handle, tvcDevice->modeGPIO, &val);
    tvcDevice->mode = val;
    if(cyStatus != CY_SUCCESS || mode != tvcDevice->mode)
        return LIBTVCTL_E_GPIO_FAIL;
    
    return LIBTVCTL_E_OK;
}

tvcErr_t toggleUSBMode(tvcontrol_t *tvcDevice) {
    if (tvcDevice == NULL)
        return LIBTVCTL_E_INVALID_ARGUMENTS;

    tvcUSBMode_t mode = (tvcDevice->mode == NORMAL_MODE) ? DFU_BOOT : NORMAL_MODE;
    return setUSBMode(tvcDevice, mode);
}

tvcErr_t rebootDevice(tvcontrol_t *tvcDevice){
    
    return LIBTVCTL_E_NOT_SUPPORTED;
}

tvcErr_t tvctrl_find_device(tvcontrol_t **tvcDevice) {
    CY_DEVICE_INFO devInfo;
    CY_HANDLE dhandle;
    uint8_t currentMode;
    int usbDevices = 0;

    if (tvcDevice == NULL || *tvcDevice != NULL)
        return LIBTVCTL_E_INVALID_ARGUMENTS;
    else if (CY_SUCCESS != CyLibraryInit())
        return LIBTVCTL_E_CYLIB_ERR;
    else if (CY_SUCCESS != CyGetListofDevices((UINT8 *)&usbDevices))
        return LIBTVCTL_E_USB_ERR;

#ifdef STATIC_BUILD
#ifdef __linux__
    /* we have to unbind devices from the 'cytherm' driver, otherwise opening the device will fail */
    DIR* dir = opendir("/sys/bus/usb/drivers/cytherm");
    if (dir) {
        struct dirent* ep;
        while ((ep = readdir(dir))) {
            if (isdigit(ep->d_name[0])) {
                char* colon = strchr(ep->d_name, ':');
                if (!colon) {
                    continue;
                }
                char devpath[256];
                char* p = stpcpy(devpath, "/sys/bus/usb/devices/");
                p = stpncpy(p, ep->d_name, colon - &ep->d_name[0]);
                p = stpcpy(p, "/product");
                FILE* f = fopen(devpath, "r");
                if (f) {
                    /* match the product name */
                    char buf[64];
                    int r = fread(buf, 1, sizeof(buf)-1, f);
                    buf[r] = '\0';
                    fclose(f);
                    if (strncmp(buf, GIZMITE_ADVANCED_BOARD, sizeof(GIZMITE_ADVANCED_BOARD)-1) == 0) {
                        f = fopen("/sys/bus/usb/drivers/cytherm/unbind", "w");
                        if (f) {
                            fprintf(f, "%s\n", ep->d_name);
                            fclose(f);
                        }
                    }
                }
            }
        }
        closedir(dir);
    }
#endif
#endif

    for (int devNum = 0; devNum < usbDevices; devNum++) {
        if (CY_SUCCESS != CyGetDeviceInfo(devNum, &devInfo))
            continue;
        else if (strncmp((const char *)devInfo.manufacturerName, MFG_NAME, sizeof(MFG_NAME)) != 0)
            continue;
        else if (false == isValidDevice(devNum))
            continue;

        for (int ifaceNum = 0; ifaceNum < devInfo.numInterfaces; ifaceNum++) {
            if (devInfo.deviceClass[ifaceNum] != CY_CLASS_VENDOR)
                continue;
            else if (CY_SUCCESS != CyOpen(devNum, ifaceNum, &dhandle))
                continue;
            else if (strncmp((const char *)devInfo.productName, GIZMITE_ADVANCED_BOARD, sizeof(GIZMITE_ADVANCED_BOARD)) != 0) {
                CyClose(dhandle);
                continue;
            } else if (CY_SUCCESS != CyGetGpioValue(dhandle, 1, &currentMode)) {
                CyClose(dhandle);
                return LIBTVCTL_E_GPIO_FAIL;
            } else if ((*tvcDevice = (tvcontrol_t *)calloc(1, sizeof(tvcontrol_t))) == NULL) {
                CyClose(dhandle);
                return LIBTVCTL_E_MALLOC_FAIL;
            }
            
            (*tvcDevice)->handle = dhandle;
            (*tvcDevice)->mode = currentMode;
            (*tvcDevice)->modeGPIO = 1;
            (*tvcDevice)->setUSBMode = (setUSBMode_prototype)&setUSBMode;
            (*tvcDevice)->toggleUSBMode = (toggleUSBMode_prototype)&toggleUSBMode;
            (*tvcDevice)->rebootDev = (rebootDevice_prototype)&rebootDevice;
            getFWVer(*tvcDevice);

            return LIBTVCTL_E_OK;
        }
    }

    return LIBTVCTL_E_NO_DEVICE;
}

tvcErr_t tvctrl_release_device(tvcontrol_t **tvcDevice) {
    
    CY_HANDLE handle;
    
    if (tvcDevice == NULL || *tvcDevice == NULL)
        return LIBTVCTL_E_INVALID_ARGUMENTS;
    
    handle = (*tvcDevice)->handle;
    free(*tvcDevice);
    *tvcDevice = NULL;

    if(CY_SUCCESS != CyClose(handle))
        return LIBTVCTL_E_CYLIB_ERR;

    return (CY_SUCCESS != CyLibraryExit()) ? LIBTVCTL_E_CYLIB_ERR : LIBTVCTL_E_OK;
}

