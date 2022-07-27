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


#include "../inc/MarlinConfig.h"

typedef enum
{
	PARAMVAL_NONE = 0,
	PARAMVAL_NUMERIC,
	PARAMVAL_BOOL,
	PARAMVAL_STRING
} VALUE_TYPE;

typedef struct
{
	double		  float_val;
	bool  		  bool_val;
	char			  *char_val;
	VALUE_TYPE	type;
} PARAM_VALUE;


class FileSettings {
  public:
    static bool SaveSettings(char *fname = NULL);
    static bool LoadSettings(char *fname /*= NULL*/);

  private:
    static char* _getParamName(char *src, char *dest, uint16_t maxlen);
    static char* _getParamValue(char *src, PARAM_VALUE *val);
    static void _skipToNextLine(char *src);
    static void postprocess();
};

extern FileSettings fileSettings;
