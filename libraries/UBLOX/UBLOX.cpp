#include <UBLOX.h>

UBLOX::UBLOX(usb_serial_class* usb, HardwareSerial* gpsPort)
{
  _usb=usb;
  _gpsPort=gpsPort;
  _gpsPort->begin(9600);
  //configGPS();
}

void UBLOX::configGPS(){
  
  sendUBX( ubxRate10Hz, sizeof(ubxRate10Hz) );
  //_gpsPort->print( baud115200 );
  //_gpsPort->flush();
  //_gpsPort->begin(115200);
  GPS=false;  
}

void UBLOX::readGPS()
{
  while(_gps.available( *_gpsPort )){
      _fix = _gps.read();
      if (_fix.valid.location) 
      {
         LAT = _fix.latitude();
         LON = _fix.longitude();
         //_usb->printf("%f, %f",LAT,LON);
         if(LAT!=oldLAT || LON!=oldLON)GPS=true;
         oldLAT=LAT; oldLON=LON;
      }
      if (_fix.valid.altitude)ALT = _fix.altitude();
  }
}

void UBLOX::sendUBX( const unsigned char *progmemBytes, size_t len )
{
  _gpsPort->write( 0xB5 ); // SYNC1
  _gpsPort->write( 0x62 ); // SYNC2

  uint8_t a = 0, b = 0;
  while (len-- > 0) {
    uint8_t c = pgm_read_byte( progmemBytes++ );
    a += c;
    b += a;
    _gpsPort->write( c );
  }

  _gpsPort->write( a ); // CHECKSUM A
  _gpsPort->write( b ); // CHECKSUM B

} // sendUBX
