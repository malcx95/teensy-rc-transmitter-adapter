#include "pwmread.h"

const int PIN_CHANNEL_1 = 10;
const int PIN_CHANNEL_2 = 0;

const uint16_t MAX_JOYSTICK = 1024;
const float MAX_16_BIT = 65535.;

PWMStatus throttle;
PWMStatus steering;

void setup()
{
    pinMode(13, OUTPUT);

    Serial.begin(9600);

    Joystick.useManualSend(true);
    digitalWrite(13, HIGH);
    delay(1000);
    digitalWrite(13, LOW);

    pwm_read_setup(PIN_CHANNEL_1, &steering, 0);
    pwm_read_setup(PIN_CHANNEL_2, &throttle, 0);

}

void loop()
{

    for (int i = 0; i < 10000; ++i) {
        update_pwm(PIN_CHANNEL_2, &throttle);
        update_pwm(PIN_CHANNEL_1, &steering);
    }

    // TODO make it so that as soon it adds a new value, send it to the computer.
    // This means making so that update_pwm returns true when it detects a falling edge.
    
    uint16_t curr_throttle = get_current_value(&throttle);
    uint16_t curr_steering = get_current_value(&steering);

    float throttle_percentage = (float)curr_throttle/MAX_16_BIT;
    float steering_percentage = (float)curr_steering/MAX_16_BIT;

    uint16_t throttle_val = (uint16_t)(throttle_percentage*MAX_JOYSTICK);
    uint16_t steering_val = (uint16_t)(steering_percentage*MAX_JOYSTICK);

    Serial.print(throttle_val);
    Serial.print(" ");
    Serial.print(steering_val);
    Serial.println("");

    Joystick.Y(throttle_val);
    Joystick.Zrotate(steering_val);
    Joystick.send_now();
}

