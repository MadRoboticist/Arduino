#ifndef _krmotors_h
#define _krmotors_h

#include <i2c_t3.h>
#include <Adafruit_PWMServoDriver.h>
#include <EEPROM.h>

#define THROTTLE 0
#define STEERING 1

#define STEERING_RC_CENTER 1408
#define THROTTLE_CENTER 280
#define THROTTLE_MAX 440
#define THROTTLE_MIN 120

struct init_dat{
       int STEERING_MAX;// 215
       int STEERING_MIN;// 105
       int STEERING_CENTER;// 160
       int THROTTLE_MAX_RC;// 330
       int THROTTLE_MIN_RC;// 261
        //PID COEFFICIENTS
       float PCOEFF_STEERING;// 1.0f
       float DCOEFF_STEERING;// 1.5f
       float ICOEFF_STEERING;// 0.0f
       float PCOEFF_THROTTLE;// 0.1f
       float DCOEFF_THROTTLE;// 0.5f
       float ICOEFF_THROTTLE;// 0.00001f
       float ST_TH_RATIO;// 8.0f
       float DEC_ANGLE;
       };
       
class KRmotors
{
 public:
        //KRmotors(usb_serial_class *usb, Adafruit_PWMServoDriver *pwm);
        KRmotors(usb_serial_class *usb, HardwareSerial *web);
        void RCmix(int throttle, int steering);
        void EStop();
        void AutonMix(float tHeading, int throttle, double heading);
        void AutonRCMix(int throttle, int steering, double heading);
        void initHeading(float heading);
        bool setParams(String webInput);
        void getConfig();
        void readEEPROM();
        void saveEEPROM();
        int prev_THROTTLE;
        int prev_STEERING_PULSE;
        float desired_HEADING;
        float DEC_ANGLE;

 private:
         usb_serial_class *_usb;
         HardwareSerial *_web;
         Adafruit_PWMServoDriver _pwm = Adafruit_PWMServoDriver();
         int prev_STEERING;
         float prev_HEADING;
         float prev_ERR_STEERING;
         float prev_ERR_THROTTLE;
         float I_HEADING;
         float I_THROTTLE;
         int STEERING_MAX;// 215
         int STEERING_MIN;// 105
         int STEERING_CENTER;// 160
         int THROTTLE_MAX_RC;// 330
         int THROTTLE_MIN_RC;// 261
         //PID COEFFICIENTS
         float PCOEFF_STEERING;// 1.0f
         float DCOEFF_STEERING;// 1.5f
         float ICOEFF_STEERING;// 0.0f
         float PCOEFF_THROTTLE;// 0.1f
         float DCOEFF_THROTTLE;// 0.5f
         float ICOEFF_THROTTLE;// 0.00001f
         float ST_TH_RATIO;// 8.0f
         
         init_dat INIT_DAT;
         
};

#endif
