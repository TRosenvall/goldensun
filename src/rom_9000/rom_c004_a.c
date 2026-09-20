/* Cluster InitActors..InitActors extracted from goldensun/asm/rom_9000/rom_c004.s.
 *
 * Total .text for this TU = 192 bytes (= 0xc0).
 * Preserves the original ROM layout when slotted between
 * asm/rom_9000/rom_be70_c_c.o and asm/rom_9000/rom_c004_b.o in goldensun/stage1.ld.
 *
 * Never attempted before batch 273. NO PINS -- and that is the finding.
 *
 * Allocates the actor header and the 0x40-slot table, zero-fills both by DMA, then
 * starts the per-frame task pair chosen by the mode.
 *
 * `volatile` ON THE FILL WORD REPLACES THE TWO-PIN IDIOM. The direct callee's own
 * elevated file, src/rom_9000/rom_b798_c_c_a_b.c (InitSprites), records
 * `DMA3_SET` + a caller-owned zero + TWO REGISTER PINS as what is needed to get
 * `mov r4, sp / str r5, [r4]` -- the pins existing to stop cse folding `*v = 0` back
 * into `str rX, [sp]`. Declaring the fill word `volatile u32` blocks that fold on its
 * own and is byte-identical to the pinned form, so this file carries no fakematch row.
 *
 * MEASURED LADDER (ref 79 encodings): two DMA3_CLEAR, 71 of 72 -- `sub sp, #8`,
 * because two `u32 value` slots against the ROM's one; DMA3_SET with a caller-owned
 * zero, 11; plus two pins (the b798 idiom), 5; one pin (v -> r4 only), 0;
 * `volatile u32 value` and NO pins, 0.
 *
 * WORTH FOLLOWING UP: b798 itself and the other members of that class
 * (src/rom_c0/rom_56cc_c_c_a.c reaches it through a loop, same mechanism) may be able
 * to drop their pins the same way. That would REMOVE fakematch rows rather than add
 * them.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char *galloc_ewram(int tag, int size);
extern void InitSprites(int mode);
extern int StartTask(void *fn, int pri);
extern void Func_800d340(void);
extern void UpdateActors(void);
extern void Func_800c880(void);
extern void Func_800c62c(void);
extern int iwram_3001d1c;
extern int iwram_3001cc0;

void InitActors(int mode)
{
    unsigned char *hdr;
    unsigned char *tbl;
    volatile u32 value;
    u32 *v;
    int z;

    hdr = galloc_ewram(6, 0x5c);
    tbl = galloc_ewram(5, 0xe0 << 5);
    InitSprites(mode);
    z = 0;
    v = (u32 *)&value;
    *v = z;
    DMA3_SET(v, tbl, 0x85000700);
    *v = z;
    DMA3_SET(v, hdr, 0x85000017);
    if (mode == 4)
        StartTask(Func_800d340, 0xc8a);
    else
        StartTask(UpdateActors, 0xc8a);
    if (mode == 3 || mode == 4) {
        StartTask(Func_800c880, 0xc8 << 4);
    } else {
        StartTask(Func_800c62c, 0xc8 << 4);
        iwram_3001d1c = 0;
        iwram_3001cc0 = 0;
    }
    {
        int t = 0;
        hdr[6] = 0xf;
        hdr[7] = t;
    }
}
