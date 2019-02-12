const int STEERING_CHANNEL = 10;
const int THROTTLE_CHANNEL = 0;

const float MAX_PULSE_WIDTH = 2000.0;
const float MIN_PULSE_WIDTH = 1000.0;

const uint16_t MAX_JOYSTICK = 1024;
const float MAX_16_BIT = 65535.;

struct PWMStatus {
    unsigned long pulse_width;
    unsigned long pulse_start_time;
    bool is_high;
};

void setup()
{
    pinMode(13, OUTPUT);
    pinMode(STEERING_CHANNEL, INPUT);
    pinMode(THROTTLE_CHANNEL, INPUT);

    Serial.begin(9600);

    Joystick.useManualSend(true);
    digitalWrite(13, HIGH);
    delay(1000);
    digitalWrite(13, LOW);

}

bool update_pwm(uint8_t pin, PWMStatus* status) {
    if (digitalRead(pin) == HIGH) {
        // if the input already was high, don't do anything
        if (status->is_high) {
            return false;
        }
        // the edge is rising, start timing.
        status->pulse_start_time = micros();
        status->is_high = true;
    } else {
        // if the input was already low, don't do anything
        if (!status->is_high) {
            return false;
        }
        // the edge is falling, stop timing
        status->pulse_width = micros() - status->pulse_start_time;
        status->is_high = false;
        return true;
    }
    return false;
}


void loop()
{

    PWMStatus throttle;
    PWMStatus steering;

    throttle.pulse_width = 0;
    throttle.pulse_start_time = 0;
    throttle.is_high = false;
    steering.pulse_width = 0;
    steering.pulse_start_time = 0;
    steering.is_high = false;

    bool throttle_read = false;
    bool steering_read = false;

    int it = 0;

    while (!(throttle_read && steering_read)) {
        if (!steering_read)
            steering_read = steering_read || update_pwm(STEERING_CHANNEL, &steering);
        if (!throttle_read)
            throttle_read = throttle_read || update_pwm(THROTTLE_CHANNEL, &throttle);
        it++;
    }

    throttle.pulse_width = min(MAX_PULSE_WIDTH, max(MIN_PULSE_WIDTH, throttle.pulse_width));
    steering.pulse_width = min(MAX_PULSE_WIDTH, max(MIN_PULSE_WIDTH, steering.pulse_width));

    // TODO make it so that as soon it adds a new value, send it to the computer.
    // This means making so that update_pwm returns true when it detects a falling edge.
    
    unsigned long curr_throttle = throttle.pulse_width;
    unsigned long curr_steering = steering.pulse_width;

    float throttle_percentage = ((float)curr_throttle - MIN_PULSE_WIDTH)/(MAX_PULSE_WIDTH - MIN_PULSE_WIDTH);
    float steering_percentage = ((float)curr_steering - MIN_PULSE_WIDTH)/(MAX_PULSE_WIDTH - MIN_PULSE_WIDTH);

    uint16_t throttle_val = (uint16_t)(throttle_percentage*MAX_JOYSTICK);
    uint16_t steering_val = (uint16_t)(steering_percentage*MAX_JOYSTICK);

    //Serial.print(throttle_val);
    //Serial.print(" ");
    //Serial.print(steering_val);
    Serial.println(it);

    Joystick.Y(throttle_val);
    Joystick.Z(steering_val);
    Joystick.send_now();
}

