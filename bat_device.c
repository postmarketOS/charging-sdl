#include <bat_device.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>

int can_access_battery (){
    int methods = 0;
    DIR* dir = opendir("/sys/class/power_supply");
    if (dir) {
        methods |= METHOD_FS;
    }
    return methods;
}

int get_battery_capacity_from_fs() {
    struct dirent* dir_item;
    DIR* dir = opendir("/sys/class/power_supply");
    if (!dir) {
        return -1;
    }
    int total = 0;
    int count = 0;
    
    char fname_buf[40];
    char capacity_buf[4];
    capacity_buf[3] = '\0';
    while ( (dir_item = readdir(dir)) != NULL ) {
        if (dir_item->d_name[0] == 'B' && dir_item->d_name[1] == 'A' && dir_item->d_name[2] == 'T') {
            sprintf(fname_buf, "/sys/class/power_supply/%s/capacity", dir_item->d_name);
            FILE* file = fopen(fname_buf, "r");
            fread(capacity_buf, 3, 1, file);
            ++count;
            total += atoi(capacity_buf);
            fclose(file);
        }
    }
    if (count > 0) {
        return total/count;
    } else {
        return -1;
    }
}