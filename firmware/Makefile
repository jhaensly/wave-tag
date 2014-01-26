C_SRC   += main.c
C_SRC   += accel.c

# The AVR device to compile for
DEVICE      = attiny48

# Target AVR clock rate in Hertz
CLOCK       = 8000000

# Options to avrdude which define the hardware you use for
# uploading to the AVR and the interface where this hardware
# is connected.
PROGRAMMER  = -c avrispmkii -P usb -F -B 100

# Parameters for avrdude to flash the fuses appropriately.
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

CFLAGS  = -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) $(DEFINES)
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
