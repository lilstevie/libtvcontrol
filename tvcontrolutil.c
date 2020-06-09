/*
 * tvcontrolutil.c
 * TV Control Utility
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
