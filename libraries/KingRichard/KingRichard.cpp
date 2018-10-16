#include <KingRichard.h>

KingRichard::KingRichard(usb_serial_class *usb, HardwareSerial *web, HardwareSerial *gps, i2c_t3 *i2c)
{
    _usb = usb;
    _web = web;
    _gps = gps;
    _i2c = i2c;
    _RC = new RCcontrol(_usb, _web);
    //_usb->println("RC control initialized");
    _motors = new KRmotors(_usb, _web);
    //_usb->println("Motor control initialized");
    _GPS = new UBLOX(_usb, _gps);
    _compass.begin(_usb, _i2c, _web);
    _compass.declinationAngle=_motors->DEC_ANGLE;
    _mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G);
    _mpu.calibrateGyro();

    delay(500);
    _usb->println("compass initialized");
    _motors->initHeading(_compass.getHeading());
    //yield();
}

void KingRichard::sendWeb()
{
  //_usb->println("Building health message...");
  _web->print("$CO:"+String(_compass.getHeading(),5)+"!\r\n");
  //_usb->print("$CO:"+String(_compass.getHeading(),5)+"!\r\n");
  _web->print("$CT:"+String(_motors->prev_STEERING_PULSE)+","+String(_motors->prev_THROTTLE)+"!\r\n");
  _web->print("$MO:"+String(_RC->getMode())+"!\r\n");

  if(_RC->getMode() == 1)
  {
  _web->print("$TH:"+String(_motors->desired_HEADING,5)+"!\r\n");
  }
  _web->print("$AC:"+String(_accel.XAxis,7)+","+
                       String(_accel.YAxis,7)+","+
                       String(_accel.ZAxis,7)+"!\r\n");
  _web->print("$GY:"+String(_gyro.XAxis,7)+","+
                       String(_gyro.YAxis,7)+","+
                       String(_gyro.ZAxis,7)+"!\r\n");
  _web->print("$TF:"+String(_tempC*9/5+32,2)+"!\r\n");
  _web->print("$TC:"+String(_tempC)+"!\r\n");
  if(_GPS->GPS)
  {
    char charLAT[20], charLON[20];
    String lat = "", lon = "";
    dtostrf(_GPS->LAT, 16, 12, charLAT);
      lat = charLAT;
    dtostrf(_GPS->LON, 16, 12, charLON);
      lon = charLON;
    _web->print("$GP:"+lat+","+lon+"!\r\n");
    //_usb->print("$GP:"+lat+","+lon+"!\r\n");
    _GPS->GPS=false;
  }
  if(_MISSION_STARTED && _RC->getMode()==2)
  {
    _web->printf("$TH:%f!\r\n",targetHeading);
    _web->printf("$TD:%f!\r\n",targetDistance);
    //_usb->printf("$TH:%f!\r\n",targetHeading);
    //_usb->printf("$TD:%f!\r\n",targetDistance);
  }
}

void KingRichard::getWeb()
{
    while(_web->available())
    {
      char inChar=_web->read();
      if(!_INCOMING && inChar=='$'){ _INCOMING=true; _inputString=""; }
      else if(_INCOMING && inChar!='\n') _inputString+=inChar;
      else
      {
        _INCOMING=false;
        _inputString+=inChar;
        if(_motors->setParams(_inputString)) return;
        else parseWeb(_inputString);
      }
    }
    return;
}

void KingRichard::parseWeb(String webInput)
{
    int separator=webInput.indexOf(':');
    if(separator > -1)
    {
        String var=webInput.substring(0,separator);
        String val=webInput.substring(separator+1);
             if(var == "Td")
             {
                thresholdDistance = val.toFloat();
                _usb->printf("Threshold distance: %f\r\n",thresholdDistance);
             }
        else if(var == "DA")
            {
                _compass.declinationAngle = val.toFloat();
                _usb->printf("Declination angle: %f\r\n",val.toFloat());
            }
        else if(var == "Ho")
             {
                int sep = val.indexOf(',');
                hLAT = val.substring(0,sep).toFloat();
                hLON = val.substring(sep+1).toFloat();
                _usb->printf("Home Coordinates: %f, %f\r\n",hLAT,hLON);
             }
        else if(var == "Gt")
             {
                int sep = val.indexOf(',');
                double prevTlat = tLAT;
                double prevTlon = tLON;
                tLAT = val.substring(0,sep).toFloat();
                tLON = val.substring(sep+1).toFloat();
                if(tLAT == prevTlat && tLON == prevTlon) return;
                _TARGET_INDEX ++;
                _usb->printf("Target Coordinates: %f, %f\r\n",tLAT,tLON);
             }
    }
    else
    {
        String req=webInput.substring(0,2);
        if(req == "RH") _GO_HOME=true;
        else if(req == "SM")
        {
            _MISSION_STARTED = true;
            _MISSION_PAUSED = false;
        }
        else if(req == "PM")
        {
            _MISSION_PAUSED = !_MISSION_PAUSED;
        }
        else if(req == "sM")
        {
            _MISSION_STARTED = false;
            _MISSION_PAUSED = false;
            _GO_HOME = false;
            _TARGET_INDEX = 0;
        }
    }
    return;
}

void KingRichard::getSensors()
{
    _GPS->readGPS();
    _gyro = _mpu.readNormalizeGyro();
    _accel = _mpu.readNormalizeAccel();
    _tempC = _mpu.readTemperature();
}

void KingRichard::manualRC()
{
    getWeb();
    getSensors();
    _motors->RCmix(_RC->getThrottle(),_RC->getSteering());
    if(millis()>_wait001)
    {
    _wait001=millis()+waitTimeMillis;
     sendWeb();
    }
}

void KingRichard::PID()
{
    getWeb();
    getSensors();
    _motors->AutonRCMix(_RC->getThrottle(),_RC->getSteering(),_compass.getHeading());
    if(millis()>_wait001)
    {
    _wait001=millis()+waitTimeMillis;
     sendWeb();
    }
}

void KingRichard::autonomous()
{
    getWeb();
    getSensors();
    if(_MISSION_STARTED && !_MISSION_PAUSED)
    {
        //step 1: Calculate desired heading and distance
        waypoint();
        if(targetDistance < thresholdDistance)
        {
            _web->printf("$TR:%d!",_TARGET_INDEX);
            _motors->AutonMix(targetHeading,0,_compass.getHeading());
        }
        //step 2: Calculate desired throttle and steering
        else
        {
            _motors->AutonMix(targetHeading, _RC->getThrottle(), _compass.getHeading());
        }
    }
    else _motors->AutonMix(targetHeading,0,_compass.getHeading());
    if(millis()>_wait001)
    {
    _wait001=millis()+waitTimeMillis;
     sendWeb();
    }
}

void KingRichard::waypoint()
{
    double deltaLAT = 0.0f;
    double deltaLON = 0.0f;
    if(!_GO_HOME)
    {
        deltaLAT = tLAT-_GPS->LAT;
        deltaLON = tLON-_GPS->LON;
    }
    else
    {
        deltaLAT = hLAT-_GPS->LAT;
        deltaLON = hLON-_GPS->LON;
    }
    double distNS = deltaLAT*111320.0f;
    double distEW = (deltaLON*111320.0f)*cosf(radians((tLAT+_GPS->LAT)/2));
    targetDistance = sqrt(distNS*distNS + distEW*distEW);
    targetHeading = degrees(atan2(distNS, distEW));
}

uint8_t KingRichard::mode()
{
    return _RC->getMode();
}
