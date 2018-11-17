# RASPPI	?=	1
# # LOGGING	?= 1
# OUTPUT	=	-DMUART
# LIBUSE	=	-DPRVLIB

-include dbuild.option.mk

# GCC 4.9.3 (Ubuntu Repo)
# TOOLCHAIN=arm-none-eabi-

# GCC 7.3.1 (ARM Toolchain)
TOOLCHAIN=/home/fra/gcc-arm-none-eabi/bin/arm-none-eabi-

# GCC 8.2.0 (Crosstool-NG)
# TOOLCHAIN=/home/fra/x-tools/arm-unknown-eabi/bin/arm-unknown-eabi-

# Header Include
CFLAGS += -I $(BASE)Drivers/
ifdef $(LIBUSE)
CFLAGS += -I $(BASE)Drivers/prvlib
endif
CFLAGS += -I $(BASE)Drivers/uspi/include/
CFLAGS += -I $(BASE)Drivers/tic/include/
# CFLAGS += -I $(BASE)Drivers/tic/lib/libyaml
CFLAGS += -I $(BASE)FreeRTOS/Source/include/
CFLAGS += -I $(BASE)FreeRTOS/Source/portable/GCC/RaspberryPi/
# CFLAGS += -I $(BASE)Drivers/FreeRTOS-Plus-TCP/include/

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
CFLAGS +=	-DRASPPI=$(RASPPI) \
			-DconfigBLUETHUNDER=$(configBLUETHUNDER) \
			$(LIBUSE) \
			$(OUTPUT)  
			# -DLOGGING=$(LOGGING)

# AFLAGS ?= $(ARCH) -DRASPPI=$(RASPPI)
