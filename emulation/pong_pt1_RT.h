/*******************************************************************************
Poorman's Standard-Emulator
---------------------------
Vendor: GoodKook, goodkook@gmail.com
Associated Filename: pong_pt1_RT.h
Purpose: Emulation Wrapper, Modeling Interface for pong_pt1
Revision History: Jun. 1, 2024
*******************************************************************************/
#ifndef _PONG_PT1_RT_H_
#define _PONG_PT1_RT_H_

#include "systemc"

// Includes for accessing Arduino via serial port
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

SC_MODULE(pong_pt1)
{
    // PORTS
    sc_in<bool>             clk;
    sc_in<bool>             reset;
    sc_in<bool>             enable;
    sc_in<bool>             up;
    sc_in<bool>             down;
    sc_out<bool>            hsync;  // For keyboard event

    // Arduino Serial IF
    int fd;                 // Serial port file descriptor
    struct termios options; // Serial port setting

#define N_TX    1

    void pong_pt1_TX_thread(void)
    {
        uint8_t x, txPacket[N_TX];
        bool    bHSync = false; // Fake HSync

        while(true)
        {
            //-------------------------------------------------------
            wait(clk.posedge_event());
            if (bHSync)
            {
                bHSync = false;
                hsync.write(bHSync);
                continue;
            }

            // Assemble bitmap for emulator input byte. Refer to Verilog wrapper
            //  stimIn[0] = {----|clk|reset|enable|up|down}
            //txPacket[0] = (uint8_t)(reset.read()?   0x04:0x00) |
            txPacket[0] = (uint8_t)(enable.read()?  0x04:0x00) |
                          (uint8_t)(up.read()?      0x02:0x00) |
                          (uint8_t)(down.read()?    0x01:0x00);

            // Send to Emulator
            for (int i=0; i<N_TX; i++)
            {
                x = txPacket[i];
                while(write(fd, &x, 1)<=0)
                {
                    //wait(SC_ZERO_TIME);
                    usleep(100);
                }
            }

            bHSync = true;  // Fake HSync
            hsync.write(bHSync);
        }
    }

    sc_trace_file* fp;  // VCD file

    SC_CTOR(pong_pt1) :   // Constructor
        clk("clk"),
        reset("reset"),
        up("up"),
        down("down"),
        hsync("hsync")
    {
        SC_THREAD(pong_pt1_TX_thread);
        sensitive << clk;

#ifdef VCD_TRACE
        // VCD Trace
        fp = sc_create_vcd_trace_file("sc_pong_pt1");
        fp->set_time_unit(100, SC_PS);
        sc_trace(fp, clk,   "clk");
        sc_trace(fp, reset, "reset");
        sc_trace(fp, up,    "up");
        sc_trace(fp, down,  "down");
        sc_trace(fp, hsync, "hsync");
#endif  // VCD_TRACE

        //--------------------------------------------------------------
        // Set-up serial port communicating Arduino DUE USB-UART
        //fd = open("/dev/ttyACM0", O_RDWR | O_NDELAY | O_NOCTTY);
        fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY);
        if (fd < 0)
        {
            perror("Error opening serial port");
            return;
        }
        // Set up serial port
        options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
        options.c_iflag = IGNPAR;
        options.c_oflag = 0;
        options.c_lflag = 0;
        // Apply the settings
        tcflush(fd, TCIFLUSH);
        tcsetattr(fd, TCSANOW, &options);

        // Establish Contact
        int len = 0;
        char rx;
        while(!len)
            len = read(fd, &rx, 1);
        if (rx=='A')
            write(fd, &rx, 1);
        printf("Connection established...\n");
    }
};

#endif

