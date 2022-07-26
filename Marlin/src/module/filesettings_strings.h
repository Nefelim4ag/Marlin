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
const char FSS_ACCEL_MAX_X[] = "Accelerate_Max_X";
const char FSSC_ACCEL_MAX_X[] = " # mm/s2, M201 X";

const char FSS_ACCEL_MAX_Y[] = "Accelerate_Max_Y";
const char FSSC_ACCEL_MAX_Y[] = " # mm/s2, M201 Y";

const char FSS_ACCEL_MAX_Z[] = "Accelerate_Max_Z";
const char FSSC_ACCEL_MAX_Z[] = " # mm/s2, M201 Z";

const char FSS_ACCEL_MAX_E[] = "Accelerate_Max_E";
const char FSSC_ACCEL_MAX_E[] = " # mm/s2, M201 E";

const char FSS_PRINT_ACCEL[] = "Accelerate_Print";
const char FSSC_PRINT_ACCEL[] = " # mm/s2, M204 S";

const char FSS_RETRACT_ACCEL[] = "Accelerate_Retract";
const char FSSC_RETRACT_ACCEL[] = " # mm/s2, M204 R";

const char FSS_TRAVEL_ACCEL[] = "Accelerate_Travel";
const char FSSC_TRAVEL_ACCEL[] = " # mm/s2, M204 T";

const char FSS_MIN_SEGMENT_TIME[] = "Min_Segment_Time";
const char FSSC_MIN_SEGMENT_TIME[] = " # µs, M205 B";

/******** STEPS PER MM ***********/
const char FSS_STEPS_PER_MM_X[] = "Steps_Per_Mm_X";
const char FSSC_STEPS_PER_MM_X[] = " # steps, M92 X";

const char FSS_STEPS_PER_MM_Y[] = "Steps_Per_Mm_Y";
const char FSSC_STEPS_PER_MM_Y[] = " # steps, M92 Y";

const char FSS_STEPS_PER_MM_Z[] = "Steps_Per_Mm_Z";
const char FSSC_STEPS_PER_MM_Z[] = " # steps, M92 Z";

const char FSS_STEPS_PER_MM_E[] = "Steps_Per_Mm_E";
const char FSSC_STEPS_PER_MM_E[] = " # steps, M92 E";

/******** SPEED ***********/
const char FSS_MAX_SPEED_X[] = "Speed_Max_X";
const char FSSC_MAX_SPEED_X[] = " # mm/s, M203 X";

const char FSS_MAX_SPEED_Y[] = "Speed_Max_Y";
const char FSSC_MAX_SPEED_Y[] = " # mm/s, M203 Y";

const char FSS_MAX_SPEED_Z[] = "Speed_Max_Z";
const char FSSC_MAX_SPEED_Z[] = " # mm/s, M203 Z";

const char FSS_MAX_SPEED_E[] = "Speed_Max_E";
const char FSSC_MAX_SPEED_E[] = " # mm/s, M203 E";

const char FSS_MIN_PRINT_SPEED[] = "Speed_Min_Print";
const char FSSC_MIN_PRINT_SPEED[] = " # mm/s, M205 S";

const char FSS_MIN_TRAVEL_SPEED[] = "Speed_Min_Travel";
const char FSSC_MIN_TRAVEL_SPEED[] = " # mm/s, M205 T";

/******** JERKS ***********/
const char FSS_MAX_JERK_X[] = "Jerk_Max_X";
const char FSSC_MAX_JERK_X[] = " # M205 X";

const char FSS_MAX_JERK_Y[] = "Jerk_Max_Y";
const char FSSC_MAX_JERK_Y[] = " # M205 Y";

const char FSS_MAX_JERK_Z[] = "Jerk_Max_Z";
const char FSSC_MAX_JERK_Z[] = " # M205 Z";

const char FSS_MAX_JERK_E[] = "Jerk_Max_E";
const char FSSC_MAX_JERK_E[] = " # M205 E";

/******** STEPPERS ***********/
const char FSS_STEPPER_INVERT_X[] = "Stepper_Invert_X";
const char FSSC_STEPPER_INVERT_X[] = " ";

const char FSS_STEPPER_INVERT_Y[] = "Stepper_Invert_Y";
const char FSSC_STEPPER_INVERT_Y[] = " ";

const char FSS_STEPPER_INVERT_Z1[] = "Stepper_Invert_Z1";
const char FSSC_STEPPER_INVERT_Z1[] = " ";

const char FSS_STEPPER_INVERT_Z2[] = "Stepper_Invert_Z2_to_Z1";
const char FSSC_STEPPER_INVERT_Z2[] = " # invert Z2 with respect to Z1";

const char FSS_STEPPER_INVERT_E[] = "Stepper_Invert_E";
const char FSSC_STEPPER_INVERT_E[] = " ";

/******** HOME OFFSET ***********/
const char FSS_HOME_OFFSET_X[] = "Home_Offset_X";
const char FSSC_HOME_OFFSET_X[] = " # mm, M206 X";

const char FSS_HOME_OFFSET_Y[] = "Home_Offset_Y";
const char FSSC_HOME_OFFSET_Y[] = " # mm, M206 Y";

const char FSS_HOME_OFFSET_Z[] = "Home_Offset_Z";
const char FSSC_HOME_OFFSET_Z[] = " # mm, M206 Z";

/******** FILAMENT RUNOUT SENSOR ***********/
const char FSS_FILAMENTSENSOR_ENABLED[] = "FilamentSensor_Enabled";
const char FSSC_FILAMENTSENSOR_ENABLED[] = " # M412 S";

const char FSS_FILAMENTSENSOR_DISTANCE[] = "FilamentSensor_Distance";
const char FSSC_FILAMENTSENSOR_DISTANCE[] = " # M412 D";

/******** BED LEVELING ***********/
const char FSS_BEDLEVEL_FADE_HEIGHT[] = "BedLevel_Fade_Height";
const char FSSC_BEDLEVEL_FADE_HEIGHT[] = " # mm";

const char FSS_BEDLEVEL_BLTOUCH_ENABLED[] = "BedLevel_Bltouch_Enabled";
const char FSSC_BEDLEVEL_BLTOUCH_ENABLED[] = " ";

const char FSS_BEDLEVEL_BLTOUCH_INVERT[] = "BedLevel_Bltouch_Invert";
const char FSSC_BEDLEVEL_BLTOUCH_INVERT[] = " ";

const char FSS_BEDLEVEL_BLTOUCH_OFFSET_X[] = "BedLevel_Bltouch_Offset_X";
const char FSSC_BEDLEVEL_BLTOUCH_OFFSET_X[] = " # mm, M851 X";

const char FSS_BEDLEVEL_BLTOUCH_OFFSET_Y[] = "BedLevel_Bltouch_Offset_Y";
const char FSSC_BEDLEVEL_BLTOUCH_OFFSET_Y[] = " # mm, M851 Y";

const char FSS_BEDLEVEL_BLTOUCH_OFFSET_Z[] = "BedLevel_Bltouch_Offset_Z";
const char FSSC_BEDLEVEL_BLTOUCH_OFFSET_Z[] = " # mm, M851 Z";

const char FSS_BEDLEVEL_POINTS_X[] = "BedLevel_Points_X";
const char FSSC_BEDLEVEL_POINTS_X[] = " ";

const char FSS_BEDLEVEL_POINTS_Y[] = "BedLevel_Points_Y";
const char FSSC_BEDLEVEL_POINTS_Y[] = " ";

const char FSS_BEDLEVEL_VALUES[] = "BedLevel_Z_Values";
const char FSSC_BEDLEVEL_VALUES[] = " # mm";

const char FSS_SERVO_ANGLES[] = "Servo_Angles";
const char FSSC_SERVO_ANGLES[] = " ";

/******** ENDSTOPS ***********/
const char FSS_ENDSTOP_INVERT_X[] = "Endstop_Invert_X";
const char FSSC_ENDSTOP_INVERT_X[] = " ";

const char FSS_ENDSTOP_INVERT_Y[] = "Endstop_Invert_Y";
const char FSSC_ENDSTOP_INVERT_Y[] = " ";

const char FSS_ENDSTOP_INVERT_Z1[] = "Endstop_Invert_Z1";
const char FSSC_ENDSTOP_INVERT_Z1[] = " ";

const char FSS_ENDSTOP_INVERT_Z2[] = "Endstop_Invert_Z2";
const char FSSC_ENDSTOP_INVERT_Z2[] = " ";

const char FSS_ENDSTOP_ADJUST_Z2[] = "Endstop_Adjust_Z2";
const char FSSC_ENDSTOP_ADJUST_Z2[] = " # mm";

/******** TEMPERATURE ***********/
const char FSS_PREHEAT_HOTEND_1[] = "Preheat_Hotend_1";
const char FSSC_PREHEAT_HOTEND_1[] = " # °C, hotend temperature";

const char FSS_PREHEAT_BED_1[] = "Preheat_Bed_1";
const char FSSC_PREHEAT_BED_1[] = " # °C, bed temperature";

const char FSS_PREHEAT_FAN_1[] = "Preheat_Fan_1";
const char FSSC_PREHEAT_FAN_1[] = " # fan speed";

const char FSS_PREHEAT_HOTEND_2[] = "Preheat_Hotend_2";
const char FSSC_PREHEAT_HOTEND_2[] = " # °C, hotend temperature";

const char FSS_PREHEAT_BED_2[] = "Preheat_Bed_2";
const char FSSC_PREHEAT_BED_2[] = " # °C, bed temperature";

const char FSS_PREHEAT_FAN_2[] = "Preheat_Fan_2";
const char FSSC_PREHEAT_FAN_2[] = " # fan speed";

const char FSS_PID_HOTEND_P[] = "PID_Hotend_P";
const char FSSC_PID_HOTEND_P[] = " ";

const char FSS_PID_HOTEND_I[] = "PID_Hotend_I";
const char FSSC_PID_HOTEND_I[] = " ";

const char FSS_PID_HOTEND_D[] = "PID_Hotend_D";
const char FSSC_PID_HOTEND_D[] = " ";

const char FSS_PID_BED_P[] = "PID_Bed_P";
const char FSSC_PID_BED_P[] = " ";

const char FSS_PID_BED_I[] = "PID_Bed_I";
const char FSSC_PID_BED_I[] = " ";

const char FSS_PID_BED_D[] = "PID_Bed_D";
const char FSSC_PID_BED_D[] = " ";

const char FSS_THERMISTOR_TYPE_HOTEND[] = "Thermistor_Type_Hotend";
const char FSSC_THERMISTOR_TYPE_HOTEND[] = " # 0 - Epcos 100k (1), 1 - ATC 104GT/104NT 100k (5), 2 - Hisens 3950 100k (13), 3 - Formbot b3950 100k (61), 4 - Dyze D500 4.7M (66), 5 - Pt1000 4.7kΩ pullup (1047)";

const char FSS_THERMISTOR_TYPE_BED[] = "Thermistor_Type_Bed";
const char FSSC_THERMISTOR_TYPE_BED[] = " # 0 - Epcos 100k (1), 1 - ATC 104GT/104NT 100k (5), 2 - Hisens 3950 100k (13), 3 - Formbot b3950 100k (61), 4 - Dyze D500 4.7M (66), 5 - Pt1000 4.7kΩ pullup (1047)";

/******** LCD ***********/
const char FSS_LCD_BRIGHTNESS[] = "LCD_Brightness";
const char FSSC_LCD_BRIGHTNESS[] = " # 0-19";

const char FSS_LCD_TOUCH_X[] = "LCD_Touch_X";
const char FSSC_LCD_TOUCH_X[] = " # do not change!";

const char FSS_LCD_TOUCH_Y[] = "LCD_Touch_Y";
const char FSSC_LCD_TOUCH_Y[] = " # do not change!";

const char FSS_LCD_TOUCH_OFFSET_X[] = "LCD_Touch_Offset_X";
const char FSSC_LCD_TOUCH_OFFSET_X[] = " # do not change!";

const char FSS_LCD_TOUCH_OFFSET_Y[] = "LCD_Touch_Offset_Y";
const char FSSC_LCD_TOUCH_OFFSET_Y[] = " # do not change!";

const char FSS_LCD_LANGUAGE[] = "LCD_Language";
const char FSSC_LCD_LANGUAGE[] = " # 0 - English, 1 - Русский";

