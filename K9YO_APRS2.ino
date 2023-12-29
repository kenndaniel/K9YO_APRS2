/*
 *  Copyright (C) 2018 - Handiko Gesang - www.github.com/handiko
 *  Modified and adapted for ballooons by Ken Daniel - www.github.com/kenndaniel
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <math.h>
#include <stdio.h>

#include "config.h"

#ifdef DEBUG
#define POUTPUT(x) Serial.print x
#else
#define POUTPUT(x)
#endif

#ifdef DEBUG
#define POUTPUTLN(x) Serial.println x
#else
#define POUTPUTLN(x)
#endif

// Lat long for testing 
// Chicago
// float latitude = 43.283375;
// float longitude = -87.843936;

// Berlin
float latitude = 52.52;
float longitude = 13.40; 


#include <Wire.h>
//#include <i2cdetect.h>
#include <TimeLib.h>


#include "APRSLoop.h"

const char *mycall = call;

void setup()
{
  delay(7000);  // This is needed 
  Wire.begin();

  set_io();
  POUTPUTLN((" Starting "));
  print_code_version();
  bool OK_to_transmit = APRSBegin();
  if (OK_to_transmit == false)
    POUTPUTLN(("APRS_Not_OK"));
  else
    POUTPUTLN(("APRS is OK"));
  POUTPUT(("GEO Frequency "));
  POUTPUTLN(((int)(GEOFENCE_APRS_frequency / 1000)));

  // Initialize the cpu time
  // hr, min, sec, day, month, year
    setTime( 8, 30, 45, 30, 30, 2023);

}

int altitude = 2000;

void loop()
{
  // POUTPUTLN(" Starting Loop ");
 //int ifreq = F14445;
  {
    // i2cdetect();
    //POUTPUTLN((" Frequency"));
    //POUTPUTLN((ifreq));
    //transmit_test();
    SendPackets();

  }
}
 