/*
*  tvcontrolutil.c
*  TV Control Utility
*
*  Copyright © 2020 Steven Barker. All rights reserved.
*/

#include <stdio.h>
#include <stdlib.h>
#include "libtvcontrol.h"

int main(){
    
    tvcontrol_t *device;
    tvcErr_t err;
    device = (tvcontrol_t*)malloc(sizeof(tvcontrol_t));
    
    err = init(device);
    if(err != E_OK){
        printf("library error\n");
        return err;
    }
    
    if(device->isDevDetected){
        printf("Found Advanced Breakout\n");
        printf("Cyprus FWVersion: %d.%d.%d.%d\n", device->fwVers.majorVersion, device->fwVers.minorVersion, device->fwVers.patchNumber, device->fwVers.buildNumber);
        printf("Current Device Mode: %s\n", device->mode == NORMAL_MODE ? "Normal Mode" : "DFU Boot");
        
        err = device->setEnterDFU((void*)device);
        if(err != E_OK){
            printf("Failed to change USB mode\n");
            releaseTvLib(device);
            return err;
        }
        
        releaseTvLib(device);
        
    }else{
        printf("No device found\n");
    }
    
    return 0;
    
}
