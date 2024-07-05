/*******************************************************************************
Poorman's Standard-Emulator
---------------------------
Vendor: GoodKook, goodkook@gmail.com
Associated Filename: pong_pt1.h
Purpose: Emulation Wrapper, Modeling Interface for pong_pt1
Revision History: Jun. 1, 2024
*******************************************************************************/
#ifndef _PONG_PT1_H_
#define _PONG_PT1_H_

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
    sc_in<bool>             up;
    sc_in<bool>             down;
    sc_out<bool>            p_tick;
    sc_out<bool>            hsync;
    sc_out<bool>            vsync;
    sc_out<sc_uint<12> >    rgb;

    // Arduino Serial IF
    int fd;                 // Serial port file descriptor
    struct termios options; // Serial port setting

#define N_TX    1
#define N_RX    2

    void pong_pt1_thread(void)
    {
        uint8_t x, y, txPacket[N_TX], rxPacket[N_RX];
        
        
        while(true)
        {
            //-------------------------------------------------------
            wait(clk.posedge_event());
            // Assemble bitmap for emulator input byte. Refer to Verilog wrapper
            //  stimIn[0] = {----|clk|reset|up|down}
            txPacket[0] = (uint8_t)(reset.read()?   0x04:0x00) |
                          (uint8_t)(up.read()?      0x02:0x00) |
                          (uint8_t)(down.read()?    0x01:0x00);

            // Send to Emulator
            for (int i=0; i<N_TX; i++)
            {
                x = txPacket[i];
                while(write(fd, &x, 1)<=0)  usleep(100);
            }
            // Receive from Emulator
            for (int i=0; i<N_RX; i++)
            {
                while(read(fd, &y, 1)<=0)   usleep(100);
                rxPacket[i] = y;
            }

            // Dis-assemble emulator output byte to Verilog wrapper
            // vectOut[0] = {-|p_tick|hsync|vsync|rgb[11:8]}
            // vectOut[1] = rgb[7:0]
            p_tick.write((rxPacket[0] & 0x40)? true:false);
            hsync.write((rxPacket[0]  & 0x20)? true:false);
            vsync.write((rxPacket[0]  & 0x10)? true:false);
            rgb.write((sc_uint<12>)(rxPacket[0] & 0x0F)<<8 | (sc_uint<12>)rxPacket[1]);
        }
    }

    sc_trace_file* fp;  // VCD file

    SC_CTOR(pong_pt1) :   // Constructor
        clk("clk"), reset("reset"), up("up"), down("down"), p_tick("p_tick"), hsync("hsync"), vsync("vsync"), rgb("rgb")
    {
        SC_THREAD(pong_pt1_thread);
        sensitive << clk;

#ifdef VCD_TRACE
        // VCD Trace
        fp = sc_create_vcd_trace_file("sc_pong_pt1");
        fp->set_time_unit(100, SC_PS);
        sc_trace(fp, clk,   "clk");
        sc_trace(fp, reset, "reset");
        sc_trace(fp, up,    "up");
        sc_trace(fp, down,  "down");
        sc_trace(fp, hsync, "p_tick");
        sc_trace(fp, hsync, "hsync");
        sc_trace(fp, vsync, "vsync");
        sc_trace(fp, rgb,   "rgb");
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

