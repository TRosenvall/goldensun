MEMORY {
	bios	(x)	: ORIGIN = 0x00000000, LENGTH = 16K
	ewram	(rwx)	: ORIGIN = 0x02000000, LENGTH = 256K
	overlay (rwx)	: ORIGIN = 0x02008000, LENGTH = 32K
	iwram	(rwx)	: ORIGIN = 0x03000000, LENGTH = 32K
	io	(rw)	: ORIGIN = 0x04000000, LENGTH = 1K
	palette	(rw)	: ORIGIN = 0x05000000, LENGTH = 1K
	vram	(rw)	: ORIGIN = 0x06000000, LENGTH = 96K
	oam	(rw)	: ORIGIN = 0x07000000, LENGTH = 1K
	rom	(rx)	: ORIGIN = 0x08000000, LENGTH = 8M
	sram	(rw)	: ORIGIN = 0x0E000000, LENGTH = 64K
}

SECTIONS {
	.text : {
		src/overlays/rom_7a37f0/exports.o(.text)
		asm/overlays/rom_7a37f0/ovl_30_a_b.o(.text)
		asm/overlays/rom_7a37f0/ovl_30_b.o(.text)
		asm/overlays/rom_7a37f0/ovl_30_c_b.o(.text)
		asm/overlays/rom_7a37f0/ovl_30_c_c_a.o(.text)
		asm/overlays/rom_7a37f0/ovl_30_c_c_b.o(.text)
		asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_a_a.o(.text)
		asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_a_b.o(.text)
		asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_b.o(.text)
		asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_a.o(.text)
		asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_b.o(.text)
		asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_c.o(.text)
		asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_c.o(.text)
		asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_b.o(.text)
		asm/overlays/rom_7a37f0/ovl_30_c_c_c_b.o(.text)
		asm/overlays/rom_7a37f0/ovl_30_c_c_c_c_b.o(.text)
		asm/overlays/rom_7a37f0/ovl_30_c_c_c_c_c_b.o(.text)
		asm/overlays/rom_7a37f0/ovl_30_c_c_c_c_c_c.o(.text)
		src/overlays/rom_7a37f0/imports.o(.text)
	} > overlay

	.data : {
		asm/overlays/rom_7a37f0/ovl_30_c_c_c_c_c_b.o(.data)
		asm/overlays/rom_7a37f0/ovl_30_c_c_c_c_c_c.o(.data)
	} > overlay

	.bss : {
		asm/overlays/rom_7a37f0/ovl_30_c_c_c_c_c_b.o(.bss)
		asm/overlays/rom_7a37f0/ovl_30_c_c_c_c_c_c.o(.bss)
	} > overlay

	/DISCARD/ : {
		*(*)
	}
}

/* gcc-2.96 emits __divsi3 for `/` and has no flag to rename it. Overlay code
 * calls the RAM-resident copy through the stub .export_func divsi3_RAM makes.
 * These are different functions at different addresses. Emits no bytes. */
__divsi3 = _divsi3_RAM;
