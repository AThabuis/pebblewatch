#pragma once
#include <pebble.h>
#ifndef ACCELERATION_PROCESS
#define ACCELERATION_PROCESS

//Called when the buffer is full of accelerater's data
void accel_data_handler(AccelData *, uint32_t);

void init_accel(void);

#endif