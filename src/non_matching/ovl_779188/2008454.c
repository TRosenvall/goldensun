/* OvlFunc_879_2008454 -- 0x02008454  [asm/overlays/rom_779188/ovl_30_c_c_c.s]
 *
 * NOT MATCHING. Best 86 lines against 83, 66 differing -- and the 66 is almost
 * entirely the shift from ONE EXTRA CALLEE-SAVED REGISTER. The .s also holds
 * OvlFunc_879_2008238 and OvlFunc_879_20082e8 plus `.section .data` and
 * `.section .bss` tails, so no split was done; it would be wasted until the
 * body lands, and the data must stay with an assembled part when it is.
 *
 * Builds the map screen: load the tileset file, DMA its palette, decompress the
 * tiles into gBuffer and DMA them to VRAM, fill a 20x30 tilemap with
 * consecutive indices and a two-entry terminator per row, zero the four scroll
 * pairs and DMA them to the BG scroll registers.
 *
 * A NEW file_table.sym MEMBER IS NEEDED AND IS MEASURED: `_FILE_1a = 0x1a;`.
 * The ROM emits `ldr r5, =0x1a` -- a value an eight-bit `mov` builds -- and
 * feeds it to __GetFile, which is exactly the id space file_table.sym covers;
 * the sibling src/overlays/rom_779188/ovl_30_c_c_b.c in this same overlay
 * already spells its own as `__GetFile((int)&_FILE_1c)`. Measured here: the
 * literal `0x1a` emits `mov r0, #0x1a` INLINE at the call (75 differing) and
 * the symbol pools it (66). THE SYMBOL WAS NOT ADDED TO file_table.sym,
 * because the function does not yet match and an unreferenced entry is clutter
 * -- add it in the same change that lands the body.
 *
 * The id also has to be ASSIGNED BEFORE the preceding __Func_8003b70 call, not
 * at its use: the ROM keeps it in r5 across that call (`ldr r5, =0x1a / bl / …
 * / mov r0, r5`). Writing `__GetFile((int)&_FILE_1a)` at the use loads it
 * straight into r0. That is the "name a value that must SURVIVE something"
 * rule, and it is worth 9 differing lines on its own.
 *
 * BLOCKER CLASS: gBuffer is CACHED IN A CALLEE-SAVED REGISTER across the
 * __DecompressLZ call where the ROM RELOADS it. gcc hoists `ldr r5,
 * =0x2010000` above the `lsl / add` that computes DecompressLZ's first
 * argument and keeps it live; the ROM emits `ldr r1, =0x2010000` at the call
 * and `ldr r0, =gBuffer` again afterwards for the DMA. That one cached value
 * is the third pushed register, and every later register is displaced by it.
 *
 * MEASURED, four spellings, three tie at 66 and one is worse:
 *
 *     gBuffer referenced inline at both sites               66 of 83
 *     a separate named local per site (rematerialisation)   66
 *     `do { } while (0)` around the __DecompressLZ call     66
 *     `do { } while (0)` around the __GetFile call          75  (worse)
 *
 * docs/elevation.md's own test applies to the first three: they share the
 * assumption that gBuffer is referenced twice in a form gcc can common. NEXT:
 * defeat the CSE itself rather than the scheduling -- nothing tried here made
 * the two references structurally different. The rematerialisation lever is
 * recorded for CONSTANTS reused across a call; this is an ARRAY ADDRESS, and
 * whether the lever reaches that case is exactly what is untested.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern int _FILE_1a;
extern unsigned char gBuffer[];
extern short iwram_3001ad0[];
extern unsigned char *iwram_3001e70[];
extern void __Func_8003b70(int a);
extern void *__GetFile(int id);
extern void __DecompressLZ(void *src, void *dst);

void OvlFunc_879_2008454(void)
{
    unsigned char *f;
    short *m;
    short *q;
    short t;
    int i;
    int j;
    int term;
    int id;

    id = (int)&_FILE_1a;
    __Func_8003b70(0);
    REG_BG2CNT = 0x681;
    iwram_3001ad0[5] = 0;
    term = 0x1ff;
    f = __GetFile(id);
    DMA3_COPY(f, (void *)(0xa0 << 19), 0x1c0);
    __DecompressLZ(f + (0xe0 << 1), gBuffer);
    DMA3_COPY(gBuffer, (void *)0x6006800, 0x9600);
    m = (short *)0x6003000;
    t = 0xd0 << 1;
    for (j = 0; j <= 0x13; j++) {
        for (i = 0; i <= 0x1d; i++) {
            *m = t;
            t = t + 1;
            m++;
        }
        *m = term;
        m++;
        *m = term;
        m++;
    }
    q = iwram_3001ad0;
    for (i = 0; i <= 3; i++) {
        q[1] = 0;
        q[0] = 0;
        q += 2;
    }
    DMA3_COPY(iwram_3001ad0, (void *)REG_ADDR_BG0HOFS, 0x10);
    *(short *)(iwram_3001e70[0] + 0x14) = 0xa0 << 5;
}
