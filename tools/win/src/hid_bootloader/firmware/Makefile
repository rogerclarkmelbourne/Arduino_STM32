TARGET = HIDBOOTLOADER

DEVICE = STM32F10X_MD

VECTOR_TABLE_OFFSET = 0x0000

SRCS = main.c usb.c hid.c led.c

DEVICE_SRCS += CMSIS/Device/ST/STM32F10x/Source/Templates/gcc/startup_stm32f10x_md.s
DEVICE_SRCS += system_stm32f10x.c

INCLUDE_DIRS += -I .
INCLUDE_DIRS += -I CMSIS/Device/ST/STM32F10x/Include 
INCLUDE_DIRS += -I CMSIS/Include

LINKER_SCRIPT = STM32F103C8T6.ld

SRCS += $(DEVICE_SRCS)
OBJS = $(addsuffix .o,  $(basename $(SRCS)))

ASFLAGS += -mcpu=cortex-m3 -mthumb

CFLAGS += -mcpu=cortex-m3 -mthumb -Wall -Os
CFLAGS += -fno-common -static
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -specs=nano.specs -specs=nosys.specs
#CFLAGS += -flto
CFLAGS += $(INCLUDE_DIRS)
CFLAGS += -D$(DEVICE)
CFLAGS += -DVECTOR_TABLE_OFFSET=$(VECTOR_TABLE_OFFSET)

LDFLAGS += -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--print-memory-usage
#LDFLAGS += -flto
LDFLAGS += -T$(LINKER_SCRIPT)

CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
OBJDUMP = arm-none-eabi-objdump
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size
GDB = arm-none-eabi-gdb

ECHO = echo
RM = rm

.PHONY: all build output info size clean flash

all: $(SRCS) gccversion build output info size

build: $(TARGET).elf

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(OBJS) -o "$@"

.SECONDARY: $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c "$<" -o "$@"

%.o: %.s
	$(AS) $(ASFLAGS) -c "$<" -o "$@"

%.s : %.c
	$(CC) -S $(CFLAGS) $< -o $@

output: $(TARGET).hex $(TARGET).bin

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@
	
gccversion:
	@$(CC) --version

info: $(TARGET).elf
	$(OBJDUMP) -x -S $(TARGET).elf > $(TARGET).lst
	$(OBJDUMP) -D $(TARGET).elf > $(TARGET).dis
	$(SIZE) $(TARGET).elf > $(TARGET).size

size: $(TARGET).elf
	$(SIZE) $(TARGET).elf

clean:
	$(RM) -f $(TARGET).elf
	$(RM) -f $(TARGET).bin
	$(RM) -f $(TARGET).hex
	$(RM) -f $(TARGET).size
	$(RM) -f $(TARGET).lst
	$(RM) -f $(TARGET).dis
	$(RM) -f $(TARGET).map
	$(RM) -f $(OBJS)
