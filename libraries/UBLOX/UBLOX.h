#include <NMEAGPS.h>



//======================================================================
//  Prerequisites:
//     1) NMEA.ino works with your device (correct TX/RX pins and baud rate)
//     2) At least one of the RMC, GGA or GLL sentences have been enabled in NMEAGPS_cfg.h.
//     3) Your device at least one of those sentences (use NMEAorder.ino to confirm).
//     4) LAST_SENTENCE_IN_INTERVAL has been set to one of those sentences in NMEAGPS_cfg.h (use NMEAorder.ino).
//     5) LOCATION and ALTITUDE have been enabled in GPSfix_cfg.h
//
//  'Serial' is for debug output to the Serial Monitor window.
//
//   Choose a serial port for the GPS device:
//
//   BEST: For a Mega, Leonardo or Due, use the extra hardware serial port
#define gpsPort Serial1

//   2nd BEST:  For other Arduinos, use AltSoftSerial on the required pins 
//                 (8&9 for an UNO)
// #include <AltSoftSerial.h>
// AltSoftSerial gpsPort;  // pin 8 to GPS TX, pin 9 to GPS RX

//   3rd BEST:  If you can't use those specific pins (are you sure?), 
//                 use NeoSWSerial on any two pins @ 9600, 19200 or 38400
// #include <NeoSWSerial.h>
// NeoSWSerial gpsPort( 2, 3 ); // pin 2 to GPS TX, pin 3 to GPS RX

//   WORST:  SoftwareSerial is NOT RECOMMENDED

//======================================================================
const unsigned char ubxRate5Hz[] PROGMEM =
  { 0x06,0x08,0x06,0x00,200,0x00,0x01,0x00,0x01,0x00 };
const unsigned char ubxRate10Hz[] PROGMEM =
  { 0x06,0x08,0x06,0x00,100,0x00,0x01,0x00,0x01,0x00 };
const char baud115200[] PROGMEM =  
  "PUBX,41,1,3,3,115200,0";
  
class UBLOX
      {
      public:
             UBLOX(usb_serial_class* usb, HardwareSerial* gpsPort);
             void readGPS();
             void configGPS();
             double LAT;
             double LON;
             float ALT;
             bool GPS;
             
      private:
              usb_serial_class* _usb;
              HardwareSerial* _gpsPort;
              void sendUBX( const unsigned char *progmemBytes, size_t len );
              NMEAGPS  _gps; // This parses the GPS characters
              gps_fix  _fix; // This holds on to the latest values
              double oldLAT;
              double oldLON;
      };
