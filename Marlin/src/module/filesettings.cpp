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

  if (fname == NULL)
    filename = (char*)"/printer_settings.ini";
  else
    filename = fname;
  
  if (!card.openFileWrite(filename, true))
    return false;
  
  do
  {
    /******** MOTION ***********/
    sprintf(curline, "%s", (char*)"# ====== MOTION SETTINGS ======\r\n");
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    // planner settings
    sprintf(curline, "%s = %ld %s\r\n", FSS_MOTION_ACCEL_MAX_X, planner.settings.max_acceleration_mm_per_s2[X_AXIS], FSSC_MOTION_ACCEL_MAX_X);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %ld %s\r\n", FSS_MOTION_ACCEL_MAX_Y, planner.settings.max_acceleration_mm_per_s2[Y_AXIS], FSSC_MOTION_ACCEL_MAX_Y);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %ld %s\r\n", FSS_MOTION_ACCEL_MAX_Z, planner.settings.max_acceleration_mm_per_s2[Z_AXIS], FSSC_MOTION_ACCEL_MAX_Z);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %ld %s\r\n", FSS_MOTION_ACCEL_MAX_E, planner.settings.max_acceleration_mm_per_s2[E_AXIS], FSSC_MOTION_ACCEL_MAX_E);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %.2f %s\r\n", FSS_MOTION_PRINT_ACCEL, planner.settings.acceleration, FSSC_MOTION_PRINT_ACCEL);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %.2f %s\r\n", FSS_MOTION_RETRACT_ACCEL, planner.settings.retract_acceleration, FSSC_MOTION_RETRACT_ACCEL);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %.2f %s\r\n", FSS_MOTION_TRAVEL_ACCEL, planner.settings.travel_acceleration, FSSC_MOTION_TRAVEL_ACCEL);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %ld %s\r\n", FSS_MOTION_MIN_SEGMENT_TIME, planner.settings.min_segment_time_us, FSSC_MOTION_MIN_SEGMENT_TIME);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_MOTION_STEPS_PER_MM_X, planner.settings.axis_steps_per_mm[X_AXIS], FSSC_MOTION_STEPS_PER_MM_X);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_MOTION_STEPS_PER_MM_Y, planner.settings.axis_steps_per_mm[Y_AXIS], FSSC_MOTION_STEPS_PER_MM_Y);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_MOTION_STEPS_PER_MM_Z, planner.settings.axis_steps_per_mm[Z_AXIS], FSSC_MOTION_STEPS_PER_MM_Z);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_MOTION_STEPS_PER_MM_E, planner.settings.axis_steps_per_mm[E_AXIS], FSSC_MOTION_STEPS_PER_MM_E);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_MOTION_MAX_SPEED_X, planner.settings.max_feedrate_mm_s[X_AXIS], FSSC_MOTION_MAX_SPEED_X);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_MOTION_MAX_SPEED_Y, planner.settings.max_feedrate_mm_s[Y_AXIS], FSSC_MOTION_MAX_SPEED_Y);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_MOTION_MAX_SPEED_Z, planner.settings.max_feedrate_mm_s[Z_AXIS], FSSC_MOTION_MAX_SPEED_Z);
    len = strlen(curline);
    if (card.write(curline, len) != len)
      break;
    sprintf(curline, "%s = %0.2f %s\r\n", FSS_MOTION_MAX_SPEED_E, planner.settings.max_feedrate_mm_s[E_AXIS], FSSC_MOTION_MAX_SPEED_E);
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
