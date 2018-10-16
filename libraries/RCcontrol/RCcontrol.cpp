#include <RCcontrol.h>

void RCcontrol::throttleCB()
{
    bool level = digitalReadFast(THROTTLE_PIN);
    if(level) throttle_time= micros();
    else
    {
        unsigned long int time = micros();
        throttle_pulse = time-throttle_time;    
    }
}

void RCcontrol::steeringCB()
{
     bool level = digitalReadFast(STEERING_PIN);
    if(level) steering_time= micros();
    else
    {
        unsigned long int time = micros();
        steering_pulse = time-steering_time;    
    }
}

void RCcontrol::eStopCB()
{
    bool level = digitalReadFast(E_STOP_PIN);
    if(level) ESTOP=false;
    else ESTOP=true;
    return;
}

void RCcontrol::modeCB()
{
     bool level = digitalReadFast(MODE_PIN);
    if(level) mode_time= micros();
    else
    {
        unsigned long int time = micros();
        mode_pulse = time-mode_time;
        if(mode_pulse < 1200) RCmode = MANUAL;
        else if(mode_pulse > 1700) RCmode = AUTONOMOUS;
        else RCmode = AUGMENTED;    
    }
}

void RCcontrol::LS_LR_CB()
{
    bool level = digitalReadFast(LS_LR_PIN);
    if(level) LS_LR_time= micros();
    else
    {
        int time = micros();
        LS_LR_pulse = time-LS_LR_time;    
    }
}

void RCcontrol::RS_UD_CB()
{
     bool level = digitalReadFast(RS_UD_PIN);
    if(level) RS_UD_time= micros();
    else
    {
        int time = micros();
        RS_UD_pulse = time-RS_UD_time;    
    }
}

int RCcontrol::getThrottle()
{
    return throttle_pulse;   
}
int RCcontrol::getSteering()
{
    return steering_pulse;   
}

int RCcontrol::getMode()
{
    //_web->print("$MO:"+(String)RCmode+"!\r\n");
    //_web->flush();
    return RCmode;   
}
bool RCcontrol::getESTOP()
{
    return ESTOP;
}
int RCcontrol::getLSLR()
{
    if(EXTRAS) return LS_LR_pulse;
    else return 0;   
}

int RCcontrol::getRSUD()
{
    if(EXTRAS) return RS_UD_pulse;
    else return 0;   
}

void RCcontrol::initKR()
{
     pinMode(THROTTLE_PIN, INPUT);
     pinMode(STEERING_PIN, INPUT);
     pinMode(E_STOP_PIN, INPUT);
     pinMode(MODE_PIN, INPUT);
     attachInterrupt(digitalPinToInterrupt(THROTTLE_PIN),&throttleCB,CHANGE);
     attachInterrupt(digitalPinToInterrupt(STEERING_PIN),&steeringCB,CHANGE);
     attachInterrupt(digitalPinToInterrupt(E_STOP_PIN),&eStopCB,CHANGE);
     attachInterrupt(digitalPinToInterrupt(MODE_PIN),&modeCB,CHANGE); 
     EXTRAS=false;
     ESTOP=true;   
}

void RCcontrol::initExtras()
{
     pinMode(LS_LR_PIN, INPUT);
     pinMode(RS_UD_PIN, INPUT);
     attachInterrupt(digitalPinToInterrupt(LS_LR_PIN),&LS_LR_CB,CHANGE);
     attachInterrupt(digitalPinToInterrupt(RS_UD_PIN),&RS_UD_CB,CHANGE);
     EXTRAS = true;    
}

RCcontrol::RCcontrol(usb_serial_class *usb, HardwareSerial *web)
{
     _usb = usb;
     _web = web;
     initKR();                                          
}
