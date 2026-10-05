/* OvlFunc_924_200d158 -- 0x0200d158, asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a.s
 *
 * NON-MATCHING, 7 differing encodings of 40.  SIZE EXACT and INSTRUCTION COUNT
 * EXACT (40 = 40), relocations identical -- a TRUE distance.  Figure re-derived
 * batch 327; the batch-319 claim of 7 SURVIVES unchanged.  ZERO pins, no
 * devices, no per-file flag group.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7ac2d8/200d158.c asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a.s --func OvlFunc_924_200d158
 *   XX ENCODINGS differ in 7 place(s) (ref 40, ours 40)
 *      first at index 12: ref 1c2b  ours 1c2a
 *
 * SPLIT SHAPE.  TWO functions in the reference (OvlFunc_924_200cfcc FIRST, then
 * this one), so this one needs a TEXT split with no _b part.  tools/datacheck.py
 * prints nothing (no data section).  EXPORT LIST: EMPTY.
 *
 * *** BUT SEE "THE WHOLE PIECE IS THE RIGHT UNIT" BELOW -- the other function in
 * *** this .s is ALSO a park, and solving both means NO split at all.
 *
 * WHAT IT DOES: spawns actor kind 0x18 at the caller's coordinates, gives it a
 * script, sets three bytes on it, and if it has a sprite host sets that host's
 * animation and two more bytes.
 *
 * THREE LEVERS GOT THIS FROM 29 TO 7, and all three are about byte stores whose
 * offsets exceed Thumb's `strb` immediate range (31), so each needs its own
 * address register.  THEY ALL STILL STAND:
 *   1. TWO INDEPENDENT BASES, NOT ONE CHAINED.  Plain struct stores make gcc
 *      compute &f55 and DERIVE &f22 from it with `sub r2, #51`; the ROM computes
 *      both from the actor.  Naming a pointer for the f22/f23 pair splits them.
 *   2. THE PAIR IS A WALKING POINTER.  `ac->f23 = 2` gives `strb r3, [r2, #1]`;
 *      the ROM has `add r2, #1 / strb r3, [r2]`.
 *   3. THE ZERO IS SHARED.  The ROM keeps 0 in r7 across the body and uses it
 *      for both `f55 = 0` and the sprite host's `f26 = 0`.  A named local does
 *      it; two literals do not (18 differing).
 *
 * ================= BATCH 327: THE RESIDUE IS NOW CLOSED-FORM =================
 *
 * The 7 are ONE RUN, encodings 12-18:
 *
 *   ROM                       this body
 *   mov  r3, r5   p (&f55)    mov  r2, r5    q (&f22)
 *   add  r3, #0x55            mov  r1, r5    p (&f55)
 *   mov  r7, #0   z           add  r2, #34
 *   mov  r2, r5   q (&f22)    mov  r3, #1
 *   strb r7, [r3] *p = z      mov  r7, #0
 *   add  r2, #0x22            add  r1, #85
 *   mov  r3, #1               strb r7, [r1]
 *   strb r3, [r2]             strb r3, [r2]
 *   (the final four encodings are identical in both)
 *
 * THE PARK PREVIOUSLY BLAMED sched2 ("sched2 placing an independent `mov` early,
 * the same shape as the recorded Func_942e0 residue").  THAT IS BACKWARDS.
 * sched2 runs AFTER reload, so in the ROM the hoist of `mov r3, #1` above the
 * f55 store is IMPOSSIBLE -- r3 is p's register there and the hard-register
 * dependence forbids it.  The ALLOCATION forbids the hoist; the schedule is
 * downstream.  Attack the allocation, not the schedule.
 *
 * ALL FOUR COMPETITORS ARE BLOCK-LOCAL, so local-alloc decides, and
 * REG_ALLOC_ORDER (config/arm/arm.h:989) begins 3,2,1,0 -- find_free_reg hands
 * r3 to whichever qty is allocated FIRST.  The order is
 *     QTY_CMP_PRI = floor_log2(n_refs) * n_refs * size / (death - birth)
 * and for this body:
 *     const 1 : n_refs 2, span 1  -> 20000   <- takes r3
 *     const 2 : n_refs 2, span 1  -> 20000   <- takes r3 too (they do not conflict)
 *     q       : n_refs 5, span 6  -> 16666   -> r2
 *     p       : n_refs 2, span 3  ->  6666   -> r1 / r2
 * The ROM needs **p** to hold r3 and the two constants to RE-USE r3 after p
 * dies.  For that p must reach 20000, i.e. **span 1** -- its address insn
 * immediately followed by its store.  (At 20000 it TIES the constants and wins
 * on qty number, which is lower because p is born earlier in the scan.)
 *
 * AND HERE IS THE TENSION, WHICH IS THE REAL REMAINING CAUSE:
 *
 *   GIVING p SPAN 1 LETS gcc CHAIN q OFF p, WHICH IS LEVER 1 FAILING.
 *   `z = 0; p = &ac->f55; *p = z; q = &ac->f22; ...` emits
 *       mov r2,r5 / add r2,#85 / mov r7,#0 / strb r7,[r2] / mov r3,#1 / sub r2,#51
 *   -- 39 instructions against 40, 29 differing.
 *
 * So p needs span 1 to win r3, and q needs to be materialised BEFORE p's store
 * to stay independent, which forces p's span to 2 or more.  The 7 is a genuine
 * LOCAL OPTIMUM between two requirements that currently exclude each other, not
 * an untried lever.  A construct that materialises q early WITHOUT extending p's
 * span past its store is what would close this.
 *
 * MEASURED INERT at 7 of 40, batch 327 -- naming BOTH pointers with the f55 one
 * first, in four statement orders (p,z,q / z,p,q / p,q,z and z,q,p).  Three of
 * them move the first differing encoding from `1c2a` to `1c29` (p lands in r1
 * rather than r2) at the same count: A DIFFERENT WRONG ANSWER AT THE SAME
 * DISTANCE, not progress.
 * MEASURED WORSE, 39 instructions of 40 and 29 differing: every form that puts
 * `*p = z` adjacent to `p = &ac->f55` (four tried, including a `q[0]`/`q[1]`
 * subscript form) -- all of them re-derive q from p.
 * CORRECTION: the old header said the residue register is "r3 in the ROM, r1
 * here".  The installed body measures **r2** here; r1 appears only once both
 * pointers are named.
 *
 * ================= THE WHOLE PIECE IS THE RIGHT UNIT =================
 *
 * tools/dupfuncs.py pairs this function with OvlFunc_923_2009bc8 in
 * asm/overlays/rom_7aa430/ovl_1a3c_a_a_a.s -- and its TU-mate
 * OvlFunc_924_200cfcc pairs with OvlFunc_923_2009a3c in THE SAME foreign piece.
 * Both pieces are 244 lines, hold exactly two functions, in the same order.  So
 * `ovl_1a3c_a_a_a.s` is a wholesale copy of `ovl_35b8_a_a_c_c_a.s`:
 *   - solving BOTH functions in ONE .c gives a whole-piece match with NO split;
 *   - the same .c with symbol renames covers the foreign piece as well;
 *   - that is FOUR functions from one translation unit.
 * (`ovl_1a3c` copies more of `ovl_35b8` than this: 200d244 = 2009cb4 and
 * 200d5c0 = 200a030 pair the same way, and both are parked here too.)
 */
#include "gba/types.h"

struct SpriteHost {
    u8 pad_00[9];
    u8 f9;
    u8 pad_0a[0x1c];
    u8 f26;
};

struct Actor {
    u8 pad_00[8];
    int f8;
    int fc;
    int f10;
    u8 pad_14[0x22 - 0x14];
    u8 f22;
    u8 f23;
    u8 pad_24[0x50 - 0x24];
    struct SpriteHost *f50;
    u8 pad_54;
    u8 f55;
};

extern unsigned char gScript_924__0200de08[];
extern struct Actor *__CreateActor(int kind, int x, int y, int z);
extern void __Actor_SetScript(struct Actor *a, unsigned char *s);
extern void __Sprite_SetAnim(struct SpriteHost *h, int n);

void OvlFunc_924_200d158(struct Actor *src)
{
    struct Actor *ac;
    struct SpriteHost *h;
    u8 *p;
    u8 *q;
    int z;

    ac = __CreateActor(0x18, src->f8, src->fc, src->f10);
    if (ac != NULL) {
        h = ac->f50;
        __Actor_SetScript(ac, gScript_924__0200de08);
        p = &ac->f22;
        z = 0;
        ac->f55 = z;
        *p = 1;
        p++;
        *p = 2;
        if (h != NULL) {
            __Sprite_SetAnim(h, 2);
            h->f26 = z;
            h->f9 |= 0xc;
        }
    }
}
