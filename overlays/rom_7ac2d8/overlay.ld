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
		src/overlays/rom_7ac2d8/exports.o(.text)
		asm/overlays/common/common0_a_a.o(.text)
		asm/overlays/common/common0_a_b_a.o(.text)
		asm/overlays/common/common0_a_b_b.o(.text)
		asm/overlays/common/common0_b.o(.text)
		asm/overlays/common/common0_c.o(.text)
		asm/overlays/rom_7ac2d8/ovl_314_a_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_314_a_c_a_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_314_a_c_a_c_a.o(.text)
		asm/overlays/rom_7ac2d8/ovl_314_a_c_a_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_314_a_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_314_a_c_c_a.o(.text)
		asm/overlays/rom_7ac2d8/ovl_314_a_c_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_314_a_c_c_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_314_a_c_c_c_c.o(.text)
		asm/overlays/rom_7ac2d8/ovl_314_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_314_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_314_c_c.o(.text)
		asm/overlays/rom_7ac2d8/ovl_d58_a.o(.text)
		asm/overlays/rom_7ac2d8/ovl_d58_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_d58_c.o(.text)
		asm/overlays/rom_7ac2d8/ovl_e20_a.o(.text)
		asm/overlays/rom_7ac2d8/ovl_e20_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_e20_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_e20_c_c_a.o(.text)
		asm/overlays/rom_7ac2d8/ovl_e20_c_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_e20_c_c_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_e20_c_c_c_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_e20_c_c_c_c_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_e20_c_c_c_c_c_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_e20_c_c_c_c_c_c_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_e20_c_c_c_c_c_c_c_c.o(.text)
		asm/overlays/rom_7ac2d8/ovl_f84_a_a_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_f84_a_a_c.o(.text)
		asm/overlays/rom_7ac2d8/ovl_f84_a_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_f84_a_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_a.o(.text)
		asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_a_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_a_c.o(.text)
		asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_c.o(.text)
		asm/overlays/rom_7ac2d8/ovl_f84_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_f84_c_a.o(.text)
		asm/overlays/rom_7ac2d8/ovl_f84_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_f84_c_c_a.o(.text)
		asm/overlays/rom_7ac2d8/ovl_f84_c_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_f84_c_c_c.o(.text)
		asm/overlays/rom_7ac2d8/ovl_1db4.o(.text)
		asm/overlays/rom_7ac2d8/ovl_22c4_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_22c4_c_a.o(.text)
		asm/overlays/rom_7ac2d8/ovl_22c4_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_22c4_c_c_a.o(.text)
		asm/overlays/rom_7ac2d8/ovl_22c4_c_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_22c4_c_c_c_a.o(.text)
		asm/overlays/rom_7ac2d8/ovl_22c4_c_c_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_22c4_c_c_c_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_22c4_c_c_c_c_c.o(.text)
		asm/overlays/rom_7ac2d8/ovl_2dcc_a.o(.text)
		asm/overlays/rom_7ac2d8/ovl_2dcc_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_2dcc_c.o(.text)
		asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_35b8_a_a_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_a.o(.text)
		asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c.o(.text)
		asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a.o(.text)
		asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_35b8_a_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_35b8_a_c_a_a.o(.text)
		asm/overlays/rom_7ac2d8/ovl_35b8_a_c_a_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_35b8_a_c_a_c.o(.text)
		asm/overlays/rom_7ac2d8/ovl_35b8_a_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_35b8_a_c_c_a.o(.text)
		asm/overlays/rom_7ac2d8/ovl_35b8_a_c_c_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_35b8_b.o(.text)
		asm/overlays/rom_7ac2d8/ovl_35b8_c.o(.text)
		src/overlays/rom_7ac2d8/imports.o(.text)
		src/lib/call_via.o(.text)
	} > overlay

	.data : {
		asm/overlays/common/common0_c.o(.data)
		asm/overlays/rom_7ac2d8/ovl_314_c_c.o(.data)
		asm/overlays/rom_7ac2d8/ovl_35b8_c.o(.data)
		asm/overlays/common/common0_c.o(.data1)
		asm/overlays/rom_7ac2d8/ovl_2dcc_a.o(.data)
		asm/overlays/rom_7ac2d8/ovl_2dcc_b.o(.data)
		asm/overlays/rom_7ac2d8/ovl_2dcc_c.o(.data)
		asm/overlays/rom_7ac2d8/ovl_1db4.o(.data)
		asm/overlays/rom_7ac2d8/ovl_d58_c.o(.data)
		asm/overlays/rom_7ac2d8/ovl_f84_c_c_a.o(.data)
		asm/overlays/rom_7ac2d8/ovl_f84_c_c_b.o(.data)
		asm/overlays/rom_7ac2d8/ovl_f84_c_c_c.o(.data)
		asm/overlays/rom_7ac2d8/ovl_22c4_c_c_c_c_b.o(.data)
		asm/overlays/rom_7ac2d8/ovl_22c4_c_c_c_c_c.o(.data)
		asm/overlays/rom_7ac2d8/ovl_e20_c_c_c_c_c_c_c_b.o(.data)
		asm/overlays/rom_7ac2d8/ovl_e20_c_c_c_c_c_c_c_c.o(.data)
	} > overlay

	/DISCARD/ : {
		*(*)
	}
}

/* gcc-2.96 emits __divsi3 for the corresponding C operator and has
 * no flag to rename it.  Overlay code calls the RAM-resident copy through
 * the stub .export_func divsi3_RAM makes; they are different functions at
 * different addresses, so this alias is what lets the C form link to the
 * routine the ROM actually calls.  Emits no bytes.
 * See overlays/rom_7a5214/overlay.ld for the original of this note. */
__divsi3 = _divsi3_RAM;
__umodsi3 = _umodsi3_RAM;
