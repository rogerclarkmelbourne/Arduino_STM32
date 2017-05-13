# Standard things
sp              := $(sp).x
dirstack_$(sp)  := $(d)
d               := $(dir)
BUILDDIRS       += $(BUILD_PATH)/$(d)
BUILDDIRS       += $(BUILD_PATH)/$(d)/usbF4/STM32_USB_Device_Library/Core/src
BUILDDIRS       += $(BUILD_PATH)/$(d)/usbF4/STM32_USB_Device_Library/Class/cdc/src
BUILDDIRS       += $(BUILD_PATH)/$(d)/usbF4/STM32_USB_OTG_Driver/src
BUILDDIRS       += $(BUILD_PATH)/$(d)/usbF4/VCP
LIBMAPLE_INCLUDES := -I$(LIBMAPLE_PATH) -I$(LIBMAPLE_PATH)/usbF4


# Local flags
CFLAGS_$(d) = -I$(d) $(LIBMAPLE_INCLUDES) -Wall 
#-Werror

# Local rules and targets
#              bkp.c                    

cSRCS_$(d) := adc.c                    \
              dac.c                    \
              dma.c                    \
              exti.c                   \
              flash.c                  \
              fsmc.c                   \
              gpio.c                   \
              iwdg.c                   \
              nvic.c                   \
              pwr.c		       \
              i2c.c                    \
              rcc.c                    \
              spi.c                    \
              syscalls.c               \
              systick.c                \
              timer.c                  \
              usart.c                  \
              util.c                   

	V=1
	cSRCS_$(d) += \
		usbF4/STM32_USB_Device_Library/Core/src/usbd_core.c \
		usbF4/STM32_USB_Device_Library/Core/src/usbd_ioreq.c \
		usbF4/STM32_USB_Device_Library/Core/src/usbd_req.c \
		usbF4/STM32_USB_Device_Library/Class/cdc/src/usbd_cdc_core.c \
		usbF4/STM32_USB_OTG_Driver/src/usb_dcd.c \
		usbF4/STM32_USB_OTG_Driver/src/usb_core.c \
		usbF4/STM32_USB_OTG_Driver/src/usb_dcd_int.c \
		usbF4/VCP/usb_bsp.c \
		usbF4/VCP/usbd_cdc_vcp.c \
		usbF4/VCP/usbd_desc.c \
		usbF4/VCP/usbd_usr.c \
		usbF4/usb.c \
		usbF4/VCP/misc.c 


sSRCS_$(d) := exc.S

cFILES_$(d) := $(cSRCS_$(d):%=$(d)/%)
sFILES_$(d) := $(sSRCS_$(d):%=$(d)/%)

OBJS_$(d) := $(cFILES_$(d):%.c=$(BUILD_PATH)/%.o) $(sFILES_$(d):%.S=$(BUILD_PATH)/%.o)
DEPS_$(d) := $(OBJS_$(d):%.o=%.d)

$(OBJS_$(d)): TGT_CFLAGS := $(CFLAGS_$(d))
$(OBJS_$(d)): TGT_ASFLAGS :=

TGT_BIN += $(OBJS_$(d))

# Standard things
-include        $(DEPS_$(d))
d               := $(dirstack_$(sp))
sp              := $(basename $(sp))
