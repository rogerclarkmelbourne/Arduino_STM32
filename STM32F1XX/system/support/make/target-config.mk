# TARGET_FLAGS are to be passed while compiling, assembling, linking.
TARGET_FLAGS :=
# TARGET_LDFLAGS go to the linker
TARGET_LDFLAGS :=

# Configuration derived from $(MEMORY_TARGET)

LD_SCRIPT_PATH := $(LDDIR)/$(MEMORY_TARGET).ld

ifeq ($(MEMORY_TARGET), ram)
VECT_BASE_ADDR := VECT_TAB_RAM
endif
ifeq ($(MEMORY_TARGET), flash)
VECT_BASE_ADDR := VECT_TAB_FLASH
endif
ifeq ($(MEMORY_TARGET), jtag)
VECT_BASE_ADDR := VECT_TAB_BASE
endif

# Pull in the board configuration file here, so it can override the
# above.

include $(BOARD_INCLUDE_DIR)/$(BOARD).mk

# Configuration derived from $(BOARD).mk

LD_SERIES_PATH := $(LDDIR)/stm32/series/$(MCU_SERIES)
LD_MEM_PATH := $(LDDIR)/stm32/mem/$(LD_MEM_DIR)
ifeq ($(MCU_SERIES), stm32f1)
# Due to the Balkanization on F1, we need to specify the line when
# making linker decisions.
LD_SERIES_PATH := $(LD_SERIES_PATH)/$(MCU_F1_LINE)
endif

ifeq ($(MCU_SERIES), stm32f1)
TARGET_FLAGS += -mcpu=cortex-m3 -march=armv7-m
endif
ifeq ($(MCU_SERIES), stm32f2)
TARGET_FLAGS += -mcpu=cortex-m3 -march=armv7-m
endif
ifeq ($(MCU_SERIES), stm32f4)
TARGET_FLAGS += -mcpu=cortex-m4 -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16
endif

TARGET_LDFLAGS += -Xlinker -T$(LD_SCRIPT_PATH) \
                  -L $(LD_SERIES_PATH) \
                  -L $(LD_MEM_PATH) \
                  -L $(LDDIR)
TARGET_FLAGS += -mthumb -DBOARD_$(BOARD) -DMCU_$(MCU) \
                -DERROR_LED_PORT=$(ERROR_LED_PORT) \
                -DERROR_LED_PIN=$(ERROR_LED_PIN) \
                -D$(VECT_BASE_ADDR)

LIBMAPLE_MODULE_SERIES := $(LIBMAPLE_PATH)/$(MCU_SERIES)
