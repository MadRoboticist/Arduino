#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <stdlib.h>
#include <FS.h>
#include <list>

using namespace std;

#define LED 0 //LED pin
#define websockMillis 100 //10Hz refresh rate

const char *ssid = "KingRichard";
const char *password = "iLoveRobots";

ESP8266WebServer server(80);

/* Go to http://192.168.4.1 in a web browser*/
// CONFIG values *********************************************************
float pSteer = 1.0; bool PSTEER = false;
float iSteer = 0.0; bool ISTEER = false;
float dSteer = 1.5; bool DSTEER = false;
float pThrottle = 0.1; bool PTHROTTLE = false;
float iThrottle = 0.00001; bool ITHROTTLE = false;
float dThrottle = 0.5; bool DTHROTTLE = false;
int minSteer = 105; bool MINSTEER = false;
int maxSteer = 215; bool MAXSTEER = false;
int midSteer = 160; bool MIDSTEER = false;
int maxReverse = 20; bool MAXREVERSE = false;
int maxForward = 50; bool MAXFORWARD = false;
float ratio = 8.0; bool RATIO = false;
float threshold = 1;

// UPDATE values ***********************************************************************
float LAT = 0.0f, LON = 0.0f; bool GPS = false;
String tLAT = "", tLON = ""; bool tGPS = false;
String hLAT = "", hLON = "";
String decAngle = ""; bool DECANGLE = false;
int tIndex = 0; float tDistance = 0;
bool TINDEX = false, TDIST = false;
bool mSTART = false; bool mPAUSE = false;
bool mRETURN = false;
float heading = 0.0f; bool COMPASS = false;
float theading = 0.0f; bool tCOMPASS = false;
float accX = 0.0f, accY = 0.0f, accZ = 0.0f; bool ACC = false;
float gyroX = 0.0f, gyroY = 0.0f, gyroZ = 0.0f; bool GYRO = false;
int steeringVAL = 0.0f, throttleVAL = 0.0f; bool CTRL = false;
String Mode = "MANUAL"; bool MODE = false;
float tempC = 25.0, tempF = 75; bool TEMPC = false, TEMPF = false;

String GPSlist;
int GPSlength = 0;
bool LIST = false;

WebSocketsServer webSocket = WebSocketsServer(88);
String inputString;
unsigned long wait001 = 0UL;
bool INCOMING = false;

void handleWebsite() {
  if (SPIFFS.begin())
  {
    if (SPIFFS.exists("/index.html"))
    {
      File KRweb = SPIFFS.open("/index.html", "r");
      if (!KRweb) return;
      else
      {
        server.send(200, "text/html", KRweb.readString());
        KRweb.close();
      }
    }
  }
}

String millis2time() {
  String Time = "";
  unsigned long ss;
  byte mm, hh;
  ss = millis() / 1000;
  hh = ss / 3600;
  mm = (ss - hh * 3600) / 60;
  ss = (ss - hh * 3600) - mm * 60;
  if (hh < 10)Time += "0";
  Time += (String)hh + ":";
  if (mm < 10)Time += "0";
  Time += (String)mm + ":";
  if (ss < 10)Time += "0";
  Time += (String)ss;
  return Time;
}

void sendJSON()
{
  digitalWrite(LED, HIGH);
  bool EXTRA = false;
  String JSONtxt = "{\"runtime\":\"" + millis2time() + "\"";
  if (PSTEER) {
    JSONtxt += ",\"pSteer\":\"" + (String)pSteer + "\"";
    PSTEER = false;
    EXTRA = true;
  }
  if (ISTEER) {
    JSONtxt += ",\"iSteer\":\"" + String(iSteer, 5) + "\"";
    ISTEER = false;
    EXTRA = true;
  }
  if (DSTEER) {
    JSONtxt += ",\"dSteer\":\"" + (String)dSteer + "\"";
    DSTEER = false;
    EXTRA = true;
  }
  if (PTHROTTLE) {
    JSONtxt += ",\"pThrottle\":\"" + (String)pThrottle + "\"";
    PTHROTTLE = false;
    EXTRA = true;
  }
  if (ITHROTTLE) {
    JSONtxt += ",\"iThrottle\":\"" + String(iThrottle, 5) + "\"";
    ITHROTTLE = false;
    EXTRA = true;
  }
  if (DTHROTTLE) {
    JSONtxt += ",\"dThrottle\":\"" + (String)dThrottle + "\"";
    DTHROTTLE = false;
    EXTRA = true;
  }
  if (MINSTEER) {
    JSONtxt += ",\"minSteer\":\"" + (String)minSteer + "\"";
    MINSTEER = false;
    EXTRA = true;
  }
  if (MAXSTEER) {
    JSONtxt += ",\"maxSteer\":\"" + (String)maxSteer + "\"";
    MAXSTEER = false;
    EXTRA = true;
  }
  if (MIDSTEER) {
    JSONtxt += ",\"midSteer\":\"" + (String)midSteer + "\"";
    MIDSTEER = false;
    EXTRA = true;
  }
  if (MAXREVERSE) {
    JSONtxt += ",\"maxReverse\":\"" + (String)maxReverse + "\"";
    MAXREVERSE = false;
    EXTRA = true;
  }
  if (MAXFORWARD) {
    JSONtxt += ",\"maxForward\":\"" + (String)maxForward + "\"";
    MAXFORWARD = false;
    EXTRA = true;
  }
  if (RATIO) {
    JSONtxt += ",\"ratio\":\"" + String(ratio, 2) + "\"";
    RATIO = false;
    EXTRA = true;
  }
  if (TEMPF) {
    JSONtxt += ",\"tempF\":\"" + String(tempF, 2) + "\"";
    TEMPF = false;
    EXTRA = true;
  }
  if (TEMPC) {
    JSONtxt += ",\"tempC\":\"" + String(tempC, 2) + "\"";
    TEMPC = false;
    EXTRA = true;
  }
  if (GPS) {
    char charLAT[20], charLON[20];
    String lat = "", lon = "";
    dtostrf(LAT, 16, 10, charLAT);
    lat = charLAT;
    dtostrf(LON, 16, 10, charLON);
    lon = charLON;
    JSONtxt += ",\"lat\":\"" + lat + "\"";
    JSONtxt += ",\"lon\":\"" + lon + "\"";
    GPS = false; EXTRA = true;
  }
  if (tGPS) {
    /*char chartLAT[20], chartLON[20];
    String tlat = "", tlon = "";
    dtostrf(tLAT, 16, 10, chartLAT);
    tlat = chartLAT;
    dtostrf(tLON, 16, 10, chartLON);
    tlon = chartLON;*/
    JSONtxt += ",\"tlat\":\"" + tLAT + "\"";
    JSONtxt += ",\"tlon\":\"" + tLON + "\"";
    tGPS = false; EXTRA = true;
  }
  if (DECANGLE)
  {
    JSONtxt+= ",\"decAngle\":\"" + decAngle + "\"";
    DECANGLE = false; EXTRA = true;
  }
  if (TINDEX)
  {
    JSONtxt+= ",\"tindex\":\"" + String(tIndex) + "\"";
    TINDEX = false; EXTRA = true;
  } 
  if (TDIST)
  {
    JSONtxt+= ",\"tdistance\":\"" + String(tDistance) + "\"";
    TDIST = false; EXTRA = true;
  }
  if (COMPASS) {
    JSONtxt += ",\"heading\":\"" + String(heading,1) + "\""; 
    COMPASS = false; EXTRA = true;
  }
  if (tCOMPASS) {
    JSONtxt += ",\"theading\":\"" + String(theading,1) + "\""; 
    tCOMPASS = false; EXTRA = true;
  }
  if (ACC) {
    JSONtxt += ",\"accX\":\"" + String(accX, 6) + "\"";
    JSONtxt += ",\"accY\":\"" + String(accY, 6) + "\"";
    JSONtxt += ",\"accZ\":\"" + String(accZ, 6) + "\"";
    ACC = false; EXTRA = true;
  }
  if (GYRO) {
    JSONtxt += ",\"gyroX\":\"" + String(gyroX, 6) + "\"";
    JSONtxt += ",\"gyroY\":\"" + String(gyroY, 6) + "\"";
    JSONtxt += ",\"gyroZ\":\"" + String(gyroZ, 6) + "\"";
    GYRO = false; EXTRA = true;
  }
  if (CTRL) {
    JSONtxt += ",\"steeringVAL\":\"" + String(steeringVAL) + "\"";
    JSONtxt += ",\"throttleVAL\":\"" + String(throttleVAL) + "\"";
    CTRL = false; EXTRA = true;
  }
  if (MODE) {
    JSONtxt += ",\"mode\":\"" + Mode + "\"";
    MODE = false;
    EXTRA = true;
  }
  JSONtxt += "}";
  //if(EXTRA) Serial.println(JSONtxt);
  webSocket.broadcastTXT(JSONtxt);
  digitalWrite(LED, LOW);
  return;
}

void nextWaypoint(int index)
{
    tIndex = index;
    TINDEX=true;
    if(tIndex < GPSlength)
    {    
        sendGPS();
    }
    else
    {
        mSTART=false;
        mPAUSE=false;
        Serial.println("$sM");   
    }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t wslength) {
  String payloadString = (const char *)payload;
  //Serial.println("payload: '"+payloadString+"', channel: "+(String)num);
  if (type == WStype_TEXT) {
    byte separator = payloadString.indexOf('=');
    String var = payloadString.substring(0, separator);
    String val = payloadString.substring(separator + 1);

    if (var == "pSteer") {
      pSteer = val.toFloat();
      PSTEER = true;
      Serial.println("$PS:" + String(pSteer, 2));
    }
    else if (var == "iSteer") {
      iSteer = val.toFloat();
      ISTEER = true;
      Serial.println("$IS:" + String(iSteer, 5));
    }
    else if (var == "dSteer") {
      dSteer = val.toFloat();
      DSTEER = true;
      Serial.println("$DS:" + String(dSteer, 2));
    }
    else if (var == "pThrottle") {
      pThrottle = val.toFloat();
      PTHROTTLE = true;
      Serial.println("$PT:" + String(pThrottle, 2));
    }
    else if (var == "iThrottle") {
      iThrottle = val.toFloat();
      ITHROTTLE = true;
      Serial.println("$IT:" + String(iThrottle, 5));
    }
    else if (var == "dThrottle") {
      dThrottle = val.toFloat();
      DTHROTTLE = true;
      Serial.println("$DT:" + String(dThrottle, 2));
    }
    else if (var == "minSteer") {
      minSteer = val.toInt();
      MINSTEER = true;
      Serial.println("$mS:" + (String)minSteer);
    }
    else if (var == "maxSteer") {
      maxSteer = val.toInt();
      MAXSTEER = true;
      Serial.println("$MS:" + (String)maxSteer);
    }
    else if (var == "midSteer") {
      midSteer = val.toInt();
      MIDSTEER = true;
      Serial.println("$CS:" + (String)midSteer);
    }
    else if (var == "maxReverse") {
      maxReverse = val.toInt();
      MAXREVERSE = true;
      Serial.println("$mT:" + (String)maxReverse);
    }
    else if (var == "maxForward") {
      maxForward = val.toInt();
      MAXFORWARD = true;
      Serial.println("$MT:" + (String)maxForward);
    }
    else if (var == "ratio") {
      ratio = val.toFloat();
      RATIO = true;
      Serial.println("$ST:" + String(ratio, 2));
    }
    else if (var == "LOAD") load();
    else if (var == "SAVE") Serial.println("$SA");
    else if (var == "RESET") reset();
    else if (var == "MISSION")
    {
      if(val == "START" && LIST)
      { 
          mSTART=true;
          mRETURN=false;
          mPAUSE=false;
          Serial.println("$SM");
          TINDEX=true;
          sendGPS();
      }
      else if(val == "PAUSE" && mSTART)
      { 
        mPAUSE=!mPAUSE;
        Serial.println("$PM");
      }
      else if(val == "SKIP" && mSTART)
      {
        nextWaypoint(tIndex+1);
      }
      else if(val == "STOP")
      { 
        mSTART=false;
        mPAUSE=false;
        mRETURN=false;
        Serial.println("$sM");
        tIndex=0; TINDEX=true;
      }
      else if(val == "RETURN")
      {
        mRETURN=true;
        mSTART=false;
        mPAUSE=false;
        tLAT = hLAT;
        tLON = hLON;
        tGPS = true;
        Serial.println("$RH");
      }
    }
    else if(var == "HOME")
    {
      int comma = val.indexOf(',');
      hLAT = val.substring(0, comma);
      hLON = val.substring(comma + 1);
      Serial.println("$Ho:"+hLAT+","+hLON);
    }
    else if(var == "LIST")
    {
      int semi = val.indexOf(';');
      GPSlist = val.substring(semi + 1);
      GPSlength = val.substring(0, semi).toInt();
      if(GPSlength > 0) LIST=true;
      //sendGPS();
    }
    else if(var == "threshold")
    {
      threshold = val.toFloat();
      Serial.println("$Td:"+val);
    }
    else if(var == "decAngle")
    {
      decAngle = val;
      Serial.println("$DA:"+val);
      Serial.println("$SA");
    }
  }  
  return;
}

void sendGPS()
{
    String GPSpoint = GPSlist.substring(getIndex(GPSlist, ';', tIndex),getIndex(GPSlist, ';', 1 + tIndex)-1);
    Serial.println("$Gt:"+GPSpoint);
    int comma = GPSpoint.indexOf(',');
    tLAT = GPSpoint.substring(0,comma);
    tLON = GPSpoint.substring(comma+1);
    tGPS = true;
}

int getIndex(String string, char separator, int index)
{
    
     if(index > 0)
     {
       int sep = string.indexOf(separator);
       return sep + 1 + getIndex(string.substring(sep+1),separator,index-1);
     }
     else return 0;
}

void load() {
  PSTEER = true; ISTEER = true; DSTEER = true;
  PTHROTTLE = true; ITHROTTLE = true; DTHROTTLE = true;
  MINSTEER = true; MAXSTEER = true; MIDSTEER = true;
  MAXREVERSE = true; MAXFORWARD = true; RATIO = true;
  return;
}

void reset() {
  Serial.println("$RE");
  Serial.println("$CF");
  load();
  return;
}

void parseMessage() 
{
  //Serial.println("incoming");
  int8_t separator = inputString.indexOf(':');
  if (separator > -1) 
  {
    int8_t _eol = inputString.indexOf('!');
    String var = inputString.substring(0, separator);
    String val = inputString.substring(separator + 1, _eol);
    //Serial.println("Message: "+var+" Value: "+val);
    if (var == "PS") {
      pSteer = val.toFloat();
      PSTEER = true;
    }
    else if (var == "IS") {
      iSteer = val.toFloat();
      ISTEER = true;
    }
    else if (var == "DS") {
      dSteer = val.toFloat();
      DSTEER = true;
    }
    else if (var == "PT") {
      pThrottle = val.toFloat();
      PTHROTTLE = true;
    }
    else if (var == "IT") {
      iThrottle = val.toFloat();
      ITHROTTLE = true;
    }
    else if (var == "DT") {
      dThrottle = val.toFloat();
      DTHROTTLE = true;
    }
    else if (var == "mS") {
      minSteer = val.toInt();
      MINSTEER = true;
    }
    else if (var == "MS") {
      maxSteer = val.toInt();
      MAXSTEER = true;
    }
    else if (var == "CS") {
      midSteer = val.toInt();
      MIDSTEER = true;
    }
    else if (var == "mT") {
      maxReverse = val.toInt();
      MAXREVERSE = true;
    }
    else if (var == "MT") {
      maxForward = val.toInt();
      MAXFORWARD = true;
    }
    else if (var == "ST") {
      ratio = val.toFloat();
      RATIO = true;
    }
    else if (var == "GP")
    { //LAT=""; LON="";
      byte split = val.indexOf(',');
      LAT = val.substring(0, split).toFloat();
      LON = val.substring(split + 1, _eol).toFloat();
      GPS = true;
    }
    else if (var == "CO") {
      heading = val.toFloat();
      COMPASS = true;
    }
    else if (var == "TH") {
      theading = val.toFloat();
      tCOMPASS = true;
    }
    else if (var == "TD"){
      tDistance = val.toFloat();
      TDIST = true;
    }
    else if (var == "AC")
    {
      //accX=""; accY=""; accZ="";
      byte split1 = val.indexOf(',');
      byte split2 = val.indexOf(',', split1 + 1);
      accX = val.substring(0, split1).toFloat();
      accY = val.substring(split1 + 1, split2).toFloat();
      accZ = val.substring(split2 + 1, _eol).toFloat();
      ACC = true;
    }
    else if (var == "GY")
    {
      //gyroX=""; gyroY=""; gyroZ="";
      byte split1 = val.indexOf(',');
      byte split2 = val.indexOf(',', split1 + 1);
      gyroX = val.substring(0, split1).toFloat();
      gyroY = val.substring(split1 + 1, split2).toFloat();
      gyroZ = val.substring(split2 + 1, _eol).toFloat();
      GYRO = true;
    }
    else if (var == "TF")
    {
      tempF = val.toFloat();
      TEMPF = true;
    }
    else if (var == "TC")
    {
      tempC = val.toFloat();
      TEMPC = true;
    }
    else if (var == "CT")
    {
      //steeringVAL=""; throttleVAL="";
      byte split = val.indexOf(',');
      steeringVAL = val.substring(0, split).toInt();
      throttleVAL = val.substring(split + 1, _eol).toInt();
      CTRL = true;
    }
    else if (var == "MO")
    {
      if (val.toInt() == 0) Mode = "MANUAL";
      else if (val.toInt() == 1) Mode = "PID";
      else if (val.toInt() == 2) Mode = "AUTONOMOUS";
      MODE = true;
    }
    else if (var == "TR" && mSTART)
    {
        nextWaypoint(val.toInt()+1);
        //Serial.println("next point");
    }
  }
  return;
}

void getSerial() {
  while (Serial.available()) {
    char inChar = Serial.read();
    if (!INCOMING && inChar == '$') {
      INCOMING = true;
      inputString = "";
    }
    else if (INCOMING && inChar != '!') inputString += inChar;
    else {
      INCOMING = false;
      inputString += '!';
      parseMessage();
      return;
    }
  }
  return;
}

void setup() {
  Serial.begin(230400);
  digitalWrite(LED, LOW);
  pinMode(LED, OUTPUT);
  WiFi.softAP(ssid, password);
  WiFi.setMaxConnections(1);
  load();
  server.on("/", handleWebsite);
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
  server.handleClient();
  getSerial();
  if (millis() > wait001) {
    wait001 = millis() + websockMillis;
    sendJSON();
  }
}

