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
// Reference txco frequency for SI5351
#define SI5351_XTAL 27000000  
//#define SI5351_XTAL 25000000

const char call[] = "K9YO";     // Amateur callsign
// 
//  WSPR Configuration variables
//
#define DEBUG // Debug output is generated if DEBUG is defined

#define APRS_SID  11
#define VCXO_GAIN 52 // 45 should be ok - Adjustment of the difference between the low and high tone frequencies 9-9.5MHz Critical
