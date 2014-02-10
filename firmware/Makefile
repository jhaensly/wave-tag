C_SRC   += main.c
C_SRC   += accel.c
C_SRC   += vlc_decoder_data.c
C_SRC   += wave.c
C_SRC   += vlc.c
C_SRC   += sleep.c

# The AVR device to compile for
DEVICE      = attiny48

# Target AVR clock rate in Hertz
CLOCK       = 8000000

# Parameters for avrdude to flash the fuses appropriately.
FUSES       = -U hfuse:w:0xDF:m -U lfuse:w:0x6E:m

# Options to avrdude which define the hardware you use for
# uploading to the AVR and the interface where this hardware
# is connected. This can be overridden in the file config.mk,
# but will otherwise default to the AVRISP mkii.
-include config.mk
PROGRAMMER ?= -c avrispmkii -P usb

SRC_PATH    = $(shell pwd)
APP_NAME    = wave-tag
OBJ_DIR     = $(SRC_PATH)/obj
ELF_FILE    = $(OBJ_DIR)/$(APP_NAME).elf
HEX_FILE    = $(OBJ_DIR)/$(APP_NAME).hex
SYM_FILE    = $(OBJ_DIR)/$(APP_NAME).sym
MAP_FILE    = $(OBJ_DIR)/$(APP_NAME).map
SYM_FILE    = $(OBJ_DIR)/$(APP_NAME).sym
OBJECTS     = $(addprefix $(OBJ_DIR)/, $(C_SRC:.c=.o))
Q          ?= @

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

CPPFLAGS= -DF_CPU=$(CLOCK) $(DEFINES)
CFLAGS  = -Os -std=c99 -mmcu=$(DEVICE) $(CPPFLAGS) \
			-Wall -Werror -Wno-empty-body -Wmissing-field-initializers \
			-Wpointer-arith -Wuninitialized -Wstrict-overflow -Wundef \
			-Wlogical-op -Wshadow -fshort-enums
LDFLAGS = -Wl,--gc-sections,-Map=$(MAP_FILE) 

AVRDUDE = avrdude $(PROGRAMMER) -F -B 10 -p $(DEVICE)
COMPILE = $(CC) $(CFLAGS) $(LDFLAGS)

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
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

ifneq "$(MAKECMDGOALS)" "clean"
-include $(OBJECTS:.o=.d)
endif
$(OBJ_DIR)/%.d: %.c
	@mkdir -p $(dir $@)
	$(Q)$(CC) $(CFLAGS) -MM -MT $(patsubst %.c, $(OBJ_DIR)/%.o,$<) $< -MF $@

$(ELF_FILE): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(Q)$(COMPILE) $^ -o $@
	$(SIZE) -Bxt $@

$(HEX_FILE): $(ELF_FILE)
	$(Q)rm -f $@
	$(Q)$(OBJCOPY) -j .text -j .data -O ihex $< $@

$(SYM_FILE): $(ELF_FILE)
	$(Q)$(NM) -n $< > $@
