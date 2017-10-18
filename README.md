# charging-sdl
"charge required" screen for pmOS.
This application is young! Consider it __highly__ unstable.

# TODOs
- [ ] display whether or not the device is charging
- [ ] display charging type
- [ ] display battery tempurature
- [ ] hide logs with a command line flag
- [ ] maybe add a config file

## build
1. `cd charging-sdl`
2. `make`

## run
1. build the program
2. `./charge -tn`

## usage
- `-t` test mode (stays open until explicitly closed and is not in full screen)
- `-n` no text (at the moment this just means it won't display battery capacity)
- `-f` font used to display all text
