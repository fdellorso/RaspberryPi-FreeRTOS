-include dbuild.option.mk

# GCC 4.9.3 (Ubuntu Repo)
# TOOLCHAIN=arm-none-eabi-

# GCC 7.3.1 (ARM Toolchain)
TOOLCHAIN=$(BASE)../gcc-arm-none-eabi/bin/arm-none-eabi-

# GCC 8.2.0 (Crosstool-NG)
# TOOLCHAIN=/home/fra/x-tools/arm-unknown-eabi/bin/arm-unknown-eabi-

# Header Include
CFLAGS += -I $(BASE)Main/
CFLAGS += -I $(BASE)Drivers/
ifdef $(LIBUSE)
CFLAGS += -I $(BASE)Drivers/prvlib
endif
CFLAGS += -I $(BASE)Uspi/include/
# CFLAGS += -I $(BASE)Uspi2/include/
CFLAGS += -I $(BASE)Tic/include/
CFLAGS += -I $(BASE)Rsta_Bt/include/
# CFLAGS += -I $(BASE)Tic/lib/libyaml
CFLAGS += -I $(BASE)FreeRTOS/Source/include/
CFLAGS += -I $(BASE)FreeRTOS/Source/portable/GCC/RaspberryPi/
# CFLAGS += -I $(BASE)FreeRTOS-Plus-TCP/include/

# C Language Options
CFLAGS += -std=gnu99 -fsigned-char

# Indie Language Options
# CFLAGS += -fdiagnostics-show-option

#Warning Flags
# CFLAGS += -Werror
CFLAGS +=	-Wall -Wextra
CFLAGS +=	-Wpedantic
# CFLAGS += -Wno-psabi
# CFLAGS += -Wno-implicit
# CFLAGS +=	-Wnull-dereference # Newer GCC
# CFLAGS +=	-Wcast-qual
# CFLAGS +=	-Wconversion
# CFLAGS +=	-Wredundant-decls	
# CFLAGS +=	-Wno-missing-braces				\
# 			-Wno-missing-field-initializers	\
# 			-Wno-parentheses				\
#			-Wno-unused						\
# 			-Wno-variadic-macros
CFLAGS +=	-Wbad-function-cast		\
			-Wcast-align			\
			-Wdisabled-optimization	\
			-Wfloat-equal			\
			-Wformat=2				\
			-Winit-self				\
			-Winline				\
			-Wlogical-op			\
			-Wmissing-include-dirs	\
			-Wmissing-prototypes	\
			-Wnested-externs		\
			-Wold-style-definition	\
			-Wpointer-arith			\
			-Wshadow				\
			-Wstrict-aliasing=1		\
			-Wstrict-overflow=5		\
			-Wstrict-prototypes		\
			-Wswitch-default		\
			-Wswitch-enum			\
			-Wundef					\
			-Wunreachable-code		\
			-Wwrite-strings


# Debug Options
CFLAGS += -g

# Optimization Options
CFLAGS += -Og # -fdelete-null-pointer-checks
# CFLAGS += -fno-omit-frame-pointer -ffloat-store -fno-common -fstrict-aliasing

# Linker Options
CFLAGS += -nostdlib

# ARM Options
ifeq ($(strip $(RASPPI)),1)
ARCH ?= -march=armv6zk -mtune=arm1176jzf-s #-mfloat-abi=hard 
else
ARCH ?= -march=armv7-a -mtune=cortex-a7 #-mfloat-abi=hard
endif
ARCH += -mfloat-abi=softfp -mno-unaligned-access
CFLAGS += $(ARCH)

# Code Generation Options
# CFLAGS += -finstrument-functions

# Define Flags
CFLAGS +=	-DRASPPI=$(RASPPI)							\
			-DconfigBLUETHUNDER=$(configBLUETHUNDER)	\
			$(LIBUSE)									\
			$(LOGGER)									\
			$(USBDEV)									\
			-DVIDEO_WIDTH=$(VIDEO_WIDTH)				\
			-DVIDEO_HEIGHT=$(VIDEO_HEIGHT)

# AFLAGS ?= $(ARCH) -DRASPPI=$(RASPPI)
