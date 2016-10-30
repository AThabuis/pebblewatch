#pragma once
#include <pebble.h>
#ifndef ACCELERATION_PROCESS
#define ACCELERATION_PROCESS

void accel_data_handler(AccelData *, uint32_t);
void init_accel(void);

#endif