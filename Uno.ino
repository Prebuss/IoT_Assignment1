// PIController.h
#ifndef PIController_h
#define PIController_h

class PIController {
    public:
        PIController(float Kp, float Ki);
        float compute(float setpoint, float measured_value, float dt);
    private:
        float Kp;
        float Ki;
        float integral;
};

#endif

// PIController.cpp
#include "PIController.h"

PIController::PIController(float Kp, float Ki) : Kp(Kp), Ki(Ki), integral(0.0) {}

float PIController::compute(float setpoint, float measured_value, float dt) {
    float error = setpoint - measured_value;
    integral += error * dt;
    return Kp * error + Ki * integral;
}

// LowpassFilter.h
#ifndef LowpassFilter_h
#define LowpassFilter_h

class LowpassFilter {
    public:
        LowpassFilter(float alpha);
        float filter(float new_value);
    private:
        float alpha;
        float filtered_value;
};

#endif

// LowpassFilter.cpp
#include "LowpassFilter.h"

LowpassFilter::LowpassFilter(float alpha) : alpha(alpha), filtered_value(0.0) {}

float LowpassFilter::filter(float new_value) {
    filtered_value = alpha * new_value + (1 - alpha) * filtered_value;
    return filtered_value;
}

// Main Arduino Uno Code
#include "PIController.h"
#include "LowpassFilter.h"
#include <Wire.h>

const int sensorPin = A0;  // TMP36 connected to analog pin A0
const int pwmPin = 9;
PIController piController(1.0, 0.1);  // Proportional and Integral gains
LowpassFilter lowpassFilter(0.1);     // Smoothing factor

float setpoint = 100.0;  // Desired temperature

void setup() {
    Serial.begin(9600);
    pinMode(pwmPin, OUTPUT);
    Wire.begin();  // Initialize I2C communication
}

void loop() {
    int sensorValue = analogRead(sensorPin);
    float voltage = sensorValue * (5.0 / 1023.0);
    float temperatureC = (voltage - 0.5) * 100.0;  // TMP36 conversion
    float filtered_temperature = lowpassFilter.filter(temperatureC);

    float control_signal = piController.compute(setpoint, filtered_temperature, 1.0);

    int pwmValue = (int)(control_signal * 255 / 5.0);  // Scale control signal to PWM range
    analogWrite(pwmPin, pwmValue);

    // Send filtered temperature over I2C
    Wire.beginTransmission(8);  // Address of the receiving device (Arduino MKR NB 1500)
    Wire.write((byte*)&filtered_temperature, sizeof(filtered_temperature));
    Wire.endTransmission();

    Serial.print("Raw Temperature: ");
    Serial.print(temperatureC);
    Serial.print(" C, Filtered Temperature: ");
    Serial.print(filtered_temperature);
    Serial.println(" C");

    delay(1000);  // 1 second delay
}
