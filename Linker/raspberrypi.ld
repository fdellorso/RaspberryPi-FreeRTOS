OUTPUT_ARCH(arm)
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
	.init 0x8000 : {
		KEEP(*(.init))
	}

	.module_entries : {
		__module_entries_start = .;
	  	KEEP(*(.module_entries))
		KEEP(*(.module_entries.*))
		__module_entries_end = .;
		__module_entries_size = SIZEOF(.module_entries);
	}

	/**
 	 *	This is the main code section, it is essentially of unlimited size. (128Mb).
	 *
	 **/
	.text : {
		. = ALIGN(4);
		 __text_start__ = .;     		/* Label in case we want address of text section start */
		*(.text .text.*)
		 __text_end__ = .;      		/* Label in case we want address of text section end */
	}

	/*
	* Next we put the read only data.
	*/
	.rodata : {
		. = ALIGN(4);
        __rodata_start__ = .;			/* Label in case we want address of rodata section start */
        *(.rodata .rodata.*)
        __rodata_end__ = .;				/* Label in case we want address of rodata section start */
	}

	/*
	* Next we put the data.
	*/
	.data : {
		. = ALIGN(4);
		 __data_start__ = .;     		/* Label in case we want address of data section start */
		*(.data .data.*)
		 __data_end__ = .;     			/* Label in case we want address of data section end */
	}

	/*
	* Next we put the data1 .. 16 byte aligned data.
	*/
	.data1 : {
		. = ALIGN(16);
		 __data1_start__ = .;     		/* Label in case we want address of data section start */
		*(.data1 .data1.*)
		 __data1_end__ = .;     		/* Label in case we want address of data section end */
	}

	/* 
	 * Next we put stack for Core0 
	 */
	.stack0 : {
        . = ALIGN(8); 					/* Stack must always be aligned to 8 byte boundary AAPCS32 call standard */
        __stack_start__core0 = .;		/* Label in case we want address of stack core 0 section start */
        . = . + 512;     				/* IRQ stack size core 0 */
        __IRQ_stack_core0 = .;
        . = . + 512;     				/* FIQ stack size core 0 */
        __FIQ_stack_core0 = .;
        . = . + 32768;    				/* SVC stack size core 0 */
        __SVC_stack_core0 = .;
        . = . + 32768;    				/* SYS stack size core 0 */
        __SYS_stack_core0 = .;
        __stack_end__core0 = .;  		/* Label in case we want address of stack core 0 section end */
	}

	.bss : {
		. = ALIGN(4);
		__bss_start = .;
		*(.bss .bss.*)
		__bss_end = .;
	}

	/**
	 *	Stack starts at the top of the RAM, and moves down!
	 **/
	. = ALIGN(8); 					/* Stack must always be aligned to 8 byte boundary AAPCS32 call standard */
	. = . + 32768;
	_estack = .;

	__NULL_ptr = . + 1;
}
