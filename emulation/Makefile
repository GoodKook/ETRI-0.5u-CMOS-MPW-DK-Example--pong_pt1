#
## Poorman's Standard-Emulator by GoodKook, goodkook@gmail.com
#	Makefile for Co-Simulation of sample project "DUT"
#

TOP_MODULE   = pong_pt1

#VERILOG_SRCS = ../source/$(TOP_MODULE).v \
#			   ../source/pixel_gen.v \
#			   ../source/vga_controller.v
SC_SRCS      = ../simulation/sc_main.cpp \
               ../simulation/sc_$(TOP_MODULE)_TB.cpp
SC_TOP_H     = ../simulation/sc_$(TOP_MODULE)_TB.h \
               ./pong_pt1.h \
               ./pong_pt1_SA.h \
			   ./pong_pt1_RT.h

#VERILATOR    = verilator
CFLAGS       = -std=c++17
CFLAGS      += -g -Wall -Wfatal-errors
#CFLAGS      += -DCO_EMULATION
#CFLAGS      += -DCO_EMULATION_SA
#CFLAGS      += -DCO_EMULATION_SA2
CFLAGS      += -DCO_EMULATION_RT
#CFLAGS      += -DVCD_TRACE
# For CO_EMULATION_SA, specify display device type
#   1. VGAX:
#CFLAGS      += -DVGAX_120x60
#   2. GLCD:
CFLAGS      += -DGLCD_128x64

LDFLAGS      = -lSDL2
LDFLAGS     += -lsystemc
#TARGET       = V$(TOP_MODULE)
#TARGET_DIR   = obj_dir

TARGET       = $(TOP_MODULE)
TARGET_DIR   = .

export SYSTEMC			= /usr/local/systemc-3.0.0
export SYSTEMC_HOME		= $(SYSTEMC)
export SYSTEMC_INCLUDE	= $(SYSTEMC_HOME)/include
export SYSTEMC_LIBDIR	= $(SYSTEMC_HOME)/lib-linux64
export LD_LIBRARY_PATH	:=$(LD_LIBRARY_PATH):$(SYSTEMC_LIBDIR)
export CXX				= clang++
export CXXFLAGS			= -std=c++17

all :
	@echo ''
	@echo 'Makefile for "'$(TOP_MODULE)'" Co-Emulation: FPGA implemented Verilog RTL(*) under SystemC Testbench[*]'
	@echo ''
	@echo '    make build'
	@echo '        Build executable of "'$(TOP_MODULE)'" with Verilated RTL and SystemC TB'
	@echo '        1. Cycle-Accurate mode, compiled with -DCO_EMULATION'
	@echo '        2. Tranaction-HSync mode, compiled with -DCO_EMULATION_SA'
	@echo '        3. Tranaction-NONE mode, compiled with -DCO_EMULATION_RT'
	@echo '        {*} Change compiler option by modifying variable CFLAGS in "Makefile"'
	@echo ''
	@echo '    make run'
	@echo '        Simulate "'$(TOP_MODULE)'"'
	@echo ''
	@echo '    make wave'
	@echo '        View SystemC generated waveform'
	@echo ''
	@echo '    make clean'
	@echo '        Clean working folder'
	@echo ''
	@echo '  (*) https://github.com/FPGADude/Digital-Design/tree/main/FPGA%20Projects/VGA%20Projects/Pong%20pt1'
	@echo '  [*] Poorman`s Std. Co-Emulation(PSCE), GoodKook, goodkook@gmail.com'
	@echo ''

build : $(TARGET_DIR)/$(TARGET)

$(TARGET_DIR)/$(TARGET) : $(VERILOG_SRCS) $(SC_SRCS) $(SC_TOP_H)
	clang++ -I$(SYSTEMC_INCLUDE) -I../emulation \
			$(CFLAGS) \
			$(LDFLAGS) \
			-L$(SYSTEMC_LIBDIR) \
			-o $(TARGET) \
			$(SC_SRCS)

#	$(VERILATOR) --sc -Wall --top-module $(TOP_MODULE) --exe --build \
#		-CFLAGS "$(CFLAGS)" \
#		-LDFLAGS "$(LDFLAGS)" \
#		$(VFLAGS) \
#		$(VERILOG_SRCS) $(SC_SRCS)

lint : $(VERILOG_SRCS)
	$(VERILATOR) --sc -Wall --top-module $(TOP_MODULE) $(VERILOG_SRCS)

run : $(TARGET_DIR)/$(TARGET)
	./$(TARGET_DIR)/$(TARGET)

wave : sc_$(TOP_MODULE)_TB.vcd
	gtkwave sc_$(TOP_MODULE).vcd --save=sc_$(TOP_MODULE).gtkw &
#	gtkwave sc_$(TOP_MODULE)_TB.vcd --save=sc_$(TOP_MODULE)_TB.gtkw &

clean :
	rm -f $(TARGET)
	rm -f *.vcd

debug : $(TARGET_DIR)/$(TARGET)
	ddd $(TARGET_DIR)/$(TARGET)

