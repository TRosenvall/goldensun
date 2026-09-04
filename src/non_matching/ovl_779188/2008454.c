/* OvlFunc_879_2008454 -- 0x02008454  [asm/overlays/rom_779188/ovl_30_c_c_c.s]
 *
 * STILL NOT MATCHING, but the body below is a long way past the previous park:
 * tryc.py reports 83 lines against 83 with ONE differing line, and that one is
 * the `_FILE_1a` symbol phantom which resolves at link. THE OBJECT IS STILL
 * FOUR BYTES LARGER (232 against 228), and that is the whole remaining defect.
 *
 * WHERE THE FOUR BYTES ARE. Both objects hold SIXTEEN pool words, so the pool
 * itself is right; the extra four bytes are in the CODE. That means one more
 * pool DUMP than the ROM has -- a dump costs the `b` that jumps over it plus
 * its alignment, and tryc normalises both away, which is why the screen calls
 * this exact. Its sibling OvlFunc_880_2008054 measured the mechanism: a NARROW
 * (HImode) pool reference is created at expand time and forces a dump where it
 * is used, and naming that constant in an `int` deletes the dump entirely. So
 * the next move is to find which of this function's constants is taking a
 * narrow reference and give it a wider one -- NOT to keep respelling the body.
 *
 * THREE LEVERS LANDED SINCE THE FIRST PARK, and the first is new and general.
 *
 *  1. A PIN ON THE ARGUMENT REGISTER DEFEATS A CONSTANT-ADDRESS CSE. The
 *     original park's blocker was gBuffer cached in a callee-saved register
 *     across __DecompressLZ where the ROM reloads it, with four spellings tied
 *     at 66 and the note "defeat the CSE itself -- nothing tried made the two
 *     references structurally different". This does:
 *
 *         register unsigned char *b1 __asm__("r1");
 *         b1 = gBuffer;
 *         __DecompressLZ(f + (0xe0 << 1), b1);
 *
 *     gcc force_regs a non-immediate call argument into a PSEUDO, and CSE
 *     commons that pseudo with the later DMA's. Binding it to the CALL-CLOBBERED
 *     argument register puts the value where the call invalidates it, so CSE
 *     cannot reuse it and gcc reloads from the SAME pool word. 66 to 53, and it
 *     drops a pushed register. THAT THE REGISTER IS CALL-CLOBBERED IS THE WHOLE
 *     MECHANISM -- the ordinary rematerialisation lever is on file for
 *     CONSTANTS and does not reach an array address.
 *
 *  2. THE TILE LOOP MUST BE `goto`s AT BOTH LEVELS. The ROM rebuilds 0x10000
 *     INSIDE the inner loop; a `for` at either level lets LICM hoist it to the
 *     preheader, and rewriting only the inner loop is not enough because the
 *     outer one still hoists just as far. 53 to 12.
 *
 *  3. The scroll-pair zeros want a TYPED STRUCT FIELD and the final `0xa0 << 5`
 *     a NAMED LOCAL, or both pool where the ROM builds them. 12 to 1.
 *
 * TWO THINGS DELIBERATELY NOT LEFT IN THE TREE. `_FILE_1a = 0x1a;` is measured
 * and needed -- the literal emits an inline `mov`, the symbol pools it as the
 * ROM does -- but it is NOT in file_table.sym, because an unreferenced symbol
 * is clutter; add it with the body. And the .s is NOT split, because a split
 * for a parked function is three files where one was.
 *
 * WHEN IT IS SPLIT, IT WILL NEED TWO `.global`s: `.L68c` and `.L6a0` are
 * `.lcomm` cells in the data tail that the other two functions in the file
 * reference, and the link fails without them. Byte-neutral, since they are
 * `.bss`. This was hit and fixed once already; it is recorded so the next
 * attempt does not rediscover it.
 *
 * ONE MORE MEASURED NEGATIVE, worth its own line because it looks so
 * reasonable: giving the struct view of iwram_3001ad0 an `__asm__` alias while
 * other references use the plain name COSTS A POOL WORD. gcc compares SYMBOL_REF
 * strings by pointer and `__asm__` names carry a `*` prefix, so an aliased name
 * can never share a pool slot with the plain one. Declare the ONE type you need
 * and use it at every site.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern int _FILE_1a;
extern unsigned char gBuffer[];
extern short iwram_3001ad0[];
struct BgOfs {
    unsigned short h;
    unsigned short v;
};

extern struct BgOfs iwram_3001ad0_o[] __asm__("iwram_3001ad0");
extern unsigned char *iwram_3001e70;
extern void __Func_8003b70(int a);
extern void *__GetFile(int id);
extern void __DecompressLZ(void *src, void *dst);

void OvlFunc_879_2008454(void)
{
    unsigned char *f;
    register unsigned char *b1 __asm__("r1");
    short *m;
    struct BgOfs *q;
    unsigned char *h;
    int y;
    int t;
    unsigned int i;
    unsigned int j;
    int w;
    int term;
    int id;

    id = (int)&_FILE_1a;
    __Func_8003b70(0);
    REG_BG2CNT = 0x681;
    iwram_3001ad0[5] = 0;
    term = 0x1ff;
    f = __GetFile(id);
    DMA3_COPY(f, (void *)(0xa0 << 19), 0x1c0);
    b1 = gBuffer;
    __DecompressLZ(f + (0xe0 << 1), b1);
    DMA3_COPY(gBuffer, (void *)0x6006800, 0x9600);
    m = (short *)0x6003000;
    t = 0xd0 << 1;
    j = 0;
row:
    i = 0;
cell:
    w = t;
    t = ((t << 16) + 0x10000) >> 16;
    *m++ = w;
    i++;
    if (i <= 0x1d)
        goto cell;
    *m++ = term;
    *m++ = term;
    j++;
    if (j <= 0x13)
        goto row;
    q = iwram_3001ad0_o;
    j = 0;
    do {
        j++;
        q->v = 0;
        q->h = 0;
        q++;
    } while (j <= 3);
    DMA3_COPY(iwram_3001ad0, (void *)REG_ADDR_BG0HOFS, 0x10);
    h = iwram_3001e70;
    y = 0xa0 << 5;
    *(short *)(h + 0x14) = y;
}
