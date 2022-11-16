/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
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

#define ALLOW_STM32DUINO
#include "env_validate.h"

#define SDCARD_CONNECTION ONBOARD

#include "pins_MKS_ROBIN_NANO_V3_common.h"

#if HOTENDS > 2 || E_STEPPERS > 2
  #error "MKS Robin Nano V3 supports up to 2 hotends / E-steppers."
#elif HAS_FSMC_TFT
  #error "MKS Robin Nano V3 doesn't support FSMC-based TFT displays."
#endif

#define BOARD_INFO_NAME "MKS Robin Nano6"



#ifndef X_CS_PIN
  #define X_CS_PIN                          PD5
#endif

#ifndef Y_CS_PIN
  #define Y_CS_PIN                          PD7
#endif

#ifndef Z_CS_PIN
  #define Z_CS_PIN                          PD4
#endif

#ifndef E0_CS_PIN
  #define E0_CS_PIN                         PD9
#endif

#ifndef E1_CS_PIN
  #define E1_CS_PIN                         PD8
#endif

#ifndef X_ENABLE_PIN
    #define X_ENABLE_PIN                    PE4
#endif

#ifndef Y_ENABLE_PIN
    #define Y_ENABLE_PIN                    PE1
#endif

#ifndef E0_DIAG_PIN
    #define E1_DIAG_PIN                     -1
#endif

#ifndef E1_DIAG_PIN
    #define E1_DIAG_PIN                     -1
#endif

// #define E2_ENABLE_PIN                       PA3
// #define E2_STEP_PIN                         PE1
// #define E2_DIR_PIN                          PB2

#define Z2_ENABLE_PIN                       E1_ENABLE_PIN
#define Z2_STEP_PIN                         E1_STEP_PIN
#define Z2_DIR_PIN                          E1_DIR_PIN

// Swap Fan Pins
// hotend fan
#undef FAN1_PIN
#define FAN1_PIN PC14


// Cooling fan
#undef FAN_PIN
#define FAN_PIN PB1

// #undef HEATER_0_PIN
// #undef HEATER_1_PIN
// Swap heaters pin
// #define HEATER_0_PIN                        PB0   // HEATER1
// #define HEATER_1_PIN                        PE5   // HEATER2

//
// Software SPI pins for TMC2130 stepper drivers
// This board only supports SW SPI for stepper drivers
//
#if HAS_TMC_SPI
  #define TMC_USE_SW_SPI
#endif
#if ENABLED(TMC_USE_SW_SPI)
  #if !defined(TMC_SW_MOSI) || TMC_SW_MOSI == -1
    #define TMC_SW_MOSI                     PD14
  #endif
  #if !defined(TMC_SW_MISO) || TMC_SW_MISO == -1
    #define TMC_SW_MISO                     PD1
  #endif
  #if !defined(TMC_SW_SCK) || TMC_SW_SCK == -1
    #define TMC_SW_SCK                      PD0
  #endif
#endif

#define Z_MIN_PROBE_PIN                    PE6

#define MKS_WIFI_SERIAL_NUM                SERIAL_PORT_2
#define MKS_WIFI_UART                      USART1

#define MKS_WIFI_IO0 PC13
#define MKS_WIFI_IO1 PC7
#define MKS_WIFI_IO4 PC7 // backward compatibility
#define MKS_WIFI_IO_RST PE9
