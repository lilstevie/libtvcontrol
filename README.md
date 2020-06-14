# libtvcontrol

A library to control whether an Apple TV 4K boots to DFU or Normal mode using an Advanced Breakout.

## License

libtvcontrol and tvcontrolutil are licensed under the [MIT license](https://choosealicense.com/licenses/mit/) included in the `LICENSE` file. 

`libcyusbserial.a` is licensed under the [GNU Lesser General Public License, version 2.1](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html), also included in the `LICENSE_CYPRESS` file. The static object compiled from the source supplied by Cypress in [USB-Serial Software Development Kit](https://www.cypress.com/documentation/software-and-drivers/usb-serial-software-development-kit) and includes both libcyusbserial and libusb-1.0.9 sources from the Cypress archive. 

`libcyusbserial_static.a` also includes musl libc which is licensed under the [MIT license](https://choosealicense.com/licenses/mit/), also included in the `LICENSE` file.

## Supported Platforms

Current platform and architecture combinations supported:

 - macOS x86-64
 - Linux x86-64

Coming soon:

 - Linux i486
 - Linux arm
 - Linux arm64

TODO:

 - Windows x86-64

## Usage

Documentation for usage of the library is coming soon, `tvcontrolutil.c` is an example implementation that can be referred to until documentation is available.

Usage for tvcontrolutil is:

    
    ./tvcontrolutil [OPTIONS | MODE]
    Options
    -h, --help  Prints usage info
    -t, --toggle  Toggles USB mode
    
    Modes
    0, off  Sets USB mode to 'Normal Mode'
    1, on Sets USB mode to 'DFU Boot'
    
    * When no arguments are provided the tool will print the current USB mode.

After setting boot mode to DFU entry reboot your Apple TV with the Siri remote, or idevicediagnostics to enter DFU. Any reboot will automatically enter DFU until the mode is changed, or the advanced breakout USB is unplugged.

## Building
Building requires clang and related libraries on Linux, and Xcode Command Line Tools on macOS.

## Contributions

All contributions are welcome. Merge requests will be evaluated on their merits and all decisions will be discussed if rejected. 

Contributions should follow the following guidelines:

 - Follow consistent indentation and code styles with the rest of the project.
 - Commit messages should be descriptive of the change where possible.

## Credits and Acknowledgements

Apple, Apple TV, Siri, Mac, and macOS are trademarks of Apple Inc.

Thanks to [Dany Lisiansky](https://github.com/danyl) for adding a build system, and usability improvements to the library. 
