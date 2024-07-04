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

    psce.DUT_Posedge_Clk();
    psce.DUT_Negedge_Clk();
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

