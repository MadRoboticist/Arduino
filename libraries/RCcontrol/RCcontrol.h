#ifndef _rccontrol_h
#define _rccontrol_h
#include <Arduino.h>

#define THROTTLE_PIN 17
#define STEERING_PIN 20
#define E_STOP_PIN 16
#define MODE_PIN 23
#define LS_LR_PIN 22
#define RS_UD_PIN 21

#define MANUAL 0
#define AUGMENTED 1
#define AUTONOMOUS 2

volatile static unsigned long int throttle_time;
volatile static unsigned long int steering_time;
volatile static unsigned long int e_stop_time;
volatile static unsigned long int mode_time;
volatile static int throttle_pulse;
volatile static int steering_pulse;
volatile static int e_stop_pulse;
volatile static int mode_pulse;

volatile static int LS_LR_time;
volatile static int RS_UD_time;
volatile static int LS_LR_pulse;
volatile static int RS_UD_pulse;
volatile static bool ESTOP;
volatile static uint8_t RCmode;
class RCcontrol
{
      public:
             RCcontrol(usb_serial_class  *usb, HardwareSerial *web);
             void initKR();
             void initExtras();
             int getThrottle();
             int getSteering();
             int getMode();
             bool getESTOP();
             int getLSLR();
             int getRSUD();
             int calcTime();
             

      private:
             usb_serial_class *_usb;
             HardwareSerial *_web;
             bool EXTRAS;
             static void throttleCB(); 
             static void steeringCB();
             static void eStopCB();
             static void modeCB();
             static void LS_LR_CB();
             static void RS_UD_CB();
      };
      
#endif
      
