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
#include <string.h>
#include "libtvcontrol.h"

void print_usage(const char *progname) {
    fprintf(stderr, "Usage: %s [OPTIONS | MODES]\n", progname);
    fprintf(stderr, "\n  A tool to configure Apple TV 4k Advanced boards\n");
    fprintf(stderr, "\nOPTIONS:\n");
    fprintf(stderr, "    -h, --help    Prints usage info\n");
    fprintf(stderr, "    -t, --toggle  Toggles USB mode\n");
    fprintf(stderr, "\nMODES:\n");
    fprintf(stderr, "    0, off        Sets USB mode to 'Normal Mode'\n");
    fprintf(stderr, "    1, on         Sets USB mode to 'DFU Boot'\n");
    fprintf(stderr, "\n* When no arguments are provided the tool will print the current USB mode.\n");
}

int main(int argc, char *argv[]) {
    
    tvcontrol_t *device = NULL;
    tvcErr_t err;

    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        print_usage(*argv);
        return 0;
    }
    
    if (LIBTVCTL_E_OK != (err = tvctrl_find_device(&device)) || device == NULL) {
        if (LIBTVCTL_E_NO_DEVICE == err)
            printf("No device found\n");
        else
            printf("library error\n");

        return err;
    }

    printf("Found Advanced Breakout\n");
    printf("Cypress FWVersion: %d.%d.%d.%d\n", device->fwVers.majorVersion, device->fwVers.minorVersion, device->fwVers.patchNumber, device->fwVers.buildNumber);
    printf("Current Device Mode: %s\n", device->mode == NORMAL_MODE ? "Normal Mode" : "DFU Boot");

    if (argc == 2) {
        if (strcmp(argv[1], "-t") == 0 || strcmp(argv[1], "--toggle") == 0) {
            fprintf(stderr, "Toggling USB mode\n");
            err = device->toggleUSBMode(device);
        } else if (strcmp(argv[1], "1") == 0 || strcmp(argv[1], "on") == 0) {
            fprintf(stderr, "Setting USB mode to 'DFU Boot'\n");
            err = device->setUSBMode(device, DFU_BOOT);
        } else if (strcmp(argv[1], "0") == 0 || strcmp(argv[1], "off") == 0) {
            fprintf(stderr, "Setting USB mode to 'Normal Mode'\n");
            err = device->setUSBMode(device, NORMAL_MODE);
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[1]);
            print_usage(*argv);
            return LIBTVCTL_E_INVALID_ARGUMENTS;
        }

        if (LIBTVCTL_E_OK != err)
            printf("Failed to change USB mode\n");
        else
            printf("New Device Mode: %s\n", device->mode == NORMAL_MODE ? "Normal Mode" : "DFU Boot");
    }
    
    tvctrl_release_device(&device);
    return err;
}
