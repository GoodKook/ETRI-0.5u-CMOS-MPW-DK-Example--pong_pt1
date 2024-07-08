/*
  Poorman's Standard-Emulator by GoodKook, goodkook@gmail.com
  PSCE Model Interface: SystemC to/from DUT Emulator
*/

// Standard Emulator ------------------------------------------------
#include "PSCE_APIs.h"

// Co-Emulation interface -------------------------------------------
// Followings are DUT specific defs
#define DELAY_MICROS    10

#define N_RX            1   // Number of byte to DUT's inputs
                            //  stimIn[0] = {---|clk|reset|enable|up|down}
#define N_TX            2   // Number of byte from DUT's output
                            // vectOut[0] = {-|p_tick|hsync|vsync|rgb[11:8]}
                            // vectOut[1] = rgb[7:0]

#define DUT_CLK_BYTE    0
#define DUT_CLK_BITMAP  0x00

PSCE psce(DELAY_MICROS);

void setup()
{
  psce.init();  // Overclocking=114Mhz, BPS=115200
}

void loop()
{
  psce.EMU_Blinker(0x40);   // Blinker speed
  psce.RxPacket(N_RX, DUT_CLK_BYTE, DUT_CLK_BITMAP);  // CLK position: 0-th byte & 0x40
  psce.TxPacket(N_TX);
}
