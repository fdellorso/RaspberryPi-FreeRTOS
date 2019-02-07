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
OBJECTS += $(BUILD_DIR)Drivers/pwm.o
OBJECTS += $(BUILD_DIR)Drivers/spi.o
OBJECTS += $(BUILD_DIR)Drivers/sys_timer.o
OBJECTS += $(BUILD_DIR)Drivers/uart0.o

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

#
#	USPi Library (LAN and USB)
#
OBJECTS += $(BUILD_DIR)Uspi/uspibind.o
OBJECTS += $(BUILD_DIR)Uspi/lib/uspilibrary.o
OBJECTS += $(BUILD_DIR)Uspi/lib/dwhcidevice.o
OBJECTS += $(BUILD_DIR)Uspi/lib/dwhciregister.o
OBJECTS += $(BUILD_DIR)Uspi/lib/dwhcixferstagedata.o
OBJECTS += $(BUILD_DIR)Uspi/lib/usbconfigparser.o
OBJECTS += $(BUILD_DIR)Uspi/lib/usbdevice.o
OBJECTS += $(BUILD_DIR)Uspi/lib/usbdevicefactory.o
OBJECTS += $(BUILD_DIR)Uspi/lib/usbendpoint.o
OBJECTS += $(BUILD_DIR)Uspi/lib/usbrequest.o
OBJECTS += $(BUILD_DIR)Uspi/lib/usbstandardhub.o
OBJECTS += $(BUILD_DIR)Uspi/lib/devicenameservice.o
OBJECTS += $(BUILD_DIR)Uspi/lib/string.o
OBJECTS += $(BUILD_DIR)Uspi/lib/util.o
OBJECTS += $(BUILD_DIR)Uspi/lib/dwhciframeschednper.o
OBJECTS += $(BUILD_DIR)Uspi/lib/dwhciframeschedper.o
OBJECTS += $(BUILD_DIR)Uspi/lib/dwhcirootport.o
OBJECTS += $(BUILD_DIR)Uspi/lib/dwhciframeschednsplit.o
OBJECTS += $(BUILD_DIR)Uspi/lib/synchronize.o
OBJECTS += $(BUILD_DIR)Uspi/lib/usbstring.o
OBJECTS += $(BUILD_DIR)Uspi/lib/usbcommon.o

OBJECTS += $(BUILD_DIR)Uspi/lib/macaddress.o
OBJECTS += $(BUILD_DIR)Uspi/lib/smsc951x.o

ifneq (,$(findstring -DUSBKBD,$(USBDEV)))
OBJECTS += $(BUILD_DIR)Uspi/lib/usbkeyboard.o
OBJECTS += $(BUILD_DIR)Uspi/lib/keymap.o
endif

ifneq (,$(findstring -DUSBMEM,$(USBDEV)))
OBJECTS += $(BUILD_DIR)Uspi/lib/usbmassdevice.o
endif

ifneq (,$(findstring -DUSBMOU,$(USBDEV)))
OBJECTS += $(BUILD_DIR)Uspi/lib/usbmouse.o
endif

ifneq (,$(findstring -DUSBPAD,$(USBDEV)))
OBJECTS += $(BUILD_DIR)Uspi/lib/usbgamepad.o
endif

#
#	StuFA Peripherals Drivers
#
ifneq (,$(findstring -DUSBTIC,$(USBDEV)))
OBJECTS += $(BUILD_DIR)Uspi/lib/usbtict834.o
endif
ifneq (,$(findstring -DUSBBLT,$(USBDEV)))
OBJECTS += $(BUILD_DIR)Uspi/lib/usbbluetooth.o
endif

#
#	Pololu-Tic Library
#
OBJECTS += $(BUILD_DIR)Tic/lib/tic_baud_rate.o
OBJECTS += $(BUILD_DIR)Tic/lib/tic_current_limit.o
OBJECTS += $(BUILD_DIR)Tic/lib/tic_device.o
OBJECTS += $(BUILD_DIR)Tic/lib/tic_error.o
OBJECTS += $(BUILD_DIR)Tic/lib/tic_get_settings.o
OBJECTS += $(BUILD_DIR)Tic/lib/tic_handle.o
OBJECTS += $(BUILD_DIR)Tic/lib/tic_names.o
OBJECTS += $(BUILD_DIR)Tic/lib/tic_set_settings.o
OBJECTS += $(BUILD_DIR)Tic/lib/tic_settings.o
OBJECTS += $(BUILD_DIR)Tic/lib/tic_settings_fix.o
OBJECTS += $(BUILD_DIR)Tic/lib/tic_string.o
OBJECTS += $(BUILD_DIR)Tic/lib/tic_variables.o

# OBJECTS += $(BUILD_DIR)Tic/lib/tic_settings_read_from_string.o
# OBJECTS += $(BUILD_DIR)Tic/lib/tic_settings_to_string.o
# OBJECTS += $(BUILD_DIR)Tic/lib/libyaml/yaml.o

#
#	USPi Library (LAN and USB)
#
OBJECTS += $(BUILD_DIR)Rsta_Bt/lib/ptrarray.o				# Compile OK
OBJECTS += $(BUILD_DIR)Rsta_Bt/lib/btdevicemanager.o
OBJECTS += $(BUILD_DIR)Rsta_Bt/lib/bthcilayer.o
OBJECTS += $(BUILD_DIR)Rsta_Bt/lib/btinquiryresults.o		# Compile OK
OBJECTS += $(BUILD_DIR)Rsta_Bt/lib/btlogicallayer.o
OBJECTS += $(BUILD_DIR)Rsta_Bt/lib/btqueue.o				# Compile OK
OBJECTS += $(BUILD_DIR)Rsta_Bt/lib/btsubsystem.o
OBJECTS += $(BUILD_DIR)Rsta_Bt/lib/bttask.o
OBJECTS += $(BUILD_DIR)Rsta_Bt/lib/btuarttransport.o		# Compile OK

#
#	FreeRTOS portable layer for RaspberryPi
#
OBJECTS += $(BUILD_DIR)FreeRTOS/Source/portable/GCC/RaspberryPi/port.o
OBJECTS += $(BUILD_DIR)FreeRTOS/Source/portable/GCC/RaspberryPi/portisr.o

$(BUILD_DIR)FreeRTOS/Source/portable/GCC/RaspberryPi/port.o: CFLAGS += -I $(BASE)Main/

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
# OBJECTS += $(BUILD_DIR)FreeRTOS-Plus-TCP/FreeRTOS_ARP.o
# OBJECTS += $(BUILD_DIR)FreeRTOS-Plus-TCP/FreeRTOS_DHCP.o
# OBJECTS += $(BUILD_DIR)FreeRTOS-Plus-TCP/FreeRTOS_DNS.o
# OBJECTS += $(BUILD_DIR)FreeRTOS-Plus-TCP/FreeRTOS_IP.o
# OBJECTS += $(BUILD_DIR)FreeRTOS-Plus-TCP/FreeRTOS_Sockets.o
# OBJECTS += $(BUILD_DIR)FreeRTOS-Plus-TCP/FreeRTOS_Stream_Buffer.o
# OBJECTS += $(BUILD_DIR)FreeRTOS-Plus-TCP/FreeRTOS_TCP_IP.o
# OBJECTS += $(BUILD_DIR)FreeRTOS-Plus-TCP/FreeRTOS_TCP_WIN.o
# OBJECTS += $(BUILD_DIR)FreeRTOS-Plus-TCP/FreeRTOS_UDP_IP.o
# OBJECTS += $(BUILD_DIR)FreeRTOS-Plus-TCP/portable/BufferManagement/BufferAllocation_2.o
# OBJECTS += $(BUILD_DIR)FreeRTOS-Plus-TCP/portable/NetworkInterface.o
