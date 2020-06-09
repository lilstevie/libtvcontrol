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
    
    tvcontrol_t *device = NULL;
    tvcErr_t err;
    
    if (E_OK != (err = tvctrl_find_device(&device)) || device == NULL) {
        if (E_NO_DEVICE == err)
            printf("No device found\n");
        else
            printf("library error\n");

        return err;
    }

    printf("Found Advanced Breakout\n");
    printf("Cyprus FWVersion: %d.%d.%d.%d\n", device->fwVers.majorVersion, device->fwVers.minorVersion, device->fwVers.patchNumber, device->fwVers.buildNumber);
    printf("Current Device Mode: %s\n", device->mode == NORMAL_MODE ? "Normal Mode" : "DFU Boot");
    
    err = device->toggleEnterDFUMode(device);
    tvctrl_release_device(&device);
    
    if (err != E_OK) {
        printf("Failed to change USB mode\n");
        return err;
    }

    printf("New Device Mode: %s\n", device->mode == NORMAL_MODE ? "Normal Mode" : "DFU Boot");
    return 0;    
}
