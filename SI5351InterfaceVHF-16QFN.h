/*
   Low level functions that perform the final message encoding and interface with the SI5351 Transmitter
   All register settings assume a 27MHz clock
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
unsigned long long GEOFENCE_APRS_frequency = 0;

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

void Reg_write_error(const char *location, int regStatus)
{
#ifdef DEBUG
  Serial.println(location);
  Serial.println((int)regStatus);
#endif
}

void Set_drive(DriveAmp amps)
{

  si5351b_revb_register_t DriveApmRegisters[3][2] =
      // 4ma
      {{{0x0015, 0x6D},
        {0x0017, 0x3D}},

       // 6ma
       {{0x0015, 0x6E},
        {0x0017, 0x3E}},

       // 8ma
       {{0x0015, 0x6F},
        {0x0017, 0x3F}}};

  for (int i = 0; i < 2; i++)
  {
    uint8_t regStatus = si5351.si5351_write(DriveApmRegisters[amps][i].address, DriveApmRegisters[amps][i].value);
    if (regStatus != 0)
    {
      Reg_write_error(" XXX Drive Amp Registers Write error Set_drive ", regStatus);
    }

    delay(1);
  }
}

void VHF_init()
{

  const int num_reg = 43;

  si5351b_revb_register_t const si5351b_revb_registers[num_reg] =
      {
          // default set is for Clock 2 and 3 inverted 144.39 MHz
          {0x0002, 0x33},
          {0x0003, 0x00},
          {0x0004, 0x10},
          {0x0007, 0x01},
          {0x000F, 0x00},
          {0x0010, 0x8C},
          {0x0011, 0x8C},
          {0x0012, 0x8C},
          {0x0013, 0x8C},
          {0x0014, 0x8C},
          {0x0015, 0x6F},
          {0x0016, 0x8C},
          {0x0017, 0x3F},
          {0x0022, 0x42},
          {0x0023, 0x72},
          {0x0024, 0x00},
          {0x0025, 0x0E},
          {0x0026, 0x0B},
          {0x0027, 0xF1},
          {0x0028, 0x6C},
          {0x0029, 0x9A},
          {0x0052, 0x00},
          {0x0053, 0x01},
          {0x0054, 0x00},
          {0x0055, 0x01},
          {0x0056, 0x00},
          {0x0057, 0x00},
          {0x0058, 0x00},
          {0x0059, 0x00},
          {0x005A, 0x00},
          {0x005B, 0x06},
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
          {0x00AA, 0x00},
          {0x00B7, 0x92},

      };
  for (int i = 0; i < num_reg; i++)
  {
    uint8_t regStatus = si5351.si5351_write(si5351b_revb_registers[i].address, si5351b_revb_registers[i].value);

    if (regStatus != 0)
    {
      Reg_write_error(" XXX Register Write error VHF_init ", regStatus);
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
      {{{0x0022, 0x42},
        {0x0023, 0x40},
        {0x0026, 0x0C},
        {0x0027, 0xFC},
        {0x0028, 0x35},
        {0x0029, 0x00},
        {0x00A2, 0x05}},

       // 144.80
       {{0x0022, 0x42},
        {0x0023, 0x63},
        {0x0026, 0x16},
        {0x0027, 0xFB},
        {0x0028, 0x87},
        {0x0029, 0x7E},
        {0x00A2, 0x09}},

       // 144.39
       {{0x0022, 0x42},
        {0x0023, 0x72},
        {0x0026, 0x0B},
        {0x0027, 0xF1},
        {0x0028, 0x6C},
        {0x0029, 0x9A},
        {0x00A2, 0x04}},

       // 145.01
       {{0x0022, 0x43},
        {0x0023, 0x9E},
        {0x0026, 0x1C},
        {0x0027, 0xFB},
        {0x0028, 0x20},
        {0x0029, 0x38},
        {0x00A2, 0x0B}},

       // 145.525
       {{0x0022, 0x42},
        {0x0023, 0x90},
        {0x0026, 0x2B},
        {0x0027, 0xF5},
        {0x0028, 0xC3},
        {0x0029, 0xD0},
        {0x00A2, 0x11}},

       // 145.57
       {{0x0022, 0x43},
        {0x0023, 0x9E},
        {0x0026, 0x2C},
        {0x0027, 0xFA},
        {0x0028, 0x0A},
        {0x0029, 0x58},
        {0x00A2, 0x11}},

       // 144.93
       {{0x0022, 0x42},
        {0x0023, 0x72},
        {0x0026, 0x1A},
        {0x0027, 0xF6},
        {0x0028, 0xEA},
        {0x0029, 0xEC},
        {0x00A2, 0x0A}},

       // 144.64
       {{0x0022, 0x42},
        {0x0023, 0xBD},
        {0x0026, 0x12},
        {0x0027, 0xF3},
        {0x0028, 0x1E},
        {0x0029, 0xB6},
        {0x00A2, 0x07}},

       // 144.66
       {{0x0022, 0x42},
        {0x0023, 0x72},
        {0x0026, 0x12},
        {0x0027, 0xFB},
        {0x0028, 0xCC},
        {0x0029, 0xFC},
        {0x00A2, 0x07}},

       // 144.62
       {{0x0022, 0x42},
        {0x0023, 0xBD},
        {0x0026, 0x11},
        {0x0027, 0xF9},
        {0x0028, 0xB2},
        {0x0029, 0xF3},
        {0x00A2, 0x07}},

       // 145.175
       {{0x0022, 0x42},
        {0x0023, 0x90},
        {0x0026, 0x21},
        {0x0027, 0xF6},
        {0x0028, 0x71},
        {0x0029, 0x70},
        {0x00A2, 0x0D}},

       // 144.575
       {{0x0022, 0x42},
        {0x0023, 0x90},
        {0x0026, 0x10},
        {0x0027, 0xF5},
        {0x0028, 0x6D},
        {0x0029, 0x00},
        {0x00A2, 0x06}}};

  for (int i = 0; i < num_reg; i++)
  {
    uint8_t regStatus = si5351.si5351_write(APRSFreqRegisters[Frequency][i].address, APRSFreqRegisters[Frequency][i].value);
    if (regStatus != 0)
    {
      Reg_write_error(" XXX APRSFreqRegisters Write error Set_ frequency  ", regStatus);
    }
  }

  si5351.pll_reset(SI5351_PLLB);

  return;
}

bool SetTo14445 = false;
void SetFrequency14445()
{ // This frequency is not used by any country
  // Potentially used for picture downloads
  Set_frequency(F14445);
  SetTo14445 = true;
}

void VHF_off()
{ // turn the power off for clk2&3
  uint8_t regStatus = si5351.si5351_write(0x0003, 0xA0);
}

void VHF_on()
{

  pinMode(VXCO_PIN, OUTPUT);
  digitalWrite(VXCO_PIN, LOW);

  VHF_init();
  Set_drive(DRIVE_6ma);
  if (SetTo14445 == false)
  {
    Set_frequency(GEOFENCE_Freq); // set in geofence.h
  }
  SetTo14445 = false;  // set in SetFrequency14445
}

void APRSon()
{
  // POUTPUTLN((F(" APRSon start ")));
  VHF_on(); // This needs to be done for each transmission.
  digitalWrite(VXCO_PIN, LOW);
  si5351.output_enable(XMIT_CLK0, 1); // enable the output
  si5351.output_enable(XMIT_CLK1, 1);
  delay(2);
  POUTPUTLN((F(" APRS ON ")));
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

  POUTPUTLN((F(" APRS OFF ")));
}
