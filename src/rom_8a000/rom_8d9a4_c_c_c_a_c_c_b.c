/* Func_8091494 -- 0x08091494
 *
 * Sets up one sprite: allocate a 0x1c-byte record in ewram and a 0x400-byte
 * tile buffer in iwram, DMA-fill the tiles with 0x11111111, upload them, free
 * the scratch, start the per-frame task, set the blend registers, clear the
 * record, and store the actor into it -- fetching the field actor from
 * gState+0x1f4 when the caller did not supply one.
 *
 * THE TWO DMA WRITES SHARE ONE STACK WORD, and that is what the frame size
 * says. include/dma.h's DMA3_FILL and DMA3_CLEAR each declare their OWN `u32
 * value`, so using both inlines allocates eight bytes where the ROM allocates
 * four (`sub sp, #0x8` against `sub sp, #0x4`) and recomputes the source
 * address at each site. One local plus two DMA3_SET calls is the shape:
 *
 *     u32 value;  register u32 *v __asm__("r5");
 *     v = &value;  *v = 0x11111111;  DMA3_SET(v, q, 0x85000080);
 *     ...          *v = 0;           DMA3_SET(v, p, 0x85000007);
 *
 * THREE THINGS ABOUT THAT LOCAL, each measured:
 *
 *  - IT IS ASSIGNED LATE. `v = &value;` at the top of the function emits
 *    `mov r5, sp` before the first allocation; the ROM emits it immediately
 *    before the first store. Source position decides, as it usually does at
 *    this optimisation level.
 *  - IT MUST BE PINNED. Unpinned, gcc keeps `v` in r5 for the DMA argument but
 *    still writes the store as `str r3, [sp]`, folding the address it can prove
 *    equals the frame pointer. Pinning to r5 forces the store through the
 *    register, which is the ROM's `str r3, [r5, #0x0]`. Both stores, four lines.
 *  - A BARRIER ON IT IS WRONG HERE. It was tried and cost five lines, moving
 *    the assignment back up. The pin is the tool; the barrier is not.
 *
 * THE BLEND REGISTERS need named locals. Written as `REG_BLDALPHA = 0x10;` the
 * literal goes to the pool through the halfword store -- the store-width
 * pooling entry in docs/elevation.md -- where the ROM has `mov r2, #0x10`.
 * Naming all three also puts the constant in r2 and the walking address in r3,
 * which is the ROM's assignment; unnamed, the two swap.
 *
 * The task priority is pinned and shifted as its own statement so the callee's
 * pool load lands AFTER the shift rather than between the mov and the shift.
 */
#include "dma.h"

extern unsigned char *galloc_ewram(int tag, int size);
extern unsigned char *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern void UploadSpriteGFX(int a, int n, void *p);
extern void StartTask(void (*f)(void), int prio);
extern void Func_80912b8(void);
extern unsigned char gState[];
extern int GetFieldActor(int actorID);

void Func_8091494(int arg)
{
    int a;
    unsigned char *p;
    unsigned char *q;
    unsigned char *g;
    u32 value;
    register u32 *v __asm__("r5");
    int c0, c1, c2;
    register int pr __asm__("r1");

    a = arg;
    p = galloc_ewram(0x24, 0x1c);
    q = galloc_iwram(0xe, 0x80 << 3);
    v = &value;
    *v = 0x11111111;
    DMA3_SET(v, q, 0x85000080);
    UploadSpriteGFX(0x5e, 0x80 << 2, q);
    gfree(0xe);
    pr = 0xc8;
    pr <<= 4;
    StartTask(Func_80912b8, pr);
    c0 = 0x3f9e;
    REG_BLDCNT = c0;
    c1 = 0x10;
    REG_BLDALPHA = c1;
    c2 = 0x1f;
    REG_BLDY = c2;
    *v = 0;
    DMA3_SET(v, p, 0x85000007);
    if (a == 0) {
        g = gState;
        a = GetFieldActor(*(int *)(g + (0xfa << 1)));
    }
    *(int *)(p + 0x18) = a;
}
