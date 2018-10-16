#ifndef _kingrichard_h
#define _kingrichard_h
#include <Arduino.h>
#include <i2c_t3.h>
#include <HMC5883L.h>
#include <RCcontrol.h>
#include <KRmotors.h>
#include <UBLOX.h>
#include <MPU6050.h>
#include <stdlib.h>
using namespace std;

#define waitTimeMillis 100 //5Hz refresh rate

class KingRichard{
public:
    KingRichard(usb_serial_class *usb, HardwareSerial *web, HardwareSerial *gps, i2c_t3 *i2c);
    void autonomous();
    void PID();
    void manualRC();
    uint8_t mode();

private:
    void getWeb();
    void parseWeb(String webInput);
    void sendWeb();
    void getSensors();
    void waypoint();
    usb_serial_class *_usb;
    HardwareSerial *_web;
    HardwareSerial *_gps;
    i2c_t3* _i2c;
    Vector _gyro;
    Vector _accel;
    float _tempC;
    KRmotors* _motors;
    String _inputString = "";
    bool _INCOMING = false;
    bool _MISSION_STARTED = false;
    bool _MISSION_PAUSED = false;
    bool _GO_HOME = false;
    bool _TARGET_REACHED = false;
    int _TARGET_INDEX = 0;
    double hLAT = 0.0f;
    double hLON = 0.0f;
    double tLAT = 0.0f;
    double tLON = 0.0f;
    double decAngle = 0.0f;
    float targetDistance = 0.0f;
    float thresholdDistance = 1.0f;
    float targetHeading = 0.0f;
    unsigned long _wait001 = 0;
    RCcontrol* _RC;
    HMC5883L _compass;
    UBLOX* _GPS;
    MPU6050 _mpu;
};

#endif
