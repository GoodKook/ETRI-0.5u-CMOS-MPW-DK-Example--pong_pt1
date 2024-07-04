`timescale 1ns / 1ps

`define MAX_TABLE_WIDTH     640
`define MAX_TABLE_HEIGHT    480

module pong_pt1(
    input           clk,
    input           reset,
    input           up,
    input           down,
    output          p_tick,
    output          hsync,
    output          vsync,
    output [11:0]   rgb
    );

`ifdef MAX_TABLE_SIZE
    parameter TABLE_WIDTH   = `MAX_TABLE_WIDTH;
    parameter TABLE_HEIGHT  = `MAX_TABLE_HEIGHT;
    parameter X_BIT_WIDTH   = $bits(TABLE_WIDTH);
    parameter Y_BIT_WIDTH   = $bits(TABLE_HEIGHT);
`elsif HALF_TABLE_SIZE
    parameter TABLE_WIDTH   = `MAX_TABLE_WIDTH/2;
    parameter TABLE_HEIGHT  = `MAX_TABLE_HEIGHT/2;
    parameter X_BIT_WIDTH   = $bits(TABLE_WIDTH);
    parameter Y_BIT_WIDTH   = $bits(TABLE_HEIGHT);
`else
    parameter TABLE_WIDTH   = 128;
    parameter TABLE_HEIGHT  = 64;
// Yosys interprete $bits(integer) as 32-bits
// Exact bit-width must be provided according to Table's width & height
    parameter X_BIT_WIDTH   = 9;
    parameter Y_BIT_WIDTH   = 8;
`endif    

    parameter SCREEN_WIDTH  = TABLE_WIDTH  + 5;
    parameter SCREEN_HEIGHT = TABLE_HEIGHT + 5;


    wire                    vid_on;
    reg                     pixel_tick;
    reg [X_BIT_WIDTH-1:0]   x;
    reg [Y_BIT_WIDTH-1:0]   y;

    always @(posedge clk or posedge reset)
    begin
        if (reset)
            pixel_tick <= 0;
        else
            pixel_tick <= ~p_tick;  // Toggle
    end
    assign p_tick = pixel_tick;

    always @(posedge clk or posedge reset)
    begin
        if (reset)
        begin
            x <= 0;
            y <= 0;
        end
        else
        begin
            if (p_tick)
            begin
                if (x>=X_BIT_WIDTH'(SCREEN_WIDTH))
                begin
                    x <= 0;
                    if (y>=Y_BIT_WIDTH'(SCREEN_HEIGHT))
                       y <= 0;
                    else
                        y <= y + 1;
                end
                else
                    x <= x + 1;
            end
        end
    end
    assign hsync  = (x==X_BIT_WIDTH'(SCREEN_WIDTH))?  1:0;
    assign vsync  = (y==Y_BIT_WIDTH'(SCREEN_HEIGHT))? 1:0;
    assign vid_on = (x<=X_BIT_WIDTH'(TABLE_WIDTH)) && (y<=Y_BIT_WIDTH'(TABLE_HEIGHT));

    pixel_gen
        #(  .TABLE_WIDTH(TABLE_WIDTH),
            .TABLE_HEIGHT(TABLE_HEIGHT),
            .WALL_THICKNESS(8),
            .PADDLE_HEIGHT(TABLE_HEIGHT/4),
            .PADDLE_VELOCITY(1),
            .BALL_VELOCITY_POS(+1),
            .BALL_VELOCITY_NEG(-1),
            .X_BIT_WIDTH(X_BIT_WIDTH),
            .Y_BIT_WIDTH(Y_BIT_WIDTH)
        )
        pg
        (   .clk(clk),
            .reset(reset),
            .up(up),
            .down(down), 
            .video_on(vid_on),
            .x(x),
            .y(y),
            .rgb(rgb)
        );

endmodule
