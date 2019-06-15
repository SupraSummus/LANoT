Compilation
-----------

Install arm-none-eabi gcc toolchain and C standard library. For arch linux it's

    pacman -S arm-none-eabi-gcc arm-none-eabi-newlib

Download and unpack nRF5 SDK to `./sdk` dir. I'm working on [current newest version](https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v15.x.x/nRF5_SDK_15.3.0_59ac345.zip).

You need to configure toolchain path for SDK. Edit `sdk/components/toolchain/gcc/Makefile.posix`. Mine looks like this:

    # location of arm-none-eabi-... executables
    GNU_INSTALL_ROOT ?= /usr/bin/
    # version obtained by `arm-none-eabi-gcc --version`
    GNU_VERSION ?= 9.1.0
    GNU_PREFIX ?= arm-none-eabi

Then you should be able to succesffuly run `make`.

See also [the tutorial I was following](https://devzone.nordicsemi.com/nordic/nordic-blog/b/blog/posts/development-with-gcc-and-eclipse).

Flashing
--------

TODO

tools https://www.nordicsemi.com/Software-and-Tools/Development-Tools/nRF5-Command-Line-Tools/Download#infotabs

