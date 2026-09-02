/* Func_8011644 -- 0x08011644, the whole of
 * goldensun/asm/rom_9000/rom_11568_a_c_c_a.s, so no split was needed and the
 * linker script is unchanged.
 *
 * Loads the world map view's assets: decompress the palette and patch entry 0
 * back to whatever was already on screen, upload it, decompress the four tile
 * banks, install the per-frame hook and the scanline window, then bring in the
 * map data itself.
 *
 * ONE LEVER, AND IT IS REGISTER BIRTH ORDER. Using `gBuffer` inline at each
 * call site gives the right 82 instructions but sinks its pool load fourteen
 * instructions late and swaps r2/r3 at the halfword store. Naming it in a local
 * as the SECOND statement -- before the table pointer is read out of the view
 * block -- makes its pseudo born second and the whole stream falls into place.
 * That is blocker class 2 solved by statement order, the same distinction drawn
 * for a sentinel in batch 179 and for an address-holding pointer in batch 180:
 * it is where the ASSIGNMENT sits, not where the declaration does.
 *
 * Everything else came out of the natural spelling. The epilogue pops into r0,
 * so the function is `void`; every callee is `void` too, so no argument-order
 * lever was needed, and all sixteen calls emit in ROM order from the nested
 * `DecompressLZ(GetFile(x), dst)` form the sibling
 * src/rom_9000/rom_11568_a_c_c_b.c already uses.
 *
 * TWO THINGS ABOUT THE SCREEN, both worth knowing before re-reading this file:
 *
 *   Our `ldrh r5, .L11` prints differently from the ROM's `ldr r5, .L116fc @ 0`
 *   and assembles to the same `0x4d00`. That is the `*thumb_zero_extendhisi2`
 *   print form docs/elevation.md already records, and its 60-byte pool_range is
 *   what forces the mid-function pool with a `b` over it that the ROM has.
 *
 *   The reference's pooled zero is written as a `.word` under a local label
 *   rather than as `=0`, which is why this function is what caught the
 *   under-count in tryc.py's new pool-entry check.
 *
 * VERIFIED BY SIZE as well as by stream: 248 bytes = 0xf8, exactly the ROM span
 * 0x08011644 to 0x0801173c.
 *
 * MEASURED (rom 82 lines):
 *   gBuffer used inline at each call site        82 lines, 8 aligned
 *   gBuffer named as the second statement        82, MATCH
 */
#include "gba/types.h"
#include "dma.h"

extern unsigned char *iwram_3001e70;
extern void (*iwram_3001cfc)(void);
extern void Func_801161c(void);
extern void Func_80113e4(void);
extern void Func_801179c(void);
extern void Func_800439c(void *func);
extern void WaitFrames(unsigned int nframes);
extern void *GetFile(int index);
extern void DecompressLZ(void *src, void *dst);
extern void DecompressLZ1(void *src, void *dst);
extern unsigned char gBuffer[65536];
extern unsigned char ewram_2038000[];
extern unsigned char ewram_203a000[];
extern unsigned char ewram_203c000[];
extern unsigned char ewram_203e000[];
extern int _FILE_d5;

typedef struct {
	unsigned char pad[0xfc];
	unsigned char ffc;
	unsigned char pad2[3];
	unsigned short f100;
	unsigned short f102;
	unsigned char pad3[0x18];
	int *f11c;
} FieldState;

void Func_8011644(void)
{
	FieldState *base;
	int *tbl;
	unsigned char *buf;
	short pal0;

	base = (FieldState *)iwram_3001e70;
	buf = gBuffer;
	tbl = base->f11c;
	pal0 = *(short *)0x5000000;
	DecompressLZ1(GetFile(tbl[0]), buf);
	*(short *)buf = pal0;
	DMA3_COPY(buf, (void *)0x5000000, 0x1c0);
	DecompressLZ(GetFile(tbl[1]), ewram_2038000);
	DecompressLZ(GetFile(tbl[2]), ewram_203a000);
	DecompressLZ(GetFile(tbl[3]), ewram_203c000);
	DecompressLZ(GetFile(tbl[4]), ewram_203e000);
	iwram_3001cfc = Func_801161c;
	base->f100 = 0;
	base->f102 = 0x9f;
	WaitFrames(1);
	DecompressLZ(GetFile((int)&_FILE_d5), buf);
	Func_80113e4();
	base->ffc = 0;
	Func_800439c(Func_801179c);
	WaitFrames(1);
}
