/*
 *  libtvcontrol.c
 *  TV Control library
 *
 *  Copyright © 2020 Steven Barker. All rights reserved.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "libtvcontrol.h"
#include "CyUSBSerial.h"

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

tvcErr_t setEnterDFU_Advanced(tvcontrol_t *tvcDevice){

    CY_RETURN_STATUS cyStatus;
    
    if(!tvcDevice->isDevDetected)
        return E_NO_DEVICE;
    
    cyStatus = CySetGpioValue(tvcDevice->handle, tvcDevice->modeGPIO, (uint8_t)tvcDevice->mode);
    if(cyStatus != CY_SUCCESS)
        return E_GPIO_FAIL;
    
    cyStatus = CyGetGpioValue(tvcDevice->handle, tvcDevice->modeGPIO, (uint8_t*)&tvcDevice->mode);
    if(cyStatus != CY_SUCCESS)
        return E_GPIO_FAIL;
    
    return E_OK;
    
}

tvcErr_t rebootDevice_Advanced(tvcontrol_t *tvcDevice){
    
    return E_NOT_SUPPORTED;
}

tvcErr_t tvctrl_find_device(tvcontrol_t **tvcDevice)
{
    CY_DEVICE_INFO devInfo;
    CY_HANDLE dhandle;
    tvcUSBMode_t currentMode;
    int usbDevices = 0;

    if (*tvcDevice != NULL)
        return E_INVALID_ARGUMENTS;
    else if (CY_SUCCESS != CyLibraryInit())
        return E_CYLIB_ERR;
    else if (CY_SUCCESS != CyGetListofDevices((UINT8 *)&usbDevices))
        return E_USB_ERR;

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
            } else if (CY_SUCCESS != CyGetGpioValue(dhandle, 1, (uint8_t *)&currentMode)) {
                CyClose(dhandle);
                return E_GPIO_FAIL;
            } else if ((*tvcDevice = (tvcontrol_t *)calloc(1, sizeof(tvcontrol_t))) == NULL)
                CyClose(dhandle);
                return E_MALLOC_FAIL;

            (*tvcDevice)->isDevDetected = true;
            (*tvcDevice)->handle = dhandle;
            (*tvcDevice)->mode = currentMode;
            (*tvcDevice)->modeGPIO = 1;
            (*tvcDevice)->setEnterDFU = (setEnterDFU)&setEnterDFU_Advanced;
            (*tvcDevice)->rebootDev = (rebootDevice)&rebootDevice_Advanced;
            getFWVer(*tvcDevice);

            return E_OK;
        }
    }

    return E_NO_DEVICE;
}

tvcErr_t tvctrl_release_device(tvcontrol_t **tvcDevice) {
    
    CY_HANDLE handle;
    
    if (*tvcDevice == NULL)
        return E_INVALID_ARGUMENTS;
    
    handle = (*tvcDevice)->handle;
    free(*tvcDevice);
    *tvcDevice = NULL;

    return (CY_SUCCESS != CyClose(handle)) ? E_CYLIB_ERR : E_OK;
}

