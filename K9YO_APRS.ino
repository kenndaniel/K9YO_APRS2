/*
 *  Copyright (C) 2018 - Handiko Gesang - www.github.com/handiko
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

#define DEBUG
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

#include "config.h"
// Defines the Output Pin for the Voltage controlled oscillator
// #define VXCO_PIN 9 /// ProMini
#define VXCO_PIN 3 // SAMD21 pin

#include <Wire.h>
#include <i2cdetect.h>
//#include <si5351.h> // include this for QFN20
#include "src/si5351-16QFN.h"  // include this for QFN16
Si5351 si5351;
//#include "SI5351InterfaceVHF-20QFN.h"
#include "SI5351InterfaceVHF-16QFN.h"
#include "geofence.h"

#include "APRSFormat.h"
#include "APRSSendPkt.h"

const char *mycall = call;

// Chicago
float latitude = 43.283375;
float longitude = -87.843936;

/*
APRS requires data in a particular format.  This function uses several functions to put the data
into the correct format.
Using the global variable for Lat and Lon this function will check if transmission is legal at
this location. It returns false if transmission is not allowed.
If allowed it changes the frequency to the local APRS frequency and converts the lat and lon
to the APRS format.
*/
bool APRSBegin()
{
  APRSDataInit('B');

  APRSSensorInfoInit();
  APRSSetSensorInfo(-5, -5.24, 250.23, 6, 23, 8, 4); // cpu Temp, Temp, pressure, number of satellites, year, month, day
  APRSDataInit('B');
  char loc[] = "EN62ag";
  for (int i = 0; i < 5; ++i)
  {
    APRSDataAppendInt(i * 7);
    APRSDataAppendChars(loc);
    APRSDataAppendFloat(3.14159);
  }

  APRSSetCourseSpeedAltitude(9, 1, 2000);
  APRSSetTime(1, 5, 6); // hr min sec
  APRSLatLong(latitude, longitude);
  // Checks if it is ok to transmit in this location - If yes it GEOFENCE_Freq which sets the transmit frequency
  GEOFENCE_position(latitude, longitude);
  if (GEOFENCE_no_tx == APRS_Not_Ok)
    return false; // Not ok to transmit

  return true;
}

int ucount = 0;
float radius = .05;
float latitude0 = 42.283375;
float longitude0 = -87.963936;
void updateLatLon()
{
  /* Test function to fly a fake balloon in a circle around latitude0 and longitude0 */

  float dlon = sin(2 * 3.1416 * ucount / 20.) * radius;
  float dlat = cos(2 * 3.1416 * ucount / 20.) * radius;
  // Serial.println(" dlat dlon");
  Serial.print(ucount);
  Serial.print(" ");
  Serial.print(dlat);
  Serial.print(" ");
  Serial.println(dlon);
  latitude = latitude0 + dlat + .015;
  longitude = longitude0 + dlon;
  ++ucount;
  radius += .001;
}

void setup()
{
  delay(7000);
  Wire.begin();

  set_io();
  Serial.println(" Starting ");
  print_code_version();
  bool OK_to_transmit = APRSBegin();
  if (OK_to_transmit == false)
    Serial.println("APRS_Not_OK");
  else
    Serial.println("APRS is OK");
  Serial.print("GEO Frequency ");
  Serial.println((int)(GEOFENCE_APRS_frequency / 1000));

  // APRSAlternateFrequency();
}

int altitude = 2000;

void loop()
{
  // Serial.println(" Starting Loop ");
 int ifreq = F14445;
  {
    // i2cdetect();
    Serial.println(ifreq);
    //transmit_test();
    GEOFENCE_Freq = (APRSFreqs)ifreq;
    print_debug(_FIXPOS_STATUS, _NORMAL);
    send_packet(_FIXPOS_STATUS, _BEACON);
    delay(100);
    print_debug(_FIXPOS_STATUS, _BEACON);
    send_packet(_BEACON, _NORMAL);
    delay(100);
    print_debug(_STATUS, _NORMAL);
    send_packet(_STATUS, _NORMAL);
    delay(100);
    print_debug(_DATA, _NORMAL);
    send_packet(_DATA, _NORMAL);
    delay(100);
    APRSLatLong(latitude, longitude);
    APRSSetCourseSpeedAltitude(9, 1, altitude);
    print_debug(_BALLOON, _BALLOON);
    send_packet(_BALLOON, _BALLOON);
    delay(6000);
  }
}

void transmit_test(void)
{
  Serial.println("Transmit Test");
  APRSon();
  for (int i = 0; i < 10; ++i)
  {
    transmitAPRS(LOW);
    delay(500);
    transmitAPRS(HIGH);
    delay(500);
  }
  APRSoff();
  delay(500);
}