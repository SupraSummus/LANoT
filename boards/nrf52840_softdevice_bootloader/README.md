
Flashing using factory bootloader
--------

For flashing you'll need `nrfutil`. It's [available from pypi](https://pypi.org/project/nrfutil/) - but requires python2. If you have `pipenv` installed, you can simply

    pipenv install
    pipenv shell  # to enter the environment

Then to program attached device do

    make flash

It's expecting device's usb-uart bootloader to be listening at `/dev/ttyACM0`.

To enter bootlader mode on USB840x keep button pressed while powering the device up.

Binaries are signed using Fanstel disclosed key.
