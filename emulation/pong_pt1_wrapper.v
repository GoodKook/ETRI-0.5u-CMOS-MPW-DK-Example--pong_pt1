//
// Poorman's Standard-Emulator by GoodKook, goodkook@gmail.com
//  Purpose: Standard emulation wrapper for the DUT
//
//  DUT: Pong game with VGA interface
//

`ifdef EMU_MONITOR_LED
module pong_pt1_wrapper(Din_emu, Dout_emu, Addr_emu, load_emu, get_emu, clk_emu, clk_dut, clk_LED,
                        p_tick_ex, hsync_ex, vsync_ex, pixel_ex);
`else
module pong_pt1_wrapper(Din_emu, Dout_emu, Addr_emu, load_emu, get_emu, clk_emu, clk_dut,
                        p_tick_ex, hsync_ex, vsync_ex, pixel_ex);
`endif
    input  [7:0]    Din_emu;
    output [7:0]    Dout_emu;
    input  [2:0]    Addr_emu;
    input           load_emu, get_emu, clk_emu;
    input           clk_dut;

    output          p_tick_ex, hsync_ex, vsync_ex, pixel_ex;

`ifdef EMU_MONITOR_LED
    output clk_LED;
    // Monitoring emulation process by blinking LED
    reg [3:0] counter;
    always @(posedge clk_dut)
    begin
        counter <= counter + 1;
    end
    assign clk_LED = counter[3];
`endif
    // Standard Emulation wrapper: Stimulus & Output capture for DUT
    parameter   NUM_STIM_ARRAY  = 1,
                NUM_OUT_ARRAY   = 2;
    reg [7:0]   stimIn[NUM_STIM_ARRAY];
    reg [7:0]   vectOut[NUM_OUT_ARRAY];
    reg [7:0]   Dout_emu;

    // DUT interface: registered input
    reg clk;
    reg reset;          // btnR
    reg up;             // btnU
    reg down;           // btnD
    // DUT interface: output wire. DUT's output will be captured
    wire p_tick;
    wire hsync;
    wire vsync;
    wire [11:0] rgb;

    // Emulation Transactor ---------------------------------------------
    //  stimIn[0] = {----|clk|reset|up|down}
    // vectOut[0] = {-|p_tick|hsync|vsync|rgb[11:8]}
    // vectOut[1] = rgb[7:0]
    always @(posedge clk_emu)
    begin
        if (load_emu)       // Set input stimulus to DUT
        begin
            reset <= stimIn[0][2];
            up    <= stimIn[0][1];
            down  <= stimIn[0][0];
        end
        else if (get_emu)   // Capure output from DUT
        begin
            vectOut[0][6]   <= p_tick;
            vectOut[0][5]   <= hsync;
            vectOut[0][4]   <= vsync;
            vectOut[0][3:0] <= rgb[11:8];
            vectOut[1]      <= rgb[7:0];
        end
        else
        begin
            stimIn[Addr_emu] <= Din_emu;    // stimulus: Host -> DUT
            Dout_emu <= vectOut[Addr_emu];  // output vector: DUT->Host
        end
    end

    assign p_tick_ex = p_tick;
    assign hsync_ex = hsync;
    assign vsync_ex = vsync;
    assign pixel_ex = rgb[9];

    // DUT --------------------------------------------------------------
    pong_pt1 u_pong_pt1 (
                .clk(clk_dut),
                .reset(reset),
                .up(up),
                .down(down),
                .p_tick(p_tick),
                .hsync(hsync),
                .vsync(vsync),
                .rgb(rgb)
            );
endmodule
