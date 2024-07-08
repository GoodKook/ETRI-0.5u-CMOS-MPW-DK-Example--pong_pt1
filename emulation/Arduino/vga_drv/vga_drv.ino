//
// VGAX (120x60, 4-Color)
// https://github.com/smaffer/vgax
//
//    VGA         Arduino
//  D-SUB 15       MEGA
//  --------     -----------------
//  1  (RED)   - Digital #30 (PC7)
//  2  (GREEN) - Digital #31 (PC6)
//  13 (HSYNC) - PWM9        (PH6)
//  14 (VSYNC) - PWM11       (PB5)
//  5,6,7,8,10 - GND
//
//  Gowin G5A-25/TANG-25K    Arduino MEGA
//  ---------------------   -----------------
//  p_tick_ex (K2/J3-1)   - Digital #42 (PL7)
//  hsync_ex  (K1/J3-2)   - Digital #39 (PG2)
//  vsync_ex  (L1/J3-3)   - Digital #40 (PG1)
//  pixel_ex  (L2/J3-4)   - Digital #41 (PG0)
//  enable_ex (K4/J3-5)   - Digital #43 (PL6)
//
#include <VGAX.h>

#include "pirate_image.h"

#define HSYNC_PIN   39  // PG2
#define VSYNC_PIN   40  // PG1
#define PIXEL_PIN   41  // PG0
#define P_TICK_PIN  42  // PL7
#define ENABLE_PIN  43  // PL6

// MACROS --------------------------------------------------
#define HSYNC_READ  (PING & 0b00000100) // PG2
#define VSYNC_READ  (PING & 0b00000010) // PG1
#define PIXEL_READ  (PING & 0b00000001) // PG0
#define P_TICK_READ (PINL & 0b10000000) // PL7
#define ENABLE_READ (PINL & 0b01000000) // PL6

#define HSYNC_NEGEDGE   { while (!HSYNC_READ);  while (HSYNC_READ); }
#define VSYNC_NEGEDGE   { while (!VSYNC_READ);  while (VSYNC_READ); }
#define P_TICK_POSEDGE  { while (P_TICK_READ);  while (!P_TICK_READ); }

VGAX vga;
unsigned char FrameBuffer[VGAX_HEIGHT][VGAX_BWIDTH];

void setup()
{
  pinMode(HSYNC_PIN,  INPUT);
  pinMode(VSYNC_PIN,  INPUT);
  pinMode(PIXEL_PIN,  INPUT);
  pinMode(P_TICK_PIN, INPUT);
  pinMode(ENABLE_PIN, OUTPUT);

  digitalWrite(ENABLE_PIN, 1);

  vga.begin();
  vga.noTone(); // Mute Audio

  for (int i=0; i<10; i++)
  {
    for (int y=0; y!=VGAX_HEIGHT; y++)
      for (int x=0; x!=VGAX_WIDTH; x++)
        vga.putpixel(x, y, 0);

    for (int y=0; y!=VGAX_HEIGHT; y++)
      for (int x=0; x!=VGAX_WIDTH; x++)
        vga.putpixel(x, y, 1);

    for (int y=0; y!=VGAX_HEIGHT; y++)
      for (int x=0; x!=VGAX_WIDTH; x++)
        vga.putpixel(x, y, 2);

    for (int y=0; y!=VGAX_HEIGHT; y++)
      for (int x=0; x!=VGAX_WIDTH; x++)
        vga.putpixel(x, y, 3);
  }

  vga.copy((byte*)img_pirate_data);
}

void loop()
{
  int x=0, y=0;

  while(true)
  {
    VSYNC_NEGEDGE //WaitForNegedge(VSYNC_PIN);

    while(true)
    {
      P_TICK_POSEDGE  //WaitForPosedge(P_TICK_PIN);

      if (PIXEL_READ)
        //vga.putpixel(x, y, 0);
        FrameBuffer[y][x/4] |= (FrameBuffer[y][x/4] |  (0b01000000 >> ((x%4)*2)));
      else
        //vga.putpixel(x, y, 1);  // 0=BLACK 1=GREEN 2=RED 3=YELLOW
        FrameBuffer[y][x/4] &= (FrameBuffer[y][x/4] | ~(0b01000000 >> ((x%4)*2)));
      x++;

      if (x>=VGAX_WIDTH)
      {
        HSYNC_NEGEDGE //WaitForNegedge(HSYNC_PIN);
        x=0;
        y++;

        if (y>=VGAX_HEIGHT)
        {
          digitalWrite(ENABLE_PIN, 0);

          vga.copy_RAM((byte*)FrameBuffer);
          for (int i=0; i!=VGAX_HEIGHT; i++)
            for (int j=0; j!=VGAX_BWIDTH; j++)
              FrameBuffer[i][j] = 0;

          digitalWrite(ENABLE_PIN, 1);
          y=x=0;
        }
      }
    }
  }
}
