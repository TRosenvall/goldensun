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
		src/overlays/rom_7c7b9c/exports.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_a_a_a_a.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_a_a_a_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_a_a_a_c_a.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_a_a_a_c_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_a_a_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_a_a_c.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_a_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_a_c_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_a_c_c.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_a.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_b.o(.text)
		src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_a.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_a_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_a_c.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_a_a.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_a_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_a.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_c_a_a.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_c_a_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_c_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_c.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_c_a.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_c_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_c_c.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_c_a.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_c_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_c_a.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_c_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_c_a_a_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_c_a_a_c.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_c_a_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_c_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_a_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_b.o(.text)
		asm/overlays/rom_7c7b9c/ovl_30_c_c.o(.text)
		src/overlays/rom_7c7b9c/imports.o(.text)
	} > overlay

	.data : {
		asm/overlays/rom_7c7b9c/ovl_30_c_c.o(.data)
	} > overlay

	.bss : {
		asm/overlays/rom_7c7b9c/ovl_30_c_c.o(.bss)
	} > overlay

	/DISCARD/ : {
		*(*)
	}
}

/* gcc-2.96 emits __umodsi3 for `%` and has no flag to rename it. Overlay code
 * calls the RAM-resident copy through the stub .export_func umodsi3_RAM makes.
 * These are different functions at different addresses. Emits no bytes. */
__umodsi3 = _umodsi3_RAM;

/* `.L5160` is a data table exported (.global) from ovl_30_c_c.s and referenced
 * from five other objects in this overlay. `.L5160` is not spellable as a C
 * identifier, so elevated code needs a name for it. Emits no bytes.
 * See docs/elevation.md, "A short .LN extern can be captured by gcc's labels". */
_TBL_5160 = .L5160;
