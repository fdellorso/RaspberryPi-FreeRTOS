# GCC 4.9.3 (Ubuntu Repo)
# TOOLCHAIN = arm-none-eabi-

# GCC 7.3.1 (ARM Toolchain)
TOOLCHAIN = $(BASE)../gcc-arm-none-eabi-7-3-1/bin/arm-none-eabi-

# GCC 8.2.0 (Crosstool-NG)
# TOOLCHAIN = $(BASE)../gcc-arm-none-eabi-8-2-0/bin/arm-none-eabi-

# Header Include
CFLAGS += -I $(BASE)
CFLAGS += -I $(BASE)Main/
CFLAGS += -I $(BASE)Task/
CFLAGS += -I $(BASE)Drivers/
CFLAGS += -I $(BASE)Uspi/include/
CFLAGS += -I $(BASE)Tic/include/
CFLAGS += -I $(BASE)Rsta_Bt/include/
CFLAGS += -I $(BASE)FreeRTOS/Source/include/
CFLAGS += -I $(BASE)FreeRTOS/Source/portable/GCC/RaspberryPi/

# C Language Options
CFLAGS += -std=gnu99 -fsigned-char

# Indie Language Options
# CFLAGS += -fdiagnostics-show-option

#Warning Flags
# CFLAGS += -Werror
CFLAGS +=	-Wall -Wextra
CFLAGS +=	-Wpedantic
CFLAGS += 	-Wno-psabi
CFLAGS += 	-Wno-implicit
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
# CFLAGS += -Og
# CFLAGS += -fdelete-null-pointer-checks
# CFLAGS += -fno-zero-initialized-in-bss
# CFLAGS += -fno-keep-static-consts
# CFLAGS += -fno-omit-frame-pointer -ffloat-store -fno-common -fstrict-aliasing

# Linker Options
CFLAGS += -nostdlib

# ARM Options
ARCH ?= -march=armv6zk -mtune=arm1176jzf-s -mcpu=arm1176jzf-s
ARCH += -mfloat-abi=hard -mfpu=vfp
ARCH += -mno-unaligned-access
CFLAGS += $(ARCH)

# Trace Options
# CFLAGS += -finstrument-functions

# Developer Options
CFLAGS += -fstack-usage -Wstack-usage=128

# Define Flags
CFLAGS +=	-DRASPPI=$(RASPPI)							\
			$(LOGGER)									\
			-DVIDEO_WIDTH=$(VIDEO_WIDTH)				\
			-DVIDEO_HEIGHT=$(VIDEO_HEIGHT)
