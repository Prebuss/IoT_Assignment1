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
