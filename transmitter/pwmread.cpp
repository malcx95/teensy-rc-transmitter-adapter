#include "pwmread.h"
#include <Arduino.h>

// uint8_t calculate_percentage(unsigned long pulse_width);
void add_measurement(unsigned long pulse_width, PWMStatus* status);

void pwm_read_setup(uint8_t pin, PWMStatus* status, uint16_t default_value) {
    pinMode(pin, INPUT);
    status->pulse_width = 0;
    status->pulse_start_time= 0;
    status->is_high = false;
    for (uint8_t i = 0; i < NUM_VALUES_TO_STORE; ++i) {
        status->values[i] = default_value;
    }
}


void update_pwm(uint8_t pin, PWMStatus* status) {
    if (digitalRead(pin) == HIGH) {
        // if the input already was high, don't do anything
        if (status->is_high) {
            return;
        }
        // the edge is rising, start timing.
        status->pulse_start_time = micros();
        status->is_high = true;
    } else {
        // if the input was already low, don't do anything
        if (!status->is_high) {
            return;
        }
        // the edge is falling, stop timing
        status->pulse_width = micros() - status->pulse_start_time;
        status->is_high = false;
        add_measurement(status->pulse_width, status);
    }
}

void add_measurement(unsigned long pulse_width, PWMStatus* status) {
    // shift the values
    for (uint8_t i = 0; i < NUM_VALUES_TO_STORE - 1; ++i) {
        status->values[i] = status->values[i + 1];
    }
    status->values[NUM_VALUES_TO_STORE - 1] = calculate_percentage(pulse_width);
}

/*
 * Calculates the "percentage" (0-255 instead of 0-100)
 * of PWM range.
 */
uint16_t calculate_percentage(unsigned long pulse_width) {
    if (pulse_width >= MAX_PULSE_WIDTH) return 65535;
    else if (pulse_width <= MIN_PULSE_WIDTH) return 0;

    return (uint16_t)(65535.0 * (((float)pulse_width) - MIN_PULSE_WIDTH) / 
        (MAX_PULSE_WIDTH - MIN_PULSE_WIDTH));
}

uint16_t get_current_value(PWMStatus* status) {
    uint16_t min = 65535;
    for (uint8_t i = 1; i < NUM_VALUES_TO_STORE; ++i) {
        if (status->values[i] < min) {
            min = status->values[i];
        }
    }
    return min;
}

