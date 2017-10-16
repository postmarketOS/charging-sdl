# charge-me
prototype charge required display for pmOS

quick prototype I threw together for a charging screen. Right now it needs __a lot__ of work.
Specifically it's not as performant as it should be, and it only uses one method to check for battery capacity.
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
