# Standard things
sp              := $(sp).x
dirstack_$(sp)  := $(d)
d               := $(dir)
BUILDDIRS       += $(BUILD_PATH)/$(d)

# Local flags
CFLAGS_$(d) = -I$(d) $(LIBMAPLE_PRIVATE_INCLUDES) $(LIBMAPLE_INCLUDES) -Wall -Werror
ASFLAGS_$(d) = -I$(d) $(LIBMAPLE_PRIVATE_INCLUDES) $(LIBMAPLE_INCLUDES) -Wall -Werror

# Extra BUILDDIRS
BUILDDIRS += $(BUILD_PATH)/$(d)/F30xxx

# Local rules and targets
sSRCS_$(d) := F30xxx/isrs.S
sSRCS_$(d) += F30xxx/vector_table.S

cSRCS_$(d) := adc.c
cSRCS_$(d) += dma.c
cSRCS_$(d) += exti.c
cSRCS_$(d) += gpio.c
cSRCS_$(d) += i2c.c
cSRCS_$(d) += rcc.c
cSRCS_$(d) += spi.c
cSRCS_$(d) += timer.c
cSRCS_$(d) += usart.c
cSRCS_$(d) += syscfg.c
cSRCS_$(d) += fpu.c
cSRCS_$(d) += comp.c
cSRCS_$(d) += opamp.c
cSRCS_$(d) += bkp.c

sFILES_$(d) := $(sSRCS_$(d):%=$(d)/%)
cFILES_$(d) := $(cSRCS_$(d):%=$(d)/%)

OBJS_$(d) := $(sFILES_$(d):%.S=$(BUILD_PATH)/%.o) \
             $(cFILES_$(d):%.c=$(BUILD_PATH)/%.o)
DEPS_$(d) := $(OBJS_$(d):%.o=%.d)

$(OBJS_$(d)): TGT_ASFLAGS := $(ASFLAGS_$(d))
$(OBJS_$(d)): TGT_CFLAGS := $(CFLAGS_$(d))

TGT_BIN += $(OBJS_$(d))

# Standard things
-include        $(DEPS_$(d))
d               := $(dirstack_$(sp))
sp              := $(basename $(sp))
