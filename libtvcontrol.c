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

tvcErr_t init(tvcontrol_t *tvcDevice) {
    
    CY_RETURN_STATUS cyStatus;
    CY_DEVICE_INFO devInfo;
    CY_HANDLE dhandle;
    
    int usbDevices = 0;
    
    tvcDevice->isDevDetected = false;
    cyStatus = CyLibraryInit();
    if(cyStatus != CY_SUCCESS){
        return E_CYLIB_ERR;
    }
    
    cyStatus = CyGetListofDevices((UINT8 *)&usbDevices);
    if(cyStatus != CY_SUCCESS)
        return E_USB_ERR;
    
    for(int devNum = 0; devNum < usbDevices; devNum++){
        cyStatus = CyGetDeviceInfo(devNum, &devInfo);
        if(cyStatus == CY_SUCCESS){
            if(strncmp((const char*)devInfo.manufacturerName, MFG_NAME, sizeof(MFG_NAME)) == 0){
                if(isValidDevice(devNum)){
                    for(int ifaceNum = 0; ifaceNum < devInfo.numInterfaces; ifaceNum++){
                        if(devInfo.deviceClass[ifaceNum] == CY_CLASS_VENDOR){
                            cyStatus = CyOpen(devNum, ifaceNum, &dhandle);
                            if(cyStatus == CY_SUCCESS){
                                tvcDevice->handle = dhandle;
                                tvcDevice->isDevDetected = true;
                                getFWVer(tvcDevice);
                                if(strncmp((const char*)devInfo.productName, GIZMITE_ADVANCED_BOARD,    sizeof(GIZMITE_ADVANCED_BOARD)) == 0){
                                    tvcDevice->modeGPIO = 1;
                                    cyStatus = CyGetGpioValue(tvcDevice->handle, tvcDevice->modeGPIO, (uint8_t*)&tvcDevice->mode);
                                    if(cyStatus != CY_SUCCESS)
                                        return E_GPIO_FAIL;
                                    tvcDevice->setEnterDFU = &setEnterDFU_Advanced;
                                    tvcDevice->rebootDev = &rebootDevice_Advanced;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    return E_OK;
}

tvcErr_t releaseTvLib(tvcontrol_t *tvcDevice){
    
    CY_RETURN_STATUS cyStatus;
    
    cyStatus = CyClose(tvcDevice->handle);
    if(cyStatus != CY_SUCCESS)
        return E_CYLIB_ERR;
    
    return E_OK;
}

