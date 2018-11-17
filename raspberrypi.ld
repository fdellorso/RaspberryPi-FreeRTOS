/**
 * BlueThunder Linker Script for the raspberry Pi!
 *
 *
 *
 **/
MEMORY
{
	RESERVED	(r)		: ORIGIN = 0x00000000,									LENGTH = 32K
	INIT_RAM	(rwx)	: ORIGIN = ORIGIN(RESERVED)		+ LENGTH(RESERVED),		LENGTH = 32K
	TEXT_RAM	(rwx)	: ORIGIN = ORIGIN(INIT_RAM)		+ LENGTH(INIT_RAM),		LENGTH = 152K
	RODATA_RAM	(rwx)	: ORIGIN = ORIGIN(TEXT_RAM)		+ LENGTH(TEXT_RAM),		LENGTH = 32K
	DATA_RAM	(rwx)	: ORIGIN = ORIGIN(RODATA_RAM)	+ LENGTH(RODATA_RAM),	LENGTH = 2K
	BSS_RAM		(rwx)	: ORIGIN = ORIGIN(DATA_RAM)		+ LENGTH(DATA_RAM),		LENGTH = 2K
	HEAP_RAM	(rwx)	: ORIGIN = ORIGIN(BSS_RAM)		+ LENGTH(BSS_RAM),		LENGTH = 128K
	STACK_RAM	(rwx)	: ORIGIN = ORIGIN(HEAP_RAM)		+ LENGTH(HEAP_RAM),		LENGTH = 128K
	NULL_PTR	(rwx)	: ORIGIN = ORIGIN(STACK_RAM)	+ LENGTH(STACK_RAM),	LENGTH = 2K
	BOOT_LDR	(r)		: ORIGIN = ORIGIN(NULL_PTR)		+ LENGTH(NULL_PTR),		LENGTH = 2K
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
		*(.text)
		*(.text.*)
	} > TEXT_RAM

	.rodata : {
		*(.rodata)
		*(.rodata.*)
		*(.glue_7)
		*(.glue_7t)
	} > RODATA_RAM

	/*
	* Next we put the data.
	*/
	.data : {
		*(.data)
		*(.data.*)
	} > DATA_RAM

	.bss :
	{
		__bss_start = .;
		*(EXCLUDE_FILE(*heap_4.o) .bss)
		*(EXCLUDE_FILE(*heap_4.o).bss.*)
		*(COMMON)
		__bss_end = .;
	} > BSS_RAM

	/**
	 *	Place HEAP here???
	 **/
	.heap :
	{
		*heap_4.o(.bss)
	} > HEAP_RAM

	/**
	 *	Stack starts at the top of the RAM, and moves down!
	 **/
	_estack = ORIGIN(STACK_RAM) + LENGTH(STACK_RAM);

	/**
	 *	NULL Pointer Deferencing
	 **/
	__NULL_PTR_START = ORIGIN(NULL_PTR);

	/**
	 *	Bootloader Reserved Space
	 **/
	__BOOT_LDR_START = ORIGIN(BOOT_LDR);
}
