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
// KS0108_GLCD(DI, RW, E, DB0, DB1, DB2, DB3, DB4, DB5, DB6, DB7, CS1, CS2, RES);
KS0108_GLCD display = KS0108_GLCD(22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35);

#define P_TICK_PIN  38
#define HSYNC_PIN   39
#define VSYNC_PIN   40
#define PIXEL_PIN   41

void setup()
{
  pinMode(P_TICK_PIN, INPUT);
  pinMode(HSYNC_PIN, INPUT);
  pinMode(VSYNC_PIN, INPUT);
  pinMode(PIXEL_PIN, INPUT);

  //Serial.begin(9600);

  // initialize KS0108 GLCD module with active high CS pins
  if ( display.begin(KS0108_CS_ACTIVE_HIGH) == false )
  {
    //Serial.println( F("display initialization failed!") );    // lack of RAM space
    while(true);  // stay here forever!
  }

  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, KS0108_ON);

  for(int n=0; n<10; n++)
  {
  for (int x=0; x<display.width(); x++)
  {
    for (int y=0; y<display.height(); y++)
    {
      display.drawPixel(x, y, KS0108_ON);
      //display.display();
    }
    //display.display();
  }
  display.display();

  for (int x=0; x<display.width(); x++)
  {
    for (int y=0; y<display.height(); y++)
    {
      display.drawPixel(x, y, KS0108_OFF);
      //display.display();
    }
    //display.display();
  }
  display.display();
  }
}

// MACROS --------------------------------------------------
#define VSYNC_NEGEDGE   { \
                          while ((PING & 0b00000010) != 0b00000010); \
                          while ((PING & 0b00000010) == 0b00000010); \
                        }
#define P_TICK_POSEDGE  { \
                          while ((PIND & 0b10000000) != 0b00000000); \
                          while ((PIND & 0b10000000) != 0b10000000); \
                        }
#define HSYNC_NEGEDGE   { \
                          while ((PING & 0b00000100) != 0b00000100); \
                          while ((PING & 0b00000100) == 0b00000100); \
                        }

void loop()
{
  int x=0, y=0;
//  uint16_t  nColor;
  uint8_t   FrameBuffer[16][64];

  //WaitForNegedge(VSYNC_PIN);
  VSYNC_NEGEDGE

  while(true)
  {
    //WaitForPosedge(P_TICK_PIN);
    //while ((PIND & 0b10000000) != 0b00000000);
    //while ((PIND & 0b10000000) != 0b10000000);
    P_TICK_POSEDGE

//    if(digitalRead(PIXEL_PIN))
//      display.drawPixel(x, y, KS0108_ON);
//    else
//      display.drawPixel(x, y, KS0108_OFF);

    if ((PING & 0b00000001) == 0b00000001)  // PIXEL: Digital Pin #41 : Port G[0]
      FrameBuffer[x/8][y] |= (FrameBuffer[x/8][y] |  (0b10000000 >> x%8));
//      nColor = KS0108_ON;
    else
      FrameBuffer[x/8][y] &= (FrameBuffer[x/8][y] | ~(0b10000000 >> x%8));
//      nColor = KS0108_OFF;

    //while ((PIND & 0b10000000) == 0b10000000);
//    display.drawPixel(x, y, nColor);

    x++;
    if (x>=128)
    {
      x=0;
      //WaitForNegedge(HSYNC_PIN);
      HSYNC_NEGEDGE

      y++;
      if (y>=64)
      {
        for (int j=0, y=0; j<64; j++, y++)
        {
          for (int i=0, x=0; i<16; i++)
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

        //WaitForNegedge(VSYNC_PIN);
        VSYNC_NEGEDGE

        y=x=0;
      }
    }
  }
}

void WaitForNegedge(uint8_t Pin)
{
  bool sig0=false, sig1=false;

  while(true)
  {
    sig1 = sig0;
    //delayMicroseconds(10);
    sig0 = digitalRead(Pin)? true:false;
    if (!sig0 && sig1)  // Falling Edge
      return;
  }
}

void WaitForPosedge(uint8_t Pin)
{
  bool sig0=false, sig1=false;

  while(true)
  {
    sig1 = sig0;
    //delayMicroseconds(10);
    sig0 = digitalRead(Pin)? true:false;
    if (sig0 && !sig1)  // Rising Edge
      return;
  }
}

// end of code.
