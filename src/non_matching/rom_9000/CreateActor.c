/* CreateActor (SpawnEntity) @ 0x0800c150 -- NON-MATCHING.
 *
 * NON-MATCHING: 86 encodings of 179 differ (objcmp).
 * Reference 179 encodings; ours 180.  THE SECTION SIZE MATCHES (392 bytes, no
 * SIZE line) and ALL TEN RELOCATIONS ARE THE SAME SYMBOLS IN THE SAME ORDER --
 * only their offsets drift, by 2 bytes, from the fifth one on.
 * First divergence at index 16: `ldr r2, =0xfff` (ROM) vs `ldr r1, =0xfff`.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this> \
 *     asm/rom_9000/rom_c004_c_a_a_a_a_a_a_c.s --whole
 *
 * rom_c004_c_a_a_a_a_a_a_c.s HOLDS ONLY THIS FUNCTION AND NO DATA SECTION
 * (datacheck.py), so landing it is a pure whole-file text conversion, no split.
 * Note the reference keeps THREE literal pools INSIDE the function body
 * (.Lc1c4/.Lc1c8, .Lc28c and a trailing `.pool`); `make compare` is the gate.
 *
 * WHAT IS EXACT, instruction for instruction: the prologue and `sub sp, #8`; the
 * TWO calls to NewActor with the first result discarded (that really is in the
 * ROM -- both `bl`s resolve to 0x0800c0cc, checked against baserom.gba); the
 * signed `desc / 0x1000` with its `ldr =0xfff / add` rounding correction and the
 * `desc &= 0xfff` that REUSES desc's register; the three-way dispatch as a
 * SWITCH (`beq case0 / cmp #2 / beq case2 / b join`, not an if-else chain); the
 * whole kind-0 arm; the bump allocator (`np = base + 0x18` as a NAMED POINTER
 * read and written twice -- inline it and you get `ldr r3,[r1,#0x18]` twice);
 * `DMA3_CLEAR(slot, 0x10)` from include/dma.h; both CreateSprite/_GetSpriteInfo
 * pairs; Actor_SetPos; and the default block's thirteen stores in order.
 *
 * LOAD-BEARING SPELLINGS ALREADY FOUND:
 *   - `v = 0x10; *(u16 *)(e + 0x20) = v;` -- an int carrier.  Written as a bare
 *     `= 0x10` gcc emits `ldr r3, =0x10` from the pool, because a HImode
 *     constant store cannot be chained by reload_cse_move2add.
 *   - `desc &= 0xfff` rather than a separate `id` local: puts the id back in
 *     r7, frees the register that otherwise forces `z` onto the stack, and takes
 *     the frame from 0x0c to the ROM's 0x08.
 *   - `mp = e + 0x55` as a named pointer (the ROM parks it in r12 across six
 *     stores): 112 -> 98 differing.
 *   - `v = x / 0x10000; *(short *)(e + 0x64) = v;` split into two statements, so
 *     the address is computed AFTER the division's sign-correction branch rather
 *     than hoisted above it: 98 -> 93.
 *   - `*(u8 **)(e + 0x50) = slot;` placed AFTER `DMA3_CLEAR`, not before:
 *     134 -> 86.  (Counter-intuitive, and the ROM's own order is the other way
 *     -- see the blocker.)
 *
 * BLOCKER: A HANDFUL OF sched2 TIES AMONG INDEPENDENT STORES, plus one
 * cse-reuse of a zero.  Concretely, all that is left:
 *
 *   1. `e[0x54] = 0` in the kind-0 else arm.  The ROM materialises a POOLED zero
 *      (`ldr r3, =0x0`, four instructions with the address); gcc knows `spr == 0`
 *      on that edge and reuses spr's register (`strb r5`), three instructions.
 *      PASS: inverting the test to `if (spr == 0)` (97), assigning through a
 *      named int first (86, inert), storing through a block-local `unsigned char
 *      *q = e + 0x54` (86, inert).
 *   2. `*(u8 **)(e + 0x50) = slot`.  The ROM emits it BETWEEN DMA3_CLEAR's
 *      `*_src = 0` and the `stmia`, from the low-register copy of `slot`
 *      (`str r4,[r6,#0x50]`); that needs the store BEFORE DMA3_CLEAR in the
 *      source plus a sched2 swap of the two independent stores, which does not
 *      happen.  PASS, all with the store before DMA3_CLEAR: `unsigned char **`
 *      (134), `void **` (134), `*(int *)... = (int)slot` (134),
 *      `unsigned char * volatile *` (134), recomputing `8 + arr` (136), and a
 *      separate `cur = slot` local for the live-range split (132).
 *   3. `strh r1, [r6, #4]` comes FIRST of the four default-block stores in the
 *      ROM and LAST in ours, though the source order is the ROM's.
 *   4. `str r1, [r6, #0x4c]` before vs after `strh r2, [r6, #6]`; the `asr`
 *      before vs after the second address computation.
 *
 * -fno-schedule-insns2 IS NOT THE ANSWER: 100 differing at 180 instructions,
 * WORSE than the 86 here.  So this is not the file-mate NewActor's blocker
 * (src/rom_9000/rom_c004_c_a_a_a_a_a_a_a.c, which does need that flag) and no
 * Makefile row is warranted.
 *
 * ALSO MEASURED AND WORSE OR INERT: `arr = base + n * 4` vs `base += n * 4`
 * (136 vs 135 -- the ROM accumulates into base's register, `add r1, r2`, which
 * the version here reproduces); `e[0x54] = kind` moved after the p50 store
 * (135); the halfword-pair spelling of the p50 store (137 at 182 instructions);
 * a separate `zero` local instead of reusing `v` (132); moving `mp = e + 0x55`
 * one statement later (93, inert); `*(int *)(e + 0x4c)` moved before `e[0x5a]`
 * (92).
 *
 * NEXT: the sched2 tie-breaks.  The recorded lever for these is ALIAS SETS
 * (store_bit_field takes the base's alias set, expr.c:5008) -- four spellings of
 * the p50 store's pointer type were tried and all four were identical, so the
 * next thing to try is a union-typed base rather than a differently-spelled
 * pointer cast.
 */
#include "dma.h"

extern unsigned char *NewActor(void);
extern void *CreateSprite(int id);
extern unsigned char *_GetSpriteInfo(int id);
extern void Actor_SetPos(unsigned char *e, int x, int y, int z);
extern unsigned char *iwram_3001e68;
extern unsigned char L1358c[] __asm__(".L1358c");

unsigned char *CreateActor(int desc, int x, int y, int z)
{
    unsigned char *e;
    unsigned char *base;
    unsigned char *arr;
    unsigned char *slot;
    int *np;
    unsigned char *mp;
    void *spr;
    int kind;
    int n;
    int v;

    NewActor();
    kind = desc / 0x1000;
    desc &= 0xfff;
    e = NewActor();
    if (e != 0) {
        v = 0x10;
        *(unsigned short *)(e + 0x20) = v;
        switch (kind) {
        case 0:
            spr = CreateSprite(desc);
            if (spr != 0) {
                e[0x54] = 1;
                *(void **)(e + 0x50) = spr;
                *(unsigned short *)(e + 0x20) = _GetSpriteInfo(desc)[9] >> 1;
            } else {
                e[0x54] = 0;
            }
            break;
        case 2:
            base = iwram_3001e68;
            np = (int *)(base + 0x18);
            n = *np;
            arr = base + n * 4;
            *np = n + 1;
            slot = 8 + arr;
            e[0x54] = kind;
            *(unsigned char **)(e + 0x50) = slot;
            DMA3_CLEAR(slot, 0x10);
            spr = CreateSprite(desc);
            if (spr != 0) {
                *(unsigned short *)(e + 0x20) = _GetSpriteInfo(desc)[9] >> 1;
                *(void **)slot = spr;
                slot = 0xc + arr;
            }
            spr = CreateSprite(desc + 1);
            if (spr != 0)
                *(void **)slot = spr;
            break;
        }
    }
    if (e != 0) {
        Actor_SetPos(e, x, y, z);
        *(unsigned char **)e = L1358c;
        *(int *)(e + 0x30) = 0x20000;
        mp = e + 0x55;
        v = 0;
        *(unsigned short *)(e + 4) = v;
        *(int *)(e + 0x18) = 0x10000;
        *(int *)(e + 0x1c) = 0x10000;
        *(int *)(e + 0x34) = 0x10000;
        *mp = 3;
        *(int *)(e + 0x48) = 0x10000;
        *(int *)(e + 0x44) = 0x4000;
        e[0x59] = 0;
        e[0x5a] = 1;
        *(int *)(e + 0x4c) = v;
        *(unsigned short *)(e + 6) = 0x4000;
        v = x / 0x10000;
        *(short *)(e + 0x64) = v;
        v = z / 0x10000;
        *(short *)(e + 0x66) = v;
    }
    return e;
}
