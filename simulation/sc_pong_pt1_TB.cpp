/*******************************************************************************
Poorman's Standard-Emulator
---------------------------
Vendor: GoodKook, goodkook@gmail.com
Associated Filename: sc_pong_pt1_TB.cpp
Purpose: Testbench for pong_pt1
Revision History: Jun. 1, 2024
*******************************************************************************/
#include "sc_pong_pt1_TB.h"

#include <iostream>
#include <iomanip>

void sc_pong_pt1_TB::test_generator()
{
    reset.write(1);
    up.write(0);
    down.write(0);

    wait(clk_100MHz.posedge_event());
    wait(clk_100MHz.posedge_event());
    wait(clk_100MHz.posedge_event());

    reset.write(0);

    SDL_Event event;
    bool quit = false;

    while(!quit)
    {
        wait(hsync.posedge_event());

        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                //std::cout << "Key pressed: " << SDL_GetKeyName(event.key.keysym.sym) << std::endl;
                switch( event.key.keysym.sym )
                {
                    case SDLK_UP:
                        up.write(true);
                        break;
                    case SDLK_DOWN:
                        down.write(true);
                        break;
                    default:
                        break;
                }
                SDL_FlushEvents(SDL_KEYDOWN, SDL_KEYUP);
                break;
            case SDL_KEYUP:
                //std::cout << "Key released: " << SDL_GetKeyName(event.key.keysym.sym) << std::endl;
                switch( event.key.keysym.sym )
                {
                    case SDLK_UP:
                        up.write(false);
                        break;
                    case SDLK_DOWN:
                        down.write(false);
                        break;
                    default:
                        break;
                }
                SDL_FlushEvents(SDL_KEYDOWN, SDL_KEYUP);
                break;
            default:
                break;
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    sc_stop();
}

void sc_pong_pt1_TB::monitor()
{
    int x = 0, y = 0, skip_hsync = 0;
    uint32_t    _rgb;
    uint8_t     R, G, B;
    
    while(true)
    {
        WAIT_FOR_VSYNC:
        wait(clk_100MHz.posedge_event());
        if (vsync.read())
        {
            while(vsync.read())    // Skip vsync
                wait(clk_100MHz.posedge_event());
        }
        else
            continue;

        // Skip hsync
        for (int i=0; i<10; i++)
        {
            while(true)
            {
                wait(clk_100MHz.posedge_event());
                if (hsync.read())
                {
                    while(hsync.read())    // Skip hsync
                        wait(clk_100MHz.posedge_event());
                }
                else
                    continue;
                break;
            }
        }

        SKIP_36CLK:
        for (int i=0; i<36; i++)
            wait(clk_100MHz.posedge_event());

        while(true)
        {
            wait(clk_100MHz.posedge_event());
            wait(clk_100MHz.posedge_event());
            _rgb = rgb.read();
            R = (_rgb>>8) & 0x0F;
            G = (_rgb>>4) & 0x0F;
            B = (_rgb)    & 0x0F;
            SDL_SetRenderDrawColor(renderer, R*16, G*16, B*16, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawPoint(renderer, x, y);
            x++;

            if (hsync.read())
            {
                x = 0;
                y++;
                while(hsync.read()) // Skip hsync
                    wait(clk_100MHz.posedge_event());

                if (vsync.read())
                {
                    y = 0;
                    x = 0;
                    SDL_RenderPresent(renderer);
                    goto WAIT_FOR_VSYNC;
                }
                else
                {
                    x = 0;
                    goto SKIP_36CLK;
                }
            }
        }
    }

    sc_time t(1, SC_NS);
#ifdef CO_EMULATION
    while(true)
    {
        wait(CLK.posedge_event());
        if ((int)Dout_emu.read() != (int)Dout_n16.read())
        {
            cout    << "Error at "   << sc_time_stamp() << ": "
                    << std::hex
                    << "Dout=0x"     << Dout_n16.read() << " "
                    << "Dout_emu=0x" << Dout_emu.read() << endl;
        }
    }
#endif
}
