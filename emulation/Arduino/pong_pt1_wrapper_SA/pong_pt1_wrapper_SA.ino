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
        delayMicroseconds(1);
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
  static int y = 0;

  uint8_t Byte = 0x00;
  int x = 0;
  bool p_tick0, p_tick1;
  bool hsync0, hsync1;

  for (int i=0; i<N_TX; i++)
    LineBuff[i] = 0x00;

  while(true)
  {
    delayMicroseconds(DELAY_MICROS);
    psce.DUT_Negedge_Clk();
    delayMicroseconds(DELAY_MICROS);
    psce.DUT_Posedge_Clk();
    
    psce.DUT_Output();
    Byte = (uint8_t)psce.EMU_Output(0);

    if ((Byte&HSYNC) || (Byte&VSYNC))
    {
      x = 0;
      continue;
    }

    p_tick1 = p_tick0;
    p_tick0 = (Byte & P_TICK)? true:false;
    if (p_tick0 && !p_tick1)  // Rising Edge of P_TICK
    {
      if (Byte & PIXEL)
        LineBuff[x/8] |= (0x80 >> (x%8));

      x++;
      if ((x/8)>=N_TX)
      {
        WaitForNegedge(HSYNC);
        y++;
        if (y>=64)
        {
          WaitForNegedge(VSYNC);
          y = 0;
        }
        return;
      }
    }
    else
      continue;
  }
}

void WaitForNegedge(const uint8_t sig)
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
      return;
  }
}

void WaitForHSync()
{
  uint8_t Byte = 0x00;
  bool hsync0, hsync1;

  while(true)
  {
    delayMicroseconds(DELAY_MICROS);
    psce.DUT_Negedge_Clk();
    delayMicroseconds(DELAY_MICROS);
    psce.DUT_Posedge_Clk();

    psce.DUT_Output();
    Byte = (uint8_t)psce.EMU_Output(0);

    hsync1 = hsync0;
    hsync0 = (Byte&HSYNC)? true:false;
    if (!hsync0 && hsync1)  // Falling Edge of HSYNC
      return;
  }
}
