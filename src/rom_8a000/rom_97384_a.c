/* Func_8097384  --  0x08097384
 *
 * Queues three DMA copies of portrait data, then picks one of eight entries
 * from a table according to the highest set flag in a run of eight, and hands
 * it to the sprite loader.
 *
 * THIS FUNCTION WAS INVISIBLE UNTIL BATCH 222 -- tools/filtered.py rejected it
 * as the duplicate-constant class, on a park that is now byte-exact.
 *
 * THE EXTERN'S DECLARED SHAPE DECIDES WHETHER THE BASE STAYS BARE. The ROM
 * pools the bare `iwram_3001ebc` and reaches both globals with immediate
 * offsets off it, `ldr r5, [r3, #0x14]` and `ldr r4, [r3, #0x0]`. Declared the
 * way the rest of the tree declares it -- `extern unsigned char
 * *iwram_3001ebc;` -- and reached as `(&iwram_3001ebc)[5]`, gcc FOLDS the
 * offset into the pool word (`ldr r3, =iwram_3001ebc+20`) and then pays a
 * `sub r3, #0x14` to get back to the base. One extra instruction, and every
 * line after it shifts: 106 lines against 105 with 103 differing.
 *
 * Declaring it as an ARRAY here, `extern unsigned char *iwram_3001ebc[];`, and
 * writing `iwram_3001ebc[5]` and `iwram_3001ebc[0]`, keeps the base bare and
 * emits both immediate offsets. 103 differing -> 0. This is the element-type
 * folding lever from batch 222 in its second form: there it was byte offset
 * versus subscript on one object, here it is address-of-a-scalar versus a
 * declared array. THE TELL IS THE SAME -- a folded `=sym+offset` in our output
 * where the ROM has a bare symbol, and a length one instruction over.
 *
 * The declaration deliberately disagrees with the other files that spell this
 * symbol as a single pointer. Both are true of the same address; this file
 * needs the one that reaches the neighbouring global.
 *
 * r5 CARRIES TWO UNRELATED VALUES and that is the ROM's own allocation, not a
 * trick: the second global's pointer up to the last DMA, then the table index
 * for the rest. Their ranges do not overlap and gcc finds it without help.
 *
 * THE FLAG IDS ALTERNATE BETWEEN TWO ENCODINGS, which is worth knowing before
 * reading the eight tests as inconsistent. 0x148, 0x14a, 0x14c and 0x14e are
 * built `mov r0, #0xa4 / lsl r0, #1`; the odd ids in between cannot be reached
 * that way and are pooled. Writing them as `0xa4 << 1` and `0x149` matches
 * what gcc does with each.
 *
 * `_TBL_a0108` is the eight-entry table, aliased in label.sym -- a new file,
 * because the overlays each had somewhere to put such an assignment and code
 * linking through stage1.ld did not.
 */
#include "dma.h"

extern unsigned char *iwram_3001ebc[];
extern int iwram_3001e40;
extern int _TBL_a0108[];

extern int _GetFlag(int id);
extern void Func_8091200(int a, int b);
extern void Func_8091254(int a);

void Func_8097384(void)
{
    unsigned char *p;
    unsigned char *q;
    int v;

    q = iwram_3001ebc[5];
    p = iwram_3001ebc[0];
    DMA3_SET(q + (0x9a << 5), p + 0x776, 0x84000150);
    if (*(short *)(p + 0xcb8) == 0)
        DMA3_SET(q + (0xe0 << 4), p + 0x236, 0x84000150);
    DMA3_SET(q + (0xe0 << 4), q + (0xe0 << 2), 0x840002a0);
    v = iwram_3001e40 & 7;
    if (_GetFlag(0xa4 << 1))
        v = 0;
    if (_GetFlag(0x149))
        v = 1;
    if (_GetFlag(0xa5 << 1))
        v = 2;
    if (_GetFlag(0x14b))
        v = 3;
    if (_GetFlag(0xa6 << 1))
        v = 4;
    if (_GetFlag(0x14d))
        v = 5;
    if (_GetFlag(0xa7 << 1))
        v = 6;
    if (_GetFlag(0x14f))
        v = 7;
    Func_8091200(_TBL_a0108[v], 1);
    Func_8091254(8);
}
