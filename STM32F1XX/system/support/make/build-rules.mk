# Useful tools
CROSS_COMPILE ?= arm-none-eabi-

CC       := $(CROSS_COMPILE)gcc
CXX      := $(CROSS_COMPILE)g++
LD       := $(CROSS_COMPILE)ld -v
AR       := $(CROSS_COMPILE)ar
AS       := $(CROSS_COMPILE)gcc
OBJCOPY  := $(CROSS_COMPILE)objcopy
DISAS    := $(CROSS_COMPILE)objdump
OBJDUMP  := $(CROSS_COMPILE)objdump
SIZE     := $(CROSS_COMPILE)size
DFU      ?= dfu-util

# Suppress annoying output unless V is set
ifndef V
   SILENT_CC       = @echo '  [CC]       ' $(@:$(BUILD_PATH)/%.o=%.c);
   SILENT_AS       = @echo '  [AS]       ' $(@:$(BUILD_PATH)/%.o=%.S);
   SILENT_CXX      = @echo '  [CXX]      ' $(@:$(BUILD_PATH)/%.o=%.cpp);
   SILENT_LD       = @echo '  [LD]       ' $(@F);
   SILENT_AR       = @echo '  [AR]       '
   SILENT_OBJCOPY  = @echo '  [OBJCOPY]  ' $(@F);
   SILENT_DISAS    = @echo '  [DISAS]    ' $(@:$(BUILD_PATH)/%.bin=%).disas;
   SILENT_OBJDUMP  = @echo '  [OBJDUMP]  ' $(OBJDUMP);
endif

# Extra build configuration

BUILDDIRS :=
TGT_BIN   :=

CFLAGS   = $(GLOBAL_CFLAGS) $(TGT_CFLAGS)
CXXFLAGS = $(GLOBAL_CXXFLAGS) $(TGT_CXXFLAGS)
ASFLAGS  = $(GLOBAL_ASFLAGS) $(TGT_ASFLAGS)

# Hacks to determine extra libraries we need to link against based on
# the toolchain. The default specifies no extra libraries, but it can
# be overridden.
LD_TOOLCHAIN_PATH := $(LDDIR)/toolchains/generic
ifneq ($(findstring ARM/embedded,$(shell $(CC) --version)),)
# GCC ARM Embedded, https://launchpad.net/gcc-arm-embedded/
LD_TOOLCHAIN_PATH := $(LDDIR)/toolchains/gcc-arm-embedded
endif
ifneq ($(findstring Linaro GCC,$(shell $(CC) --version)),)
# Summon/Linaro GCC ARM Embedded, https://github.com/esden/summon-arm-toolchain
LD_TOOLCHAIN_PATH := $(LDDIR)/toolchains/gcc-arm-embedded
endif
# Add toolchain directory to LD search path
TOOLCHAIN_LDFLAGS := -L $(LD_TOOLCHAIN_PATH)

# General directory independent build rules, generate dependency information
$(BUILD_PATH)/%.o: %.c
	$(SILENT_CC) $(CC) $(CFLAGS) -MMD -MP -MF $(@:%.o=%.d) -MT $@ -o $@ -c $<

$(BUILD_PATH)/%.o: %.cpp
	$(SILENT_CXX) $(CXX) $(CFLAGS) $(CXXFLAGS) -MMD -MP -MF $(@:%.o=%.d) -MT $@ -o $@ -c $<

$(BUILD_PATH)/%.o: %.S
	$(SILENT_AS) $(AS) $(ASFLAGS) -MMD -MP -MF $(@:%.o=%.d) -MT $@ -o $@ -c $<
