C_SRC   += main.c
C_SRC   += accel.c
C_SRC   += vlc_decoder_data.c
C_SRC   += alphabet.c
C_SRC   += vlc.c
C_SRC   += timer.c

# The AVR device to compile for
DEVICE      = attiny48

# Target AVR clock rate in Hertz
DEFINES    += -DF_CPU=1000000

# Options to avrdude which define the hardware you use for
# uploading to the AVR and the interface where this hardware
# is connected.
PROGRAMMER  = -c avrispmkii -P usb -F -B 10

# Parameters for avrdude to flash the fuses appropriately.
# For the ATTiny48, clear to enable:
#   Fuse high byte
#   x___ ____ RSTDSBL:  Disable external reset (0b1: reset enabled)
#   _x__ ____ DWEN:     Enable debugWIRE (0b1: debug disabled)
#   __x_ ____ SPIEN:    Enable SPI programming (0b0: SPI enabled)
#   ___x ____ WDTON:    Watchdog always on (0b1: disabled)
#   ____ x___ EESAVE:   Preserve EEPROM across chip erase (0b1: not preserved)
#   ____ _xxx BODLEVEL: Brown-out detector trigger level (0b111: BOD disabled)
#
#   Fuse low byte
#   x___ ____ CKDIV8:   Divide clock by 8 (0b0: sys clock div 8)
#   _x__ ____ CKOUT:    Output sys clock on the CLKO pin (0b1: disabled) 
#   __xx ____ SUT:      Select start up time (0b10: slowly rising power)
#   ____ xx__           Unused
#   ____ __xx CKSEL:    Select clock source (0b10: 8MHz internal osc)
FUSES       = -U hfuse:w:0xDF:m -U lfuse:w:0x6E:m

SRC_PATH    = $(shell pwd)
APP_NAME    = wave-tag
OBJ_DIR     = $(SRC_PATH)/obj
ELF_FILE    = $(OBJ_DIR)/$(APP_NAME).elf
HEX_FILE    = $(OBJ_DIR)/$(APP_NAME).hex
MAP_FILE    = $(OBJ_DIR)/$(APP_NAME).map
OBJECTS     = $(addprefix $(OBJ_DIR)/, $(C_SRC:.c=.o))

# Toolchain aliases
TC_PRE  = avr-
CC      = $(TC_PRE)gcc
CXX     = $(TC_PRE)g++
CPP     = $(TC_PRE)cpp
AR      = $(TC_PRE)ar
NM      = $(TC_PRE)nm
OBJCOPY = $(TC_PRE)objcopy
OBJDUMP = $(TC_PRE)objdump
SIZE    = $(TC_PRE)size

include version.inc

CFLAGS  = -std=c99 -Os -mmcu=$(DEVICE) $(DEFINES) \
			-Wall -Werror -Wno-tautological-compare -Wno-empty-body \
			-Wmissing-field-initializers -Wpointer-arith -Wuninitialized \
			-Wstrict-overflow -Wundef -Wlogical-op -Wshadow -fshort-enums
LDFLAGS = -Wl,--gc-sections,-Map=$(MAP_FILE) 

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = $(CC) $(CFLAGS) $(LDFLAGS)

.PHONY : all
all: obj_dir $(APP_NAME)

obj_dir:
	mkdir -p $(OBJ_DIR)

.PHONY : $(APP_NAME)
$(APP_NAME): $(HEX_FILE)

$(OBJ_DIR)/%.o: %.c
	$(COMPILE) -c $< -o $@

.PHONY : flash
flash:	all
	$(AVRDUDE) -U flash:w:$(HEX_FILE):i

.PHONY : fuse
fuse:
	$(AVRDUDE) $(FUSES)

# Xcode uses the Makefile targets "", "clean" and "install"
.PHONY : install
install: flash fuse

.PHONY : load
load: all
	bootloadHID $(HEX_FILE)

.PHONY : doc
doc:
	doxygen Doxyfile

.PHONY : clean
clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(OBJ_DIR)/*.elf
	rm -f $(OBJ_DIR)/*.map
	rm -f $(OBJ_DIR)/*.hex
	rm -rf $(OBJ_DIR)

# file targets:
$(ELF_FILE): $(OBJECTS)
	$(COMPILE) -o $(ELF_FILE) $(OBJECTS)

$(HEX_FILE): $(ELF_FILE)
	rm -f $(HEX_FILE)
	$(OBJCOPY) -j .text -j .data -O ihex $(ELF_FILE) $(HEX_FILE)
# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.

# Targets for code debugging and analysis:
disasm:	$(ELF_FILE)
	$(OBJDUMP) -d $(ELF_FILE)
