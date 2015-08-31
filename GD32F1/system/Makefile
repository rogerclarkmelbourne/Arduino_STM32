# Try "make help" first

.DEFAULT_GOAL := sketch

##
## Useful paths, constants, etc.
##

ifeq ($(LIB_MAPLE_HOME),)
SRCROOT := .
else
SRCROOT := $(LIB_MAPLE_HOME)
endif

BUILD_PATH     = build
LIBMAPLE_PATH  := $(SRCROOT)/libmaple
WIRISH_PATH    := $(SRCROOT)/wirish
SUPPORT_PATH   := $(SRCROOT)/support
LIBRARIES_PATH := $(SRCROOT)/libraries
# Support files for linker
LDDIR          := $(SUPPORT_PATH)/ld
# Support files for this Makefile
MAKEDIR        := $(SUPPORT_PATH)/make
BOARD_INCLUDE_DIR := $(MAKEDIR)/board-includes

##
## Target-specific configuration.  This determines some compiler and
## linker options/flags.
##

# Try "make help" for more information on BOARD and MEMORY_TARGET;
# these default to a Maple Flash build.
BOARD         ?= maple
MEMORY_TARGET ?= flash

# Chooses the bootloader, available: maple and robotis
BOOTLOADER    ?= maple

# This is the serial port used by robotis bootloader
ROBOTIS_PORT ?= /dev/ttyACM0

# $(BOARD)- and $(MEMORY_TARGET)-specific configuration
include $(MAKEDIR)/target-config.mk

##
## Build rules and useful templates
##

include $(MAKEDIR)/build-rules.mk
include $(MAKEDIR)/build-templates.mk

##
## Compilation flags
##

# FIXME: the following allows for deprecated include style, e.g.:
#     #include "libmaple.h"
# or
#     #include "wirish.h"
# It slows compilation noticeably; remove after 1 release.
TARGET_FLAGS    += -I$(LIBMAPLE_PATH)/include/libmaple                       \
                   -I$(WIRISH_PATH)/include/wirish
TARGET_FLAGS    += -I$(LIBRARIES_PATH) # for internal lib. includes, e.g. <Wire/WireBase.h>
GLOBAL_CFLAGS   := -Os -g3 -gdwarf-2 -nostdlib \
                   -ffunction-sections -fdata-sections \
		   -Wl,--gc-sections $(TARGET_FLAGS) \
		   -DBOOTLOADER_$(BOOTLOADER)
GLOBAL_CXXFLAGS := -fno-rtti -fno-exceptions -Wall $(TARGET_FLAGS)
GLOBAL_ASFLAGS  := -x assembler-with-cpp $(TARGET_FLAGS)
LDFLAGS  = $(TARGET_LDFLAGS) $(TOOLCHAIN_LDFLAGS) -mcpu=cortex-m3 -mthumb \
           -Xlinker --gc-sections \
           -Xassembler --march=armv7-m -Wall
#          -Xlinker --print-gc-sections \

##
## Set all submodules here
##

LIBMAPLE_MODULES += $(SRCROOT)/libmaple
LIBMAPLE_MODULES += $(SRCROOT)/libmaple/usb   # The USB module is kept separate
LIBMAPLE_MODULES += $(LIBMAPLE_MODULE_SERIES) # STM32 series submodule in libmaple
## LIBMAPLE_MODULES += $(SRCROOT)/wirish

# Official libraries:
##LIBMAPLE_MODULES += $(SRCROOT)/libraries/Servo
##LIBMAPLE_MODULES += $(SRCROOT)/libraries/LiquidCrystal
##LIBMAPLE_MODULES += $(SRCROOT)/libraries/Wire
# Experimental libraries:
##LIBMAPLE_MODULES += $(SRCROOT)/libraries/FreeRTOS

# User modules:
ifneq ($(USER_MODULES),)
LIBMAPLE_MODULES += $(USER_MODULES)
endif

# Call each module's rules.mk:
$(foreach m,$(LIBMAPLE_MODULES),$(eval $(call LIBMAPLE_MODULE_template,$(m))))

##
## Targets
##

# main target
include $(SRCROOT)/build-targets.mk

.PHONY: install sketch clean help cscope tags ctags ram flash jtag doxygen mrproper list-boards

# Target upload commands
# USB ID for DFU upload -- FIXME: do something smarter with this
BOARD_USB_VENDOR_ID  := 1EAF
BOARD_USB_PRODUCT_ID := 0003

ifeq ($(BOOTLOADER),maple)
UPLOAD_ram   := $(SUPPORT_PATH)/scripts/reset.py && \
                sleep 1                  && \
                $(DFU) -a0 -d $(BOARD_USB_VENDOR_ID):$(BOARD_USB_PRODUCT_ID) -D $(BUILD_PATH)/$(BOARD).bin -R
UPLOAD_flash := $(SUPPORT_PATH)/scripts/reset.py && \
                sleep 1                  && \
                $(DFU) -a1 -d $(BOARD_USB_VENDOR_ID):$(BOARD_USB_PRODUCT_ID) -D $(BUILD_PATH)/$(BOARD).bin -R
endif

ifeq ($(BOOTLOADER),robotis)
UPLOAD_flash := $(SUPPORT_PATH)/scripts/robotis-loader.py $(ROBOTIS_PORT) $(BUILD_PATH)/$(BOARD).bin
endif

# Conditionally upload to whatever the last build was
install: INSTALL_TARGET = $(shell cat $(BUILD_PATH)/build-type 2>/dev/null)
install: $(BUILD_PATH)/$(BOARD).bin
	@echo "Install target:" $(INSTALL_TARGET)
	$(UPLOAD_$(INSTALL_TARGET))

# Force a rebuild if the target changed
PREV_BUILD_TYPE = $(shell cat $(BUILD_PATH)/build-type 2>/dev/null)
build-check:
ifneq ($(PREV_BUILD_TYPE), $(MEMORY_TARGET))
	$(shell rm -rf $(BUILD_PATH))
endif

sketch: build-check MSG_INFO $(BUILD_PATH)/$(BOARD).bin

clean:
	rm -rf build

mrproper: clean
	rm -rf doxygen

help:
	@echo ""
	@echo "Basic usage (BOARD defaults to maple):"
	@echo "    $$ cp your-main.cpp main.cpp"
	@echo "    $$ make BOARD=your_board"
	@echo "    $$ make BOARD=your_board install"
	@echo ""
	@echo "(Multiple source files? Link with libmaple.a (\`$$ make library')"
	@echo "or hack build-targets.mk appropriately.)"
	@echo ""
	@echo "Important targets:"
	@echo "    sketch:   Compile for BOARD to MEMORY_TARGET (default)."
	@echo "    install:  Compile and upload over USB using Maple bootloader"
	@echo ""
	@echo "You *must* set BOARD if not compiling for Maple (e.g."
	@echo "use BOARD=maple_mini for mini, etc.), and MEMORY_TARGET"
	@echo "if not compiling to Flash. Run \`$$ make list-boards' for"
	@echo "a list of all boards."
	@echo ""
	@echo "Valid MEMORY_TARGETs (default=flash):"
	@echo "    ram:    Compile to RAM (doesn't touch Flash)"
	@echo "    flash:  Compile to Flash (for Maple bootloader)"
	@echo "    jtag:   Compile for JTAG/SWD upload (overwrites bootloader)"
	@echo ""
	@echo "Other targets:"
	@echo "    clean: Remove all build and object files"
	@echo "    doxygen: Build Doxygen HTML and XML documentation"
	@echo "    help: Show this message"
	@echo "    mrproper: Remove all generated files"
	@echo ""

cscope:
	rm -rf cscope.*
	find . -name '*.[hcS]' -o -name '*.cpp' | xargs cscope -b

tags:
	etags `find . -name "*.c" -o -name "*.cpp" -o -name "*.h"`
	@echo "Made TAGS file for EMACS code browsing"

ctags:
	ctags-exuberant -R .
	@echo "Made tags file for VIM code browsing"

ram:
	@$(MAKE) MEMORY_TARGET=ram --no-print-directory sketch

flash:
	@$(MAKE) MEMORY_TARGET=flash --no-print-directory sketch

jtag:
	@$(MAKE) MEMORY_TARGET=jtag --no-print-directory sketch

doxygen:
	doxygen $(SUPPORT_PATH)/doxygen/Doxyfile

# This output is kind of ugly, but I don't understand make very well.
list-boards:
	@echo " $(addsuffix "\\n",$(basename $(notdir $(wildcard $(BOARD_INCLUDE_DIR)/*.mk))))"
