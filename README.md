# Balloon Tracker APRS Code

The code in this repository builds on the work of others to generate APRS messages.  Its main contrabution is that it provides the ability to generate a variety of properly formatted messages. This is expecially valuable when sending scientific data in the body of the APRS message.  The pdf file in this repository (README.pdf) provides complete documentation of the kinds messages and how to generate them.  A minor part of this program is an interface to an SI5351B rf generator.  This version of the chip contains a vco (voltage controlled oscillator).  The voltage change is made with a digital output froma CPU changing at a specific rate. This part of the code to generate the tones and code to provide geofensing has been adoped from other publically available software.

This code can easily provide a framework for adaptions other wspr based trackers to also send APRS messages.
