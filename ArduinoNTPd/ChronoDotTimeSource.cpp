/*
 * File: ChronoDotDataSource.cpp
 * Description:
 *   ChronoDot (i.e. DS3231) time source.
 * Author: Lauri Nurmi <lanurmi@iki.fi>
 * License: New BSD License
 */

#include <SoftwareSerial.h>

#include "ChronoDotTimeSource.h"
#include "TimeUtilities.h"

#include <Wire.h>

typedef char byte;

void getChronoDotTime(int *y, int *mon, int *day, int *h, int *m, int *s);

void ChronoDotTimeSource::now(uint32_t *secs, uint32_t *fract) {

  int y, mon, day, h, m, s;
  getChronoDotTime(&y, &mon, &day, &h, &m, &s);

  h -= 2; // ei tuotantoon

  secondsOfRecv_ = TimeUtilities::numberOfSecondsSince1900Epoch(y, mon, day, h, m, s);
  fractionalSecondsOfRecv_ = 3;

  if (secs)
    *secs = secondsOfRecv_;

  if (fract)
    *fract = fractionalSecondsOfRecv_;
}


uint32_t ChronoDotTimeSource::timeRecv(uint32_t *secs, uint32_t *fract) const {
  *secs = secondsOfRecv_;
  *fract = fractionalSecondsOfRecv_;
  return 0;
}

// Cheat: This is really the initialization.
// Source: http://docs.macetech.com/doku.php/chronodot_v2.0
void ChronoDotTimeSource::enableInterrupts() {
  Wire.begin();
//  Serial.begin(9600);
 
  // clear /EOSC bit
  // Sometimes necessary to ensure that the clock
  // keeps running on just battery power. Once set,
  // it shouldn't need to be reset but it's a good
  // idea to make sure.
  Wire.beginTransmission(0x68); // address DS3231
  Wire.write(0x0E); // select register
  Wire.write(0b00011100); // write register bitmap, bit 7 is /EOSC
  Wire.endTransmission();
}

void getChronoDotTime(int *y, int *mon, int *day, int *h, int *m, int *s)
{
  // send request to receive data starting at register 0
  Wire.beginTransmission(0x68); // 0x68 is DS3231 device address
  Wire.write((byte)0); // start at register 0
  Wire.endTransmission();
  Wire.requestFrom(0x68, 7); // request three bytes (seconds, minutes, hours)
 
  int seconds = Wire.read(); // get seconds
  int minutes = Wire.read(); // get minutes
  int hours = Wire.read();   // get hours
  Wire.read(); // get wday
  int mday = Wire.read();
  int month = Wire.read();
  int year = Wire.read();
 
  seconds = (((seconds & 0b11110000)>>4)*10 + (seconds & 0b00001111)); // convert BCD to decimal
  minutes = (((minutes & 0b11110000)>>4)*10 + (minutes & 0b00001111)); // convert BCD to decimal
  hours = (((hours & 0b00100000)>>5)*20 + ((hours & 0b00010000)>>4)*10 + (hours & 0b00001111)); // convert BCD to decimal (assume 24 hour mode)
  mday = (((mday & 0b11110000)>>4)*10 + (mday & 0b00001111)); // convert BCD to decimal
  int century = (month & 0b10000000) >> 7;
  month = (((month & 0b00010000)>>4)*10 + (month & 0b00001111)); // convert BCD to decimal
  year = 1970 + (((year & 0b11110000)>>4)*10 + (year & 0b00001111)); // convert BCD to decimal
  // Not sure why 1970 needs to be added here; cannot deduce such a requirement from DS3231 datasheet.

  *y = year;
  *mon = month;
  *day = mday;
  *h = hours;
  *m = minutes;
  *s = seconds;
}
