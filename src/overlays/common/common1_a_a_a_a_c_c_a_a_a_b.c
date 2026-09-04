/* OvlFunc_common1_88c -- shared overlay helper
 *
 * Allocates a scratch buffer, claims a sprite slot if one is not cached,
 * decompresses a file into it, DMAs a palette out of it, uploads the sprite
 * graphics, waits for the DMA to retire and frees the buffer.
 *
 * A LABEL-NUMBER COLLISION THAT tryc CANNOT SEE, and the reason this file
 * spells its data label through an alias. `.L10` is a real cross-object data
 * symbol -- `.global .L10` in asm/overlays/common/common1_c_c_b.s -- but gcc's
 * own first branch label in THIS function is also numbered `.L10`, so the
 * pool's `.word .L10` captures the LOCAL label instead. The instruction stream
 * is byte-identical either way; the defect lives entirely in the relocation
 * table (`R_ARM_ABS32 .text` where the reference has `R_ARM_ABS32 .L10`), so
 * tryc.py screens it clean and only tools/objcmp.py catches it.
 *
 * TWO FIXES EXIST AND THIS FILE TAKES THE DURABLE ONE. A statement that merely
 * ALLOCATES LABEL NUMBERS above the first branch -- `do { } while (0);`, or an
 * empty `else` -- shifts gcc's numbering off the collision and also passes,
 * with byte-identical output. That is a real and previously unrecorded trick,
 * but it is the wrong thing to ship: the construct has no meaning to a reader,
 * its effect is invisible, and any unrelated edit that adds or removes a branch
 * silently breaks it. The linker alias is the project's documented remedy for
 * this class, the immediate sibling common1_a_a_a_a_c_c_a_a_b.c already uses it
 * for five labels, and absolute assignments emit no bytes. `_TBL_L10 = .L10;`
 * was added beside them in the three overlay scripts that list this object.
 *
 * TWO LEVERS ON THE BODY, both measured across ~35 spellings:
 *
 *  - THE OPERAND ORDER NEEDED AN INTEGER CAST, not a reordering. Every
 *    pointer-typed spelling of the upload address gives the three-operand
 *    `add r2, r6, r2`, because the frontend canonicalises the pointer operand
 *    to the front; writing the arithmetic in `int` and casting back puts the
 *    shifted value first and produces the ROM's two-operand `add r2, r6`.
 *  - THE SIZE CONSTANT HAD TO MOVE TO A DIFFERENT BASIC BLOCK. As a literal at
 *    the call it schedules one slot early. Naming it and assigning it in a
 *    DOMINATING block rematerialises it at the call in the ROM's phase --
 *    docs/elevation.md's "rebuilt: a local in a dominating block". Assigning it
 *    in the SAME block as the call is catastrophic, 49 of 59.
 *
 * Naming the buffer pointer in a local is also catastrophic here (47-49): it
 * forces the data label's base into r8 and reallocates the whole function.
 *
 * The structural assumption every losing attempt shared was that the residue
 * was reachable by respelling the call's third argument. It was not -- one half
 * was a TYPE question and the other a STATEMENT-PLACEMENT question, and the
 * last defect was not in the instruction stream at all.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern short L10[] __asm__("_TBL_L10");
extern unsigned char L1[] __asm__(".L1");
extern int _FILE_e7;

extern void *__Func_8004970(int size);
extern short __AllocSpriteSlot(void);
extern void *__GetFile(int id);
extern void __DecompressLZ(void *src, void *dst);
extern void __UploadSpriteGFX(int slot, int size, void *src);
extern void __free(void *p);

void OvlFunc_common1_88c(int a)
{
    unsigned char *buf;
    vu32 *dma;
    int off;
    int size;

    buf = __Func_8004970(0xe5 << 5);
    if (*L10 == -1)
        *L10 = __AllocSpriteSlot();
    size = 0x80 << 3;
    off = L1[a];
    if (a == 8)
        a = 4;
    __DecompressLZ(__GetFile((int)&_FILE_e7), buf);
    DMA3_COPY(buf + off, (void *)0x50003e0, 0x20);
    __UploadSpriteGFX(*L10, size, (void *)((a << 10) + (int)buf + 0xa0));
    dma = (vu32 *)&REG_DMA3SAD;
    while (dma[2] & 0x80000000)
        ;
    __free(buf);
}
