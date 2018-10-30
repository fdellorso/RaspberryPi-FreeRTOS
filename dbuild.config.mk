RASPPI	?=	1
# LOGGING	?= 1
OUTPUT	=	-DMUART
LIBUSE	=	-DPRVLIB

ifeq ($(strip $(RASPPI)),1)
ARCH	?= -march=armv6j -mtune=arm1176jzf-s -mfloat-abi=hard 
else
ARCH	?= -march=armv7-a -mtune=cortex-a7 -mfloat-abi=hard
endif

AFLAGS ?= $(ARCH) -DRASPPI=$(RASPPI)
CFLAGS += $(ARCH) -g -std=gnu99 -Wno-psabi -fsigned-char -nostdlib -Wno-implicit -mfloat-abi=softfp 
# CFLAGS += -finstrument-functions
CFLAGS += -mno-unaligned-access
CFLAGS += -DRASPPI=$(RASPPI) $(OUTPUT) $(LIBUSE) # -DLOGGING=$(LOGGING)
# CFLAGS += -Wall -Wextra
CFLAGS += -I $(BASE)Drivers/
ifdef $(LIBUSE)
CFLAGS += -I $(BASE)Drivers/prvlib
endif
CFLAGS += -I $(BASE)Drivers/uspi/include/
CFLAGS += -I $(BASE)Drivers/pololu-tic/include/
# CFLAGS += -I $(BASE)Drivers/pololu-tic/lib/libyaml
CFLAGS += -I $(BASE)FreeRTOS/Source/include/
CFLAGS += -I $(BASE)FreeRTOS/Source/portable/GCC/RaspberryPi/
# CFLAGS += -I $(BASE)Drivers/FreeRTOS-Plus-TCP/include/

TOOLCHAIN=arm-none-eabi-
