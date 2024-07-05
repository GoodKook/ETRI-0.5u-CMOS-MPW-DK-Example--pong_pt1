/*
  Poorman's Standard-Emulator by GoodKook, goodkook@gmail.com
  PSCE Model Interface: SystemC to/from DUT Emulator
*/

// Standard Emulator ------------------------------------------------
#include "PSCE_APIs.h"

// Co-Emulation interface -------------------------------------------
// Followings are DUT specific defs
#define DELAY_MICROS    1

#define N_RX            1   // Number of byte to DUT's inputs
                            //  Bitmap must match with SystemC TB and Verilog wrapper
                            //  stimIn[0] = {----|clk|reset|up|down}
#define N_TX            16   // Number of byte from DUT's output (Burst out of 'LINE')

#define DUT_CLK_BYTE    0
#define DUT_CLK_BITMAP  0x00

PSCE psce(DELAY_MICROS);

void setup()
{
  psce.init();  // Overclocking=114Mhz, BPS=115200
}

//-----------------------------------------------------
#define RESET_ON  0x04
#define UP_BUTTON 0x02
#define DN_BUTTON 0x01

#define P_TICK    0x40
#define HSYNC     0x20
#define VSYNC     0x10

#define PIXEL     0x02

void loop()
{
  int Rx;
  uint8_t LineBuff[80]; // 640/8 = 80

  Resetting();

  while(true)
  {
    psce.EMU_Blinker(0x40);   // Blinker speed

    if (Serial.available())
    {
      Rx = Serial.read();
      psce.EMU_Input( 0, Rx);
      psce.DUT_Input();
    }

    Transact_HSync(LineBuff);

    for(int nTx=0; nTx<N_TX; nTx++)
    {
      while (Serial.availableForWrite()<1)
        delayMicroseconds(10);
      Serial.write(LineBuff[nTx]);
    }
  }
}

void Resetting()
{
  int Byte = RESET_ON;

  psce.EMU_Input( 0, (int)Byte);
  psce.DUT_Input();

  for (uint8_t i=0; i<10; i++)
  {
    psce.DUT_Posedge_Clk();
    psce.DUT_Negedge_Clk();
  }
}

void Transact_HSync(uint8_t* LineBuff)
{
  uint8_t Byte = 0x00;
  int x = 0;

  for (int i=0; i<N_TX; i++)
    LineBuff[i] = 0x00;

  WaitForNegedge(HSYNC);

  while(true)
  {
    Byte = WaitForPosedge(P_TICK);

    if (Byte & PIXEL)
      LineBuff[x/8] |= (0x80 >> (x%8));

    x++;
    if ((x/8)>=N_TX)
      return;
  }
}

uint8_t WaitForNegedge(const uint8_t sig)
{
  uint8_t Byte = 0x00;
  bool sig0, sig1;

  while(true)
  {
    delayMicroseconds(DELAY_MICROS);
    psce.DUT_Negedge_Clk();
    delayMicroseconds(DELAY_MICROS);
    psce.DUT_Posedge_Clk();

    psce.DUT_Output();
    Byte = (uint8_t)psce.EMU_Output(0);

    sig1 = sig0;
    sig0 = (Byte & sig)? true:false;
    if (!sig0 && sig1)  // Falling Edge
      return Byte;
  }
}

uint8_t WaitForPosedge(const uint8_t sig)
{
  uint8_t Byte = 0x00;
  bool sig0, sig1;

  while(true)
  {
    delayMicroseconds(DELAY_MICROS);
    psce.DUT_Negedge_Clk();
    delayMicroseconds(DELAY_MICROS);
    psce.DUT_Posedge_Clk();

    psce.DUT_Output();
    Byte = (uint8_t)psce.EMU_Output(0);

    sig1 = sig0;
    sig0 = (Byte & sig)? true:false;
    if (sig0 && !sig1)  // Rising Edge
      return Byte;
  }
}
