# Standard things
sp              := $(sp).x
dirstack_$(sp)  := $(d)
d               := $(dir)
BUILDDIRS       += $(BUILD_PATH)/$(d)

# Local flags
CFLAGS_$(d) = -I$(d) -I$(d)/$(MCU_SERIES) -I$(d)/usb_lib $(LIBMAPLE_INCLUDES) $(LIBMAPLE_PRIVATE_INCLUDES) -Wall

# Add usblib and series subdirectory to BUILDDIRS.
BUILDDIRS += $(BUILD_PATH)/$(d)/$(MCU_SERIES)
BUILDDIRS += $(BUILD_PATH)/$(d)/usb_lib

# Local rules and targets
sSRCS_$(d) :=
cSRCS_$(d) :=
# We currently only have F1 performance line support. Sigh.
ifeq ($(MCU_SERIES), stm32f1)
ifeq ($(MCU_F1_LINE), performance)
cSRCS_$(d) += $(MCU_SERIES)/usb.c
cSRCS_$(d) += $(MCU_SERIES)/usb_reg_map.c
cSRCS_$(d) += $(MCU_SERIES)/usb_cdcacm.c
cSRCS_$(d) += usb_lib/usb_core.c
cSRCS_$(d) += usb_lib/usb_init.c
cSRCS_$(d) += usb_lib/usb_mem.c
cSRCS_$(d) += usb_lib/usb_regs.c
endif
endif

sFILES_$(d) := $(sSRCS_$(d):%=$(d)/%)
cFILES_$(d) := $(cSRCS_$(d):%=$(d)/%)

OBJS_$(d) := $(sFILES_$(d):%.S=$(BUILD_PATH)/%.o) \
             $(cFILES_$(d):%.c=$(BUILD_PATH)/%.o)
DEPS_$(d) := $(OBJS_$(d):%.o=%.d)

$(OBJS_$(d)): TGT_CFLAGS := $(CFLAGS_$(d))
$(OBJS_$(d)): TGT_ASFLAGS :=

TGT_BIN += $(OBJS_$(d))

# Standard things
-include        $(DEPS_$(d))
d               := $(dirstack_$(sp))
sp              := $(basename $(sp))
