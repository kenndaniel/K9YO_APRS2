
// const char *dest = "APRS";
//const char *dest = "APX879";  // Experimental packet 879
const char *dest_balloon = "APLIGA";  // LightAPRS Tracker used for Soundhub upload _BALLOON
const char *dest_beacon = "BEACON";  // Used with _BEACON
const char *dest = "APX783";  // Experimantal packet APXxxx  (xxx any number) _NORMAL

#define _1200 1
#define _2400 0

#define _FLAG 0x7e
#define _CTRL_ID 0x03
#define _PID 0xf0
#define _DT_EXP ','
#define _DT_STATUS '>'
#define _DT_POS '!'
#define _DT_DATA '{'
#define _DT_AT '@'

#define _NORMAL 1   // sends dest as destination 
#define _BEACON 2  // Sends dest_beacon as destination

#define _POSITION 1  // Sends only position information
#define _STATUS 2  // Sends only status information
#define _POSITION_STATUS 3  // Sends position and status infomaton
#define _DATA 4  // Sends only data

// Standard balloon spot - simulates qrplabs APRSLite tracker for Soundhub
// Sends dest_balloon in destination position
#define _BALLOON  5   

bool nada = _2400;

/*
 * SQUARE WAVE SIGNAL GENERATION
 *
 * baud_adj lets you to adjust or fine tune overall baud rate
 * by simultaneously adjust the 1200 Hz and 2400 Hz tone,
 * so that both tone would scales synchronously.
 * adj_1200 determined the 1200 hz tone adjustment.
 * tc1200 is the half of the 1200 Hz signal periods.
 *
 *      -------------------------                           -------
 *     |                         |                         |
 *     |                         |                         |
 *     |                         |                         |
 * ----                           -------------------------
 *
 *     |<------ tc1200 --------->|<------ tc1200 --------->|
 *
 * adj_2400 determined the 2400 hz tone adjustment.
 * tc2400 is the half of the 2400 Hz signal periods.
 *
 *      ------------              ------------              -------
 *     |            |            |            |            |
 *     |            |            |            |            |
 *     |            |            |            |            |
 * ----              ------------              ------------
 *
 *     |<--tc2400-->|<--tc2400-->|<--tc2400-->|<--tc2400-->|
 *
 */
const float baud_adj = .975;
const float adj_1200 = 1.0 * baud_adj;
const float adj_2400 = 1.0 * baud_adj;
unsigned int tc1200 = (unsigned int)(0.5 * adj_1200 * 1000000.0 / 1200.0);
unsigned int tc2400 = (unsigned int)(0.5 * adj_2400 * 1000000.0 / 2400.0);
char myssid = APRS_SID;




const char *digi = "WIDE2";  // WIDE2-1 will only contact wide area repeaters
char digissid = 2; //two hops  wide2-2
const char sym_ovl = '/';
char sym_tab = '\\';
const char sym_space = ' ';
const char sym_balloon = 'O';

//unsigned int tx_delay = 15000;
unsigned int str_len = 400;

char bit_stuff = 0;
unsigned short crc = 0xffff;


/*
 *
 */
void set_nada_1200(void);
void set_nada_2400(void);
void set_nada(bool nada);

void send_char_NRZI(unsigned char in_byte, bool enBitStuff);
void send_string_len(const char *in_string, int len);

void calc_crc(bool in_bit);
void send_crc(void);

void send_packet(char packet_type, char dest_type);
void send_flag(unsigned char flag_len);
void send_header(char msg_type);
void send_payload(char type);

void set_io(void);
void print_code_version(void);
void print_debug(char type, char dest_type);



/*
 *
 */
void set_nada_1200(void)
{
  transmitAPRS(HIGH);
  delayMicroseconds(tc1200);
  transmitAPRS(LOW);
  delayMicroseconds(tc1200);
}

void set_nada_2400(void)
{
  transmitAPRS(HIGH);
  delayMicroseconds(tc2400);
  transmitAPRS(LOW);
  delayMicroseconds(tc2400);

  transmitAPRS(HIGH);
  delayMicroseconds(tc2400);
  transmitAPRS(LOW);
  delayMicroseconds(tc2400);
}

void set_nada(bool nada)
{
  if (nada)
    set_nada_1200();
  else
    set_nada_2400();
}

/*
 * This function will calculate CRC-16 CCITT for the FCS (Frame Check Sequence)
 * as required for the HDLC frame validity check.
 *
 * Using 0x1021 as polynomial generator. The CRC registers are initialized with
 * 0xFFFF
 */
void calc_crc(bool in_bit)
{
  unsigned short xor_in;

  xor_in = crc ^ in_bit;
  crc >>= 1;

  if (xor_in & 0x01)
    crc ^= 0x8408;
}

void send_crc(void)
{
  unsigned char crc_lo = crc ^ 0xff;
  unsigned char crc_hi = (crc >> 8) ^ 0xff;

  send_char_NRZI(crc_lo, HIGH);
  send_char_NRZI(crc_hi, HIGH);
}

void send_header(char msg_type)
{
  char temp;

  /*
   * APRS AX.25 Header
   * ........................................................
   * |   DEST   |  SOURCE  |   DIGI   | CTRL FLD |    PID   |
   * --------------------------------------------------------
   * |  7 bytes |  7 bytes |  7 bytes |   0x03   |   0xf0   |
   * --------------------------------------------------------
   *
   * DEST   : 6 byte "callsign" + 1 byte ssid
   * SOURCE : 6 byte your callsign + 1 byte ssid
   * DIGI   : 6 byte "digi callsign" + 1 byte ssid
   *
   * ALL DEST, SOURCE, & DIGI are left shifted 1 bit, ASCII format.
   * DIGI ssid is left shifted 1 bit + 1
   *
   * CTRL FLD is 0x03 and not shifted.
   * PID is 0xf0 and not shifted.
   */

  /********* DEST ***********/
  if (msg_type == _NORMAL)
  {
    temp = strlen(dest);
    for (int j = 0; j < temp; j++)
      send_char_NRZI(dest[j] << 1, HIGH);
  }
  else if (msg_type == _BEACON)
  {
    temp = strlen(dest_beacon);
    for (int j = 0; j < temp; j++)
      send_char_NRZI(dest_beacon[j] << 1, HIGH);
  }
    else if (msg_type == _BALLOON)
  {
    temp = strlen(dest_balloon);
    for (int j = 0; j < temp; j++)
      send_char_NRZI(dest_balloon[j] << 1, HIGH);
  }

  if (temp < 6)
  {
    for (int j = 0; j < (6 - temp); j++)
      send_char_NRZI(' ' << 1, HIGH);
  }
  send_char_NRZI('0' << 1, HIGH);

  /********* SOURCE *********/
  temp = strlen(call);
  for (int j = 0; j < temp; j++)
    send_char_NRZI(call[j] << 1, HIGH);
  if (temp < 6)
  {
    for (int j = 0; j < (6 - temp); j++)
      send_char_NRZI(' ' << 1, HIGH);
  }
  send_char_NRZI((myssid + '0') << 1, HIGH);

  /********* DIGI ***********/
  temp = strlen(digi);
  for (int j = 0; j < temp; j++)
    send_char_NRZI(digi[j] << 1, HIGH);
  if (temp < 6)
  {
    for (int j = 0; j < (6 - temp); j++)
      send_char_NRZI(' ' << 1, HIGH);
  }
  send_char_NRZI(((digissid + '0') << 1) + 1, HIGH);

  /***** CTRL FLD & PID *****/
  send_char_NRZI(_CTRL_ID, HIGH);
  send_char_NRZI(_PID, HIGH);
}

void send_payload(char type)
{
  /*
   * APRS AX.25 Payloads
   *
   * TYPE : POSITION
   * ........................................................
   * |DATA TYPE |    LAT   |SYMB. OVL.|    LON   |SYMB. TBL.|
   * --------------------------------------------------------
   * |  1 byte  |  8 bytes |  1 byte  |  9 bytes |  1 byte  |
   * --------------------------------------------------------
   *
   * DATA TYPE  : !
   * LAT        : ddmm.mmN or ddmm.mmS
   * LON        : dddmm.mmE or dddmm.mmW
   *
   *
   * TYPE : STATUS
   * ..................................
   * |DATA TYPE |    STATUS TEXT      |
   * ----------------------------------
   * |  1 byte  |       N bytes       |
   * ----------------------------------
   *
   * DATA TYPE  : >
   * STATUS TEXT: Free form text
   *
   *
   * TYPE : POSITION & STATUS
   * ..............................................................................
   * |DATA TYPE |    LAT   |SYMB. OVL.|    LON   |SYMB. TBL.|    STATUS TEXT      |
   * ------------------------------------------------------------------------------
   * |  1 byte  |  8 bytes |  1 byte  |  9 bytes |  1 byte  |       N bytes       |
   * ------------------------------------------------------------------------------
   *
   * DATA TYPE  : !
   * LAT        : ddmm.ssN or ddmm.ssS
   * LON        : dddmm.ssE or dddmm.ssW
   * STATUS TEXT: Free form text
   *
   * TYPE : EXTENDED POSITION & STATUS
   * "089/033/A=001234";   course in deg / speed in knots /A= altitude in feet
   * ..............................................................................
   * |DATA TYPE |    LAT   |SYMB. OVL.|    LON   |SYMB. TBL.|COURSE| 
   * ------------------------------------------------------------------------------
   * |  1 byte  |  8 bytes |  1 byte  |  9 bytes |  1 byte  | 3 bytes  |
   * ------------------------------------------------------------------------------
   *
     * ......................................................................................
   * |SYMB. OVL. |   SPEED  |SYMB. OVL.| DATA TYPE* |ALTITUDE| SPACE |INFO TEXT      |
   * -----------------------------------------------------------------------------------------
   * |  1 byte  |  3 bytes |  1 byte  |  2 bytes |  6 bytes  | 1 byte  |       N bytes       |
   * -----------------------------------------------------------------------------------------
   *
   * 
   * DATA TYPE  : !
   * LAT        : ddmm.ssN or ddmm.ssS
   * LON        : dddmm.ssE or dddmm.ssW
   * COURSE     : degrees clockwise from North
   * SPEED      : speed in knots
   * DATA TYPE* : A=
   * ALTITUDE   : feet
   * STATUS TEXT: Free form text
   * 
   * 
   * TYPE : USER DEFINED DATA FORMAT
  * .........................................................
   * |DATA TYPE |    UID   |PACKET TYPE |    DATA TEXT      |
   * --------------------------------------------------------
   * |  1 byte  |  1 bytes |  1 byte     |    N bytes       |
   * --------------------------------------------------------
   * 
   * DATA TYPE  : {
   * UID        : {
   * PACKET TYPE : USER DEFINED ASCII PRINTABLE CHARACTER e.g. B
   * DATA TEST   : Free from text (ASCII Printable) N<250
   *
   * 
   * All of the data are sent in the form of ASCII Text, not shifted.
   *
   */
  if (type == _POSITION)
  {
    send_char_NRZI(_DT_POS, HIGH);
    send_string_len(lat, strlen(lat));
    send_char_NRZI(sym_ovl, HIGH);
    send_string_len(lon, strlen(lon));
    send_char_NRZI(sym_tab, HIGH);
  }
  else if (type == _STATUS)
  {
    send_char_NRZI(_DT_STATUS, HIGH);
    send_string_len(mystatus, strlen(mystatus));
  }
  else if (type == _POSITION_STATUS)
  {
    send_char_NRZI(_DT_POS, HIGH); 
    send_string_len(lat, strlen(lat));
    send_char_NRZI(sym_ovl, HIGH);
    send_string_len(lon, strlen(lon));
    send_char_NRZI(sym_tab, HIGH);
    send_char_NRZI(' ', HIGH);
    send_string_len(mystatus, strlen(mystatus));
  }
  else if (type == _BALLOON)
  {
    send_char_NRZI(_DT_AT, HIGH);
    send_string_len(cTime, strlen(cTime));   
    send_string_len(lat, strlen(lat));
    send_char_NRZI(sym_ovl, HIGH);
    send_string_len(lon, strlen(lon));
    send_char_NRZI(sym_balloon, HIGH);  // will appear as a balloon on aprs.fi
    send_string_len(cCorSpdAlt, strlen(cCorSpdAlt));
    send_char_NRZI(' ', HIGH);
    send_string_len(APRSinfo, strlen(APRSinfo));
  }
    else if (type == _DATA)
  {
    
    send_char_NRZI(_DT_DATA, HIGH);
    send_char_NRZI(_DT_DATA, HIGH);
    send_string_len(APRSdata, strlen(APRSdata));
  }

}

/*
 * This function will send one byte input and convert it
 * into AFSK signal one bit at a time LSB first.
 *
 * The encode which used is NRZI (Non Return to Zero, Inverted)
 * bit 1 : transmitted as no change in tone
 * bit 0 : transmitted as change in tone
 */
void send_char_NRZI(unsigned char in_byte, bool enBitStuff)
{
  bool bits;

  for (int i = 0; i < 8; i++)
  {
    bits = in_byte & 0x01;

    calc_crc(bits);

    if (bits)
    {
      set_nada(nada);
      bit_stuff++;

      if ((enBitStuff) && (bit_stuff == 5))
      {
        nada ^= 1;
        set_nada(nada);

        bit_stuff = 0;
      }
    }
    else
    {
      nada ^= 1;
      set_nada(nada);

      bit_stuff = 0;
    }

    in_byte >>= 1;
  }
}

void send_string_len(const char *in_string, int len)
{
  for (int j = 0; j < len; j++)
    send_char_NRZI(in_string[j], HIGH);
}

void send_flag(unsigned char flag_len)
{
  for (int j = 0; j < flag_len; j++)
    send_char_NRZI(_FLAG, LOW);
}

  /*
   * Sends the AX25 FRAME
   *
   * ........................................................
   * |  FLAG(s) |  HEADER  | PAYLOAD  | FCS(CRC) |  FLAG(s) |
   * --------------------------------------------------------
   * |  1 bytes | 22 bytes |  N bytes | 2 bytes  |  1 bytes |
   * --------------------------------------------------------
   *
   * FLAG(s)  : 0x7e
   * HEADER   : see header
   * PAYLOAD  : 1 byte data type + N byte info
   * FCS      : 2 bytes calculated from HEADER + PAYLOAD
   */
void send_packet( char dest_type , char packet_type )
{
  digitalWrite(LED_BUILTIN, 1);


  APRSon();
  send_flag(_FLAG); 
  crc = 0xffff;
  send_header(dest_type);
  send_payload(packet_type);
  send_crc();
  send_flag(_FLAG); 
  APRSoff();
  digitalWrite(LED_BUILTIN, 0);
}



/*
 *
 */
void set_io(void)
{
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
}

void print_code_version(void)
{
  #ifdef DEBUG
  Serial.println(" ");
  Serial.print("Sketch:   ");
  Serial.println(__FILE__);
  Serial.print("Uploaded: ");
  Serial.println(__DATE__);
  Serial.println(" ");
  #endif
}

void print_debug( int dest_type, int type)
{
  /*
   * PROTOCOL DEBUG.
   *
   * Will outputs the transmitted data to the serial monitor
   * in the form of TNC2 string format.
   *
   * call-N>APRS,DIGIn-N:<PAYLOAD STRING> <CR><LF>
   */
  /****** MYCALL ********/
  #ifdef DEBUG
  Serial.print(call);
  Serial.print('-');
  Serial.print(myssid, DEC);
  Serial.print('>');

  /******** DEST ********/
  if (dest_type == _NORMAL)
  {
    Serial.print(dest);
  }
  else if (dest_type == _BEACON)
  {
    Serial.print(dest_beacon);
  }
    else if (dest_type == _BALLOON)
  {
    Serial.print(dest_balloon);
  }
  Serial.print(',');

  /******** DIGI ********/
  Serial.print(digi);
  Serial.print('-');
  Serial.print(digissid, DEC);
  Serial.print(':');

  /******* PAYLOAD ******/
  if (type == _POSITION)
  {
    Serial.print(_DT_POS);
    Serial.print(lat);
    Serial.print(sym_ovl);
    Serial.print(lon);
    Serial.print(sym_tab);
  }
  else if (type == _STATUS)
  {
    Serial.print(_DT_STATUS);
    Serial.print(mystatus);
  }
  else if (type == _POSITION_STATUS)
  {
    Serial.print(_DT_POS);
    Serial.print(lat);
    Serial.print(sym_ovl);
    Serial.print(lon);
    Serial.print(sym_tab);

    Serial.print(' ');
    Serial.print(mystatus);
  }
    else if (type == _BALLOON)
  {
    Serial.print(_DT_AT);
    Serial.print(cTime);
    Serial.print(lat);
    Serial.print(sym_ovl);
    Serial.print(lon);
    Serial.print(sym_balloon);
    Serial.print(cCorSpdAlt);
    Serial.print(sym_ovl);
    Serial.print(" ");
    Serial.print(APRSinfo);
  }
    else if (type == _DATA)
  {
    Serial.print(_DT_DATA);
    Serial.print(_DT_DATA);
    Serial.print(APRSdata);
  }

  Serial.println(' ');
  #endif
}



int ucount = 0;
float radius = .05;
float latitude0 = latitude;
float longitude0 = longitude;
void Simulate_flight()
{
  /* Test function to fly a fake balloon in a circle around latitude0 and longitude0 */

  float dlon = sin(2 * 3.1416 * ucount / 20.) * radius;
  float dlat = cos(2 * 3.1416 * ucount / 20.) * radius;
  // Serial.println(" dlat dlon");
  POUTPUT((ucount));
  POUTPUT((" "));
  POUTPUT((dlat));
  POUTPUT((" "));
  POUTPUTLN((dlon));
  latitude = latitude0 + dlat + .015;
  longitude = longitude0 + dlon;
  ++ucount;
  radius += .001;
}
