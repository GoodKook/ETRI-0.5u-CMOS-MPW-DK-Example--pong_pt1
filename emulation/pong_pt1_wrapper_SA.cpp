/*
    CPU_6502_SA

    CPU_6502 Stand-Alone FPGA Emulator
    
    g++ -o CPU_6502_SA CPU_6502_SA.cpp
*/

#include <stdio.h>
#include <stdlib.h>

// Includes for accessing Arduino via serial port
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h> // For FIONREAD

#include <SDL2/SDL.h>

int main(int argc, char* argv[])
{
    // Arduino Serial IF
    int fd;                 // Serial port file descriptor
    struct termios options; // Serial port setting
    
    // ------------------------------------------------------------
    // Init COM port to communicate Arduino Emulator 
    //fd = open("/dev/ttyACM0", O_RDWR | O_NDELAY | O_NOCTTY);
    fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        perror("Error opening serial port");
        return -1;
    }

    // Set up serial port/B38400/B115200
    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;

    // Apply the settings
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &options);

    // Establish Contact
    int len = 0;
    unsigned char rx, tx[4];
    while(!len)
        len = read(fd, &rx, 1);
    if (rx=='A')
        write(fd, &rx, 1);
    printf("Connection established...\n");


    ////////////////////////////////////////////////////////////////
    // SDL
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL Initialization Fail: %s\n", SDL_GetError());
        return -1;
    }
    window = SDL_CreateWindow("SDL2 Window",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          800, 600,
                          SDL_WINDOW_SHOWN);
    if (!window) {
        printf("SDL Initialization Fail: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    SDL_SetWindowTitle(window, "Verilog Pong Game");
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // -------------------------------------------------------------
    // Command-Line Loop
    unsigned char   szInBuff[80];
    char            szBinFile[128];
    int             nInBuff = 0;
    
    len = 0;
    bool quit = false;

    while(true)
    {
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
                        
                        break;
                    case SDLK_DOWN:
                        
                        break;
                    case SDLK_r:
                        goto EXIT;
                        break;
                    default:
                        break;
                }
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
                break;
            default:
                break;
            }
        }
    }
































        // Keyboard input
        while (!kbhit())
        {
            // Reading from emulator
            len = read(fd, &rx, 1); // Something to read?
            if (len)
            {
                if (rx==0x0D)   // CR ?
                {
                    rx = '\n';
                    nInBuff = 0;
                }
                else
                {
                    szInBuff[nInBuff++] = rx;
                    szInBuff[nInBuff]   = '\0'; // NULL terminate
                }
                putchar(rx);
                fflush(stdout);
            }
            fflush(stdout);
            usleep(500);
        }
        tx[0] = getchar();
        tx[1] = '\0';
        //printf("%c", tx[0]);
        //printf(" %02X", tx[0]);

        //while(write(fd, &tx, 1)<=0)  usleep(1); // Send to Emulator
    }
    
    close(fd);
    return 0;
}
