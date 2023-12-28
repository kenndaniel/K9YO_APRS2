/*
   Low level functions that perform the final message encoding and interface with the SI5351 Transmitter

 */

// unsigned long long freqAPRSTest = APRS_DEBUG_FREQ;

// Previous breadboard was CLK2&3
// V5 CLK0 & 1
#define XMIT_CLK0 SI5351_CLK2
#define XMIT_CLK1 SI5351_CLK3

enum APRSFreqs // world wide APRS frequencies
{
  F14445,
  F1448,
  F14439,
  F14501,
  F145525,
  F14557,
  F14493,
  F14464,
  F14466,
  F14462,
  F145175,
  F144575
};

// set in geofence.h
APRSFreqs GEOFENCE_Freq = F14445;
unsigned long long GEOFENCE_APRS_frequency;

typedef struct
{
  unsigned int address; /* 16-bit register address */
  unsigned char value;  /* 8-bit register data */

} si5351b_revb_register_t;

enum DriveAmp
{
  DRIVE_4ma,
  DRIVE_6ma,
  DRIVE_8ma,
};

void Set_drive(DriveAmp amps)
{

  si5351b_revb_register_t DriveApmRegisters[3][2] =
      // 4ma
      {{{0x0012, 0x6D},
        {0x0013, 0x7D}},

       // 6ma
       {{0x0012, 0x6E},
        {0x0013, 0x7E}},

       // 8ma
       {{0x0012, 0x6F},
        {0x0013, 0x7F}}};

  for (int i = 0; i < 2; i++)
  {
    uint8_t regStatus = si5351.si5351_write(DriveApmRegisters[amps][i].address, DriveApmRegisters[amps][i].value);
    if (regStatus != 0)
    {
      Serial.print(" XXX DriveApmRegisters Write error ");
      Serial.println((int)regStatus);
    }

    delay(1);
  }
}

void VHF_init()
{

  // Start I2C comms
  Wire.begin();

  // Check for a device on the bus, bail out if it is not there
  Wire.beginTransmission(0x60);
  uint8_t reg_val;
  reg_val = Wire.endTransmission();

  if (reg_val != 0)
  {
    Serial.println(" Si5351 not found on I2C bus");
    delay(1200000);
  }

  // Wait for SYS_INIT flag to be clear, indicating that device is ready
  uint8_t status_reg = 0;
  do
  {
    status_reg = si5351.si5351_read(SI5351_DEVICE_STATUS);
  } while (status_reg >> 7 == 1);

  const int num_reg = 52;

  si5351b_revb_register_t const si5351b_revb_registers[num_reg] =
      {
          {0x0002, 0x33},
          {0x0003, 0x00},
          {0x0004, 0x10},
          {0x0007, 0x01},
          {0x000F, 0x00},
          {0x0010, 0x8C},
          {0x0011, 0x8C},
          {0x0012, 0x6F},
          {0x0013, 0x7F},
          {0x0014, 0x8C},
          {0x0015, 0x8C},
          {0x0016, 0x8C},
          {0x0017, 0x8C},
          {0x0022, 0x42},
          {0x0023, 0x72},
          {0x0024, 0x00},
          {0x0025, 0x0E},
          {0x0026, 0x0B},
          {0x0027, 0xF1},
          {0x0028, 0x6C},
          {0x0029, 0x9A},
          {0x003A, 0x00},
          {0x003B, 0x01},
          {0x003C, 0x00},
          {0x003D, 0x01},
          {0x003E, 0x00},
          {0x003F, 0x00},
          {0x0040, 0x00},
          {0x0041, 0x00},
          {0x0042, 0x00},
          {0x0043, 0x01},
          {0x0044, 0x00},
          {0x0045, 0x01},
          {0x0046, 0x00},
          {0x0047, 0x00},
          {0x0048, 0x00},
          {0x0049, 0x00},
          {0x005A, 0x00},
          {0x005B, 0x00},
          {0x0095, 0x00},
          {0x0096, 0x00},
          {0x0097, 0x00},
          {0x0098, 0x00},
          {0x0099, 0x00},
          {0x009A, 0x00},
          {0x009B, 0x00},
          {0x00A2, 0x04},
          {0x00A3, 0x38},
          {0x00A4, 0x03},
          {0x00A7, 0x00},
          {0x00A8, 0x00},
          {0x00B7, 0xD2},

      };
  /*  {
       // default set is for Clock 2 and 3 inverted 144.39 MHz
       {0x0002, 0x33},
       {0x0003, 0x00},
       {0x0004, 0x10},
       {0x0007, 0x01},
       {0x000F, 0x00},
       {0x0010, 0x8C},
       {0x0011, 0x8C},
       {0x0012, 0x6F},
       {0x0013, 0x7F},
       {0x0014, 0x8C},
       {0x0015, 0x8C},
       {0x0016, 0x8C},
       {0x0017, 0x8C},
       {0x0022, 0x42},
       {0x0023, 0x40},
       {0x0024, 0x00},
       {0x0025, 0x0F},
       {0x0026, 0x53},
       {0x0027, 0xFA},
       {0x0028, 0x15},
       {0x0029, 0x40},
       {0x003A, 0x00},
       {0x003B, 0x01},
       {0x003C, 0x00},
       {0x003D, 0x01},
       {0x003E, 0x00},
       {0x003F, 0x00},
       {0x0040, 0x00},
       {0x0041, 0x00},
       {0x0042, 0x00},
       {0x0043, 0x01},
       {0x0044, 0x00},
       {0x0045, 0x01},
       {0x0046, 0x00},
       {0x0047, 0x00},
       {0x0048, 0x00},
       {0x0049, 0x00},
       {0x005A, 0x00},
       {0x005B, 0x00},
       {0x0095, 0x00},
       {0x0096, 0x00},
       {0x0097, 0x00},
       {0x0098, 0x00},
       {0x0099, 0x00},
       {0x009A, 0x00},
       {0x009B, 0x00},
       {0x00A2, 0xA1},
       {0x00A3, 0x6B},
       {0x00A4, 0x03},
       {0x00A7, 0x00},
       {0x00A8, 0x00},
       {0x00B7, 0xD2},

   }; */

  for (int i = 0; i < num_reg; i++)
  {
    uint8_t regStatus = si5351.si5351_write(si5351b_revb_registers[i].address, si5351b_revb_registers[i].value);
    if (regStatus != 0)
    {
      Serial.print(" XXX Register Write error ");
      Serial.println((int)regStatus);
    }

    delay(1);
  }
  si5351.pll_reset(SI5351_PLLB);
  return;
}

void Set_frequency(APRSFreqs Frequency)
{

  int num_reg = 7;
  si5351b_revb_register_t APRSFreqRegisters[12][num_reg] =
      // si5351-16QFN does not propely set frequency.  Consequently regs are set manually
      // Register values for Clock 2 and 3(inverted)
      // 144.45 Picture Download and test frequency
      {

          {{0x0022, 0x42},   // This section only for 27MHz test
           {0x0023, 0x40},
           {0x0026, 0x0C},
           {0x0027, 0xFC},
           {0x0028, 0x35},
           {0x0029, 0x00},
           {0x00A2, 0x05}},

/*           {{0x0022, 0x42},
           {0x0023, 0x40},
           {0x0026, 0x55},
           {0x0027, 0xF7},
           {0x0028, 0xB0},
           {0x0029, 0xC0},
           {0x00A2, 0xA2}},
 */
          // 144.80 Done
          {{0x0022, 0x42},
           {0x0023, 0x40},
           {0x0026, 0x60},
           {0x0027, 0xF3},
           {0x0028, 0xE8},
           {0x0029, 0x00},
           {0x00A2, 0xA6}},

          // 144.39
          {{0x0022, 0x42},
           {0x0023, 0x40},
          {0x0026, 0x0B},
          {0x0027, 0xF1},
          {0x0028, 0x6C},
          {0x0029, 0x9A},
           {0x00A2, 0x04}},



/*           {{0x0022, 0x42},
           {0x0023, 0x40},
           {0x0026, 0x53},
           {0x0027, 0xFA},
           {0x0028, 0x15},
           {0x0029, 0x40},
           {0x00A2, 0xA1}}, */

          // 145.01
          {{0x0022, 0x42},
           {0x0023, 0x40},
           {0x0026, 0x66},
           {0x0027, 0xFA},
           {0x0028, 0xCA},
           {0x0029, 0x80},
           {0x00A2, 0xA9}},

          // 145.525
          {{0x0022, 0x42},
           {0x0023, 0x40},
           {0x0026, 0x76},
           {0x0027, 0xF8},
           {0x0028, 0x0E},
           {0x0029, 0x80},
           {0x00A2, 0xAF}},

          // 145.57
          {{0x0022, 0x42},
           {0x0023, 0x40},
           {0x0026, 0x77},
           {0x0027, 0xFD},
           {0x0028, 0xE4},
           {0x0029, 0x40},
           {0x00A2, 0xB0}},

          // 144.93
          {{0x0022, 0x42},
           {0x0023, 0x40},
           {0x0026, 0x64},
           {0x0027, 0xF3},
           {0x0028, 0xCF},
           {0x0029, 0x00},
           {0x00A2, 0xA8}},

          // 144.64
          {{0x0022, 0x42},
           {0x0023, 0x40},
           {0x0026, 0x5B},
           {0x0027, 0xF5},
           {0x0028, 0x33},
           {0x0029, 0x40},
           {0x00A2, 0xA4}},

          // 144.66
          {{0x0022, 0x42},
           {0x0023, 0x40},
           {0x0026, 0x5B},
           {0x0027, 0xFE},
           {0x0028, 0x93},
           {0x0029, 0x40},
           {0x00A2, 0xA4}},

          // 144.62
          {{0x0022, 0x42},
           {0x0023, 0x40},
           {0x0026, 0x5A},
           {0x0027, 0xFB},
           {0x0028, 0x15},
           {0x0029, 0x80},
           {0x00A2, 0xA4}},

          // 145.175
          {{0x0022, 0x42},
           {0x0023, 0x40},
           {0x0026, 0x6B},
           {0x0027, 0xFB},
           {0x0028, 0xD7},
           {0x0029, 0x40},
           {0x00A2, 0xAB}},

          // 144.575
          {{0x0022, 0x42},
           {0x0023, 0x40},
           {0x0026, 0x59},
           {0x0027, 0xF5},
           {0x0028, 0x3F},
           {0x0029, 0xC0},
           {0x00A2, 0xA3}}};

  for (int i = 0; i < num_reg; i++)
  {
    uint8_t regStatus = si5351.si5351_write(APRSFreqRegisters[Frequency][i].address, APRSFreqRegisters[Frequency][i].value);
    if (regStatus != 0)
    {
      Serial.print(" XXX APRSFreqRegisters Write error ");
      Serial.println((int)regStatus);
    }
  }

  si5351.pll_reset(SI5351_PLLB);

  return;
}

void VHF_off()
{ // turn the power off for clk2&3
  Serial.println("VHF Off");
  si5351.output_enable(XMIT_CLK0, 0); // disable the output
  si5351.output_enable(XMIT_CLK1, 0);
}

void VHF_on()
{

  pinMode(VXCO_PIN, OUTPUT);
  digitalWrite(VXCO_PIN, LOW);

  VHF_init();
  //Set_drive(DRIVE_6ma);
  //Set_frequency(GEOFENCE_Freq); // set in geofence.h
}

void APRSon()
{
  // POUTPUTLN((F(" APRSon start ")));
  VHF_on(); // This needs to be done for each transmission.
  digitalWrite(VXCO_PIN, LOW);
  si5351.output_enable(XMIT_CLK0, 1); // enable the output
  si5351.output_enable(XMIT_CLK1, 1);
  delay(2);
}

void transmitAPRS(int MODE)
{

  if (MODE == LOW)
  {
    digitalWrite(VXCO_PIN, LOW);
  }
  else
  {
    digitalWrite(VXCO_PIN, HIGH);
  }
}

void APRSoff()
{
  digitalWrite(VXCO_PIN, LOW);
  VHF_off();

  // POUTPUTLN((F(" APRSon end ")));
}
