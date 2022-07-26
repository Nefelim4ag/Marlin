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

// Change EEPROM version if the structure changes
#define EEPROM_VERSION "V86"
#define EEPROM_OFFSET 100

// Check the integrity of data offsets.
// Can be disabled for production build.
//#define DEBUG_EEPROM_READWRITE

#include "filesettings.h"
#include "filesettings_strings.h"

#include "settings.h"
#include "endstops.h"
#include "planner.h"
#include "stepper.h"
#include "temperature.h"

#include "../lcd/marlinui.h"
#include "../libs/vector_3.h"   // for matrix_3x3
#include "../gcode/gcode.h"
#include "../MarlinCore.h"

#include "../lcd/tft/tft_string.h"
#include "../libs/numtostr.h"

#include "../feature/power.h"

#include "probe.h"

#if HAS_LEVELING
  #include "../feature/bedlevel/bedlevel.h"
  #if ENABLED(X_AXIS_TWIST_COMPENSATION)
    #include "../feature/x_twist.h"
  #endif
#endif

#if ENABLED(Z_STEPPER_AUTO_ALIGN)
  #include "../feature/z_stepper_align.h"
#endif

#if ENABLED(HOST_PROMPT_SUPPORT)
  #include "../feature/host_actions.h"
#endif

#if HAS_SERVOS
  #include "servo.h"
#endif

#if HAS_SERVOS && HAS_SERVO_ANGLES
  #define EEPROM_NUM_SERVOS NUM_SERVOS
#else
  #define EEPROM_NUM_SERVOS NUM_SERVO_PLUGS
#endif

#include "../feature/fwretract.h"

#if ENABLED(POWER_LOSS_RECOVERY)
  #include "../feature/powerloss.h"
#endif

#if HAS_POWER_MONITOR
  #include "../feature/power_monitor.h"
#endif

#include "../feature/pause.h"

#if ENABLED(BACKLASH_COMPENSATION)
  #include "../feature/backlash.h"
#endif

#if HAS_FILAMENT_SENSOR
  #include "../feature/runout.h"
  #ifndef FIL_RUNOUT_ENABLED_DEFAULT
    #define FIL_RUNOUT_ENABLED_DEFAULT true
  #endif
#endif

#if ENABLED(EXTRA_LIN_ADVANCE_K)
  extern float other_extruder_advance_K[EXTRUDERS];
#endif

#if HAS_MULTI_EXTRUDER
  #include "tool_change.h"
  void M217_report(const bool eeprom);
#endif

#if ENABLED(BLTOUCH)
  #include "../feature/bltouch.h"
#endif

#if HAS_TRINAMIC_CONFIG
  #include "stepper/indirection.h"
  #include "../feature/tmc_util.h"
#endif

#if HAS_PTC
  #include "../feature/probe_temp_comp.h"
#endif

#include "../feature/controllerfan.h"

#if ENABLED(CASE_LIGHT_ENABLE)
  #include "../feature/caselight.h"
#endif

#if ENABLED(PASSWORD_FEATURE)
  #include "../feature/password/password.h"
#endif

#if ENABLED(TOUCH_SCREEN_CALIBRATION)
  #include "../lcd/tft_io/touch_calibration.h"
#endif

#if HAS_ETHERNET
  #include "../feature/ethernet.h"
#endif

#if ENABLED(SOUND_MENU_ITEM)
  #include "../libs/buzzer.h"
#endif

#if HAS_FANCHECK
  #include "../feature/fancheck.h"
#endif

#if ENABLED(DGUS_LCD_UI_MKS)
  #include "../lcd/extui/dgus/DGUSScreenHandler.h"
  #include "../lcd/extui/dgus/DGUSDisplayDef.h"
#endif

#pragma pack(push, 1) // No padding between variables

FileSettings fileSettings;

#define DEBUG_OUT EITHER(EEPROM_CHITCHAT, DEBUG_LEVELING_FEATURE)
#include "../core/debug_out.h"




bool FileSettings::SaveSettings(char *fname)
{
  if (card.isFileOpen())
    return false;

  char *filename;
  char curline[512];
  bool wres = false;
  uint32_t len = 0;

  if (fname == NULL || strlen(fname) < 2)
    filename = (char*)"/printer_settings.ini";
  else
    filename = fname;
  
  if (!card.openFileWrite(filename, true))
    return false;
  
  do
  {
    /******** ACCELERATION ***********/
    sprintf(curline, "%s", (char*)"# ====== ACCELERATION ======\r\n");
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %ld %s\r\n", FSS_ACCEL_MAX_X, planner.settings.max_acceleration_mm_per_s2[X_AXIS], FSSC_ACCEL_MAX_X);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %ld %s\r\n", FSS_ACCEL_MAX_Y, planner.settings.max_acceleration_mm_per_s2[Y_AXIS], FSSC_ACCEL_MAX_Y);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %ld %s\r\n", FSS_ACCEL_MAX_Z, planner.settings.max_acceleration_mm_per_s2[Z_AXIS], FSSC_ACCEL_MAX_Z);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %ld %s\r\n", FSS_ACCEL_MAX_E, planner.settings.max_acceleration_mm_per_s2[E_AXIS], FSSC_ACCEL_MAX_E);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %.2f %s\r\n", FSS_PRINT_ACCEL, planner.settings.acceleration, FSSC_PRINT_ACCEL);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %.2f %s\r\n", FSS_RETRACT_ACCEL, planner.settings.retract_acceleration, FSSC_RETRACT_ACCEL);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %.2f %s\r\n", FSS_TRAVEL_ACCEL, planner.settings.travel_acceleration, FSSC_TRAVEL_ACCEL);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %ld %s\r\n", FSS_MIN_SEGMENT_TIME, planner.settings.min_segment_time_us, FSSC_MIN_SEGMENT_TIME);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    
    /******** STEPS PER MM ***********/
    sprintf(curline, "%s", (char*)"\r\n# ====== STEPS PER MM ======\r\n");
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_STEPS_PER_MM_X, planner.settings.axis_steps_per_mm[X_AXIS], FSSC_STEPS_PER_MM_X);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_STEPS_PER_MM_Y, planner.settings.axis_steps_per_mm[Y_AXIS], FSSC_STEPS_PER_MM_Y);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_STEPS_PER_MM_Z, planner.settings.axis_steps_per_mm[Z_AXIS], FSSC_STEPS_PER_MM_Z);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_STEPS_PER_MM_E, planner.settings.axis_steps_per_mm[E_AXIS], FSSC_STEPS_PER_MM_E);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    
    /******** SPEED ***********/
    sprintf(curline, "%s", (char*)"\r\n# ====== SPEED ======\r\n");
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_MAX_SPEED_X, planner.settings.max_feedrate_mm_s[X_AXIS], FSSC_MAX_SPEED_X);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_MAX_SPEED_Y, planner.settings.max_feedrate_mm_s[Y_AXIS], FSSC_MAX_SPEED_Y);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_MAX_SPEED_Z, planner.settings.max_feedrate_mm_s[Z_AXIS], FSSC_MAX_SPEED_Z);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_MAX_SPEED_E, planner.settings.max_feedrate_mm_s[E_AXIS], FSSC_MAX_SPEED_E);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    
    /******** JERKS ***********/
    sprintf(curline, "%s", (char*)"\r\n# ====== JERKS ======\r\n");
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_MAX_JERK_X, planner.max_jerk.x, FSSC_MAX_JERK_X);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_MAX_JERK_Y, planner.max_jerk.y, FSSC_MAX_JERK_Y);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_MAX_JERK_Z, planner.max_jerk.z, FSSC_MAX_JERK_Z);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_MAX_JERK_E, planner.max_jerk.e, FSSC_MAX_JERK_E);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;

    /******** STEPPERS ***********/
    sprintf(curline, "%s", (char*)"\r\n# ====== STEPPERS ======\r\n");
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %s %s\r\n", FSS_STEPPER_INVERT_X, (planner.invert_axis.invert_axis[X_AXIS] ? "Yes" : "No"), FSSC_STEPPER_INVERT_X);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %s %s\r\n", FSS_STEPPER_INVERT_Y, (planner.invert_axis.invert_axis[Y_AXIS] ? "Yes" : "No"), FSSC_STEPPER_INVERT_Y);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %s %s\r\n", FSS_STEPPER_INVERT_Z1, (planner.invert_axis.invert_axis[Z_AXIS] ? "Yes" : "No"), FSSC_STEPPER_INVERT_Z1);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %s %s\r\n", FSS_STEPPER_INVERT_Z2, (planner.invert_axis.z2_vs_z_dir ? "Yes" : "No"), FSSC_STEPPER_INVERT_Z2);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %s %s\r\n", FSS_STEPPER_INVERT_E, (planner.invert_axis.invert_axis[E_AXIS] ? "Yes" : "No"), FSSC_STEPPER_INVERT_E);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;

    /******** HOME OFFSET ***********/
    sprintf(curline, "%s", (char*)"\r\n# ====== HOME OFFSET ======\r\n");
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_HOME_OFFSET_X, home_offset.x, FSSC_HOME_OFFSET_X);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_HOME_OFFSET_Y, home_offset.y, FSSC_HOME_OFFSET_Y);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_HOME_OFFSET_Z, home_offset.z, FSSC_HOME_OFFSET_Z);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    
    /******** FILAMENT RUNOUT SENSOR ***********/
    sprintf(curline, "%s", (char*)"\r\n# ====== FILAMENT RUNOUT SENSOR ======\r\n");
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %s %s\r\n", FSS_FILAMENTSENSOR_ENABLED, (runout.enabled ? "Yes" : "No"), FSSC_FILAMENTSENSOR_ENABLED);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_FILAMENTSENSOR_DISTANCE, runout.runout_distance(), FSSC_FILAMENTSENSOR_DISTANCE);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    
    /******** BED LEVELING ***********/
    sprintf(curline, "%s", (char*)"\r\n# ====== BED LEVELING ======\r\n");
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_BEDLEVEL_FADE_HEIGHT, planner.z_fade_height, FSSC_BEDLEVEL_FADE_HEIGHT);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %s %s\r\n", FSS_BEDLEVEL_BLTOUCH_ENABLED, (bedlevel_settings.bltouch_enabled ? "Yes" : "No"), FSSC_BEDLEVEL_BLTOUCH_ENABLED);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %s %s\r\n", FSS_BEDLEVEL_BLTOUCH_INVERT, (endstop_settings.Z_MIN_PROBE_INVERTING ? "Yes" : "No"), FSSC_BEDLEVEL_BLTOUCH_INVERT);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_BEDLEVEL_BLTOUCH_OFFSET_X, probe.offset.x, FSSC_BEDLEVEL_BLTOUCH_OFFSET_X);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_BEDLEVEL_BLTOUCH_OFFSET_Y, probe.offset.y, FSSC_BEDLEVEL_BLTOUCH_OFFSET_Y);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.3f %s\r\n", FSS_BEDLEVEL_BLTOUCH_OFFSET_Z, probe.offset.z, FSSC_BEDLEVEL_BLTOUCH_OFFSET_Z);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %d %s\r\n", FSS_BEDLEVEL_POINTS_X, bedlevel_settings.bedlevel_points.x, FSSC_BEDLEVEL_POINTS_X);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %d %s\r\n", FSS_BEDLEVEL_POINTS_Y, bedlevel_settings.bedlevel_points.y, FSSC_BEDLEVEL_POINTS_Y);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;

    sprintf(curline, "%s = ", FSS_BEDLEVEL_VALUES);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    for (uint8_t i = 0; i < GRID_MAX_POINTS_Y; i++)
    {
      for (uint8_t j = 0; j < GRID_MAX_POINTS_X; j++)
      {
        sprintf(curline, "%0.3f%s ",
                    isnan(bedlevel.z_values[i][j]) ? 0 : bedlevel.z_values[i][j],
                    (i == GRID_MAX_POINTS_Y-1 && j == GRID_MAX_POINTS_X-1) ? "" : ","
                );
        len = strlen(curline);
        if (card.write(curline, len) != len)
          break;
      }
    }
    sprintf(curline, "%s\r\n", FSSC_BEDLEVEL_VALUES);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;

    sprintf(curline, "%s = %d, %d %s\r\n", FSS_SERVO_ANGLES, servo_angles[0][0], servo_angles[0][1], FSSC_SERVO_ANGLES);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    
    /******** ENDSTOPS ***********/
    sprintf(curline, "%s", (char*)"\r\n# ====== ENDSTOPS ======\r\n");
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %s %s\r\n", FSS_ENDSTOP_INVERT_X, (endstop_settings.X_MIN_INVERTING ? "Yes" : "No"), FSSC_ENDSTOP_INVERT_X);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %s %s\r\n", FSS_ENDSTOP_INVERT_Y, (endstop_settings.Y_MIN_INVERTING ? "Yes" : "No"), FSSC_ENDSTOP_INVERT_Y);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %s %s\r\n", FSS_ENDSTOP_INVERT_Z1, (endstop_settings.Z_MIN_INVERTING ? "Yes" : "No"), FSSC_ENDSTOP_INVERT_Z1);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %s %s\r\n", FSS_ENDSTOP_INVERT_Z2, (endstop_settings.Z2_MIN_INVERTING ? "Yes" : "No"), FSSC_ENDSTOP_INVERT_Z2);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.3f %s\r\n", FSS_ENDSTOP_ADJUST_Z2, endstops.z2_endstop_adj, FSSC_ENDSTOP_ADJUST_Z2);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    
    /******** TEMPERATURE ***********/
    sprintf(curline, "%s", (char*)"\r\n# ====== TEMPERATURE ======\r\n");
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %d %s\r\n", FSS_PREHEAT_HOTEND_1, ui.material_preset[0].hotend_temp, FSSC_PREHEAT_HOTEND_1);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %d %s\r\n", FSS_PREHEAT_BED_1, ui.material_preset[0].bed_temp, FSSC_PREHEAT_BED_1);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %d %s\r\n", FSS_PREHEAT_FAN_1, ui.material_preset[0].fan_speed, FSSC_PREHEAT_FAN_1);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %d %s\r\n", FSS_PREHEAT_HOTEND_2, ui.material_preset[1].hotend_temp, FSSC_PREHEAT_HOTEND_2);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %d %s\r\n", FSS_PREHEAT_BED_2, ui.material_preset[1].bed_temp, FSSC_PREHEAT_BED_2);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %d %s\r\n", FSS_PREHEAT_FAN_2, ui.material_preset[1].fan_speed, FSSC_PREHEAT_FAN_2);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.5f %s\r\n", FSS_PID_HOTEND_P, thermalManager.temp_hotend[0].pid.Kp, FSSC_PID_HOTEND_P);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.5f %s\r\n", FSS_PID_HOTEND_I, thermalManager.temp_hotend[0].pid.Ki, FSSC_PID_HOTEND_I);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.5f %s\r\n", FSS_PID_HOTEND_D, thermalManager.temp_hotend[0].pid.Kd, FSSC_PID_HOTEND_D);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.5f %s\r\n", FSS_PID_BED_P, thermalManager.temp_bed.pid.Kp, FSSC_PID_BED_P);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.5f %s\r\n", FSS_PID_BED_I, thermalManager.temp_bed.pid.Ki, FSSC_PID_BED_I);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.5f %s\r\n", FSS_PID_BED_D, thermalManager.temp_bed.pid.Kd, FSSC_PID_BED_D);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %d %s\r\n", FSS_THERMISTOR_TYPE_HOTEND, thermistors_data.heater_type[0], FSSC_THERMISTOR_TYPE_HOTEND);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %d %s\r\n", FSS_THERMISTOR_TYPE_BED, thermistors_data.bed_type, FSSC_THERMISTOR_TYPE_BED);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
   
    /******** LCD ***********/
    sprintf(curline, "%s", (char*)"\r\n# ====== LCD ======\r\n");
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %d %s\r\n", FSS_LCD_BRIGHTNESS, ui.brightness, FSSC_LCD_BRIGHTNESS);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %ld %s\r\n", FSS_LCD_TOUCH_X, touch_calibration.calibration.x, FSSC_LCD_TOUCH_X);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %ld %s\r\n", FSS_LCD_TOUCH_Y, touch_calibration.calibration.y, FSSC_LCD_TOUCH_Y);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %d %s\r\n", FSS_LCD_TOUCH_OFFSET_X, touch_calibration.calibration.offset_x, FSSC_LCD_TOUCH_OFFSET_X);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %d %s\r\n", FSS_LCD_TOUCH_OFFSET_Y, touch_calibration.calibration.offset_y, FSSC_LCD_TOUCH_OFFSET_Y);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %d %s\r\n", FSS_LCD_LANGUAGE, ui.language, FSSC_LCD_LANGUAGE);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;

    


    wres = true;
  } while (0);

  card.closefile();
  return wres;
}


void FileSettings::postprocess()
{

}


#pragma pack(pop)
