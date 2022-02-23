/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// SPDX-License-Identifier: Apache-2.0
#ifndef MBED_OBJECTS_H
#define MBED_OBJECTS_H

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "PinNames.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "objects_flash.h"
#include "objects_gpio.h"
#include "objects_uart.h"
#include "objects_iom.h"
#include "objects_spi.h"
#include "objects_i2c.h"

struct pwmout_s {
    ap3_gpio_pad_t pad;
    int period_us;
    int pulse_width_us;
    float c_clock_frq;
    uint32_t timer;
    uint32_t segment;
    am_hal_ctimer_outputtype_e output;
    uint32_t resolution;
    // am_hal_gpio_drivestrength_e drive_strength;
};

#ifdef __cplusplus
}
#endif

#endif
