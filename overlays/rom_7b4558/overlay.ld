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
		src/overlays/rom_7b4558/exports.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_a_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_a_c_a_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_a_c_a_c_a.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_a_c_a_c_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_a_c_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_a_c_c_a.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_a_c_c_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_a_c_c_c_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_a_c_c_c_c_a_a.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_a_c_c_c_c_a_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_a_c_c_c_c_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_a_c_c_c_c_c_a.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_a_c_c_c_c_c_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_a_c_c_c_c_c_c.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_c_a_a.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_c_a_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_c_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_c_c_a_a.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_c_c_a_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_c_c_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_a_c_c_c.o(.text)
		asm/overlays/rom_7b4558/ovl_30_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_c_a_a_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_c_a_a_c.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_c_a_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a_a.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a_c.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_c_a_c_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_c_a_c_c_a.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_c_a_c_c_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_c_a_c_c_c_a_a.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_c_a_c_c_c_a_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_c_a_c_c_c_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_c_a_c_c_c_c.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_c_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_c_c_a_a_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_c_c_a_a_c.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_c_c_a_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_c_c_a_c.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_c_c_b.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_c_c_c_a.o(.text)
		asm/overlays/rom_7b4558/ovl_30_c_c_c_c_b.o(.text)
		src/overlays/rom_7b4558/imports.o(.text)
		src/lib/call_via.o(.text)
	} > overlay

	.data : {
		asm/overlays/rom_7b4558/ovl_30_c_c_c_c_b.o(.data)
	} > overlay

/* gcc-2.96 emits __divsi3 for `/` and has no flag to rename it.  Overlay code
 * calls the RAM-resident copy through the stub .export_func divsi3_RAM makes.
 * Same treatment as overlays/rom_7a5214/overlay.ld.  Emits no bytes. */
__divsi3 = _divsi3_RAM;

	/DISCARD/ : {
		*(*)
	}
}
