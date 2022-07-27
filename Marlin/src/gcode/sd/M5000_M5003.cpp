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

#include "../../inc/MarlinConfig.h"

#include "../gcode.h"
#include "../../lcd/marlinui.h"

#include "../../module/filesettings.h"

/**
 * M5000 - Store parameters in .ini file. 
 *
 * The path is relative to the root directory
 */
void GcodeSuite::M5000()
{
  SERIAL_ECHOLNPGM("M5000: file - \"", parser.command_args, "\"");

  uint32_t msecs = millis();
  if (fileSettings.SaveSettings(parser.command_args))
    SERIAL_ECHOLNPGM("M5000: success");
  else
    SERIAL_ECHOLNPGM("M5000: failure");
  msecs = millis() - msecs;
  SERIAL_ECHOLNPGM("M5000: total msec - ", msecs);

}


/**
 * M5001 - Load parameters from .ini file. 
 *
 * The path is relative to the root directory
 */
void GcodeSuite::M5001()
{
  SERIAL_ECHOLNPGM("M5001: file - \"", parser.command_args, "\"");

  uint32_t msecs = millis();
  if (fileSettings.LoadSettings(parser.command_args))
    SERIAL_ECHOLNPGM("M5001: success");
  else
    SERIAL_ECHOLNPGM("M5001: failure");
  msecs = millis() - msecs;
  SERIAL_ECHOLNPGM("M5001: total msec - ", msecs);

}

