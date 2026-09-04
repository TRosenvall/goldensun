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
		src/overlays/rom_7e0928/exports.o(.text)
		asm/overlays/rom_7e0928/ovl_30_a_a_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_a_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_a_c_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_a_c_c_a_a.o(.text)
		asm/overlays/rom_7e0928/ovl_30_a_c_c_a_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_a_a.o(.text)
		asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_a_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_c_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_c_c_a.o(.text)
		asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_c_c_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_c_c_c.o(.text)
		asm/overlays/rom_7e0928/ovl_30_a_c_c_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_a_c_c_c.o(.text)
		asm/overlays/rom_7e0928/ovl_30_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_a_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_a_c_a_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_a_c_a_c.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_a_c_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_a_c_c_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_a_c_c_c.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_c_a_a.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_c_a_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_c_a_c_a.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_c_a_c_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_c_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_c_c_a_a.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_c_c_a_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_c_c_a_c_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_c_c_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_a.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_a.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_c.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_c_c_c_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_c_c_c_c_a.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_c_c_c_c_b.o(.text)
		asm/overlays/rom_7e0928/ovl_30_c_c_c_c_c_c.o(.text)
		asm/overlays/common/common1_a_a_a_a_a_b.o(.text)
		asm/overlays/common/common1_a_a_a_a_a_c_a.o(.text)
		asm/overlays/common/common1_a_a_a_a_a_c_b.o(.text)
		asm/overlays/common/common1_a_a_a_a_b.o(.text)
		asm/overlays/common/common1_a_a_a_a_c_a.o(.text)
		asm/overlays/common/common1_a_a_a_a_c_b.o(.text)
		asm/overlays/common/common1_a_a_a_a_c_c_a_a_a_b.o(.text)
		asm/overlays/common/common1_a_a_a_a_c_c_a_a_a_c.o(.text)
		asm/overlays/common/common1_a_a_a_a_c_c_a_a_b.o(.text)
		asm/overlays/common/common1_a_a_a_a_c_c_a_b.o(.text)
		asm/overlays/common/common1_a_a_a_a_c_c_a_c.o(.text)
		asm/overlays/common/common1_a_a_a_a_c_c_b.o(.text)
		asm/overlays/common/common1_a_a_a_b.o(.text)
		asm/overlays/common/common1_a_a_b.o(.text)
		asm/overlays/common/common1_a_a_c_a.o(.text)
		asm/overlays/common/common1_a_a_c_b.o(.text)
		asm/overlays/common/common1_a_b.o(.text)
		asm/overlays/common/common1_a_c_b.o(.text)
		asm/overlays/common/common1_a_c_c_b.o(.text)
		asm/overlays/common/common1_a_c_c_c.o(.text)
		asm/overlays/common/common1_b.o(.text)
		asm/overlays/common/common1_c_a_b.o(.text)
		asm/overlays/common/common1_c_a_c_b.o(.text)
		asm/overlays/common/common1_c_a_c_c_a_a.o(.text)
		asm/overlays/common/common1_c_a_c_c_a_b.o(.text)
		asm/overlays/common/common1_c_a_c_c_a_c.o(.text)
		asm/overlays/common/common1_c_a_c_c_b.o(.text)
		asm/overlays/common/common1_c_b.o(.text)
		asm/overlays/common/common1_c_c_a.o(.text)
		asm/overlays/common/common1_c_c_b.o(.text)
		src/overlays/rom_7e0928/imports.o(.text)
		src/lib/call_via.o(.text)
	} > overlay

	.data : {
		asm/overlays/rom_7e0928/ovl_30_c_c_c_c_c_a.o(.data)
		asm/overlays/rom_7e0928/ovl_30_c_c_c_c_c_b.o(.data)
		asm/overlays/rom_7e0928/ovl_30_c_c_c_c_c_c.o(.data)
		asm/overlays/common/common1_c_c_b.o(.data)
		asm/overlays/rom_7e0928/ovl_30_c_c_c_c_c_a.o(.data1)
		asm/overlays/rom_7e0928/ovl_30_c_c_c_c_c_b.o(.data1)
		asm/overlays/rom_7e0928/ovl_30_c_c_c_c_c_c.o(.data1)
		asm/overlays/common/common1_c_c_b.o(.data1)
	} > overlay

	.bss : {
		asm/overlays/rom_7e0928/ovl_30_c_c_c_c_c_a.o(.bss)
		asm/overlays/rom_7e0928/ovl_30_c_c_c_c_c_b.o(.bss)
		asm/overlays/rom_7e0928/ovl_30_c_c_c_c_c_c.o(.bss)
		asm/overlays/common/common1_c_c_b.o(.bss)
	} > overlay

/* gcc-2.96 emits __divsi3 for `/` and has no flag to rename it.  Overlay code
 * calls the RAM-resident copy through the stub .export_func divsi3_RAM makes.
 * Same treatment as overlays/rom_7a5214/overlay.ld.  Emits no bytes. */
__divsi3 = _divsi3_RAM;

	/DISCARD/ : {
		*(*)
	}
}

/* gcc-2.96 emits __modsi3 for the corresponding C operator and has
 * no flag to rename it.  Overlay code calls the RAM-resident copy through
 * the stub .export_func modsi3_RAM makes; they are different functions at
 * different addresses, so this alias is what lets the C form link to the
 * routine the ROM actually calls.  Emits no bytes.
 * See overlays/rom_7a5214/overlay.ld for the original of this note. */
__modsi3 = _modsi3_RAM;

/* Aliases for five script tables whose ROM labels are .L2, .L3, .L11, .L12 and
 * .L13.  gcc generates local labels with those same names for its own branch
 * targets, and the assembler then resolves a C reference to the LOCAL label
 * rather than the exported data symbol -- silently wrong code.  These give the
 * same addresses a name gcc cannot collide with.  Emit no bytes.
 * See docs/elevation.md, "A short .LN extern can be captured by gcc's labels". */
_TBL_L10 = .L10;
_TBL_L2 = .L2;
_TBL_L3 = .L3;
_TBL_L11 = .L11;
_TBL_L12 = .L12;
_TBL_L13 = .L13;
