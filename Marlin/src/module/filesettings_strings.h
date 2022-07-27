/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#pragma once

/******** ACCELERATION ***********/
const char FSS_ACCEL_MAX_X[] = "ACCELERATE_MAX_X";
const char FSSC_ACCEL_MAX_X[] = " # mm/s2, M201 X";

const char FSS_ACCEL_MAX_Y[] = "ACCELERATE_MAX_Y";
const char FSSC_ACCEL_MAX_Y[] = " # mm/s2, M201 Y";

const char FSS_ACCEL_MAX_Z[] = "ACCELERATE_MAX_Z";
const char FSSC_ACCEL_MAX_Z[] = " # mm/s2, M201 Z";

const char FSS_ACCEL_MAX_E[] = "ACCELERATE_MAX_E";
const char FSSC_ACCEL_MAX_E[] = " # mm/s2, M201 E";

const char FSS_PRINT_ACCEL[] = "ACCELERATE_PRINT";
const char FSSC_PRINT_ACCEL[] = " # mm/s2, M204 S";

const char FSS_RETRACT_ACCEL[] = "ACCELERATE_RETRACT";
const char FSSC_RETRACT_ACCEL[] = " # mm/s2, M204 R";

const char FSS_TRAVEL_ACCEL[] = "ACCELERATE_TRAVEL";
const char FSSC_TRAVEL_ACCEL[] = " # mm/s2, M204 T";

const char FSS_MIN_SEGMENT_TIME[] = "MIN_SEGMENT_TIME";
const char FSSC_MIN_SEGMENT_TIME[] = " # µs, M205 B";

/******** STEPS PER MM ***********/
const char FSS_STEPS_PER_MM_X[] = "STEPS_PER_MM_X";
const char FSSC_STEPS_PER_MM_X[] = " # steps, M92 X";

const char FSS_STEPS_PER_MM_Y[] = "STEPS_PER_MM_Y";
const char FSSC_STEPS_PER_MM_Y[] = " # steps, M92 Y";

const char FSS_STEPS_PER_MM_Z[] = "STEPS_PER_MM_Z";
const char FSSC_STEPS_PER_MM_Z[] = " # steps, M92 Z";

const char FSS_STEPS_PER_MM_E[] = "STEPS_PER_MM_E";
const char FSSC_STEPS_PER_MM_E[] = " # steps, M92 E";

/******** SPEED ***********/
const char FSS_MAX_SPEED_X[] = "SPEED_MAX_X";
const char FSSC_MAX_SPEED_X[] = " # mm/s, M203 X";

const char FSS_MAX_SPEED_Y[] = "SPEED_MAX_Y";
const char FSSC_MAX_SPEED_Y[] = " # mm/s, M203 Y";

const char FSS_MAX_SPEED_Z[] = "SPEED_MAX_Z";
const char FSSC_MAX_SPEED_Z[] = " # mm/s, M203 Z";

const char FSS_MAX_SPEED_E[] = "SPEED_MAX_E";
const char FSSC_MAX_SPEED_E[] = " # mm/s, M203 E";

const char FSS_MIN_PRINT_SPEED[] = "SPEED_MIN_PRINT";
const char FSSC_MIN_PRINT_SPEED[] = " # mm/s, M205 S";

const char FSS_MIN_TRAVEL_SPEED[] = "SPEED_MIN_TRAVEL";
const char FSSC_MIN_TRAVEL_SPEED[] = " # mm/s, M205 T";

/******** JERKS ***********/
const char FSS_MAX_JERK_X[] = "JERK_MAX_X";
const char FSSC_MAX_JERK_X[] = " # M205 X";

const char FSS_MAX_JERK_Y[] = "JERK_MAX_Y";
const char FSSC_MAX_JERK_Y[] = " # M205 Y";

const char FSS_MAX_JERK_Z[] = "JERK_MAX_Z";
const char FSSC_MAX_JERK_Z[] = " # M205 Z";

const char FSS_MAX_JERK_E[] = "JERK_MAX_E";
const char FSSC_MAX_JERK_E[] = " # M205 E";

/******** STEPPERS ***********/
const char FSS_STEPPER_INVERT_X[] = "STEPPER_INVERT_X";
const char FSSC_STEPPER_INVERT_X[] = " ";

const char FSS_STEPPER_INVERT_Y[] = "STEPPER_INVERT_Y";
const char FSSC_STEPPER_INVERT_Y[] = " ";

const char FSS_STEPPER_INVERT_Z1[] = "STEPPER_INVERT_Z1";
const char FSSC_STEPPER_INVERT_Z1[] = " ";

const char FSS_STEPPER_INVERT_Z2[] = "STEPPER_INVERT_Z2_TO_Z1";
const char FSSC_STEPPER_INVERT_Z2[] = " # invert Z2 with respect to Z1";

const char FSS_STEPPER_INVERT_E[] = "STEPPER_INVERT_E";
const char FSSC_STEPPER_INVERT_E[] = " ";

/******** HOME OFFSET ***********/
const char FSS_HOME_OFFSET_X[] = "HOME_OFFSET_X";
const char FSSC_HOME_OFFSET_X[] = " # mm, M206 X";

const char FSS_HOME_OFFSET_Y[] = "HOME_OFFSET_Y";
const char FSSC_HOME_OFFSET_Y[] = " # mm, M206 Y";

const char FSS_HOME_OFFSET_Z[] = "HOME_OFFSET_Z";
const char FSSC_HOME_OFFSET_Z[] = " # mm, M206 Z";

/******** FILAMENT RUNOUT SENSOR ***********/
const char FSS_FILAMENTSENSOR_ENABLED[] = "FILAMENTSENSOR_ENABLED";
const char FSSC_FILAMENTSENSOR_ENABLED[] = " # M412 S";

const char FSS_FILAMENTSENSOR_DISTANCE[] = "FILAMENTSENSOR_DISTANCE";
const char FSSC_FILAMENTSENSOR_DISTANCE[] = " # M412 D";

/******** BED LEVELING ***********/
const char FSS_BEDLEVEL_FADE_HEIGHT[] = "BEDLEVEL_FADE_HEIGHT";
const char FSSC_BEDLEVEL_FADE_HEIGHT[] = " # mm";

const char FSS_BEDLEVEL_BLTOUCH_ENABLED[] = "BEDLEVEL_BLTOUCH_ENABLED";
const char FSSC_BEDLEVEL_BLTOUCH_ENABLED[] = " ";

const char FSS_BEDLEVEL_BLTOUCH_INVERT[] = "BEDLEVEL_BLTOUCH_INVERT";
const char FSSC_BEDLEVEL_BLTOUCH_INVERT[] = " ";

const char FSS_BEDLEVEL_BLTOUCH_OFFSET_X[] = "BEDLEVEL_BLTOUCH_OFFSET_X";
const char FSSC_BEDLEVEL_BLTOUCH_OFFSET_X[] = " # mm, M851 X";

const char FSS_BEDLEVEL_BLTOUCH_OFFSET_Y[] = "BEDLEVEL_BLTOUCH_OFFSET_Y";
const char FSSC_BEDLEVEL_BLTOUCH_OFFSET_Y[] = " # mm, M851 Y";

const char FSS_BEDLEVEL_BLTOUCH_OFFSET_Z[] = "BEDLEVEL_BLTOUCH_OFFSET_Z";
const char FSSC_BEDLEVEL_BLTOUCH_OFFSET_Z[] = " # mm, M851 Z";

const char FSS_BEDLEVEL_POINTS_X[] = "BEDLEVEL_POINTS_X";
const char FSSC_BEDLEVEL_POINTS_X[] = " ";

const char FSS_BEDLEVEL_POINTS_Y[] = "BEDLEVEL_POINTS_Y";
const char FSSC_BEDLEVEL_POINTS_Y[] = " ";

const char FSS_BEDLEVEL_VALUES[] = "BEDLEVEL_Z_VALUES";
const char FSSC_BEDLEVEL_VALUES[] = " # mm";

const char FSS_SERVO_ANGLES[] = "SERVO_ANGLES";
const char FSSC_SERVO_ANGLES[] = " ";

/******** ENDSTOPS ***********/
const char FSS_ENDSTOP_INVERT_X[] = "ENDSTOP_INVERT_X";
const char FSSC_ENDSTOP_INVERT_X[] = " ";

const char FSS_ENDSTOP_INVERT_Y[] = "ENDSTOP_INVERT_Y";
const char FSSC_ENDSTOP_INVERT_Y[] = " ";

const char FSS_ENDSTOP_INVERT_Z1[] = "ENDSTOP_INVERT_Z1";
const char FSSC_ENDSTOP_INVERT_Z1[] = " ";

const char FSS_ENDSTOP_INVERT_Z2[] = "ENDSTOP_INVERT_Z2";
const char FSSC_ENDSTOP_INVERT_Z2[] = " ";

const char FSS_ENDSTOP_ADJUST_Z2[] = "ENDSTOP_ADJUST_Z2";
const char FSSC_ENDSTOP_ADJUST_Z2[] = " # mm";

/******** TEMPERATURE ***********/
const char FSS_PREHEAT_HOTEND_1[] = "PREHEAT_HOTEND_1";
const char FSSC_PREHEAT_HOTEND_1[] = " # °C, hotend temperature";

const char FSS_PREHEAT_BED_1[] = "PREHEAT_BED_1";
const char FSSC_PREHEAT_BED_1[] = " # °C, bed temperature";

const char FSS_PREHEAT_FAN_1[] = "PREHEAT_FAN_1";
const char FSSC_PREHEAT_FAN_1[] = " # fan speed";

const char FSS_PREHEAT_HOTEND_2[] = "PREHEAT_HOTEND_2";
const char FSSC_PREHEAT_HOTEND_2[] = " # °C, hotend temperature";

const char FSS_PREHEAT_BED_2[] = "PREHEAT_BED_2";
const char FSSC_PREHEAT_BED_2[] = " # °C, bed temperature";

const char FSS_PREHEAT_FAN_2[] = "PREHEAT_FAN_2";
const char FSSC_PREHEAT_FAN_2[] = " # fan speed";

const char FSS_PID_HOTEND_P[] = "PID_HOTEND_P";
const char FSSC_PID_HOTEND_P[] = " ";

const char FSS_PID_HOTEND_I[] = "PID_HOTEND_I";
const char FSSC_PID_HOTEND_I[] = " ";

const char FSS_PID_HOTEND_D[] = "PID_HOTEND_D";
const char FSSC_PID_HOTEND_D[] = " ";

const char FSS_PID_BED_P[] = "PID_BED_P";
const char FSSC_PID_BED_P[] = " ";

const char FSS_PID_BED_I[] = "PID_BED_I";
const char FSSC_PID_BED_I[] = " ";

const char FSS_PID_BED_D[] = "PID_BED_D";
const char FSSC_PID_BED_D[] = " ";

const char FSS_THERMISTOR_TYPE_HOTEND[] = "THERMISTOR_TYPE_HOTEND";
const char FSSC_THERMISTOR_TYPE_HOTEND[] = " # 0 - Epcos 100k (1), 1 - ATC 104GT/104NT 100k (5), 2 - Hisens 3950 100k (13), 3 - Formbot b3950 100k (61), 4 - Dyze D500 4.7M (66), 5 - Pt1000 4.7kΩ pullup (1047)";

const char FSS_THERMISTOR_TYPE_BED[] = "THERMISTOR_TYPE_BED";
const char FSSC_THERMISTOR_TYPE_BED[] = " # 0 - Epcos 100k (1), 1 - ATC 104GT/104NT 100k (5), 2 - Hisens 3950 100k (13), 3 - Formbot b3950 100k (61), 4 - Dyze D500 4.7M (66), 5 - Pt1000 4.7kΩ pullup (1047)";

/******** LCD ***********/
const char FSS_LCD_BRIGHTNESS[] = "LCD_BRIGHTNESS";
const char FSSC_LCD_BRIGHTNESS[] = " # 0-19";

const char FSS_LCD_TOUCH_X[] = "LCD_TOUCH_X";
const char FSSC_LCD_TOUCH_X[] = " # do not change!";

const char FSS_LCD_TOUCH_Y[] = "LCD_TOUCH_Y";
const char FSSC_LCD_TOUCH_Y[] = " # do not change!";

const char FSS_LCD_TOUCH_OFFSET_X[] = "LCD_TOUCH_OFFSET_X";
const char FSSC_LCD_TOUCH_OFFSET_X[] = " # do not change!";

const char FSS_LCD_TOUCH_OFFSET_Y[] = "LCD_TOUCH_OFFSET_Y";
const char FSSC_LCD_TOUCH_OFFSET_Y[] = " # do not change!";

const char FSS_LCD_LANGUAGE[] = "LCD_LANGUAGE";
const char FSSC_LCD_LANGUAGE[] = " # 0 - English, 1 - Русский";

/******** PSU ***********/
const char FSS_PSU_ENABLED[] = "PSU_ENABLED";
const char FSSC_PSU_ENABLED[] = " ";

