-include dbuild.option.mk

#
#	Main Program
#
OBJECTS += $(BUILD_DIR)Main/main.o
OBJECTS += $(BUILD_DIR)Main/stufa_task.o

#
#	Startup and platform initialisation code.
#
OBJECTS += $(BUILD_DIR)Startup/startup.o

#
#	Private Library
#
ifeq ($(strip $(LIBUSE)),-DPRVLIB)
OBJECTS += $(BUILD_DIR)Drivers/prvlib/stdio.o
OBJECTS += $(BUILD_DIR)Drivers/prvlib/stdlib.o
# OBJECTS += $(BUILD_DIR)Drivers/prvlib/string.o
else
OBJECTS += $(BUILD_DIR)Drivers/prvlib/syscalls.o
endif

#
#	BCM2835 Hardware Drivers
#
OBJECTS += $(BUILD_DIR)Drivers/bcm2835_base.o
OBJECTS += $(BUILD_DIR)Drivers/arm_timer.o
OBJECTS += $(BUILD_DIR)Drivers/gpio.o
OBJECTS += $(BUILD_DIR)Drivers/interrupts.o
OBJECTS += $(BUILD_DIR)Drivers/mailbox.o
OBJECTS += $(BUILD_DIR)Drivers/spi.o
OBJECTS += $(BUILD_DIR)Drivers/sys_timer.o

# McCauley Library
# OBJECTS += $(BUILD_DIR)Drivers/bcm2835.o

#
#	Logger Library
#
ifeq ($(strip $(LOGGER)),-DVIDEO)
OBJECTS += $(BUILD_DIR)Drivers/video.o
endif
ifeq ($(strip $(LOGGER)),-DILI9340)
OBJECTS += $(BUILD_DIR)Drivers/ili9340.o
endif
ifeq ($(strip $(LOGGER)),-DMUART)
OBJECTS += $(BUILD_DIR)Drivers/muart.o
endif

# LCD ili9340 Clock Library
# OBJECTS += $(BUILD_DIR)Drivers/clock.o

#
#	Trace Debugger
#
# OBJECTS += $(BUILD_DIR)trace/trace.o

# smsc9514 (LAN and USB)
OBJECTS += $(BUILD_DIR)Drivers/uspi/uspibind.o
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/uspilibrary.o
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/dwhcidevice.o
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/dwhciregister.o
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/dwhcixferstagedata.o
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/usbconfigparser.o
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/usbdevice.o
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/usbdevicefactory.o
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/usbendpoint.o
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/usbrequest.o
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/usbstandardhub.o
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/devicenameservice.o
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/string.o
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/util.o
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/dwhciframeschednper.o
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/dwhciframeschedper.o
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/dwhcirootport.o
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/dwhciframeschednsplit.o
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/synchronize.o
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/usbstring.o

OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/macaddress.o
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/smsc951x.o

ifeq ($(strip $(USBDEV)),-DUSBKBD)
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/usbkeyboard.o
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/keymap.o
endif

ifeq ($(strip $(USBDEV)),-DUSBMEM)
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/usbmassdevice.o
endif

ifeq ($(strip $(USBDEV)),-DUSBMOU)
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/usbmouse.o
endif

ifeq ($(strip $(USBDEV)),-DUSBPAD)
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/usbgamepad.o
endif

#
#	StuFA Peripherals Drivers
#
ifeq ($(strip $(USBDEV)),-DUSBTIC)
OBJECTS += $(BUILD_DIR)Drivers/uspi/lib/usbtict834.o
endif

#
#	Pololu-Tic Library
#
OBJECTS += $(BUILD_DIR)Drivers/tic/lib/tic_baud_rate.o
OBJECTS += $(BUILD_DIR)Drivers/tic/lib/tic_current_limit.o
OBJECTS += $(BUILD_DIR)Drivers/tic/lib/tic_device.o
OBJECTS += $(BUILD_DIR)Drivers/tic/lib/tic_error.o
OBJECTS += $(BUILD_DIR)Drivers/tic/lib/tic_get_settings.o
OBJECTS += $(BUILD_DIR)Drivers/tic/lib/tic_handle.o
OBJECTS += $(BUILD_DIR)Drivers/tic/lib/tic_names.o
OBJECTS += $(BUILD_DIR)Drivers/tic/lib/tic_set_settings.o
OBJECTS += $(BUILD_DIR)Drivers/tic/lib/tic_settings.o
OBJECTS += $(BUILD_DIR)Drivers/tic/lib/tic_settings_fix.o
OBJECTS += $(BUILD_DIR)Drivers/tic/lib/tic_string.o
OBJECTS += $(BUILD_DIR)Drivers/tic/lib/tic_variables.o

# OBJECTS += $(BUILD_DIR)Drivers/tic/lib/tic_settings_read_from_string.o
# OBJECTS += $(BUILD_DIR)Drivers/tic/lib/tic_settings_to_string.o
# OBJECTS += $(BUILD_DIR)Drivers/tic/lib/libyaml/yaml.o

#
#	FreeRTOS portable layer for RaspberryPi
#
OBJECTS += $(BUILD_DIR)FreeRTOS/Source/portable/GCC/RaspberryPi/port.o
OBJECTS += $(BUILD_DIR)FreeRTOS/Source/portable/GCC/RaspberryPi/portisr.o

$(BUILD_DIR)FreeRTOS/Source/portable/GCC/RaspberryPi/port.o: CFLAGS += -I $(BASE)Demo/

#
#	Selected HEAP implementation for FreeRTOS.
#
OBJECTS += $(BUILD_DIR)FreeRTOS/Source/portable/MemMang/heap_4.o

#
#	FreeRTOS Core
#
OBJECTS += $(BUILD_DIR)FreeRTOS/Source/croutine.o
OBJECTS += $(BUILD_DIR)FreeRTOS/Source/list.o
OBJECTS += $(BUILD_DIR)FreeRTOS/Source/queue.o
OBJECTS += $(BUILD_DIR)FreeRTOS/Source/tasks.o
OBJECTS += $(BUILD_DIR)FreeRTOS/Source/event_groups.o

#freeRTOS-TCP
# OBJECTS += $(BUILD_DIR)Drivers/FreeRTOS-Plus-TCP/FreeRTOS_ARP.o
# OBJECTS += $(BUILD_DIR)Drivers/FreeRTOS-Plus-TCP/FreeRTOS_DHCP.o
# OBJECTS += $(BUILD_DIR)Drivers/FreeRTOS-Plus-TCP/FreeRTOS_DNS.o
# OBJECTS += $(BUILD_DIR)Drivers/FreeRTOS-Plus-TCP/FreeRTOS_IP.o
# OBJECTS += $(BUILD_DIR)Drivers/FreeRTOS-Plus-TCP/FreeRTOS_Sockets.o
# OBJECTS += $(BUILD_DIR)Drivers/FreeRTOS-Plus-TCP/FreeRTOS_Stream_Buffer.o
# OBJECTS += $(BUILD_DIR)Drivers/FreeRTOS-Plus-TCP/FreeRTOS_TCP_IP.o
# OBJECTS += $(BUILD_DIR)Drivers/FreeRTOS-Plus-TCP/FreeRTOS_TCP_WIN.o
# OBJECTS += $(BUILD_DIR)Drivers/FreeRTOS-Plus-TCP/FreeRTOS_UDP_IP.o
# OBJECTS += $(BUILD_DIR)Drivers/FreeRTOS-Plus-TCP/portable/BufferManagement/BufferAllocation_2.o
# OBJECTS += $(BUILD_DIR)Drivers/FreeRTOS-Plus-TCP/portable/NetworkInterface.o
