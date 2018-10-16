/*
HMC5883L.cpp - Class file for the HMC5883L Triple Axis Digital Compass Arduino Library.

Version: 1.1.0
(c) 2014 Korneliusz Jarzebski
www.jarzebski.pl

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif



#include "HMC5883L.h"

bool HMC5883L::begin(usb_serial_class* usb, i2c_t3* wire, HardwareSerial *web)
{    _wire=wire;
     _usb = usb;
     _web = web;
    //Wire.begin(I2C_MASTER, 0x00,I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_100);

    if ((fastRegister8(HMC5883L_REG_IDENT_A) != 0x48)
    || (fastRegister8(HMC5883L_REG_IDENT_B) != 0x34)
    || (fastRegister8(HMC5883L_REG_IDENT_C) != 0x33))
    {
	return false;
    }

    setRange(HMC5883L_RANGE_0_88GA);
    setMeasurementMode(HMC5883L_CONTINOUS);
    setDataRate(HMC5883L_DATARATE_75HZ);
    setSamples(HMC5883L_SAMPLES_8);
    setOffset(-22,-172);
    mgPerDigit = 0.92f;

    return true;
}

Vector HMC5883L::readRaw(void)
{
    v.XAxis = readRegister16(HMC5883L_REG_OUT_X_M) - xOffset;
    v.YAxis = readRegister16(HMC5883L_REG_OUT_Y_M) - yOffset;
    v.ZAxis = readRegister16(HMC5883L_REG_OUT_Z_M);

    return v;
}

Vector HMC5883L::readNormalize(void)
{
    v.XAxis = ((float)readRegister16(HMC5883L_REG_OUT_X_M) - xOffset) * mgPerDigit;
    v.YAxis = ((float)readRegister16(HMC5883L_REG_OUT_Y_M) - yOffset) * mgPerDigit;
    v.ZAxis = (float)readRegister16(HMC5883L_REG_OUT_Z_M) * mgPerDigit;

    return v;
}

void HMC5883L::setOffset(int xo, int yo)
{
    xOffset = xo;
    yOffset = yo;
}

void HMC5883L::setRange(hmc5883l_range_t range)
{
    switch(range)
    {
	case HMC5883L_RANGE_0_88GA:
	    mgPerDigit = 0.073f;
	    break;

	case HMC5883L_RANGE_1_3GA:
	    mgPerDigit = 0.92f;
	    break;

	case HMC5883L_RANGE_1_9GA:
	    mgPerDigit = 1.22f;
	    break;

	case HMC5883L_RANGE_2_5GA:
	    mgPerDigit = 1.52f;
	    break;

	case HMC5883L_RANGE_4GA:
	    mgPerDigit = 2.27f;
	    break;

	case HMC5883L_RANGE_4_7GA:
	    mgPerDigit = 2.56f;
	    break;

	case HMC5883L_RANGE_5_6GA:
	    mgPerDigit = 3.03f;
	    break;

	case HMC5883L_RANGE_8_1GA:
	    mgPerDigit = 4.35f;
	    break;

	default:
	    break;
    }

    writeRegister8(HMC5883L_REG_CONFIG_B, range << 5);
}

hmc5883l_range_t HMC5883L::getRange(void)
{
    return (hmc5883l_range_t)((readRegister8(HMC5883L_REG_CONFIG_B) >> 5));
}

void HMC5883L::setMeasurementMode(hmc5883l_mode_t mode)
{
    uint8_t value;

    value = readRegister8(HMC5883L_REG_MODE);
    value &= 0b11111100;
    value |= mode;

    writeRegister8(HMC5883L_REG_MODE, value);
}

hmc5883l_mode_t HMC5883L::getMeasurementMode(void)
{
    uint8_t value;

    value = readRegister8(HMC5883L_REG_MODE);
    value &= 0b00000011;

    return (hmc5883l_mode_t)value;
}

void HMC5883L::setDataRate(hmc5883l_dataRate_t dataRate)
{
    uint8_t value;

    value = readRegister8(HMC5883L_REG_CONFIG_A);
    value &= 0b11100011;
    value |= (dataRate << 2);

    writeRegister8(HMC5883L_REG_CONFIG_A, value);
}

hmc5883l_dataRate_t HMC5883L::getDataRate(void)
{
    uint8_t value;

    value = readRegister8(HMC5883L_REG_CONFIG_A);
    value &= 0b00011100;
    value >>= 2;

    return (hmc5883l_dataRate_t)value;
}

void HMC5883L::setSamples(hmc5883l_samples_t samples)
{
    uint8_t value;

    value = readRegister8(HMC5883L_REG_CONFIG_A);
    value &= 0b10011111;
    value |= (samples << 5);

    writeRegister8(HMC5883L_REG_CONFIG_A, value);
}

hmc5883l_samples_t HMC5883L::getSamples(void)
{
    uint8_t value;

    value = readRegister8(HMC5883L_REG_CONFIG_A);
    value &= 0b01100000;
    value >>= 5;

    return (hmc5883l_samples_t)value;
}

// Write byte to register
void HMC5883L::writeRegister8(uint8_t reg, uint8_t value)
{
    _wire->beginTransmission(HMC5883L_ADDRESS);
    #if ARDUINO >= 100
        _wire->write(reg);
        _wire->write(value);
    #else
        _wire->send(reg);
        _wire->send(value);
    #endif
    _wire->endTransmission();
}

// Read byte to register
uint8_t HMC5883L::fastRegister8(uint8_t reg)
{
    uint8_t value;
    _wire->beginTransmission(HMC5883L_ADDRESS);
    #if ARDUINO >= 100
        _wire->write(reg);
    #else
        _wire->send(reg);
    #endif
    _wire->endTransmission();

    _wire->requestFrom(HMC5883L_ADDRESS, 1);
    #if ARDUINO >= 100
        value = _wire->read();
    #else
        value = _wire->receive();
    #endif
    _wire->endTransmission();

    return value;
}

// Read byte from register
uint8_t HMC5883L::readRegister8(uint8_t reg)
{
    uint8_t value;
    _wire->beginTransmission(HMC5883L_ADDRESS);
    #if ARDUINO >= 100
        _wire->write(reg);
    #else
        _wire->send(reg);
    #endif
    _wire->endTransmission();

    _wire->beginTransmission(HMC5883L_ADDRESS);
    _wire->requestFrom(HMC5883L_ADDRESS, 1);
    while(!_wire->available()) {};
    #if ARDUINO >= 100
        value = _wire->read();
    #else
        value = _wire->receive();
    #endif
    _wire->endTransmission();

    return value;
}

// Read word from register
int16_t HMC5883L::readRegister16(uint8_t reg)
{
    int16_t value;
    _wire->beginTransmission(HMC5883L_ADDRESS);
    #if ARDUINO >= 100
        _wire->write(reg);
    #else
        _wire->send(reg);
    #endif
    _wire->endTransmission();

    _wire->beginTransmission(HMC5883L_ADDRESS);
    _wire->requestFrom(HMC5883L_ADDRESS, 2);
    while(!_wire->available()) {};
    #if ARDUINO >= 100
        uint8_t vha = _wire->read();
        uint8_t vla = _wire->read();
    #else
        uint8_t vha = _wire->receive();
        uint8_t vla = _wire->receive();
    #endif
    _wire->endTransmission();

    value = vha << 8 | vla;

    return value;
}

double HMC5883L::getHeading()
{
  Vector raw = readRaw();

  // Calculate heading
  double heading = atan2(raw.YAxis, raw.XAxis);
  //_usb->print(" Xnorm = ");
  //_usb->print(norm.XAxis);
  //_usb->print(" Ynorm = ");
  //_usb->print(norm.YAxis);
  //_usb->print(" ZNorm = ");
  //_usb->print(norm.ZAxis);
  //_usb->println(); 
  // Set declination angle on your location and fix heading
  // You can find your declination on: http://magnetic-declination.com/
  // (+) Positive or (-) for negative
  // For Bytom / Poland declination angle is 4'26E (positive)

  // Formula: (deg + (min / 60.0)) / (180 / M_PI);

  //double declinationAngle = (-3.0 + (50.0 / 60.0)) / (180 / M_PI);
  heading += declinationAngle / (180 / M_PI);
  //heading -= PI/2;
  // Correct for heading < 0deg and heading > 360deg
  if (heading < -1 * PI)
  {
    heading += 2 * PI;
  }

  if (heading > PI)
  {
    heading -= 2 * PI;
  }

  // Convert to degrees
  double headingDegrees = heading * 180/M_PI; 

  // Output
  //Serial.print(" Heading = ");
  //Serial.print(heading);
  //_usb->print(" Degrees = ");
  //_usb->println(headingDegrees);
  //Serial.println();
  //_web->print("$CO:"+String(headingDegrees,10)+"\n\n\n");
  //_web->flush();
  //_usb->println("$CO:"+String(headingDegrees,10));
  return headingDegrees;
}
