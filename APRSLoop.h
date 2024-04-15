
// Defines the Output Pin for the Voltage controlled oscillator
#define VXCO_PIN 3 // SAMD21 pin


#ifndef APRS_WSPR  // If not defined -- defined when combined with wspr to eliminate duplicate si5351 objectys
    #include <si5351.h> 
    Si5351 si5351;
#endif
#include "SI5351InterfaceVHF-20QFN.h"
//#include "SI5351InterfaceVHF-16QFN.h"
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
    // Set status message
    char status[] = "APRS Balloon Tracker by K9YO";
    APRSSetStatus(status);

    // Set data that will be automatically forwarded to Soundhub (Simulates qrplab APRS tracker)
    APRSSensorInfoInit();
    // cpu Temp, Temp, pressure, number of satellites, year, month, day
    APRSSetSensorInfo(-5, getTemperature(), getPressure(), 6, (int)clock.getYear(), (int)clock.getMonth(), (int)clock.getDay());

    // Example of how to add a string of information to the data porting of a messate

    APRSDataInit('B'); // Third character in data string used to identify the type of body
    // Various different bodies can be sent by the tracker using different third characters
    // Typocally this type of body is not displayed by aprs.fi.  It needs to be forwarded to an iGate
    for (int i = 0; i < 5; ++i)
    {
        // Build the data body with a comma delimited items
        APRSDataAppendInt(i * 7);
        APRSDataAppendChars(loc6);
        APRSDataAppendFloat(3.14159);
    }

    // course in deg clockwise from N, speed in knots, altitude in ft

    APRSSetCourseSpeedAltitude(gpsCourse, gpsSpeed * .5399, gpsAltitude * 3.28);
    APRSFormatTime((int)clock.getHours(), (int)clock.getMinutes(), (int)clock.getSeconds()); // hr min sec
    APRSLatLong(latitude, longitude);
    // Checks if it is ok to transmit in this location - If yes it GEOFENCE_Freq which sets the transmit frequency
    GEOFENCE_position(latitude, longitude);
    if (GEOFENCE_no_tx == APRS_Not_Ok)
        return false; // Not ok to transmit

    return true;
}

void SendAPRSPacket(int msg)
{
    // transmit_test();
    bool OK_to_transmit = APRSBegin();
    if (OK_to_transmit == false)
    {
        POUTPUTLN(F("APRS_Not_OK for this location "));
        return;
    }
    else
        POUTPUTLN(F("APRS is in an OK locaton "));

    POUTPUT(F("GEO Frequency "));
    POUTPUTLN(((int)(GEOFENCE_APRS_frequency / 1000)));

    //     SetOverrideFrequency(F14445);  // Frequency not used by any country
    if (msg == 0)
    {   print_debug(_BEACON, _POSITION_STATUS);
        send_packet(_BEACON, _POSITION_STATUS);
    }
    //     SetOverrideFrequency(F14445);  // Frequency not used by any country
    // delay(10000);
    if (msg == 1)
    { print_debug(_NORMAL, _POSITION_STATUS);
     send_packet(_NORMAL, _POSITION_STATUS);
     }
    //     SetOverrideFrequency(F14445);  // Frequency not used by any country
    // delay(10000);
    if (msg == 2)
    {    print_debug(_NORMAL, _STATUS);
        send_packet(_NORMAL, _STATUS);
    }

    //     SetOverrideFrequency(F14445);  // Frequency not used by any country
    // delay(15000);
    if (msg == 3)
    {    print_debug(_NORMAL, _DATA);
        send_packet(_NORMAL, _DATA);
    }
    // delay(10000);
    if (msg == 4)
    {    print_debug(_BALLOON, _BALLOON);
        send_packet(_BALLOON, _BALLOON);
    }
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