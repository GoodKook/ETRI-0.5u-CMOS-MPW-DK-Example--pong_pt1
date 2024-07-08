/*******************************************************************************
Poorman's Standard-Emulator
---------------------------
Vendor: GoodKook, goodkook@gmail.com
Associated Filename: sc_pong_pt1_TB.h
Purpose: Testbench for pong_pt1
Revision History: Jun. 1, 2024
*******************************************************************************/
#ifndef _SC_PONG_PT1_TB_H_
#define _SC_PONG_PT1_TB_H_

#include <systemc.h>

#if defined(VERILATION)         // Cycle-Accurate Co-Simulation
#include "Vpong_pt1.h"
#elif defined(CO_EMULATION)     // Cycle-Accurate Co-Emulation
#include "pong_pt1.h"
#elif defined(CO_EMULATION_SA)  // HSync Screen-Transacted Co-Emulation
#include "pong_pt1_SA.h"
#elif defined(CO_EMULATION_SA2) // VSync Screen-Transacted Co-Emulation
#include "pong_pt1_SA2.h"
#elif defined(CO_EMULATION_RT)  // No Screen-Transacted Co-Emulation
#include "pong_pt1_RT.h"
#else
#error "DUT NOT defined. See 'Makefile'"
#endif

#include <SDL2/SDL.h>

SC_MODULE(sc_pong_pt1_TB)
{
    sc_clock            clk;
    sc_signal<bool>     reset;
    sc_signal<bool>     enable;
    sc_signal<bool>     up;
    sc_signal<bool>     down;
    sc_signal<bool>     p_tick;
    sc_signal<bool>     hsync;
    sc_signal<bool>     vsync;

#if defined(VERILATION)
    // Verilated pong_pt1 or Foreign Verilog
    Vpong_pt1*              u_Vpong_pt1;
    sc_signal<uint32_t>     rgb;    // Verilator treats all Verilog's vector as <uint32_t>
#elif defined(CO_EMULATION)
    pong_pt1*               u_pong_pt1;
    sc_signal<sc_uint<12> > rgb;
#elif  defined(CO_EMULATION_SA) || defined(CO_EMULATION_SA2) || defined(CO_EMULATION_RT)
    pong_pt1*               u_pong_pt1;
#endif

    void test_generator();  // Keyboard event

#if defined(VERILATION) || defined(CO_EMULATION)
    void monitor();     // Cycle-Accurate Monitor Screen
#elif defined(CO_EMULATION_SA)
    void monitor_SA();  // HSync-Transacted Monitor Screen
#elif defined(CO_EMULATION_SA2)
    void monitor_SA2();  // VSync-Transacted Monitor Screen
#elif defined(CO_EMULATION_RT)
    // No Screen Transaction Monitor
#endif
    
    sc_trace_file* fp;  // VCD file
    SDL_Window* window;
    SDL_Renderer* renderer;

    SC_CTOR(sc_pong_pt1_TB) :   // Constructor
        clk("clk", 100, SC_NS, 0.5, 0.0, SC_NS, false)
    {
        SC_THREAD(test_generator);
        sensitive << hsync;

#if  defined(CO_EMULATION_SA) || defined(CO_EMULATION_SA2) || defined(CO_EMULATION_RT)
        enable.write(false);
#else
        enable.write(true);
#endif

#if defined(VERILATION) || defined(CO_EMULATION)
        SC_THREAD(monitor);     // Cycle-Accurate Screen Monitor
        sensitive << clk;
#elif defined(CO_EMULATION_SA)
        SC_THREAD(monitor_SA);  // HSync-Transacted Screen Monitor
        sensitive << hsync;
#elif defined(CO_EMULATION_SA2)
        SC_THREAD(monitor_SA2); // VSync-Transacted Screen Monitor
        sensitive << hsync;
#elif defined(CO_EMULATION_RT)
        // No Screen Transaction Monitor
#endif

        ////////////////////////////////////////////////////////////////
        // SDL
        window = NULL;
        renderer = NULL;

        // SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            printf("SDL Initialization Fail: %s\n", SDL_GetError());
            return;
        }

        window = SDL_CreateWindow("SDL2 Window",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              800, 600,
                              SDL_WINDOW_SHOWN);
        if (!window) {
            printf("SDL Initialization Fail: %s\n", SDL_GetError());
            SDL_Quit();
            return;
        }
        SDL_SetWindowTitle(window, "Verilog Pong Game");
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        // DUT: "pong_pt1"
#if defined(VERILATION)
        u_Vpong_pt1 = new Vpong_pt1("u_Vpong_pt1");
        u_Vpong_pt1->clk(clk);
        u_Vpong_pt1->reset(reset);
        u_Vpong_pt1->enable(enable);
        u_Vpong_pt1->up(up);
        u_Vpong_pt1->down(down);
        u_Vpong_pt1->p_tick(p_tick);
        u_Vpong_pt1->hsync(hsync);
        u_Vpong_pt1->vsync(vsync);
        u_Vpong_pt1->rgb(rgb);
#elif defined(CO_EMULATION)
        u_pong_pt1 = new pong_pt1("u_pong_pt1");
        u_pong_pt1->clk(clk);
        u_pong_pt1->reset(reset);
        u_pong_pt1->enable(enable);
        u_pong_pt1->up(up);
        u_pong_pt1->down(down);
        u_pong_pt1->p_tick(p_tick);
        u_pong_pt1->hsync(hsync);
        u_pong_pt1->vsync(vsync);
        u_pong_pt1->rgb(rgb);
#elif defined(CO_EMULATION_SA) || defined(CO_EMULATION_SA2) || defined(CO_EMULATION_RT)
        u_pong_pt1 = new pong_pt1("u_pong_pt1");
        u_pong_pt1->clk(clk);
        u_pong_pt1->reset(reset);
        u_pong_pt1->enable(enable);
        u_pong_pt1->up(up);
        u_pong_pt1->down(down);
        u_pong_pt1->hsync(hsync);
#endif

#ifdef VCD_TRACE
        // VCD Trace
        fp = sc_create_vcd_trace_file("sc_pong_pt1_TB");
        fp->set_time_unit(100, SC_PS);
        sc_trace(fp, clk,   "clk");
        sc_trace(fp, reset, "reset");
        sc_trace(fp, enable,"enable");
        sc_trace(fp, up,    "up");
        sc_trace(fp, down,  "down");
        sc_trace(fp, p_tick,"p_tick");
        sc_trace(fp, hsync, "hsync");
        sc_trace(fp, vsync, "vsync");
#if defined(VERILATION) || defined(CO_EMULATION)
        sc_trace(fp, rgb,   "rgb");
#endif
#endif  // VCD_TRACE
    }

    // Destructor
    ~sc_pong_pt1_TB()
    {}
};

#endif // _SC_pong_pt1_H_
