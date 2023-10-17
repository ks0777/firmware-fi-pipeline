MEMORY
{
   RAM (rwx)  : ORIGIN = 0x80000000, LENGTH = 128M 
}
ENTRY(_start)

PROVIDE(__stack_top = ORIGIN(RAM) + LENGTH(RAM));

SECTIONS {
    . = ORIGIN(RAM);
    .init : { *(.init) }

    .text :
    { 
	KEEP(*(.note.gnu.build-id))
	*(.text)
    }

    . = ALIGN(16);
    PROVIDE(__global_pointer = .);

    .sbss : 
    {
	*(.sbss)
    }
}
