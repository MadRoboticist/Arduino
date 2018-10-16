#include <KRmotors.h>

//KRmotors::KRmotors(usb_serial_class *usb, Adafruit_PWMServoDriver *pwm)
KRmotors::KRmotors(usb_serial_class *usb, HardwareSerial *web)
{  //_pwm = pwm;
   _usb = usb;
   _web = web;
   readEEPROM();
   getConfig();
   //_usb->println("setting PWM frequency to 50Hz");
   //pwm = new Adafruit_PWMServoDriver();
   _pwm.begin();
   _pwm.setPWMFreq(50);
   yield();
   _pwm.setPWM(THROTTLE,0,THROTTLE_CENTER);
   prev_THROTTLE = THROTTLE_CENTER;
   _pwm.setPWM(STEERING,0,STEERING_CENTER);
   prev_STEERING = STEERING_RC_CENTER;
   prev_STEERING_PULSE = STEERING_CENTER;
   prev_ERR_STEERING = 0;
   prev_ERR_THROTTLE = 0;
   I_HEADING = 0;
   I_THROTTLE = 0;
   
   //delay(2000);
}

void KRmotors::EStop()
{
     _pwm.setPWM(THROTTLE,0,THROTTLE_CENTER); 
     prev_THROTTLE=THROTTLE_CENTER;
}

void KRmotors::RCmix(int throttle, int steering)
{
     //SET STEERING
     int steering_pulse = map(steering,1000,1740,STEERING_MIN,STEERING_MAX);
     _pwm.setPWM(STEERING,0,steering_pulse);
     prev_STEERING = steering;
     prev_STEERING_PULSE=steering_pulse;
     //THROTTLE STATE MACHINE
     int throttle_pulse = 280;
     //STATE: MOVING FORWARD 
     if(throttle>1520)
     {
         throttle_pulse = map(throttle,1520,1860,288,THROTTLE_MAX_RC);
         if(throttle_pulse<289 && prev_THROTTLE<288)
         {
             throttle_pulse = 289;
             prev_THROTTLE = 289;                      
         }            
     }
     //STATE: MOVING REVERSE
     else if(throttle<1440)
     {
          throttle_pulse = map(throttle,1050,1440,THROTTLE_MIN_RC,273);
          if(throttle_pulse>270 && prev_THROTTLE>273)
          {
              throttle_pulse = 270;
              prev_THROTTLE = 270;                      
          }
     }
     //STATE: STOPPED
     else
     {
         prev_THROTTLE = throttle_pulse;
     }
     _pwm.setPWM(THROTTLE,0,throttle_pulse);
     prev_THROTTLE = throttle_pulse;
     //_web->printf("$CT:%d,%d\n\n\n",steering_pulse,throttle_pulse);
     //_web->print("$CT:"+(String)steering_pulse+","+(String)throttle_pulse+"!\r\n");
     //_web->flush();
     //_usb->printf("$CT:%d,%d\n",steering_pulse,throttle_pulse);
}

void KRmotors::AutonRCMix(int throttle, int steering, double heading)
{
    
    
     if(steering<1400) 
     {
          desired_HEADING=heading+map(steering,1000,1400,-30.0f,0.0f);
     }
     else if(steering>1420)
     {
          desired_HEADING=heading+map(steering,1420,1740,0.0f,30.0f);
     }
     else if(prev_STEERING<1400 || prev_STEERING>1420) 
     {
          desired_HEADING = heading;
          I_HEADING = 0;
          prev_ERR_STEERING = 0;
     }
     //_usb->printf("desired heading: %f\n",desired_HEADING);
     float err = desired_HEADING-heading;
     if(err>180) err -= 360;
     else if(err < -180) err += 360;
     //_usb->printf("heading error: %f\n",err);
     I_HEADING += err;
     //_usb->printf("integrated heading error: %f\n",I_HEADING);
     float d_err = err - prev_ERR_STEERING;
     //_usb->printf("derivative heading error: %f\n",d_err);
     prev_ERR_STEERING = err;
     float PID_STEERING = PCOEFF_STEERING*err \
                        + ICOEFF_STEERING*I_HEADING \
                        + DCOEFF_STEERING*d_err;
     int steering_pulse = STEERING_CENTER + PID_STEERING;
     //_usb->printf("steering pulse: %d\n\n\n\n",steering_pulse);
     if(steering_pulse>STEERING_MAX) steering_pulse = STEERING_MAX;
     else if(steering_pulse<STEERING_MIN) steering_pulse = STEERING_MIN;
     
     prev_STEERING = steering;
     prev_STEERING_PULSE = steering_pulse;
     prev_HEADING = heading;
    
    
     //STEERING CONTROL
     float THROTTLE_RATIO = map(steering_pulse,STEERING_MIN,STEERING_MAX,-ST_TH_RATIO,ST_TH_RATIO);
     float THROTTLE_COEFF = 0;
     if(THROTTLE_RATIO > 0) THROTTLE_COEFF = 1/(1+THROTTLE_RATIO);
     else if(THROTTLE_RATIO < 0) THROTTLE_COEFF = 1/(THROTTLE_RATIO-1);
     else THROTTLE_COEFF = 1;
     //_usb->printf("RC steering pulse: %d\n",steering);
     //_usb->printf("current heading: %f\n",heading);
     //THROTTLE STATE MACHINE
     int throttle_pulse = 280;
     //STATE: MOVING FORWARD 
     //_usb->printf("RC throttle pulse: %d\n", throttle);
     if(throttle>1520)
     {
         throttle_pulse = map(throttle,1520,1860,288,288+abs(THROTTLE_COEFF)*(THROTTLE_MAX_RC-288));
         if(prev_THROTTLE<288)
         {
             throttle_pulse = 289;
             prev_THROTTLE = 289;                      
         }
         else
         {
             float err = throttle_pulse - prev_THROTTLE;
             float d_err = err - prev_ERR_THROTTLE;
             I_THROTTLE += err;
             prev_ERR_THROTTLE = err;
             throttle_pulse = prev_THROTTLE\
                            + PCOEFF_THROTTLE*err \
                            + ICOEFF_THROTTLE*I_THROTTLE \
                            + DCOEFF_THROTTLE*d_err;
             prev_THROTTLE = throttle_pulse;
             //_usb->printf("throttle error: %f\n",err);
             //_usb->printf("derivative throttle error: %f\n",d_err);
             //_usb->printf("throttle pulse: %d\n", throttle_pulse);    
         }
                     
     }
     //STATE: MOVING REVERSE
     else if(throttle<1440)
     {
          throttle_pulse = map(throttle,1050,1440,273-abs(THROTTLE_COEFF)*(273-THROTTLE_MIN_RC),273);
          if(prev_THROTTLE>273)
          {
              throttle_pulse = 270;
              prev_THROTTLE = 270;                      
          }
          else
          {
              float err = throttle_pulse - prev_THROTTLE;
             float d_err = err - prev_ERR_THROTTLE;
             prev_ERR_THROTTLE = err;
             I_THROTTLE += err;
             throttle_pulse = prev_THROTTLE\
                            + PCOEFF_THROTTLE*err\
                            + ICOEFF_THROTTLE*I_THROTTLE\
                            + DCOEFF_THROTTLE*d_err;
             prev_THROTTLE = throttle_pulse;
             //_usb->printf("throttle error: %f\n",err);
             //_usb->printf("derivative throttle error: %f\n",d_err);
             //_usb->printf("throttle pulse: %d\n", throttle_pulse);
          }
     }
     //STATE: STOPPED
     else
     {
         prev_THROTTLE = throttle_pulse;
         prev_ERR_THROTTLE = 0;
         I_THROTTLE = 0;
         desired_HEADING = heading;
     }
     
     
     _pwm.setPWM(STEERING,0,steering_pulse);
     _pwm.setPWM(THROTTLE,0,throttle_pulse);
     //_web->print("$CT:"+(String)steering_pulse+","+(String)throttle_pulse+"!\r\n");
     //_web->flush();
     //_web->print("$CO:"+String(heading,10)+"!\r\n");
     return;
}

void KRmotors::AutonMix(float tHeading, int throttle, double heading)
{
     //_usb->printf("desired heading: %f\n",desired_HEADING);
     float err = tHeading-heading;
     if(err>180) err -= 360;
     else if(err < -180) err += 360;
     //_usb->printf("heading error: %f\n",err);
     I_HEADING += err;
     //_usb->printf("integrated heading error: %f\n",I_HEADING);
     float d_err = err - prev_ERR_STEERING;
     //_usb->printf("derivative heading error: %f\n",d_err);
     prev_ERR_STEERING = err;
     float PID_STEERING = PCOEFF_STEERING*err \
                        + ICOEFF_STEERING*I_HEADING \
                        + DCOEFF_STEERING*d_err;
     int steering_pulse = STEERING_CENTER + PID_STEERING;
     //_usb->printf("steering pulse: %d\n\n\n\n",steering_pulse);
     if(steering_pulse>STEERING_MAX) steering_pulse = STEERING_MAX;
     else if(steering_pulse<STEERING_MIN) steering_pulse = STEERING_MIN;

     //prev_STEERING = steering;
     //prev_STEERING_PULSE = steering_pulse;
     prev_HEADING = heading;


     //STEERING CONTROL
     float THROTTLE_RATIO = map(steering_pulse,STEERING_MIN,STEERING_MAX,-ST_TH_RATIO,ST_TH_RATIO);
     float THROTTLE_COEFF = 0;
     if(THROTTLE_RATIO > 0) THROTTLE_COEFF = 1/(1+THROTTLE_RATIO);
     else if(THROTTLE_RATIO < 0) THROTTLE_COEFF = 1/(THROTTLE_RATIO-1);
     else THROTTLE_COEFF = 1;
     //_usb->printf("RC steering pulse: %d\n",steering);
     //_usb->printf("current heading: %f\n",heading);
     //THROTTLE STATE MACHINE
     int throttle_pulse = 280;
     //STATE: MOVING FORWARD
     //_usb->printf("RC throttle pulse: %d\n", throttle);
     if(throttle>1520)
     {
         throttle_pulse = map(throttle,1520,1860,288,288+abs(THROTTLE_COEFF)*(THROTTLE_MAX_RC-288));
         if(prev_THROTTLE<288)
         {
             throttle_pulse = 289;
             prev_THROTTLE = 289;
         }
         else
         {
             float err = throttle_pulse - prev_THROTTLE;
             float d_err = err - prev_ERR_THROTTLE;
             I_THROTTLE += err;
             prev_ERR_THROTTLE = err;
             throttle_pulse = prev_THROTTLE\
                            + PCOEFF_THROTTLE*err \
                            + ICOEFF_THROTTLE*I_THROTTLE \
                            + DCOEFF_THROTTLE*d_err;
             prev_THROTTLE = throttle_pulse;
             //_usb->printf("throttle error: %f\n",err);
             //_usb->printf("derivative throttle error: %f\n",d_err);
             //_usb->printf("throttle pulse: %d\n", throttle_pulse);
         }

     }
     //STATE: MOVING REVERSE
     else if(throttle<1440)
     {
          throttle_pulse = map(throttle,1050,1440,273-abs(THROTTLE_COEFF)*(273-THROTTLE_MIN_RC),273);
          if(prev_THROTTLE>273)
          {
              throttle_pulse = 270;
              prev_THROTTLE = 270;
          }
          else
          {
              float err = throttle_pulse - prev_THROTTLE;
             float d_err = err - prev_ERR_THROTTLE;
             prev_ERR_THROTTLE = err;
             I_THROTTLE += err;
             throttle_pulse = prev_THROTTLE\
                            + PCOEFF_THROTTLE*err\
                            + ICOEFF_THROTTLE*I_THROTTLE\
                            + DCOEFF_THROTTLE*d_err;
             prev_THROTTLE = throttle_pulse;
             //_usb->printf("throttle error: %f\n",err);
             //_usb->printf("derivative throttle error: %f\n",d_err);
             //_usb->printf("throttle pulse: %d\n", throttle_pulse);
          }
     }
     //STATE: STOPPED
     else
     {
         prev_THROTTLE = throttle_pulse;
         prev_ERR_THROTTLE = 0;
         I_THROTTLE = 0;
         desired_HEADING = heading;
     }


     _pwm.setPWM(STEERING,0,steering_pulse);
     _pwm.setPWM(THROTTLE,0,throttle_pulse);
     //_web->print("$CT:"+(String)steering_pulse+","+(String)throttle_pulse+"!\r\n");
     //_web->flush();
     //_web->print("$CO:"+String(heading,10)+"!\r\n");
     return;
}

void KRmotors::initHeading(float heading)
{
     prev_HEADING=heading;
     desired_HEADING=heading;
     I_HEADING=0;
     return;     
}

void KRmotors::readEEPROM()
{
     _usb->println("Reading from EEPROM..");
     EEPROM.get(0,INIT_DAT);
     STEERING_MAX=INIT_DAT.STEERING_MAX;// 215
     STEERING_MIN=INIT_DAT.STEERING_MIN;// 105
     STEERING_CENTER=INIT_DAT.STEERING_CENTER;// 160
     THROTTLE_MAX_RC=INIT_DAT.THROTTLE_MAX_RC;// 330
     THROTTLE_MIN_RC=INIT_DAT.THROTTLE_MIN_RC;// 261
        //PID COEFFICIENTS
     PCOEFF_STEERING=INIT_DAT.PCOEFF_STEERING;// 1.0f
     DCOEFF_STEERING=INIT_DAT.DCOEFF_STEERING;// 1.5f
     ICOEFF_STEERING=INIT_DAT.ICOEFF_STEERING;// 0.0f
     PCOEFF_THROTTLE=INIT_DAT.PCOEFF_THROTTLE;// 0.1f
     DCOEFF_THROTTLE=INIT_DAT.DCOEFF_THROTTLE;// 0.5f
     ICOEFF_THROTTLE=INIT_DAT.ICOEFF_THROTTLE;// 0.00001f
     ST_TH_RATIO=INIT_DAT.ST_TH_RATIO;// 8.0f 
     DEC_ANGLE=INIT_DAT.DEC_ANGLE;
 /*    _usb->printf("STEERING_MAX: %d, STEERING_MIN: %d, STEERING_CENTER: %d\n",
                                 STEERING_MAX,STEERING_MIN,STEERING_CENTER);
     _usb->printf("THROTTLE_MAX_RC: %d, THROTTLE_MIN_RC: %d\n",
                                 THROTTLE_MAX_RC,THROTTLE_MIN_RC);
     _usb->printf("STEERING P: %f, I: %f, D: %f\n",
                            PCOEFF_STEERING,ICOEFF_STEERING,DCOEFF_STEERING);
     _usb->printf("THROTTLE P: %f, I: %f, D: %f\n",
                            PCOEFF_THROTTLE,ICOEFF_THROTTLE,DCOEFF_THROTTLE);
     _usb->printf("STEERING to THROTTLE ratio: %f\n", ST_TH_RATIO);*/
}

void KRmotors::saveEEPROM()
{
     _usb->println("Saving config data to EEPROM..");
     INIT_DAT.STEERING_MAX=STEERING_MAX;// 215
     INIT_DAT.STEERING_MIN=STEERING_MIN;// 105
     INIT_DAT.STEERING_CENTER=STEERING_CENTER;// 160
     INIT_DAT.THROTTLE_MAX_RC=THROTTLE_MAX_RC;// 330
     INIT_DAT.THROTTLE_MIN_RC=THROTTLE_MIN_RC;// 261
        //PID COEFFICIENTS
     INIT_DAT.PCOEFF_STEERING=PCOEFF_STEERING;// 1.0f
     INIT_DAT.DCOEFF_STEERING=DCOEFF_STEERING;// 1.5f
     INIT_DAT.ICOEFF_STEERING=ICOEFF_STEERING;// 0.0f
     INIT_DAT.PCOEFF_THROTTLE=PCOEFF_THROTTLE;// 0.1f
     INIT_DAT.DCOEFF_THROTTLE=DCOEFF_THROTTLE;// 0.5f
     INIT_DAT.ICOEFF_THROTTLE=ICOEFF_THROTTLE;// 0.00001f
     INIT_DAT.ST_TH_RATIO=ST_TH_RATIO;// 8.0f
     INIT_DAT.DEC_ANGLE=DEC_ANGLE;
     EEPROM.put(0,INIT_DAT);
     return;
}

bool KRmotors::setParams(String webInput)
{
    _usb->print("web input: ");
    _usb->println(webInput);
    //_usb->print(" sizeof input: ");
    //_usb->println(sizeof(webInput));
    int separator=webInput.indexOf(':');
    //_usb->printf(" Separator: %d\n",separator);
    if(separator > -1)
    {   //_usb->println("Pass separator test");
        String var=webInput.substring(0,separator);
        String val=webInput.substring(separator+1);
            //Serial.println("Message: "+var+" Value: "+val);
             if(var=="PS"){PCOEFF_STEERING=val.toFloat(); _usb->println("Psteer set");}
        else if(var=="IS"){ICOEFF_STEERING=val.toFloat(); _usb->println("Isteer set");}
        else if(var=="DS"){DCOEFF_STEERING=val.toFloat(); _usb->println("Dsteer set");}
        else if(var=="PT"){PCOEFF_THROTTLE=val.toFloat(); _usb->println("Pthrot set");}
        else if(var=="IT"){ICOEFF_THROTTLE=val.toFloat(); _usb->println("Ithrot set to "+val); _usb->println(ICOEFF_THROTTLE,5);}
        else if(var=="DT"){DCOEFF_THROTTLE=val.toFloat(); _usb->println("Dthrot set");}
        else if(var=="mS"){STEERING_MIN=val.toInt(); _usb->println("Smin set");}
        else if(var=="MS"){STEERING_MAX=val.toInt(); _usb->println("Smax set");}
        else if(var=="CS"){STEERING_CENTER=val.toInt(); _usb->println("Csteer set");}
        else if(var=="mT"){THROTTLE_MIN_RC=map(val.toInt(),0,100,THROTTLE_CENTER,THROTTLE_MIN);
                           _usb->printf("New THROTTLE_MIN_RC: %d\n",THROTTLE_MIN_RC);}
        else if(var=="MT"){THROTTLE_MAX_RC=map(val.toInt(),0,100,THROTTLE_CENTER,THROTTLE_MAX);
                           _usb->printf("New THROTTLE_MAX_RC: %d\n",THROTTLE_MAX_RC);}
        else if(var=="ST"){ST_TH_RATIO=val.toFloat(); _usb->println("ratio set");}
        else if(var=="DA"){DEC_ANGLE=val.toFloat(); _usb->println("declination angle set"); return false;}
        else return false;
    }
    else
    {  String req=webInput.substring(0,2);
            if(req=="SA") {_usb->println("attempting to save.."); saveEEPROM();}
       else if(req=="RE") {_usb->println("attempting to read.."); readEEPROM();}
       else if(req=="CF") {_usb->println("getting configuration.."); getConfig();}
       else return false;
    }
    return true;
}

void KRmotors::getConfig()
{
     _usb->println("sending config info to webpage...");
     _web->print("$PS:"+String(PCOEFF_STEERING,2)+"!\r\n");
     //_web->flush();
     _web->print("$IS:");_web->print(ICOEFF_STEERING,5);_web->print("!\r\n");
     //_web->flush();
     _web->print("$DS:"+String(DCOEFF_STEERING,2)+"!\r\n");
     //_web->flush();
     _web->print("$PT:"+String(PCOEFF_THROTTLE,2)+"!\r\n");
     //_web->flush();
     _web->print("$IT:");_web->print(ICOEFF_THROTTLE,5);_web->print("!\r\n");
     //_web->flush();
     _web->print("$DT:"+String(DCOEFF_THROTTLE,2)+"!\r\n");
     //_web->flush();
     _web->print("$mS:"+(String)STEERING_MIN+"!\r\n");
     //_web->flush();
     _web->print("$MS:"+(String)STEERING_MAX+"!\r\n");
     //_web->flush();
     _web->print("$CS:"+(String)STEERING_CENTER+"!\r\n");
     //_web->flush();
     _web->print("$mT:"+(String)map(THROTTLE_MIN_RC,THROTTLE_MIN,THROTTLE_CENTER,100,0)+"!\r\n");
     //_web->flush();
     _web->print("$MT:"+(String)map(THROTTLE_MAX_RC,THROTTLE_CENTER,THROTTLE_MAX,0,100)+"!\r\n");
     //_web->flush();
     _web->print("$ST:"+(String)ST_TH_RATIO+"!\r\n");
     //_web->flush();
     _usb->println("$PS:"+String(PCOEFF_STEERING,2));
     _usb->print("$IS:");_usb->println(ICOEFF_STEERING,5);
     _usb->println("$DS:"+String(DCOEFF_STEERING,2));
     _usb->println("$PT:"+String(PCOEFF_THROTTLE,2));
     _usb->print("$IT:");_usb->println(ICOEFF_THROTTLE,5);
     _usb->println("$DT:"+String(DCOEFF_THROTTLE,2));
     _usb->println("$mS:"+(String)STEERING_MIN);
     _usb->println("$MS:"+(String)STEERING_MAX);
     _usb->println("$CS:"+(String)STEERING_CENTER);
     _usb->println("$mT:"+(String)map(THROTTLE_MIN_RC,THROTTLE_CENTER,THROTTLE_MIN,0,100));
     _usb->println("$MT:"+(String)map(THROTTLE_MAX_RC,THROTTLE_CENTER,THROTTLE_MAX,0,100));
     _usb->println("$ST:"+(String)ST_TH_RATIO);
     _usb->println("$DA:"+(String)DEC_ANGLE);
}
