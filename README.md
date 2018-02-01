# charging-sdl

[![build status](https://travis-ci.org/postmarketOS/charging-sdl.svg?branch=master)](https://travis-ci.org/postmarketOS/charging-sdl)

Charging screen for the initramfs.

![photo](https://wiki.postmarketos.org/images/d/d8/Charging-sdl.jpg)



## TODOs

- [x] display whether or not the device is charging
- [ ] speed up the font atlas rendering
- [ ] display charging type
- [ ] display battery temperature
- [ ] show battery voltage

## build

1. `cd charging-sdl`
2. `make` or if you want fancy features `make LIBBATTERY=true`

## run

- without text `./charging_sdl -t`
- with text `./charging_sdl -tpcf path/to/font`

## usage

- `-t` test mode, keep the application open until explicitly closed and is not in full screen
- `-p` display the percent charged the battery is
- `-c` display current when charging (if available - see build section)
- `-o` display a square, and move it randomly around the screen to prevent burn-in on OLED screens
- `-f` font used to display all text
