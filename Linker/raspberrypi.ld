/**
 * BlueThunder Linker Script for the raspberry Pi!
 *
 *
 *
 **/
MEMORY
{
	RESERVED	(r)		: ORIGIN = 0x00000000,									LENGTH = 32K
	INIT_RAM	(rwx)	: ORIGIN = ORIGIN(RESERVED)		+ LENGTH(RESERVED),		LENGTH = 8K
	TEXT_RAM	(rwx)	: ORIGIN = ORIGIN(INIT_RAM)		+ LENGTH(INIT_RAM),		LENGTH = 256K
	RODATA_RAM	(rwx)	: ORIGIN = ORIGIN(TEXT_RAM)		+ LENGTH(TEXT_RAM),		LENGTH = 64K
	DATA_RAM	(rwx)	: ORIGIN = ORIGIN(RODATA_RAM)	+ LENGTH(RODATA_RAM),	LENGTH = 8K
	BSS_RAM		(rwx)	: ORIGIN = ORIGIN(DATA_RAM)		+ LENGTH(DATA_RAM),		LENGTH = 8K
	HEAP_RAM	(rwx)	: ORIGIN = ORIGIN(BSS_RAM)		+ LENGTH(BSS_RAM),		LENGTH = 128K
	/* STACK_RAM	(rwx)	: ORIGIN = ORIGIN(HEAP_RAM)		+ LENGTH(HEAP_RAM),		LENGTH = 96K
	NULL_PTR	(rwx)	: ORIGIN = ORIGIN(STACK_RAM)	+ LENGTH(STACK_RAM),	LENGTH = 8K */
	NULL_PTR	(rwx)	: ORIGIN = ORIGIN(HEAP_RAM)		+ LENGTH(HEAP_RAM),		LENGTH = 8K
	BOOT_LDR	(r)		: ORIGIN = ORIGIN(NULL_PTR)		+ LENGTH(NULL_PTR),		LENGTH = 0
}

ENTRY(_start)

SECTIONS {
    /*
	 * Our init section allows us to place the bootstrap code at address 0x8000
	 *
	 * This is where the Graphics processor forces the ARM to start execution.
	 * However the interrupt vector code remains at 0x0000, and so we must copy the correct
	 * branch instructions to 0x0000 - 0x001C in order to get the processor to handle interrupts.
	 *
	 */
	.init : {
		KEEP(*(.init))
	} > INIT_RAM = 0

	.module_entries : {
		__module_entries_start = .;
	  	KEEP(*(.module_entries))
		KEEP(*(.module_entries.*))
		__module_entries_end = .;
		__module_entries_size = SIZEOF(.module_entries);
	} > INIT_RAM


	/**
 	 *	This is the main code section, it is essentially of unlimited size. (128Mb).
	 *
	 **/
	.text : {
		. = ORIGIN(TEXT_RAM);
		*(.text)
		*(.text.*)
	} > TEXT_RAM

	.rodata : {
		. = ORIGIN(RODATA_RAM);
		*(.rodata)
		*(.rodata.*)
		*(.glue_7)
		*(.glue_7t)
	} > RODATA_RAM

	/*
	* Next we put the data.
	*/
	.data : {
		. = ORIGIN(DATA_RAM);
		*(.data)
		*(.data.*)
	} > DATA_RAM

	.bss : {
		. = ORIGIN(BSS_RAM);
		__bss_start = .;
		*(EXCLUDE_FILE(*heap_4.o).bss)
		*(EXCLUDE_FILE(*heap_4.o).bss.*)
		*(COMMON)
		__bss_end = .;
	} > BSS_RAM

	/**
	 *	Place HEAP here???
	 **/
	.heap : {
		. = ORIGIN(HEAP_RAM);
		__HeapBase = .;
		*heap_4.o(.bss)
		HEAP_SIZE = LENGTH(HEAP_RAM);
		__HeapLimit = .;
	} > HEAP_RAM

	/**
	 *	Stack starts at the top of the RAM, and moves down!
	 **/
	/* .stack : {
		. = ORIGIN(STACK_RAM);
		_estack = .;
		. = . + LENGTH(STACK_RAM);
		_sstack = .;
	} > STACK_RAM */

	/**
	 *	NULL Pointer Deferencing
	 **/
	.null : {
		. = ORIGIN(NULL_PTR);
		__NULL_PTR_START = .;
		. = . + LENGTH(NULL_PTR);
		__NULL_PTR_END = .;
	} > NULL_PTR

	/**
	 *	Bootloader Reserved Space
	 **/
	.boot : {
		. = ORIGIN(BOOT_LDR);
		__BOOT_LDR_START = .;
		. = . + LENGTH(BOOT_LDR);
	} > BOOT_LDR
}
