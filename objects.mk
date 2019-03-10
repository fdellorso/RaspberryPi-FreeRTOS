#
#	Main Program
#
OBJECTS += $(BUILD_DIR)Main/main.o

#
#	StuFA Tasks Program
#
OBJECTS += $(BUILD_DIR)Task/bluetooth_Task.o
OBJECTS += $(BUILD_DIR)Task/drv8825_Task.o
OBJECTS += $(BUILD_DIR)Task/shared_Task.o
OBJECTS += $(BUILD_DIR)Task/tic_Task.o
OBJECTS += $(BUILD_DIR)Task/ticfunc_Task.o
OBJECTS += $(BUILD_DIR)Task/uspi_Task.o
OBJECTS += $(BUILD_DIR)Task/watchdog_Task.o

#
#	Startup and platform initialisation code.
#
OBJECTS += $(BUILD_DIR)Startup/startup.o

#
#	Private Library
#
OBJECTS += $(BUILD_DIR)prvlib/stdio.o
OBJECTS += $(BUILD_DIR)prvlib/stdlib.o

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

#
#	LCD ili9340 Clock Application
#
# OBJECTS += $(BUILD_DIR)Drivers/clock.o

#
#	Trace Debugger
#
# OBJECTS += $(BUILD_DIR)trace/trace.o

#
#	USPi Library (LAN and USB)
#
OBJECTS += $(BUILD_DIR)Uspi/lib/string.o
OBJECTS += $(BUILD_DIR)Uspi/lib/synchronize.o
OBJECTS += $(BUILD_DIR)Uspi/lib/util.o
OBJECTS += $(BUILD_DIR)Uspi/lib/devicenameservice.o
OBJECTS += $(BUILD_DIR)Uspi/uspibind.o
OBJECTS += $(BUILD_DIR)Uspi/lib/uspilibrary.o
OBJECTS += $(BUILD_DIR)Uspi/lib/dwhcidevice.o
OBJECTS += $(BUILD_DIR)Uspi/lib/dwhciframeschednper.o
OBJECTS += $(BUILD_DIR)Uspi/lib/dwhciframeschednsplit.o
OBJECTS += $(BUILD_DIR)Uspi/lib/dwhciframeschedper.o
OBJECTS += $(BUILD_DIR)Uspi/lib/dwhciregister.o
OBJECTS += $(BUILD_DIR)Uspi/lib/dwhcirootport.o
OBJECTS += $(BUILD_DIR)Uspi/lib/dwhcixferstagedata.o
OBJECTS += $(BUILD_DIR)Uspi/lib/usbcommon.o
OBJECTS += $(BUILD_DIR)Uspi/lib/usbconfigparser.o
OBJECTS += $(BUILD_DIR)Uspi/lib/usbdevice.o
OBJECTS += $(BUILD_DIR)Uspi/lib/usbdevicefactory.o
OBJECTS += $(BUILD_DIR)Uspi/lib/usbendpoint.o
OBJECTS += $(BUILD_DIR)Uspi/lib/usbrequest.o
OBJECTS += $(BUILD_DIR)Uspi/lib/usbstandardhub.o
OBJECTS += $(BUILD_DIR)Uspi/lib/usbstring.o
OBJECTS += $(BUILD_DIR)Uspi/lib/macaddress.o
OBJECTS += $(BUILD_DIR)Uspi/lib/smsc951x.o
OBJECTS += $(BUILD_DIR)Uspi/lib/usbkeyboard.o
OBJECTS += $(BUILD_DIR)Uspi/lib/keymap.o
OBJECTS += $(BUILD_DIR)Uspi/lib/usbmassdevice.o
OBJECTS += $(BUILD_DIR)Uspi/lib/usbmouse.o
OBJECTS += $(BUILD_DIR)Uspi/lib/usbgamepad.o

# #
# #	StuFA Peripherals Drivers
# #
OBJECTS += $(BUILD_DIR)Uspi/lib/usbtict834.o
OBJECTS += $(BUILD_DIR)Uspi/lib/usbbluetooth.o

#
#	Pololu-Tic Library
#
OBJECTS += $(BUILD_DIR)Tic/lib/strdup.o
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

#
#	USPi Library (LAN and USB)
#
OBJECTS += $(BUILD_DIR)Rsta_Bt/lib/ptrarray.o
OBJECTS += $(BUILD_DIR)Rsta_Bt/lib/btdevicemanager.o
OBJECTS += $(BUILD_DIR)Rsta_Bt/lib/bthcilayer.o
OBJECTS += $(BUILD_DIR)Rsta_Bt/lib/btinquiryresults.o
OBJECTS += $(BUILD_DIR)Rsta_Bt/lib/btlogicallayer.o
OBJECTS += $(BUILD_DIR)Rsta_Bt/lib/btqueue.o
OBJECTS += $(BUILD_DIR)Rsta_Bt/lib/btsubsystem.o
# OBJECTS += $(BUILD_DIR)Rsta_Bt/lib/bttask.o
OBJECTS += $(BUILD_DIR)Rsta_Bt/lib/btuarttransport.o

#
#	FreeRTOS portable layer for RaspberryPi
#
OBJECTS += $(BUILD_DIR)FreeRTOS/Source/portable/GCC/RaspberryPi/port.o
OBJECTS += $(BUILD_DIR)FreeRTOS/Source/portable/GCC/RaspberryPi/portisr.o

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
OBJECTS += $(BUILD_DIR)FreeRTOS/Source/timers.o
