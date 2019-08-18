Compilation
-----------

Install arm-none-eabi gcc toolchain and C standard library. For arch linux it's

    pacman -S arm-none-eabi-gcc arm-none-eabi-newlib

Download and unpack [nRF5 SDK](https://www.nordicsemi.com/Software-and-Tools/Software/nRF5-SDK) to `./nRF5_SDK_15.3.0_59ac345` dir. I'm working on [current newest version](https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v15.x.x/nRF5_SDK_15.3.0_59ac345.zip).

You need to configure toolchain path for SDK. Edit `nRF5_SDK_15.3.0_59ac345/components/toolchain/gcc/Makefile.posix`. Mine looks like this:

    # location of arm-none-eabi-... executables
    GNU_INSTALL_ROOT ?= /usr/bin/
    # version obtained by `arm-none-eabi-gcc --version`
    GNU_VERSION ?= 9.1.0
    GNU_PREFIX ?= arm-none-eabi

Download and unpack [freeRTOS](https://www.freertos.org/) source to `./FreeRTOSv10.2.1` dir.

You can check downloaded zip(s) against `sha256sums` file.

Then you should be able to succesffuly run `make`.

See also [the tutorial I was following](https://devzone.nordicsemi.com/nordic/nordic-blog/b/blog/posts/development-with-gcc-and-eclipse).

Research level links
--------------------

* https://github.com/libopencm3/libopencm3/
* https://github.com/nRFMesh/nRF52_Mesh https://www.homesmartmesh.com/
* list of rtoses https://www.osrtos.com/
