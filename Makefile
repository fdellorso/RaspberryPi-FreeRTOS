#
#	Makefile for FreeRTOS demo on Raspberry Pi
#
BASE=$(shell pwd)/
BUILD_DIR=$(shell pwd)/build/

MODULE_NAME="RaspberryPi BSP"

TARGETS=kernel.img kernel.list kernel.syms kernel.elf kernel.map
LINKER_SCRIPT=Linker/raspberrypi.ld

-include .dbuild/dbuild.mk

all: kernel.list kernel.img kernel.syms
	@$(SIZE) kernel.elf

kernel.img: kernel.elf
	$(Q)$(PRETTY) IMAGE $(MODULE_NAME) $@
	$(Q)$(OBJCOPY) kernel.elf -O binary $@

kernel.list: kernel.elf
	$(Q)$(PRETTY) LIST $(MODULE_NAME) $@
	$(Q)$(OBJDUMP) -D -S  kernel.elf > $@

kernel.syms: kernel.elf
	$(Q)$(PRETTY) SYMS $(MODULE_NAME) $@
	$(Q)$(OBJDUMP) -t kernel.elf > $@

# GCC 4.9.3 (Ubuntu Repo)
# kernel.elf: LDFLAGS += -L "/usr/lib/gcc/arm-none-eabi/4.9.3" -lgcc
# kernel.elf: LDFLAGS += -L "/usr/lib/arm-none-eabi/lib" -lc

# GCC 7.3.1 (ARM Toolchain)
kernel.elf: LDFLAGS += -L "$(BASE)../gcc-arm-none-eabi/lib/gcc/arm-none-eabi/7.3.1" -lgcc
kernel.elf: LDFLAGS += -L "$(BASE)../gcc-arm-none-eabi/arm-none-eabi/lib" -lc

# GCC 8.2.0 (Crosstool-NG)
# kernel.elf: LDFLAGS += -L "/home/fra/x-tools/arm-unknown-eabi/lib/gcc/arm-unknown-eabi/8.2.0" -lgcc
# kernel.elf: LDFLAGS += -L "/home/fra/x-tools/arm-unknown-eabi/arm-unknown-eabi/lib" -lc

kernel.elf: $(OBJECTS)
	$(Q)$(LD) $(OBJECTS) -Map kernel.map -o $@ -T $(LINKER_SCRIPT) $(LDFLAGS)
