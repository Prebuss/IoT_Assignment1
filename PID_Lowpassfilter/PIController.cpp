#include "PIController.h"

PIController::PIController(float Kp, float Ki) : Kp(Kp), Ki(Ki), integral(0.0) {}

float PIController::compute(float setpoint, float measured_value, float dt) {
    float error = setpoint - measured_value;
    integral += error * dt;
    return Kp * error + Ki * integral;
}

