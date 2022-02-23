/*******************************************************************************
 *
 *
 *******************************************************************************
 */

#include "pwmout_api.h"

#include "PeripheralPins.h"
#include "mbed_assert.h"
#include "pinmap.h"

// void pwmout_init_direct(pwmout_t *obj, const PinMap *pinmap) {}

void pwmout_init(pwmout_t *obj, PinName pin) {
    // makes sure the pad / pin has the correct type
    ap3_gpio_pad_t pad = (ap3_gpio_pad_t)pin;

    // // pad cannot be greater than 49
    // MBED_ASSERT(pad <= 49);

    // // gets the correct ctx ctimer output signal for the pad
    uint8_t ctx = PADCTXNUM(pad);

    // if ctx is 0xFF and ctx_fnc is 0xFF then the pad is not connected to a
    // ctimer output signal and cannot output PWM, throw a mbed error
    MBED_ASSERT(ctx != 0xFF);
    // MBED_ASSERT(ctx_fnc != 0xFF);

    // // looks up the associating Timer, Segment, and
    uint32_t timer = OUTCTIMN(ctx, 0);  // what timer number ex. 0-7

    uint32_t segment = AM_HAL_CTIMER_TIMERA;  // A or B for that timer number
    if (OUTCTIMB(ctx, 0)) {
        segment = AM_HAL_CTIMER_TIMERB;
    }

    am_hal_ctimer_outputtype_e output =
        AM_HAL_CTIMER_OUTPUT_NORMAL;  // either OUT or OUT2
    if (OUTCO2(ctx, 0)) {
        output = AM_HAL_CTIMER_OUTPUT_SECONDARY;
    }

    obj->timer = timer;
    obj->segment = segment;
    obj->output = output;
    obj->pad = pad;
    obj->c_clock_frq = 0;
    obj->resolution = 256;
    obj->pulse_width_us = 0;

    am_hal_ctimer_output_config(
        timer, segment, pad, output, AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA);
}

void pwmout_free(pwmout_t *obj) {
    am_hal_ctimer_stop(obj->timer, obj->segment);
    // am_hal_ctimer_output_config(
    //     timer,
    //     segment,
    //     pad,
    //     output,
    //     AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA // obj->drive_strength
    // );
    // am_hal_ctimer_start(
    //     timer,
    //     segment
    // );
}

void pwmout_write(pwmout_t *obj, float percent) {
    // Saturate percent if outside of range
    if (percent < 0.0f) {
        percent = 0.0f;
    } else if (percent > 1.0f) {
        percent = 1.0f;
    }

    // Resize the pulse width to set the duty cycle
    pwmout_pulsewidth_us(obj, (int)(percent * obj->period_us));
}

float pwmout_read(pwmout_t *obj) {
    // Check for when pulsewidth or period equals 0
    if ((obj->pulse_width_us == 0) || (obj->period_us == 0)) {
        return 0;
    }

    // Return the duty cycle
    return ((float)obj->pulse_width_us / (float)obj->period_us);
}

void pwmout_period(pwmout_t *obj, float seconds) {
    pwmout_period_us(obj, (int)(seconds * 1000000.0f));
}

void pwmout_period_ms(pwmout_t *obj, int ms) {
    pwmout_period_us(obj, ms * 1000);
}

void pwmout_period_us(pwmout_t *obj, int us) {
    // Set pulse width to half the period if uninitialized
    if (obj->pulse_width_us == -1) {
        obj->pulse_width_us = us / 2;
    }

    // Save the period
    obj->period_us = us;

    // Update the registers
    pwmout_update_period(obj);
}

int pwmout_read_period_us(pwmout_t *obj) { return obj->period_us; }

void pwmout_pulsewidth(pwmout_t *obj, float seconds) {
    pwmout_pulsewidth_us(obj, (int)(seconds * 1000000.0));
}

void pwmout_pulsewidth_ms(pwmout_t *obj, int ms) {
    pwmout_pulsewidth_us(obj, ms * 1000);
}

void pwmout_pulsewidth_us(pwmout_t *obj, int us) {
    obj->pulse_width_us = us;
    pwmout_update(obj);
}

void pwmout_update_period(pwmout_t *obj) {
    float clock_threshold = (1000000.0 / obj->period_us) * 256.0;

    // determine what clock to use for a specific period
    uint32_t ctimer_ctl;
    if (clock_threshold <= 1 / 16) {
        ctimer_ctl = (AM_HAL_CTIMER_FN_PWM_REPEAT | AM_HAL_CTIMER_LFRC_1_16HZ);
        obj->c_clock_frq = 1 / 16;
    } else if (clock_threshold <= 1) {
        ctimer_ctl = (AM_HAL_CTIMER_FN_PWM_REPEAT | AM_HAL_CTIMER_LFRC_1HZ);
        obj->c_clock_frq = 1;
    } else if (clock_threshold <= 32) {
        ctimer_ctl = (AM_HAL_CTIMER_FN_PWM_REPEAT | AM_HAL_CTIMER_LFRC_32HZ);
        obj->c_clock_frq = 32;
    } else if (clock_threshold <= 256) {
        ctimer_ctl = (AM_HAL_CTIMER_FN_PWM_REPEAT | AM_HAL_CTIMER_XT_256HZ);
        obj->c_clock_frq = 256;
    } else if (clock_threshold <= 2048) {
        ctimer_ctl = (AM_HAL_CTIMER_FN_PWM_REPEAT | AM_HAL_CTIMER_XT_2_048KHZ);
        obj->c_clock_frq = 2048;
    } else if (clock_threshold <= 16384) {
        ctimer_ctl = (AM_HAL_CTIMER_FN_PWM_REPEAT | AM_HAL_CTIMER_XT_16_384KHZ);
        obj->c_clock_frq = 16384;
    } else if (clock_threshold <= 32768) {
        ctimer_ctl = (AM_HAL_CTIMER_FN_PWM_REPEAT | AM_HAL_CTIMER_XT_32_768KHZ);
        obj->c_clock_frq = 32768;
    } else if (clock_threshold <= 187500) {
        ctimer_ctl =
            (AM_HAL_CTIMER_FN_PWM_REPEAT | AM_HAL_CTIMER_HFRC_187_5KHZ);
        obj->c_clock_frq = 187500;
    } else if (clock_threshold <= 3000000) {
        ctimer_ctl = (AM_HAL_CTIMER_FN_PWM_REPEAT | AM_HAL_CTIMER_HFRC_3MHZ);
        obj->c_clock_frq = 3000000;
    } else {
        ctimer_ctl = (AM_HAL_CTIMER_FN_PWM_REPEAT | AM_HAL_CTIMER_HFRC_12MHZ);
        obj->c_clock_frq = 12000000;
    }

    am_hal_ctimer_config_single(obj->timer, obj->segment, ctimer_ctl);
    am_hal_ctimer_start(obj->timer, obj->segment);
    pwmout_update(obj);
}

void pwmout_update(pwmout_t *obj) {
    am_hal_ctimer_period_set(
        obj->timer,
        obj->segment,
        (uint32_t)obj->c_clock_frq * obj->period_us / 1000000,
        (uint32_t)obj->c_clock_frq * obj->pulse_width_us / 1000000);
}

int pwmout_read_pulsewidth_us(pwmout_t *obj) { return obj->pulse_width_us; }

const PinMap *pwmout_pinmap(void) { return PinMap_PWM_OUT; }
