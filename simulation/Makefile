#
## Poorman's Standard-Emulator by GoodKook, goodkook@gmail.com
#	Makefile for Co-Simulation of sample project "DUT"
#

TOP_MODULE   = pong_pt1

VERILOG_SRCS = ../source/$(TOP_MODULE).v \
			   ../source/pixel_gen.v
SC_SRCS      = sc_main.cpp \
               sc_$(TOP_MODULE)_TB.cpp
SC_TOP_H     = sc_$(TOP_MODULE)_TB.h

VERILATOR    = verilator
CFLAGS       = -std=c++17
CFLAGS      += -g -Wall  -Wfatal-errors
# DUT type is VERILATION (RTL simulation)
CFLAGS      += -O3 -DVERILATION
CFLAGS      += -DVCD_TRACE
LDFLAGS      = -lSDL2
# Specify display type on Verilog RTL
#VFLAGS       = -DHALF_TABLE_SIZE
#VFLAGS       = -DMAX_TABLE_SIZE
#VFLAGS       = -DVGAX_120x60
VFLAGS       = -DGLCD_128x64

TARGET       = V$(TOP_MODULE)
TARGET_DIR   = obj_dir

export SYSTEMC			= /usr/local/systemc-3.0.0
export SYSTEMC_HOME		= $(SYSTEMC)
export SYSTEMC_INCLUDE	= $(SYSTEMC_HOME)/include
export SYSTEMC_LIBDIR	= $(SYSTEMC_HOME)/lib-linux64
export LD_LIBRARY_PATH	:=$(LD_LIBRARY_PATH):$(SYSTEMC_LIBDIR)
export CXX				= clang++
export CXXFLAGS			= -std=c++17

all :
	@echo ''
	@echo 'Makefile for "'$(TOP_MODULE)'" Co-Simulation: Verilog RTL(*) under SystemC Testbench[*]'
	@echo ''
	@echo '    make build'
	@echo '        Build executable of "'$(TOP_MODULE)'" with Verilated RTL and SystemC TB'
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
	$(VERILATOR) --sc -Wall --top-module $(TOP_MODULE) --exe --build \
		-CFLAGS "$(CFLAGS)" \
		-LDFLAGS "$(LDFLAGS)" \
		$(VFLAGS) \
		$(VERILOG_SRCS) $(SC_SRCS)

lint : $(VERILOG_SRCS)
	$(VERILATOR) --sc -Wall --top-module $(TOP_MODULE) $(VERILOG_SRCS)

run : $(TARGET_DIR)/$(TARGET)
	./$(TARGET_DIR)/$(TARGET)

wave : sc_$(TOP_MODULE)_TB.vcd
	gtkwave sc_$(TOP_MODULE)_TB.vcd --save=sc_$(TOP_MODULE)_TB.gtkw &

clean :
	rm -rf $(TARGET_DIR)
	rm -f *.vcd

debug : $(TARGET_DIR)/$(TARGET)
	ddd $(TARGET_DIR)/$(TARGET)

