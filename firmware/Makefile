C_SRC   := main.c
C_SRC   += accel.c
C_SRC   += vlc_decoder_data.c
C_SRC   += wave.c
C_SRC   += vlc.c
C_SRC   += sleep.c
C_SRC   += glyph.c
C_SRC   += display.c
C_SRC   += button.c
C_SRC   += timer.c

# The AVR device to compile for
DEVICE      := attiny88
MCU         := -mmcu=$(DEVICE)

# Target AVR clock rate in Hertz
DEFINES     += -DF_CPU=8000000

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
FUSES       := -U hfuse:w:0xDF:m -U lfuse:w:0x6E:m

# Options to avrdude which define the hardware you use for
# uploading to the AVR and the interface where this hardware
# is connected. This can be overridden in the file config.mk,
# but will otherwise default to the AVRISP mkii.
-include config.mk
PROGRAMMER  ?= -c avrispmkii -P usb

SRC_PATH    := ./
APP_NAME    := wave-tag
OBJ_DIR     := $(SRC_PATH)/obj
ELF_FILE    := $(OBJ_DIR)/$(APP_NAME).elf
HEX_FILE    := $(OBJ_DIR)/$(APP_NAME).hex
SYM_FILE    := $(OBJ_DIR)/$(APP_NAME).sym
MAP_FILE    := $(OBJ_DIR)/$(APP_NAME).map
SYM_FILE    := $(OBJ_DIR)/$(APP_NAME).sym
OBJECTS     := $(addprefix $(OBJ_DIR)/, $(C_SRC:.c=.o))
Q           ?= @

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

CPPFLAGS= $(MCU) $(DEFINES)
CFLAGS  = -Os -std=gnu99 \
			-Wall -Werror -Wno-empty-body -Wmissing-field-initializers \
			-Wpointer-arith -Wuninitialized -Wstrict-overflow -Wundef \
			-Wlogical-op -Wshadow -fshort-enums
LDFLAGS = $(MCU) -Wl,--gc-sections,-Map=$(MAP_FILE) 

AVRDUDE = avrdude $(PROGRAMMER) -F -B 10 -p $(DEVICE)

.PHONY : all
all: $(HEX_FILE) $(SYM_FILE)

.PHONY : flash
flash:	all
	$(AVRDUDE) -U flash:w:$(HEX_FILE):i

.PHONY : fuse
fuse:
	$(AVRDUDE) $(FUSES)

# Xcode uses the Makefile targets "", "clean" and "install"
.PHONY : install
install: all flash fuse

.PHONY : load
load: all
	bootloadHID $(HEX_FILE)

.PHONY : doc
doc:
	doxygen Doxyfile

.PHONY : disasm
disasm:	$(ELF_FILE)
	$(OBJDUMP) -d $(ELF_FILE)

.PHONY : clean
clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(OBJ_DIR)/*.d
	rm -f $(OBJ_DIR)/*.elf
	rm -f $(OBJ_DIR)/*.map
	rm -f $(OBJ_DIR)/*.hex
	rm -f $(OBJ_DIR)/*.sym
	rm -rf $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.c 
	@echo "CC $(notdir $<)"
	@mkdir -p $(dir $@)
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

ifneq "$(MAKECMDGOALS)" "clean"
-include $(OBJECTS:.o=.d)
endif
$(OBJ_DIR)/%.d: %.c
	@mkdir -p $(dir $@)
	@set -e; rm -f $@; \
    $(CC) -MM -MT $(patsubst %.c,$(OBJ_DIR)/%.o,$<) $(CPPFLAGS) $< > $@.$$$$; \
    sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
    rm -f $@.$$$$

$(ELF_FILE): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(Q)$(CC) $^ $(LDFLAGS) -o $@
	$(SIZE) -Bxt $@

$(HEX_FILE): $(ELF_FILE)
	$(Q)rm -f $@
	$(Q)$(OBJCOPY) -j .text -j .data -O ihex $< $@

$(SYM_FILE): $(ELF_FILE)
	$(Q)$(NM) -n $< > $@
