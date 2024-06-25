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

    wait(clk.posedge_event());
    wait(clk.posedge_event());
    wait(clk.posedge_event());

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
                    case SDLK_r:
                        goto EXIT;
                        break;
                    default:
                        break;
                }
                //SDL_FlushEvents(SDL_KEYDOWN, SDL_KEYUP);
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
                //SDL_FlushEvents(SDL_KEYDOWN, SDL_KEYUP);
                break;
            default:
                break;
            }
        }
    }

    EXIT:
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    sc_stop();
}

void sc_pong_pt1_TB::monitor()
{
    int x = 0, y = 0, nHSync = 0, nVSync = 0;
    uint32_t    _rgb;
    uint8_t     R, G, B;
    
    while(true)
    {
        wait(p_tick.posedge_event());
        if (p_tick.read())
        {
            _rgb = rgb.read();
            R = (_rgb>>8) & 0x0F;
            G = (_rgb>>4) & 0x0F;
            B = (_rgb)    & 0x0F;
            SDL_SetRenderDrawColor(renderer, R*16, G*16, B*16, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawPoint(renderer, x, y);
            x++;

            if (hsync.read())
            {
                printf("nVSync[%d] nHSync[%d]\r", nVSync, nHSync++);
                fflush(stdout);

                x = 0;
                y++;

                if (vsync.read())
                {
                    y = 0;
                    nHSync = 0;
                    nVSync++;
                    SDL_RenderPresent(renderer);
                }
            }
        }
    }
}
