/* Func_800c880 (0x0800c880) -- NON-MATCHING: 94 encodings of 192 differ (objcmp).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_9000/800c880.c asm/rom_9000/rom_c880.s --func Func_800c880
 *
 * SIZE 428 == 428, INSTRUCTIONS 192 == 192.  94 IS A TRUE DISTANCE.
 * RELOCATIONS: the twelve calls and four data words are identical in symbol and
 * order; the ONLY relocation difference is the extra R_ARM_ABS32 to `_LEN_2c4`,
 * which is the .sym proposal below.  tryc --full --align: 96 lines dirty of 193.
 *
 * THE SPLIT this needs: the .s holds ONE function plus a .rodata tail (.L13190,
 * one .incrom blob).  `.L13190` is NOT currently `.global` -- that is why
 * datacheck printed no EXPORTS line for this file -- so the split must add
 * `.global .L13190` IMMEDIATELY BEFORE THE LABEL (split_s.py refuses a preamble
 * holding more than includes).  It emits no bytes.  The function reads that one
 * label and nothing else.
 *
 * ================== .sym PROPOSAL: the DMA size is a SYMBOL ==================
 * This is the single biggest lever on this function and it completes an
 * instruction the source cannot otherwise reach.
 *
 * The ROM keeps 0x2c4 in a REGISTER across two uses -- `mov r1, r5` for
 * galloc_iwram's size argument, then `lsr r5, #2 / orr r2, r5` for DMA3_COPY's
 * count.  Written as a literal (or as an `int` local holding a literal) cprop
 * folds both: `mov r1,#0xb1 / lsl r1,#2` for the call and a POOLED 0x840000b1
 * for the count, so the `orr` disappears entirely.  A symbol ADDRESS is opaque
 * to cprop and restores it.  Screened WITHOUT touching the tree, per
 * docs/elevation.md "Screening against a `.sym` addition", as
 * `extern unsigned char _LEN_2c4[]; size = (int)_LEN_2c4;`
 *
 *   MEASURED BOTH WAYS, single change, everything else identical:
 *     size = 0x2c4 (plain literal in an int local)   179 of 192, 187 instructions
 *     size = (int)_LEN_2c4                           133 of 192, 189 instructions
 *   and it is what lets the function reach 192 == 192 at all.
 *
 * Proposed: a row in const.sym (or a new length.sym) of the form
 *   _LEN_2c4 = 0x2c4
 * beside the admitted `_CONST_b` / `_CONST_2` / `_CONST_0`, and `ldr r5, =_LEN_2c4`
 * in the split .s so the reference assembles with the same relocation.  The value
 * is the byte length of the IWRAM blitter routine copied out of Func_8009bb8, so
 * a name like `_LEN_Func_8009bb8` may read better; the VALUE is what is
 * evidenced.  docs/elevation.md "A pooled constant may be a ROUTINE'S LENGTH --
 * check the neighbouring symbols" is the recorded form of this tell.
 * NOT YET ADDED to any .sym -- decision left to the owner, as the brief asks.
 *
 * ================== WHAT ELSE IS LOAD-BEARING (keep all of it) ==================
 * 1. THE FRAME.  ROM `sub sp, #0x34` with its highest used slot at 0x18 -- 24
 *    bytes of frame nothing touches, the same phantom slack ActorCmd_Player_Climb
 *    has (68 bytes there).  Reproduced by making the two 8-byte scale pairs
 *    members of ONE local aggregate with a 24-byte tail:
 *      struct { int sc2[2]; int sc1[2]; unsigned char pad[24]; } L;
 *    which puts sc2 at 0xc, sc1 at 0x14 and the pad at 0x1c..0x34, and gives
 *    `sub sp, #52`.  A bare unused `int pad[6]` is DELETED (probed separately:
 *    gcc-2.96 removes an unused stack array unless it is stored to, and storing
 *    costs instructions).  133 -> 98 of 192.
 *    BOUNDARY, and it is the next thing to fix: with sc1 and sc2 as SEPARATE
 *    objects and the pad hung off sc1
 *    (`struct { int v[2]; unsigned char pad[24]; } A; int sc2[2];`)
 *    the ADDRESSING is the ROM's exactly -- `add r6, sp, #0x14` and
 *    `add r4, sp, #0xc`, where the one-struct form gives a single base at
 *    sp+0xc with offsets 8/0xc -- but it costs 4 instructions elsewhere
 *    (128 of 188).  One of the two spellings is right and the other has a
 *    second, separate fault; the addressing evidence says the separate-objects
 *    one is the true shape.
 * 2. `e` as a WALK, `e = (unsigned char *)iwram_3001e64; e = e + 0x1b90;`.
 *    Written as one expression, cse folds `f = e + 0x18` into a second pooled
 *    constant 0x1ba8 and a three-operand add, where the ROM has
 *    `mov r7, r8 / add r7, #0x18`.  98 -> 94, and it is worth 1 instruction.
 *    The `"+r"` barrier on `e` adds nothing on top of the walk (94 either way),
 *    so THERE ARE NO SHIMS IN THIS DRAFT -- no pins, no barriers, no volatile.
 * 3. `b = &iwram_3001e80; base = b[0]; g = *(b - 6);` -- the adjacent-globals
 *    lever.  ONE pool word, the second global reached by `sub r3, #0x18` off the
 *    address register.
 * 4. `zero` as an `int` carrier for `*g = 0` (a bare 0 through a `short *` pools).
 * 5. `prio` named per call site (it is the fifth argument, `str r2,[sp]`, and the
 *    ROM materialises it FIRST at both sites).
 * 6. `ldmia r6!, {r5}` in the kind-2 inner loop is `spr = *q++` on a
 *    `unsigned char **`.
 * 7. `DMA3_COPY` from include/dma.h is the right one of the pair (the ROM's
 *    count is `0x84000000 | size/4` built at runtime); DMA3_SET would need the
 *    whole word as an argument.
 *
 * ================== THE RESIDUE ==================
 *  - the sc1/sc2 addressing choice above (the clearly-identified half);
 *  - the switch tree is missing the ROM's LOW-BOUND test: ROM
 *    `cmp r2,#1 / beq k1 / cmp r2,#1 / ble default / cmp r2,#2 / beq k2 / b default`,
 *    ours drops the second pair, i.e. gcc's node_has_low_bound returned true for
 *    us and false in the original build.  Measured and inert: a named `int mask`
 *    for the 0xf (135), the mask read inline in the switch head (128), `unsigned
 *    int kind` (128), an explicit `case 0: break;` (97 of 190 -- it changes the
 *    count, so it is not the shape either).
 *  - `pos` (kind 1's `e + 8`) is in r10 in the ROM, built as `mov r3,#8 / add
 *    r3,r8` CONSTANT-FIRST and before the sc1 stores; ours builds it later and
 *    in r7, with `f` taking r4 where the ROM has r7.
 *  - the rest is r1/r2 scratch rotation.
 *
 * NEXT: settle the sc1/sc2 aggregate question -- find what the separate-objects
 * form costs 4 instructions on, because that spelling has the addressing right.
 * Then read gcc's node_has_low_bound for the switch.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern int atan2(int a, int b);
extern void InitMatrixStack(void);
extern int _GetFlag(int id);
extern void MatrixLook(void *a, void *b);
extern void MatrixSetLook(void *a, void *b);
extern void Func_800b388(void *spr, int *pos, int *scale, int ang, int prio);
extern void Func_8009bb8(void);
extern void Func_8000a30(void *tbl);
extern unsigned int iwram_3001e80;
extern unsigned int iwram_3001e64;
extern short L13190[] __asm__(".L13190");
extern unsigned char _LEN_2c4[];

void Func_800c880(void)
{
    unsigned int *b;
    unsigned char *base;
    unsigned char *s;
    unsigned char *t;
    unsigned short *g;
    unsigned char *buf;
    unsigned char *e;
    unsigned char *f;
    unsigned char *spr;
    unsigned char **q;
    int size;
    int view;
    int n;
    int j;
    int one;
    int zero;
    int kind;
    int prio;
    int *pos;
    struct { int sc2[2]; int sc1[2]; unsigned char pad[24]; } L;
    void (*fp)(void *);

    b = &iwram_3001e80;
    base = (unsigned char *)b[0];
    g = (unsigned short *)*(b - 6);
    size = (int)_LEN_2c4;
    buf = galloc_iwram(0x34, size);
    DMA3_COPY(Func_8009bb8, buf, size);
    s = base;
    t = base + 0xc;
    if (*(int *)(base + 0x18) != 0)
        s = *(unsigned char **)(base + 0x18);
    if (*(int *)(base + 0x1c) != 0)
        t = *(unsigned char **)(base + 0x1c);
    view = (short)atan2((*(int *)s - *(int *)t) >> 16,
                        (*(int *)(s + 8) - *(int *)(t + 8)) >> 16);
    zero = 0;
    *g = zero;
    InitMatrixStack();
    if (_GetFlag(0x16b) != 0) {
        view = view - 0x2000;
        fp = Func_8000a30;
        fp(L13190);
        MatrixLook(s, t);
    } else {
        MatrixSetLook(s, t);
    }
    e = (unsigned char *)iwram_3001e64;
    e = e + 0x1b90;
    one = 0x80 << 9;
    n = 0x3f;
    f = e;
    f = f + 0x18;
    do {
        if (*(int *)e != 0) {
            kind = *(e + 0x54) & 0xf;
            switch (kind) {
            case 1:
                pos = (int *)(e + 8);
                L.sc1[0] = *(int *)f;
                L.sc1[1] = *(int *)(f + 4);
                spr = *(unsigned char **)(f + 0x38);
                if (_GetFlag(0x16b) != 0) {
                    L.sc1[0] = one;
                    L.sc1[1] = one;
                }
                prio = *(f + 0xa);
                Func_800b388(spr, pos, L.sc1,
                             *(unsigned short *)(e + 6) + view, prio);
                break;
            case 2:
                L.sc2[0] = *(int *)f;
                L.sc2[1] = *(int *)(f + 4);
                if (_GetFlag(0x16b) != 0) {
                    L.sc2[0] = one;
                    L.sc2[1] = one;
                }
                j = 3;
                q = *(unsigned char ***)(f + 0x38);
                do {
                    spr = *q++;
                    if (spr != 0) {
                        prio = *(f + 0xa);
                        Func_800b388(spr, (int *)(e + 8), L.sc2,
                                     *(unsigned short *)(e + 6) + view, prio);
                    }
                    j--;
                } while (j >= 0);
                break;
            }
        }
        n--;
        f -= 0x70;
        e -= 0x70;
    } while (n >= 0);
    gfree(0x34);
}
