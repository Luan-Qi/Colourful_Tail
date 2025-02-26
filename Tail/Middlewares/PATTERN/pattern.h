#ifndef __PATTERN_H
#define __PATTERN_H

#include "at32f421_clock.h"
#include <stdio.h>

extern const uint8_t pattern_love[];
extern const uint8_t pattern_fan[2][81];
extern const uint8_t pattern_happy_A[];
extern const uint8_t pattern_happy_B[];
extern const uint8_t pattern_happy_C[];
extern const uint8_t pattern_skeleton_A[];
extern const uint8_t pattern_skeleton_B[];
extern const uint8_t pattern_warn_A[4][81];
extern const uint8_t pattern_warn_B[];
extern const uint16_t pattern_warn_fps_delay[4];
extern const uint8_t maomao_R[];
extern const uint8_t maomao_G[];
extern const uint8_t maomao_B[];
extern const uint8_t pattern_zoom[8][81];
extern const uint8_t pattern_number[10][81];
extern const uint8_t pattern_battery[];
extern const uint8_t pattern_battery_Full[];
extern const uint8_t pattern_battery_Medium[];
extern const uint8_t pattern_battery_Low[];
extern const uint8_t pattern_battery_Warn[];
extern const uint8_t pattern_battery_empty[];
			
void pattern_show(void);

#endif

