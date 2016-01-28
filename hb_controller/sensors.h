/*
 *  Sensors header.
 *
 *  Copyright 2016 - Laurentiu Palcu <lpalcu@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#define SENSOR_ROWS		4

typedef void (*sensors_changed_cb)();

void sensors_init(sensors_changed_cb cb);
void sensors_scan_start();
void sensors_scan_stop();
void sensors_state_get(uint8_t *sensors_data);
