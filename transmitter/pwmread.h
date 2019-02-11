#ifndef PWM_READ_H
#define PWM_READ_H 

#include <stdbool.h>
#include <stdint.h>

/* 
 * The number of values to store in the PWMStatus.
 */
const uint8_t NUM_VALUES_TO_STORE = 2;

const float MAX_PULSE_WIDTH = 2000.0;
const float MIN_PULSE_WIDTH = 1000.0;

struct PWMStatus {

    unsigned long pulse_width;
    unsigned long pulse_start_time;
    bool is_high;
    uint16_t values[NUM_VALUES_TO_STORE];
};

void pwm_read_setup(uint8_t pin, PWMStatus* status, uint16_t default_value);

void update_pwm(uint8_t pin, PWMStatus* status);

/*
 * Get the smallest of the stored values. This is done since
 * a miscalculation of the time will probably always be an over-estimation.
 */
uint16_t get_current_value(PWMStatus* status);

uint16_t calculate_percentage(unsigned long pulse_width);

#endif /* ifndef PWM_READ_H */

