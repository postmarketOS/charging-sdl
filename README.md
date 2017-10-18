# charging_sdl
"charge required" screen for pmOS.
This application is young! Consider it __highly__ unstable.

# TODOs
- display whether or not the device is charging
- display charging mode if available
- hide logs

## build
1. `cd charge-me`
2. `make`

## run
1. build the program
2. `./charge -tn`

## usage
- `-t` test mode (stays open until explicitly closed and is not in full screen)
- `-n` no text (at the moment this just means it won't display battery capacity)
- `-f` font to use to display battery capacity
