/* Func_801a66c (0x0801a66c) -- NON-MATCHING.
 * Blocker class: allocation ORDER -- gcc assigns callee-saved registers upward
 * from r4, the original build downward from r7.
 *
 * 128 lines against the ROM's 125, and 105 of the 128 differ, but almost all of
 * the 105 are one rotation repeated. Both push {r5, r6, r7, lr} and both use
 * exactly three long-lived values; they are assigned to opposite ends:
 *
 *              the object pointer   the shared zero   the gfx pointer
 *     rom      r7                   r5                r6
 *     ours     r6                   r7                r5
 *
 * The ROM materialises them in the order p, z and gives r7 then r5; gcc gives
 * r6 then r7. Nothing in the source selects this, and declaration order does
 * not reach it -- the pseudos are created in use order either way.
 *
 * This is the REG_ALLOC_ORDER hypothesis in HANDOFF.md, and it is now the third
 * clean specimen alongside rom_f6000/80f6148.c (no calls at all, three values,
 * r0/r4/r5 against r5/r6/r7) and rom_9000/800bbc0.c.
 *
 * MEASURED (rom 125 lines):
 *   baseline                                              128, 105  <- best
 *   `n = 0x3e7;` named so the halfword store uses a
 *     derived `add r3, #0x2f` rather than a pooled load   127, 118 (one line
 *                         nearer, eleven lines further -- the register rotation
 *                         swallows the gain)
 *   a separate named zero for the loop's stores           128, 105 (inert)
 *   `b = p + 0x174; *(short *)(b + 2) = z;` to keep the
 *     +2 in the store rather than folded into the base    128, 105 (inert)
 *
 * THE INERT RESULTS ARE THE INFORMATIVE ONES. Both of those levers are recorded
 * and both fire elsewhere; here gcc ALREADY produces the ROM's form without
 * them. Applying a lever that is not needed costs nothing but proves the
 * residue is not where it looked.
 *
 * WHAT IS RIGHT, and is a lot of this function:
 *
 *   THE OFFSET CHAIN COMES FREE. The ROM walks a running offset register --
 *   `mov r2, #0xd2 / lsl r2, #2` for 0x348, then `add r2, #4`, `add r2, #4`,
 *   `add r2, #0x4a`, `add r2, #2` -- and gcc reproduces every step from plainly
 *   written absolute offsets. Do NOT try to spell the chain.
 *
 *   SO DOES THE DERIVED CONSTANT: `*(short *)(p + 0x3b8) = 0x3e7;` gives the
 *   ROM's `add r3, #0x2f` off the 0x3b8 already in r3 when it is written as a
 *   literal, and naming it is what BREAKS that.
 *
 *   The five-iteration do/while over two parallel 0x34-stride pointers, the
 *   bitfield block at +0x300 (a byte with fields at bits 0-1, 2-3, 4, 5, 6-7
 *   assigned in the ROM's mask order), and `UploadSpriteGFX` reading its slot
 *   argument back out of the halfword it was just stored to.
 *
 * NEXT: nothing source-level. Re-screen if gcc is ever rebuilt with
 * REG_ALLOC_ORDER starting high.
 */
struct Obj {
    unsigned char pad0[5];
    unsigned char f5a : 2, f5b : 2, f5c : 1, f5d : 1, f5e : 2;
    unsigned char pad6;
    unsigned char f7a : 1, f7b : 5, f7c : 2;
    unsigned char pad8;
    unsigned char f9a : 2, f9b : 2, f9c : 4;
};

extern unsigned char *galloc_ewram(int tag, int size);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int size, void *gfx);
extern unsigned char Data_346f8[];

void Func_801a66c(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *r;
    struct Obj *s;
    void *gfx;
    int i;
    int z;

    p = galloc_ewram(0x12, 0xf9 << 2);
    z = 0;
    *(int *)(p + 0x348) = z;
    *(int *)(p + 0x34c) = z;
    *(int *)(p + 0x350) = z;
    *(short *)(p + 0x39a) = z;
    *(short *)(p + 0x39c) = z;
    *(short *)(p + 0x39e) = 0x80;
    *(short *)(p + 0x3a0) = 0x20;
    *(short *)(p + 0x394) = z;
    *(short *)(p + 0x3b8) = 0x3e7;
    q = p + 0x1de;
    r = p + 0x72;
    i = 0;
    do {
        i++;
        *(short *)r = 0;
        *(short *)q = 0;
        r += 0x34;
        q += 0x34;
    } while (i != 5);
    z = 0;
    *(short *)(p + 0x176) = z;
    *(short *)(p + 0x1aa) = z;
    *(short *)(p + 0xa) = z;
    *(short *)(p + 0x3e) = z;
    *(short *)(p + 0x12) = z;
    *(short *)(p + 0x46) = z;
    gfx = Data_346f8;
    *(short *)(p + 0x2e4) = AllocSpriteSlot();
    *(short *)(p + 0x2e6) = UploadSpriteGFX(*(unsigned short *)(p + 0x2e4), 0x80 << 1, gfx);
    *(short *)(p + 0x2e2) = z;
    *(short *)(p + 0x2fa) = z;
    *(short *)(p + 0x316) = z;
    s = (struct Obj *)(p + 0x300);
    s->f5b = 0;
    s->f5c = 0;
    s->f5d = 1;
    s->f5a = 0;
    s->f5e = 0;
    s->f7b = 0;
    s->f7c = 1;
    s->f9b = 0;
}
