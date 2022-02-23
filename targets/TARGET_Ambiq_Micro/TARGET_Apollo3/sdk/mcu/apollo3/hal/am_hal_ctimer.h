//*****************************************************************************
//
//  am_hal_ctimer.h
//! @file
//!
//! @brief Functions for accessing and configuring the CTIMER.
//!
//! @addtogroup ctimer3 Counter/Timer (CTIMER)
//! @ingroup apollo3hal
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2020, Ambiq Micro
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision 2.4.2 of the AmbiqSuite Development Package.
//
//*****************************************************************************
// SPDX-License-Identifier: BSD-3-Clause
#ifndef AM_HAL_CTIMER_H
#define AM_HAL_CTIMER_H

#ifdef __cplusplus
extern "C"
{
#endif

//
// Designate this peripheral.
//
#define AM_APOLLO3_CTIMER   1

//*****************************************************************************
//
// CTIMERADDRn()
//  This is a specialized version of AM_REGADDRn().  It is necessary because
//  the CTIMER does not work as a multi-module peripheral.  In typical
//  multi-module peripherals, the base address is defined as MODULE0_BASE.
//  For CTIMER it's CTIMER_BASE (there is no module 0 defined).
//
//  Usage:
//  CTIMER_ADDRn(CTIMER, n, reg).
//
// periph:  Must always be CTIMER.
// n:       The timer number specified as a macro, variable, etc.
// reg:     The register name always ending in '0'.  E.g. TMR0, CTRL0, CMPRB0,
//          etc (regardless of the timernum specified by 'n').
//
//*****************************************************************************
#define CTIMERADDRn(periph, n, reg) ( periph##_BASE                 +   \
                                      offsetof(periph##_Type, reg)  +   \
                                      (n * (offsetof(periph##_Type, TMR1) - offsetof(periph##_Type, TMR0))) )

//
// Enumerations for the eOutputType argument of am_hal_ctimer_output_config().
//
typedef enum
{
    AM_HAL_CTIMER_OUTPUT_NORMAL         = 0x0,
    AM_HAL_CTIMER_OUTPUT_SECONDARY      = 0x1,
    AM_HAL_CTIMER_OUTPUT_FORCE0         = 0x2,
    AM_HAL_CTIMER_OUTPUT_FORCE1         = 0x3
} am_hal_ctimer_outputtype_e;


//*****************************************************************************
//
// Lookup tables used by am_hal_ctimer_output_config().
//
//  CTx_tbl[] relates the padnum and pad funcsel based on a given CTx.
//  Valid pads for CTx are: 4-7, 11-13, 18-19, 22-33, 35, 37, 39, 42-49.
//
//  outcfg_tbl[] contains attributes of the 4 output signal types for each
//  of the 32 CTx signals. Therefore it is indexed by CTnumber 0-31.
//  This table provides only the non-common OUTCFG attributes (2-5, other
//  settings are shown below).
//  OUTCFG 0 = Force output to 0.
//  OUTCFG 1 = Force output to 1.
//  OUTCFG 6 = A6OUT2.
//  OUTCFG 7 = A7OUT2.
//
//*****************************************************************************
#define CTXPADNUM(ctx)  ((CTx_tbl[ctx] >> 0) & 0x3f)
#define CTXPADFNC(ctx)  ((CTx_tbl[ctx] >> 8) & 0x7)
#define CTX(pad, fn)    ((fn << 8) | (pad << 0))
static const uint16_t CTx_tbl[32] =
{
    CTX(12,2), CTX(25,2), CTX(13,2), CTX(26,2), CTX(18,2),      // 0 - 4
    CTX(27,2), CTX(19,2), CTX(28,2), CTX( 5,7), CTX(29,2),      // 5 - 9
    CTX( 6,5), CTX(30,2), CTX(22,2), CTX(31,2), CTX(23,2),      // 10 - 14
    CTX(32,2), CTX(42,2), CTX( 4,6), CTX(43,2), CTX( 7,7),      // 15 - 19
    CTX(44,2), CTX(24,5), CTX(45,2), CTX(33,6), CTX(46,2),      // 20 - 24
    CTX(39,2), CTX(47,2), CTX(35,5), CTX(48,2), CTX(37,7),      // 25 - 29
    CTX(49,2), CTX(11,2)                                        // 30 - 31
};


#define PADCTXNUM(pad)  ((ctx_from_pad_tbl[pad] >> 0) & 0xFF)
#define PADCTXFNC(pad)  ((ctx_from_pad_tbl[pad] >> 8) & 0xFF)
#define PAD(ctx, fn)    ((fn << 8) | (ctx << 0))
#define NOCTX   0xFFFF

static const uint16_t ctx_from_pad_tbl[50] = {
    NOCTX,     NOCTX,     NOCTX,     NOCTX,     PAD(17,6),      // 0 - 4
    PAD(8,7),  PAD(10,5), PAD(19,7), NOCTX,     NOCTX,          // 5 - 9
    NOCTX,     PAD(31,2), PAD(0,2),  PAD(2,2),  NOCTX,          // 10 - 14
    NOCTX,     NOCTX,     NOCTX,     PAD(4,2),  PAD(6,2),       // 15 - 19
    NOCTX,     NOCTX,     PAD(12,2), PAD(14,2), PAD(21,5),      // 20 - 24
    PAD(1,2),  PAD(3,2),  PAD(5,2),  PAD(7,2),  PAD(9,2),       // 25 - 29
    PAD(11,2), PAD(13,2), PAD(15,2), PAD(23,6), NOCTX,          // 30 - 34
    PAD(27,5), NOCTX,     PAD(29,7), NOCTX,     PAD(25,2),      // 35 - 39
    NOCTX,     NOCTX,     PAD(16,2), PAD(18,2), PAD(20,2),      // 40 - 44
    PAD(22,2), PAD(24,2), PAD(26,2), PAD(28,2), PAD(30,2),      // 45 - 49
};

#define OUTC(timB,timN,N2)      ((N2 << 4) | (timB << 3) | (timN << 0))
#define OUTCTIMN(ctx,n)         (outcfg_tbl[ctx][n] & (0x7 << 0))
#define OUTCTIMB(ctx,n)         (outcfg_tbl[ctx][n] & (0x1 << 3))
#define OUTCO2(ctx,n)           (outcfg_tbl[ctx][n] & (0x1 << 4))
static const uint8_t outcfg_tbl[32][4] =
{
    {OUTC(0,0,0), OUTC(1,2,1), OUTC(0,5,1), OUTC(0,6,0)},     // CTX0:  A0OUT,  B2OUT2, A5OUT2, A6OUT
    {OUTC(0,0,1), OUTC(0,0,0), OUTC(0,5,0), OUTC(1,7,1)},     // CTX1:  A0OUT2, A0OUT,  A5OUT,  B7OUT2
    {OUTC(1,0,0), OUTC(1,1,1), OUTC(1,6,1), OUTC(0,7,0)},     // CTX2:  B0OUT,  B1OUT2, B6OUT2, A7OUT
    {OUTC(1,0,1), OUTC(1,0,0), OUTC(0,1,0), OUTC(0,6,0)},     // CTX3:  B0OUT2, B0OUT,  A1OUT,  A6OUT
    {OUTC(0,1,0), OUTC(0,2,1), OUTC(0,5,1), OUTC(1,5,0)},     // CTX4:  A1OUT,  A2OUT2, A5OUT2, B5OUT
    {OUTC(0,1,1), OUTC(0,1,0), OUTC(1,6,0), OUTC(0,7,0)},     // CTX5:  A1OUT2, A1OUT,  B6OUT,  A7OUT
    {OUTC(1,1,0), OUTC(0,1,0), OUTC(1,5,1), OUTC(1,7,0)},     // CTX6:  B1OUT,  A1OUT,  B5OUT2, B7OUT
    {OUTC(1,1,1), OUTC(1,1,0), OUTC(1,5,0), OUTC(0,7,0)},     // CTX7:  B1OUT2, B1OUT,  B5OUT,  A7OUT
    {OUTC(0,2,0), OUTC(0,3,1), OUTC(0,4,1), OUTC(1,6,0)},     // CTX8:  A2OUT,  A3OUT2, A4OUT2, B6OUT
    {OUTC(0,2,1), OUTC(0,2,0), OUTC(0,4,0), OUTC(1,0,0)},     // CTX9:  A2OUT2, A2OUT,  A4OUT,  B0OUT
    {OUTC(1,2,0), OUTC(1,3,1), OUTC(1,4,1), OUTC(0,6,0)},     // CTX10: B2OUT,  B3OUT2, B4OUT2, A6OUT
    {OUTC(1,2,1), OUTC(1,2,0), OUTC(1,4,0), OUTC(1,5,1)},     // CTX11: B2OUT2, B2OUT,  B4OUT,  B5OUT2
    {OUTC(0,3,0), OUTC(1,1,0), OUTC(1,0,1), OUTC(1,6,1)},     // CTX12: A3OUT,  B1OUT,  B0OUT2, B6OUT2
    {OUTC(0,3,1), OUTC(0,3,0), OUTC(0,6,0), OUTC(1,4,1)},     // CTX13: A3OUT2, A3OUT,  A6OUT,  B4OUT2
    {OUTC(1,3,0), OUTC(1,1,0), OUTC(1,7,1), OUTC(0,7,0)},     // CTX14: B3OUT,  B1OUT,  B7OUT2, A7OUT
    {OUTC(1,3,1), OUTC(1,3,0), OUTC(0,7,0), OUTC(0,4,1)},     // CTX15: B3OUT2, B3OUT,  A7OUT,  A4OUT2
    {OUTC(0,4,0), OUTC(0,0,0), OUTC(0,0,1), OUTC(1,3,1)},     // CTX16: A4OUT,  A0OUT,  A0OUT2, B3OUT2
    {OUTC(0,4,1), OUTC(1,7,0), OUTC(0,4,0), OUTC(0,1,1)},     // CTX17: A4OUT2, B7OUT,  A4OUT,  A1OUT2
    {OUTC(1,4,0), OUTC(1,0,0), OUTC(0,0,0), OUTC(0,3,1)},     // CTX18: B4OUT,  B0OUT,  A0OUT,  A3OUT2
    {OUTC(1,4,1), OUTC(0,2,0), OUTC(1,4,0), OUTC(1,1,1)},     // CTX19: B4OUT2, A2OUT,  B4OUT,  B1OUT2
    {OUTC(0,5,0), OUTC(0,1,0), OUTC(0,1,1), OUTC(1,2,1)},     // CTX20: A5OUT,  A1OUT,  A1OUT2, B2OUT2
    {OUTC(0,5,1), OUTC(0,1,0), OUTC(1,5,0), OUTC(0,0,1)},     // CTX21: A5OUT2, A1OUT,  B5OUT,  A0OUT2
    {OUTC(1,5,0), OUTC(0,6,0), OUTC(0,1,0), OUTC(0,2,1)},     // CTX22: B5OUT,  A6OUT,  A1OUT,  A2OUT2
    {OUTC(1,5,1), OUTC(0,7,0), OUTC(0,5,0), OUTC(1,0,1)},     // CTX23: B5OUT2, A7OUT,  A5OUT,  B0OUT2
    {OUTC(0,6,0), OUTC(0,2,0), OUTC(0,1,0), OUTC(1,1,1)},     // CTX24: A6OUT,  A2OUT,  A1OUT,  B1OUT2
    {OUTC(1,4,1), OUTC(1,2,0), OUTC(0,6,0), OUTC(0,2,1)},     // CTX25: B4OUT2, B2OUT,  A6OUT,  A2OUT2
    {OUTC(1,6,0), OUTC(1,2,0), OUTC(0,5,0), OUTC(0,1,1)},     // CTX26: B6OUT,  B2OUT,  A5OUT,  A1OUT2
    {OUTC(1,6,1), OUTC(0,1,0), OUTC(1,6,0), OUTC(1,2,1)},     // CTX27: B6OUT2, A1OUT,  B6OUT,  B2OUT2
    {OUTC(0,7,0), OUTC(0,3,0), OUTC(0,5,1), OUTC(1,0,1)},     // CTX28: A7OUT,  A3OUT,  A5OUT2, B0OUT2
    {OUTC(1,5,1), OUTC(0,1,0), OUTC(0,7,0), OUTC(0,3,1)},     // CTX29: B5OUT2, A1OUT,  A7OUT,  A3OUT2
    {OUTC(1,7,0), OUTC(1,3,0), OUTC(0,4,1), OUTC(0,0,1)},     // CTX30: B7OUT,  B3OUT,  A4OUT2, A0OUT2
    {OUTC(1,7,1), OUTC(0,6,0), OUTC(1,7,0), OUTC(1,3,1)},     // CTX31: B7OUT2, A6OUT,  B7OUT,  B3OUT2
};


//*****************************************************************************
//
//! CMSIS-Style macro for handling a variable CTIMER module number.
//
//*****************************************************************************
#define CTIMERn(n) ((CTIMER_Type*)(CTIMER_BASE + (n * ((uint32_t)&CTIMER->TMR1 - (uint32_t)&CTIMER->TMR0))))

//*****************************************************************************
//
//! Number of timers
//
//*****************************************************************************
#define AM_HAL_CTIMER_TIMERS_NUM    8

//*****************************************************************************
//
//! Timer offset value
//
//*****************************************************************************
#define AM_HAL_CTIMER_TIMER_OFFSET  ((uint32_t)&CTIMER->TMR1 - (uint32_t)&CTIMER->TMR0)

//*****************************************************************************
//
//! @name Interrupt Status Bits
//! @brief Interrupt Status Bits for enable/disble use
//!
//! These macros may be used to set and clear interrupt bits
//! @{
//
//*****************************************************************************
#define AM_HAL_CTIMER_INT_TIMERA0C0         CTIMER_INTEN_CTMRA0C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA0C1         CTIMER_INTEN_CTMRA0C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA1C0         CTIMER_INTEN_CTMRA1C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA1C1         CTIMER_INTEN_CTMRA1C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA2C0         CTIMER_INTEN_CTMRA2C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA2C1         CTIMER_INTEN_CTMRA2C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA3C0         CTIMER_INTEN_CTMRA3C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA3C1         CTIMER_INTEN_CTMRA3C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA4C0         CTIMER_INTEN_CTMRA4C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA4C1         CTIMER_INTEN_CTMRA4C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA5C0         CTIMER_INTEN_CTMRA5C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA5C1         CTIMER_INTEN_CTMRA5C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA6C0         CTIMER_INTEN_CTMRA6C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA6C1         CTIMER_INTEN_CTMRA6C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA7C0         CTIMER_INTEN_CTMRA7C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA7C1         CTIMER_INTEN_CTMRA7C1INT_Msk

#define AM_HAL_CTIMER_INT_TIMERB0C0         CTIMER_INTEN_CTMRB0C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB0C1         CTIMER_INTEN_CTMRB0C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB1C0         CTIMER_INTEN_CTMRB1C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB1C1         CTIMER_INTEN_CTMRB1C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB2C0         CTIMER_INTEN_CTMRB2C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB2C1         CTIMER_INTEN_CTMRB2C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB3C0         CTIMER_INTEN_CTMRB3C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB3C1         CTIMER_INTEN_CTMRB3C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB4C0         CTIMER_INTEN_CTMRB4C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB4C1         CTIMER_INTEN_CTMRB4C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB5C0         CTIMER_INTEN_CTMRB5C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB5C1         CTIMER_INTEN_CTMRB5C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB6C0         CTIMER_INTEN_CTMRB6C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB6C1         CTIMER_INTEN_CTMRB6C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB7C0         CTIMER_INTEN_CTMRB7C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB7C1         CTIMER_INTEN_CTMRB7C1INT_Msk
//! @}

//*****************************************************************************
//
//  DEPRECATED Interrupt Status Bits
//
//*****************************************************************************
#define AM_HAL_CTIMER_INT_TIMERA0           AM_HAL_CTIMER_INT_TIMERA0C0
#define AM_HAL_CTIMER_INT_TIMERB0           AM_HAL_CTIMER_INT_TIMERB0C0
#define AM_HAL_CTIMER_INT_TIMERA1           AM_HAL_CTIMER_INT_TIMERA1C0
#define AM_HAL_CTIMER_INT_TIMERB1           AM_HAL_CTIMER_INT_TIMERB1C0
#define AM_HAL_CTIMER_INT_TIMERA2           AM_HAL_CTIMER_INT_TIMERA2C0
#define AM_HAL_CTIMER_INT_TIMERB2           AM_HAL_CTIMER_INT_TIMERB2C0
#define AM_HAL_CTIMER_INT_TIMERA3           AM_HAL_CTIMER_INT_TIMERA3C0
#define AM_HAL_CTIMER_INT_TIMERB3           AM_HAL_CTIMER_INT_TIMERB3C0

//*****************************************************************************
//
//! @name Configuration options
//! @brief Configuration options for \e am_hal_ctimer_config_t
//!
//! These options are to be used with the \e am_hal_ctimer_config_t structure
//! used by \e am_hal_ctimer_config
//! @{
//
//*****************************************************************************
#define AM_HAL_CTIMER_CLK_PIN               _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x00)
#define AM_HAL_CTIMER_HFRC_12MHZ            _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x01)
#define AM_HAL_CTIMER_HFRC_3MHZ             _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x02)
#define AM_HAL_CTIMER_HFRC_187_5KHZ         _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x03)
#define AM_HAL_CTIMER_HFRC_47KHZ            _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x04)
#define AM_HAL_CTIMER_HFRC_12KHZ            _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x05)
#define AM_HAL_CTIMER_XT_32_768KHZ          _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x06)
#define AM_HAL_CTIMER_XT_16_384KHZ          _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x07)
#define AM_HAL_CTIMER_XT_2_048KHZ           _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x08)
#define AM_HAL_CTIMER_XT_256HZ              _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x09)
#define AM_HAL_CTIMER_LFRC_512HZ            _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x0A)
#define AM_HAL_CTIMER_LFRC_32HZ             _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x0B)
#define AM_HAL_CTIMER_LFRC_1HZ              _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x0C)
#define AM_HAL_CTIMER_LFRC_1_16HZ           _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x0D)
#define AM_HAL_CTIMER_RTC_100HZ             _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x0E)
#define AM_HAL_CTIMER_HCLK_DIV4             _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x0F)
#define AM_HAL_CTIMER_XT_DIV4               _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x10)
#define AM_HAL_CTIMER_XT_DIV8               _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x11)
#define AM_HAL_CTIMER_XT_DIV32              _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x12)
#define AM_HAL_CTIMER_RSVD                  _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x13)
//! @}

//*****************************************************************************
//
//! Timer function macros.
//!
//! @{
//
//*****************************************************************************
//! Single Count: Counts one time to the compare value, then the output
//! changes polarity and stays at that level, with an optional interrupt.
#define AM_HAL_CTIMER_FN_ONCE               _VAL2FLD(CTIMER_CTRL0_TMRA0FN, 0)
//! Repeated Count: Periodic 1-clock-cycle wide pulses with optional interrupts.
#define AM_HAL_CTIMER_FN_REPEAT             _VAL2FLD(CTIMER_CTRL0_TMRA0FN, 1)
//! Single Pulse (One Shot): A single pulse of programmed width, with an optional interrupt.
#define AM_HAL_CTIMER_FN_PWM_ONCE           _VAL2FLD(CTIMER_CTRL0_TMRA0FN, 2)
//! Repeated Pulse: A rectangular (or square) waveform with programmed high and
//! low widths, and optional interrupts on each cycle.
#define AM_HAL_CTIMER_FN_PWM_REPEAT         _VAL2FLD(CTIMER_CTRL0_TMRA0FN, 3)
//! Single Pattern: one burst of bits specified by the CMPR0/1/2/3 registers.
#define AM_HAL_CTIMER_FN_PTN_ONCE           _VAL2FLD(CTIMER_CTRL0_TMRA0FN, 4)
//! Repeated Pattern: repeated burst of bits specified by the CMPR0/1/2/3 registers.
#define AM_HAL_CTIMER_FN_PTN_REPEAT         _VAL2FLD(CTIMER_CTRL0_TMRA0FN, 5)
//! Continuous: Free running timer with a single level change on the output and
//! a single optional interrupt.
#define AM_HAL_CTIMER_FN_CONTINUOUS         _VAL2FLD(CTIMER_CTRL0_TMRA0FN, 6)
//! Alternate Pulse: like Repeated Pulse but alternating between two different
//! pulse width/spacing settings.
#define AM_HAL_CTIMER_FN_PWM_ALTERNATE      _VAL2FLD(CTIMER_CTRL0_TMRA0FN, 7)
//! @}

//*****************************************************************************
//
//! Half-timer options.
//!
//! @{
//
//*****************************************************************************
#define AM_HAL_CTIMER_INT_ENABLE            CTIMER_CTRL0_TMRA0IE0_Msk
//#define AM_HAL_CTIMER_PIN_ENABLE            CTIMER_CTRL0_TMRA0PE_Msk
#define AM_HAL_CTIMER_PIN_INVERT            CTIMER_CTRL0_TMRA0POL_Msk
#define AM_HAL_CTIMER_CLEAR                 CTIMER_CTRL0_TMRA0CLR_Msk
//! @}

//*****************************************************************************
//
//! Additional timer options.
//!
//! @{
//
//*****************************************************************************
#define AM_HAL_CTIMER_LINK                  CTIMER_CTRL0_CTLINK0_Msk
#define AM_HAL_CTIMER_ADC_TRIG              CTIMER_CTRL3_ADCEN_Msk
//! @}

//*****************************************************************************
//
//! Timer selection macros.
//!
//! @{
//
//*****************************************************************************
#define AM_HAL_CTIMER_TIMERA                0x0000FFFF
#define AM_HAL_CTIMER_TIMERB                0xFFFF0000
#define AM_HAL_CTIMER_BOTH                  0xFFFFFFFF
//! @}

//*****************************************************************************
//
//! Timer trigger options for Apollo3 Blue (rev B0 and later) including
//! Apollo3 Blue Plus.
//!
//! Valid only for CTIMER4 and CTIMER5 when CTLINK==1 and TMRA4TRIG==1
//!
//! @{
//
//*****************************************************************************
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCAP0     CTIMER_AUX4_TMRB4TRIG_A7OUT
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCAP1     CTIMER_AUX4_TMRB4TRIG_B7OUT
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCAP2     CTIMER_AUX4_TMRB4TRIG_A1OUT
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCAP3     CTIMER_AUX4_TMRB4TRIG_B1OUT
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCMP0     CTIMER_AUX4_TMRB4TRIG_B3OUT2
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCMP1     CTIMER_AUX4_TMRB4TRIG_A3OUT2
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCMP2     CTIMER_AUX4_TMRB4TRIG_A1OUT2
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCMP3     CTIMER_AUX4_TMRB4TRIG_B1OUT2
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCMP4     CTIMER_AUX4_TMRB4TRIG_A6OUT2DUAL
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCMP5     CTIMER_AUX4_TMRB4TRIG_A7OUT2DUAL
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCMP6     CTIMER_AUX4_TMRB4TRIG_B5OUT2DUAL
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCMP7     CTIMER_AUX4_TMRB4TRIG_A5OUT2DUAL

#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCAP0     CTIMER_AUX5_TMRB5TRIG_A7OUT
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCAP1     CTIMER_AUX5_TMRB5TRIG_B7OUT
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCAP2     CTIMER_AUX5_TMRB5TRIG_A1OUT
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCAP3     CTIMER_AUX5_TMRB5TRIG_B1OUT
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCMP0     CTIMER_AUX5_TMRB5TRIG_B3OUT2
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCMP1     CTIMER_AUX5_TMRB5TRIG_A3OUT2
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCMP2     CTIMER_AUX5_TMRB5TRIG_A1OUT2
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCMP3     CTIMER_AUX5_TMRB5TRIG_B1OUT2
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCMP4     CTIMER_AUX5_TMRB5TRIG_A6OUT2DUAL
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCMP5     CTIMER_AUX5_TMRB5TRIG_A7OUT2DUAL
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCMP6     CTIMER_AUX5_TMRB5TRIG_B5OUT2DUAL
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCMP7     CTIMER_AUX5_TMRB5TRIG_A5OUT2DUAL
//! @}

//*****************************************************************************
//
//! @name All-In-One Configuration
//! @brief New API for multiple timer configuration.
//!
//! These options are to be used with the \e am_hal_ctimer_config_t structure
//! used by \e am_hal_ctimer_config
//! @{
//
//*****************************************************************************
//! CTimer AIO Compare Configuration.
typedef struct
{
    //
    //! Function Number.
    //
    uint32_t FN;
    //
    //! Timer Segment. Timer A, B, BOTH selector.
    //
    uint32_t AB;
    //
    //! Compare Register A0.
    //
    uint32_t A0;
    //
    //! Compare Register A1.
    //
    uint32_t A1;
    //
    //! Compare Register A2.
    //
    uint32_t A2;
    //
    //! Compare Register A3.
    //
    uint32_t A3;
    //
    //! Compare Register B0.
    //
    uint32_t B0;
    //
    //! Compare Register B1.
    //
    uint32_t B1;
    //
    //! Compare Register B2.
    //
    uint32_t B2;
    //
    //! Compare Register B3.
    //
    uint32_t B3;
    //
    //! LMT field values.
    //
    uint32_t LMT;
    //
    //! A "T" indicates that a 1 is loaded if the OUT2 output is used, otherwise a 0 is loaded.
    //
    uint32_t EN23;
    //
    //! TRIG: a single pattern will be triggered; TERM: a repeated pattern will be terminated.
    //
    uint32_t TRIG;
    //
    //! Select clock source: internal, external, a buck pulse, or output of another CTIMER.
    //
    uint32_t CLK;
    //
    //!  Enable the primary interrupt INT.
    //
    uint32_t IE0;
    //
    //!  Enable the secondary interrupt INT2.
    //
    uint32_t IE1;
    //
    //!  Select the polarity of the OUT output.
    //
    uint32_t POL;
    //
    //!  Select the polarity of the OUT2 output.
    //
    uint32_t POL23;
    //
    //! Select polarity of both OUT and OUT2 as a function of the trigger input.
    //
    uint32_t TINV;
    //
    //!  Disable clock synchronization on read.
    //
    uint32_t NOSYNC;
    //
    //! Enable the timer.
    // This is ANDed with the global enable in GLOBEN, and allows the counter to begin counting.
    //
    uint32_t EN;
    //
    //  Clear the timer. This will hold the timer at zero even if EN is asserted.
    //  It is typically cleared at the end of a configuration and
    //  is probably not included in the function structure.
    //
    //uint32_t CLR;

}
am_hal_ctimer_aio_config_t;

//! CTimer AIO Output Selection and Interconnect.
typedef struct
{
    //! Pad 0-9
    uint32_t OUTCFG0;
    //! Pad 10-19
    uint32_t OUTCFG1;
    //! Pad 20-29
    uint32_t OUTCFG2;
    //! Pad 30-31
    uint32_t OUTCFG3;
}
am_hal_ctimer_aio_connect_t;
//! @}

//*****************************************************************************
//
//! Timer configuration structure
//
//*****************************************************************************
typedef struct
{
    //
    //! Set to 1 to operate this timer as a 32-bit timer instead of two 16-bit
    //! timers.
    //
    uint32_t ui32Link;

    //
    //! Configuration options for TIMERA
    //
    uint32_t ui32TimerAConfig;

    //
    //! Configuration options for TIMERB
    //
    uint32_t ui32TimerBConfig;

}
am_hal_ctimer_config_t;

//*****************************************************************************
//
//! Function pointer type for CTimer interrupt handlers.
//
//*****************************************************************************
typedef void (*am_hal_ctimer_handler_t)(void);

//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************
extern void am_hal_ctimer_globen(uint32_t ui32ConfigVal);

extern void am_hal_ctimer_config(uint32_t ui32TimerNumber,
                                 am_hal_ctimer_config_t *psConfig);

extern void am_hal_ctimer_config_single(uint32_t ui32TimerNumber,
                                        uint32_t ui32TimerSegment,
                                        uint32_t ui32ConfigVal);

extern void am_hal_ctimer_config_trigger(uint32_t ui32TimerNumber,
                                         uint32_t ui32TimerSegment,
                                         uint32_t ui32ConfigVal);

extern void am_hal_ctimer_start(uint32_t ui32TimerNumber,
                                uint32_t ui32TimerSegment);

extern void am_hal_ctimer_stop(uint32_t ui32TimerNumber,
                               uint32_t ui32TimerSegment);

extern void am_hal_ctimer_clear(uint32_t ui32TimerNumber,
                                uint32_t ui32TimerSegment);

extern uint32_t am_hal_ctimer_read(uint32_t ui32TimerNumber,
                                   uint32_t ui32TimerSegment);

extern uint32_t am_hal_ctimer_output_config(uint32_t ui32TimerNumber,
                                            uint32_t ui32TimerSegment,
                                            uint32_t ui32PadNum,
                                            uint32_t eOutputType,
                                            uint32_t eDriveStrength);

extern void am_hal_ctimer_input_config(uint32_t ui32TimerNumber,
                                       uint32_t ui32TimerSegment,
                                       uint32_t ui32TimerOutputConfig);

extern void am_hal_ctimer_compare_set(uint32_t ui32TimerNumber,
                                      uint32_t ui32TimerSegment,
                                      uint32_t ui32CompareReg,
                                      uint32_t ui32Value);

extern void am_hal_ctimer_aux_compare_set(uint32_t ui32TimerNumber,
                                      uint32_t ui32TimerSegment,
                                      uint32_t ui32CompareReg,
                                      uint32_t ui32Value);

extern void am_hal_ctimer_period_set(uint32_t ui32TimerNumber,
                                     uint32_t ui32TimerSegment,
                                     uint32_t ui32Period,
                                     uint32_t ui32OnTime);

extern void am_hal_ctimer_aux_period_set(uint32_t ui32TimerNumber,
                                     uint32_t ui32TimerSegment,
                                     uint32_t ui32Period,
                                     uint32_t ui32OnTime);

extern void am_hal_ctimer_adc_trigger_enable(void);
extern void am_hal_ctimer_adc_trigger_disable(void);
extern void am_hal_ctimer_int_enable(uint32_t ui32Interrupt);
extern uint32_t am_hal_ctimer_int_enable_get(void);
extern void am_hal_ctimer_int_disable(uint32_t ui32Interrupt);
extern void am_hal_ctimer_int_set(uint32_t ui32Interrupt);
extern void am_hal_ctimer_int_clear(uint32_t ui32Interrupt);
extern uint32_t am_hal_ctimer_int_status_get(bool bEnabledOnly);
extern void am_hal_ctimer_int_register(uint32_t ui32Interrupt,
                         am_hal_ctimer_handler_t pfnHandler);
extern void am_hal_ctimer_int_service(uint32_t ui32Status);

//
// General function to do triple back-to-back reads.
//
extern void am_hal_triple_read(uint32_t u32TimerAddr, uint32_t ui32Data[]);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_CTIMER_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
