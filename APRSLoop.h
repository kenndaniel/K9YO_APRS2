
// Defines the Output Pin for the Voltage controlled oscillator
#define VXCO_PIN 3 // SAMD21 pin
// #include <si5351.h> // include this for QFN20
#include "src/si5351-16QFN.h" // include this for QFN16
Si5351 si5351;
// #include "SI5351InterfaceVHF-20QFN.h"
#include "SI5351InterfaceVHF-16QFN.h"
#include "geofence.h"

#include "APRSFormat.h"
#include "APRSSendPkt.h"

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
    // Set data that will be automatically forwarded to Soundhub (Simulates qrplab APRS tracker)
    APRSDataInit('B');
    APRSSensorInfoInit();
    APRSSetSensorInfo(-5, -5.24, 250.23, 6, (int)year(), (int)month(), (int)day()); // cpu Temp, Temp, pressure, number of satellites, year, month, day

    // Example of how to add a string of information to the data porting of a messate
    /*
      APRSDataInit('B');
      char loc[] = "EN62ag";
      for (int i = 0; i < 5; ++i)
      {
        APRSDataAppendInt(i * 7);
        APRSDataAppendChars(loc);
        APRSDataAppendFloat(3.14159);
      } */

    // course in deg clockwise from N, speed in knots, altitude in ft
    APRSSetCourseSpeedAltitude(9, 1, 2000);
    APRSSetTime((int)hour(), (int)minute(), (int)second()); // hr min sec
    APRSLatLong(latitude, longitude);
    // Checks if it is ok to transmit in this location - If yes it GEOFENCE_Freq which sets the transmit frequency
    GEOFENCE_position(latitude, longitude);
    if (GEOFENCE_no_tx == APRS_Not_Ok)
        return false; // Not ok to transmit

    return true;
}

void SendPackets()
{
    bool OK_to_transmit = APRSBegin();
    if (OK_to_transmit == false)
        POUTPUTLN(F("APRS_Not_OK"));
    else
        POUTPUTLN(F("APRS is OK"));
    POUTPUT(F("GEO Frequency "));
    POUTPUTLN(((int)(GEOFENCE_APRS_frequency / 1000)));

    SetOverrideFrequency(F144575);
    print_debug(_FIXPOS_STATUS, _NORMAL);
    send_packet(_FIXPOS_STATUS, _BEACON);
    delay(100);
    // print_debug(_FIXPOS_STATUS, _BEACON);
    // send_packet(_BEACON, _NORMAL);
    // delay(100);
    // print_debug(_STATUS, _NORMAL);
    // send_packet(_STATUS, _NORMAL);
    // delay(100);
    // print_debug(_DATA, _NORMAL);
    // send_packet(_DATA, _NORMAL);
    // delay(100);
    // print_debug(_BALLOON, _BALLOON);
    // send_packet(_BALLOON, _BALLOON);

    delay(15000);
}

void transmit_test(void)
{
    POUTPUTLN(("Transmit Test"));
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