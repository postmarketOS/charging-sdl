#ifndef BAT_DEVICE_H
#define BAT_DEVICE_H

//TODO at the moment this just looks in /sys/ for battery info, it would be good to have more methods

//methods to get the battery info from
#define METHOD_FS 1

/**
  get a list of methods available to access the battery capacity
  @returns 0 if no methods are available, or all available methods or'd together
*/
int can_access_battery();

/**
  get the battery capacity use /sys/class/power_supply directory
  @returns the average capcity of all batteries in the /sys/class/power_supply directory
*/
int get_battery_capacity_from_fs();    
#endif