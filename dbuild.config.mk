RASPPI	?= 1
LOGGING	?= 1

ifeq ($(strip $(RASPPI)),1)
ARCH	?= -march=armv6j -mtune=arm1176jzf-s -mfloat-abi=hard 
else
ARCH	?= -march=armv7-a -mtune=cortex-a7 -mfloat-abi=hard
endif

AFLAGS ?= $(ARCH) -DRASPPI=$(RASPPI)
CFLAGS += $(ARCH) -g -std=gnu99 -Wno-psabi -fsigned-char -nostdlib -Wno-implicit -mfloat-abi=softfp 
# CFLAGS += -finstrument-functions
CFLAGS += -mno-unaligned-access
CFLAGS += -DRASPPI=$(RASPPI) -DLOGGING=$(LOGGING)
# CFLAGS += -Wall -Wextra
CFLAGS += -I $(BASE)Drivers/
CFLAGS += -I $(BASE)Drivers/uspi/include/
CFLAGS += -I $(BASE)FreeRTOS/Source/include/
CFLAGS += -I $(BASE)FreeRTOS/Source/portable/GCC/RaspberryPi/
CFLAGS += -I $(BASE)Drivers/FreeRTOS-Plus-TCP/include/

TOOLCHAIN=arm-none-eabi-
