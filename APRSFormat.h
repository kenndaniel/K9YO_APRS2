/* 
Functions in this file convert the latitude and longitude in degrees to the special APRS lat long format
Latitued ddmm.mmV  Longitude dddmm.mmH where (d)dd is positive degrees and mm.mm are minutes and fraction of minutes.
 V is either N or S and H is either E or W

 Functions in this file format the data string for a data message

 Functions in this file format the location, position, speed, altitude data for a position message
 */

//char *lat = "4228.33N";
char lat[] = "9000.00N  ";  // place holder
//const char *lon = "08756.39WO089/033"; // long O course / Speed
//char *lon = "08756.39W"; 
char lon[] = "10000.00W  ";  // place holder

#define LONGITUDE 1
const int LATITUDE =2;



void padThreeDig(int value, char cvalue[])
{ // Prepend 0 if there is only one or two digits.
  String str;
  if (value >= 100)
  {
    str = (String)value;
  }
  else if (value >= 10)
  {
    str = "0" + (String)value;
  }
  else if (value >= 0)
  {
    str = "00" + (String)value;
  }
  strcpy(cvalue, str.c_str());
  return;
}

void padTwoDig(int value,char cvalue[])
{ // Prepend 0 if there is only one digit.
  String str = "";
  if (value >= 10)
  {
    str = (String)value;
  }
  else
  {
    str = "0" + (String)value;
  }
  strcpy(cvalue, str.c_str());
  return;
}
char cTime[]="000000h";
void APRSFormatTime(int hrs, int min, int sec)
{ // HHMMSShLAT

  char MMM[]="00";
  char SS[]="00";
  char HH[]="00";
  char h[]= "h";

  padTwoDig(hrs,HH);
  padTwoDig(min,MMM);
  padTwoDig(sec,SS);
  cTime[0] = '\0';
  strcat(cTime,HH);
  strcat(cTime,MMM);
  strcat(cTime,SS);
  strcat(cTime,h);
  return;
}

char APRSinfo[101];
void APRSSensorInfoInit()
{
  APRSinfo[0] = '\0';
  
}

void APRSSetSensorInfo(int iTxC,float fC,float fhPa,int iS,int iY,int iM,int iD) 
{
  //cpu Temp, Temp, pressure, number of satellites, year, month, day
  // The first four variables will be parsed automatically by SoundHub
  String TxC = "TxC";
  String C = "C";
  String hPa = "hPa";
  String S = "S";
  String sp = " ";
  String p = ".";
  String dash = "-";
  String sData;
  String siC;
  siC = (String)(iTxC);
  int iC = fC;
  int idC = abs((fC -iC)*100);
  String sC = (String)iC+(String)p+(String)idC ;
  int ihPa = fhPa;
  int idhPa = (fhPa - ihPa)*100;
  String shPa = (String)ihPa+(String)p+(String)idhPa;

  sData = sp+(String)iTxC+TxC+sp+sC+C+sp+shPa+hPa+sp+(String)iS+S+sp+(String)iY+dash+(String)iM+dash+(String)iD;
  strncpy(APRSinfo, sData.c_str(),100);

}

char cCorSpdAlt[] = "000/000/A=000000";
void APRSSetCourseSpeedAltitude(float course, float speed, float altitude)
{  // course in deg clockwise from N, speed in knots, altitude in ft
   // ccc/sss/A=000000
  int iSpeed = speed;
  int iCourse = course;
  int iAltitude = altitude;
  char cSpeed[] = "000";
  char altDesignator[] = "/A=";
  char cAltitude[] = "000000";
  char cCourse[] = "000";
  padThreeDig(iCourse, cCourse);
  padThreeDig(iSpeed, cSpeed);
  int i;
  for (i= 5; i >= 0; --i, iAltitude /= 10)
  { // pad 6 digits
    cAltitude[i] = (iAltitude%10) + '0';
  }

  cCorSpdAlt[0] = '\0';

  strncat(cCorSpdAlt, cCourse,3);
  strncat(cCorSpdAlt, "/",1);
  strncat(cCorSpdAlt, cSpeed,3);
  strncat(cCorSpdAlt, altDesignator,3);
  strncat(cCorSpdAlt, cAltitude,6);
  return ;

}


char APRSdata[257];
void APRSDataInit(char dtype)
{ // Initialize the APRSdata string with the data type in the first position

  APRSdata[0] = dtype;
  APRSdata[1] = '\0';
  
}

bool APRSDataAppendFloat(float value)
{  // Append a char string to the end of the data string.
  String str = (String)value;
  str = (String)APRSdata + (String)"," + str;
  if (str.length() > 250) return false;
  strcpy(APRSdata, str.c_str());
  return true;
}

bool APRSDataAppendInt(int value)
{  // Append a char string to the end of the APRSdata string.
  String str = (String)value;
  str = (String)APRSdata + (String)"," + str;
  if (str.length() > 250) return false;
  strcpy(APRSdata, str.c_str());
  return true;

}

bool APRSDataAppendChars(char* value)
{  // Append a char string to the end of the APRSdata string.
  String str = (String)value;
  str = (String)APRSdata + (String)"," + str;
  if (str.length() > 250) return false;

  strcpy(APRSdata, str.c_str());
  return true;
}

void APRSDegMin(float degrees, int Direction)
{
  // Converts location in degrees to dddmm.mm for APRS message
  // Negative is converted to S and W 
  char Compass[] = "D";
  int DEG = int(degrees);
  float min = abs(degrees - DEG) * 60;
  int MIN = min;
  int DMIN = (min - MIN) * 100; // fraction of minute
  char cMIN[] = "00";
  char cDMIN[] = "00";
  char cDEG[] = "000";
  char* latlon;

  padTwoDig(MIN,cMIN);
  padTwoDig(DMIN,cDMIN);

  String DNS = "";
  Compass[0]='\0';

  if (Direction == LONGITUDE)
  {
    latlon = lon;
    padThreeDig(abs(DEG),cDEG);
  }
  else
  {
    latlon = lat;
    padTwoDig(abs(DEG),cDEG);
  }

  if (Direction == LATITUDE)
  {

    if (degrees > 0)
      strcpy(Compass,"N");
    else
      strcpy(Compass,"S");
  }
  if (Direction == LONGITUDE)
  {  // the O indicates that the message is from a balloon
    if (degrees > 0)
      strcpy(Compass,"E");
    else
      strcpy(Compass,"W");  
  }
  latlon[0] = '\0';
  strcat(latlon,cDEG);
  strcat(latlon,cMIN);
  strcat(latlon,".");
  strcat(latlon,cDMIN);
  strcat(latlon,Compass);

  return;
}

/*
Sets APRS Latitude and Longitude in the correct format for APRS dddmm.mm
Sets the correct local APRS frequency
Returns true if APRS can be legally transmitted at this location
*/
void APRSLatLong(float latitude, float longitude)
{

  APRSDegMin(latitude, LATITUDE);
  APRSDegMin(longitude, LONGITUDE);
  
}

