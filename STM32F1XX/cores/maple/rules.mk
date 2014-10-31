# Standard things
sp              := $(sp).x
dirstack_$(sp)  := $(d)
d               := $(dir)
BUILDDIRS       += $(BUILD_PATH)/$(d)

# Add board directory and MCU-specific directory to BUILDDIRS. These
# are in subdirectories, but they're logically part of the Wirish
# submodule.
WIRISH_BOARD_PATH := boards/$(BOARD)
BUILDDIRS += $(BUILD_PATH)/$(d)/$(WIRISH_BOARD_PATH)
BUILDDIRS += $(BUILD_PATH)/$(d)/$(MCU_SERIES)

# Safe includes for Wirish.
WIRISH_INCLUDES := -I$(d)/include -I$(d)/$(WIRISH_BOARD_PATH)/include

# Local flags. Add -I$(d) to allow for private includes.
CFLAGS_$(d) := $(LIBMAPLE_INCLUDES) $(WIRISH_INCLUDES) -I$(d)

# Local rules and targets
sSRCS_$(d) := start.S
cSRCS_$(d) := start_c.c
cSRCS_$(d) += syscalls.c
cSRCS_$(d) += $(MCU_SERIES)/util_hooks.c
cppSRCS_$(d) := boards.cpp
cppSRCS_$(d) += cxxabi-compat.cpp
cppSRCS_$(d) += ext_interrupts.cpp
cppSRCS_$(d) += HardwareSerial.cpp
cppSRCS_$(d) += HardwareTimer.cpp
cppSRCS_$(d) += Print.cpp
cppSRCS_$(d) += pwm.cpp
ifeq ($(MCU_SERIES), stm32f1)
cppSRCS_$(d) += usb_serial.cpp	# HACK: this is currently STM32F1 only.
cppSRCS_$(d) += HardwareSPI.cpp	# FIXME: port to F2 and fix wirish.h
endif
cppSRCS_$(d) += wirish_analog.cpp
cppSRCS_$(d) +=	wirish_digital.cpp
cppSRCS_$(d) +=	wirish_math.cpp
cppSRCS_$(d) +=	wirish_shift.cpp
cppSRCS_$(d) +=	wirish_time.cpp
cppSRCS_$(d) += $(MCU_SERIES)/boards_setup.cpp
cppSRCS_$(d) += $(MCU_SERIES)/wirish_digital.cpp
cppSRCS_$(d) += $(MCU_SERIES)/wirish_debug.cpp
cppSRCS_$(d) += $(WIRISH_BOARD_PATH)/board.cpp

sFILES_$(d)   := $(sSRCS_$(d):%=$(d)/%)
cFILES_$(d)   := $(cSRCS_$(d):%=$(d)/%)
cppFILES_$(d) := $(cppSRCS_$(d):%=$(d)/%)

OBJS_$(d)     := $(sFILES_$(d):%.S=$(BUILD_PATH)/%.o) \
                 $(cFILES_$(d):%.c=$(BUILD_PATH)/%.o) \
                 $(cppFILES_$(d):%.cpp=$(BUILD_PATH)/%.o)
DEPS_$(d)     := $(OBJS_$(d):%.o=%.d)

$(OBJS_$(d)): TGT_CFLAGS := $(CFLAGS_$(d))

TGT_BIN += $(OBJS_$(d))

# Standard things
-include        $(DEPS_$(d))
d               := $(dirstack_$(sp))
sp              := $(basename $(sp))
