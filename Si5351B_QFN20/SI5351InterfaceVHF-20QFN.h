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
unsigned long long GEOFENCE_APRS_frequency = 0;

#ifndef APRS_WSPR
typedef struct
{
  unsigned int address; /* 16-bit register address */
  unsigned char value;  /* 8-bit register data */

} si5351b_revb_register_t;
#endif

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
      Reg_write_error(" XXX Drive Amp Registers Write error Set_drive ", regStatus);
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
          {0x00B7, 0xD2}

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

  int num_reg = 8;
  APRSFreqRegisters[12][num_reg][2] =
      // Register values for Clock 2 and 3(inverted)
      // 144.45 Picture Download and test frequency
      {
          {
              {0x0022, 0x42},
              {0x0023, 0x40},
              {0x0025, 0x0E},
              {0x0026, 0x0C},
              {0x0027, 0xFC},
              {0x0028, 0x35},
              {0x0029, 0x00},
              {0x00A2, 0x05},
          },

          // 144.80
          {
              {0x0022, 0x42},
              {0x0023, 0x63},
              {0x0025, 0x0E},
              {0x0026, 0x16},
              {0x0027, 0xFB},
              {0x0028, 0x87},
              {0x0029, 0x7E},
              {0x00A2, 0x09},
          },

          // 144.39
          {
              {0x0022, 0x42},
              {0x0023, 0x72},
              {0x0025, 0x0E},
              {0x0026, 0x0B},
              {0x0027, 0xF1},
              {0x0028, 0x6C},
              {0x0029, 0x9A},
              {0x00A2, 0x04},
          },

          // 145.01
          {
              {0x0022, 0x43},
              {0x0023, 0x9E},
              {0x0025, 0x0E},
              {0x0026, 0x1C},
              {0x0027, 0xFB},
              {0x0028, 0x20},
              {0x0029, 0x38},
              {0x00A2, 0x0B},
          },

          // 145.525
          {
              {0x0022, 0x42},
              {0x0023, 0x90},
              {0x0025, 0x0E},
              {0x0026, 0x2B},
              {0x0027, 0xF5},
              {0x0028, 0xC3},
              {0x0029, 0xD0},
              {0x00A2, 0x11},
          },

          // 145.57
          {
              {0x0022, 0x43},
              {0x0023, 0x9E},
              {0x0025, 0x0E},
              {0x0026, 0x2C},
              {0x0027, 0xFA},
              {0x0028, 0x0A},
              {0x0029, 0x58},
              {0x00A2, 0x11},
          },

          // 144.93
          {
              {0x0022, 0x42},
              {0x0023, 0x72},
              {0x0025, 0x0E},
              {0x0026, 0x1A},
              {0x0027, 0xF6},
              {0x0028, 0xEA},
              {0x0029, 0xEC},
              {0x00A2, 0x0A},
          },

          // 144.64
          {
              {0x0022, 0x42},
              {0x0023, 0xBD},
              {0x0025, 0x0E},
              {0x0026, 0x12},
              {0x0027, 0xF3},
              {0x0028, 0x1E},
              {0x0029, 0xB6},
              {0x00A2, 0x07},
          },

          // 144.66
          {
              {0x0022, 0x42},
              {0x0023, 0x72},
              {0x0025, 0x0E},
              {0x0026, 0x12},
              {0x0027, 0xFB},
              {0x0028, 0xCC},
              {0x0029, 0xFC},
              {0x00A2, 0x07},
          },

          // 144.62
          {
              {0x0022, 0x42},
              {0x0023, 0xBD},
              {0x0025, 0x0E},
              {0x0026, 0x11},
              {0x0027, 0xF9},
              {0x0028, 0xB2},
              {0x0029, 0xF3},
              {0x00A2, 0x07},
          },

          // 145.175
          {
              {0x0022, 0x42},
              {0x0023, 0x40},
              {0x0025, 0x0C},
              {0x0026, 0x0C},
              {0x0027, 0xFC},
              {0x0028, 0x35},
              {0x0029, 0x00},
              {0x00A2, 0x05},
          },

          // 144.575
          {
              {0x0022, 0x42},
              {0x0023, 0x90},
              {0x0025, 0x0E},
              {0x0026, 0x21},
              {0x0027, 0xF6},
              {0x0028, 0x71},
              {0x0029, 0x70},
              {0x00A2, 0x0D},
          },
      };

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

bool override_flag = false;
APRSFreqs override_frequency = F14445; // default override

void SetOverrideFrequency(APRSFreqs freq)
{ // This frequency is not used by any country
  // Potentially used for picture downloads
  // The ovrride only stays in effect for one message
  // Following messages will revert to the GEO frequency unless
  // this function is called again
  Serial.print(" SetFrequency");
  Serial.println(freq);
  override_frequency = freq;
  override_flag = true;
}

void VHF_off()
{ // turn the power off for clk2&3
  xxuint8_t regStatus = si5351.si5351_write(0x0003, 0x0C);
}

void VHF_on()
{

  pinMode(VXCO_PIN, OUTPUT);
  digitalWrite(VXCO_PIN, LOW);

  VHF_init();
  Set_drive(DRIVE_8ma);
  if (override_flag == true)
  {
    Set_frequency(override_frequency);  // used to set frequencies other than aprs frequencies
    override_flag = false; // set in SetOverrideFrequency
  }
  else
  {
    Set_frequency(GEOFENCE_Freq); // set in geofence.h
  }
}

void APRSon()
{
  // POUTPUTLN((F(" APRSon start ")));
  VHF_on(); // This needs to be done for each transmission.
  POUTPUTLN((F(" VHF ON ")));
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
