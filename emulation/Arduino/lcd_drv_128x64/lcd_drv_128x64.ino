/**********************************************************************************
 * 
 * Interfacing Arduino with KS0108 Monochrome GLCD.
 * This example is for KS0108 GLCD modules with 128x64 Pixel resolution (two CS pins).
 * This is a free software with NO WARRANTY - Use it at your own risk!
 * http://simple-circuit.com/
 *
***********************************************************************************
 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
************************************************************************************
* Modified to work with KS0108 monochrome GLCD. More information including circuit
*   diagram on:
* http://simple-circuit.com/
* 
 **********************************************************************************/

#include <Adafruit_GFX.h>   // include adafruit GFX library
#include <KS0108_GLCD.h>    // include KS0108 GLCD library

// KS0108 GLCD library initialization according to the following connection:
//                    KS0108_GLCD(DI,RW, E,DB0,DB1,DB2,DB3,DB4,DB5,DB6,DB7,CS1,CS2,RES);
KS0108_GLCD display = KS0108_GLCD(22,23,24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,35);

//
//  Gowin G5A-25/TANG-25K    Arduino MEGA
//  ---------------------   -----------------
//  p_tick_ex (K2/J3-1)   - Digital #42 (PL7)
//  hsync_ex  (K1/J3-2)   - Digital #39 (PG2)
//  vsync_ex  (L1/J3-3)   - Digital #40 (PG1)
//  pixel_ex  (L2/J3-4)   - Digital #41 (PG0)
//  enable_ex (K4/J3-5)   - Digital #43 (PL6)
//
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

void setup()
{
  pinMode(HSYNC_PIN,  INPUT);
  pinMode(VSYNC_PIN,  INPUT);
  pinMode(PIXEL_PIN,  INPUT);
  pinMode(P_TICK_PIN, INPUT);
  pinMode(ENABLE_PIN, OUTPUT);

  digitalWrite(ENABLE_PIN, 1);

  // initialize KS0108 GLCD module with active high CS pins
  if ( display.begin(KS0108_CS_ACTIVE_HIGH) == false )
    while(true);  // stay here forever!

  display.display();
  delay(1000); // Pause for a second

  // Clear the buffer
  display.clearDisplay();

  // Screen Test
  for(int n=0; n<10; n++)
  {
    for (int x=0; x<display.width(); x++)
      for (int y=0; y<display.height(); y++)
        display.drawPixel(x, y, KS0108_ON);
    display.display();

    for (int x=0; x<display.width(); x++)
      for (int y=0; y<display.height(); y++)
        display.drawPixel(x, y, KS0108_OFF);
    display.display();
  }
}

#define LCD_WIDTH   128
#define LCD_BWIDTH  LCD_WIDTH/8
#define LCD_HEIGHT  64

void loop()
{
  int x=0, y=0;
  uint8_t   FrameBuffer[LCD_BWIDTH][LCD_HEIGHT];

  while(true)
  {
    VSYNC_NEGEDGE //WaitForNegedge(VSYNC_PIN);

    while(true)
    {
      P_TICK_POSEDGE  //WaitForPosedge(P_TICK_PIN);

      if (PIXEL_READ)
        FrameBuffer[x/8][y] |= (FrameBuffer[x/8][y] |  (0b10000000 >> x%8));
      else
        FrameBuffer[x/8][y] &= (FrameBuffer[x/8][y] | ~(0b10000000 >> x%8));
      x++;

      if (x>=LCD_WIDTH)
      {
        HSYNC_NEGEDGE //WaitForNegedge(HSYNC_PIN);
        x=0;
        y++;

        if (y>=LCD_HEIGHT)
        {
          digitalWrite(ENABLE_PIN, 0);

          for (int j=0, y=0; j<LCD_HEIGHT; j++, y++)
          {
            for (int i=0, x=0; i<LCD_BWIDTH; i++)
            {
              for (int k=0; k<8; k++)
              {
                if (FrameBuffer[i][j] & (0b10000000 >> k))
                  display.drawPixel(x, y, KS0108_ON);
                else
                  display.drawPixel(x, y, KS0108_OFF);
                x++;
              }
              FrameBuffer[i][j] = 0x00;
            }
          }
          display.display();

          digitalWrite(ENABLE_PIN, 1);
          y=0;
          x=0;
        }
      }
    }
  }
}
