# charging-sdl

Charging screen for the initramfs.

![](https://wiki.postmarketos.org/images/d/d8/Charging-sdl.jpg)



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
- `-f` font used to display all text
