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
		src/overlays/rom_77a7c8/exports.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_a_a_a_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_a_a_a_c_a_a.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_a_a_a_c_a_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_a_a_a_c_a_c.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_a_a_a_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_a_a_a_c_c.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_a_a_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_a_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_a_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_a_c_a.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_a_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_c_a_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_c_a_c.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_c_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_c_c_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_c_c_c_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_c_c_c_c_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_a_a.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_a_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_a.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_c_c.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_c.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_c_a.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c_c_a.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c_c_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c_c_c_c.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_c_a_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_c_a_c.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_c_c.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_c_a_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_c_a_c_a.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_c_a_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_c_a_c_c.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_c_c_a_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_c_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_c_c_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_c_c_c_c_b.o(.text)
		asm/overlays/rom_77a7c8/ovl_30_c_c_c_c_c_c.o(.text)
		src/overlays/rom_77a7c8/imports.o(.text)
	} > overlay

	.data : {
		asm/overlays/rom_77a7c8/ovl_30_c_c_c_c_b.o(.data)
		asm/overlays/rom_77a7c8/ovl_30_c_c_c_c_c_b.o(.data)
		asm/overlays/rom_77a7c8/ovl_30_c_c_c_c_c_c.o(.data)
	} > overlay

	.bss : {
		asm/overlays/rom_77a7c8/ovl_30_c_c_c_c_b.o(.bss)
		asm/overlays/rom_77a7c8/ovl_30_c_c_c_c_c_b.o(.bss)
		asm/overlays/rom_77a7c8/ovl_30_c_c_c_c_c_c.o(.bss)
	} > overlay

	/DISCARD/ : {
		*(*)
	}
}

/* gcc-2.96 emits __divsi3 for `/` and has no flag to rename it. Overlay code
 * calls the RAM-resident copy through the stub .export_func divsi3_RAM makes.
 * These are different functions at different addresses, so this alias is what
 * lets a C division in an overlay link to the routine the ROM actually calls.
 * Emits no bytes. Same reasoning as overlays/rom_7a5214/overlay.ld. */
__divsi3 = _divsi3_RAM;

/* gcc-2.96 emits __umodsi3 for the corresponding C operator and has
 * no flag to rename it.  Overlay code calls the RAM-resident copy through
 * the stub .export_func umodsi3_RAM makes; they are different functions at
 * different addresses, so this alias is what lets the C form link to the
 * routine the ROM actually calls.  Emits no bytes.
 * See overlays/rom_7a5214/overlay.ld for the original of this note. */
__umodsi3 = _umodsi3_RAM;
