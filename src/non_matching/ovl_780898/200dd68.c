/* OvlFunc_883_200dd68 (0x0200dd68) -- NON-MATCHING, still PARKED.  BODY UNCHANGED.
 *
 *   109 of 133 encodings.  Re-derived in batch 345 and UNCHANGED.
 *   INSTRUCTIONS ref 123, ours 124 -- GAP +1 (ours LONGER).
 *   SIZE ref 284, ours 288.  Stream 133 / 135.  Pool words 9 / 9.
 *   RELOCATIONS the same symbols at a shifted offset -- a consequence of the
 *   length difference, not a separate blocker (batch 322 classification).
 *   ALIGNED residue 122 of 133, 91.7%, 17 differing in 10 hunks.  THIS BODY IS
 *   THE BEST-ALIGNED ONE MEASURED; everything tried in batch 345 that lowered
 *   the raw figure raised the aligned residue.  Batch 345 brief E.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_780898/200dd68.c \
 *     asm/overlays/rom_780898/ovl_30_c_c_c_c_c_c_c_c_c_b.s --func OvlFunc_883_200dd68
 *
 * PINS: 0.  DEVICES: 0.  FAKEMATCH: no.  SPLIT: none (1 function in the .s).
 *
 * ===== BATCH 319: THIS BODY IS THE BETTER HALF OF A DUPLICATE PAIR =====
 * tools/dupfuncs.py pairs OvlFunc_883_200dd68 with OvlFunc_881_200c058
 * (ovl_77a7c8/200c058.c), which sits at 111 against this body's 109.
 * IF THIS ONE LANDS, THE PORT IS OWED ON THE TWIN -- and it is not three
 * renames: the two bodies reference different numbers of data labels, so the
 * label and pool order must be mapped by hand against both references, then
 * MEASURED.
 *
 * ===== THE WHOLE REMAINING RESIDUE, and the ONE extra instruction =====
 *
 *     rom   adds r3,r0,#0 / adds r3,#85 / movs r2,#0 / ldr r1,[pc,#16] @ 0 /
 *           strb r2,[r3] / adds r3,#15 / strh r2,[r3]              (7)
 *     ours  adds r1,r0,#0 / adds r1,#85 / movs r2,#0 / *adds r3,r0,#0* /
 *           strb r2,[r1] / ldrh r1,.L11 @ 0 / adds r3,#100 / strh r2,[r3]  (8)
 *
 * The ROM CHAINS ONE address register -- `add r3,#0x55` for `a->f55 = 0` and
 * then `add r3,#0xf` for `a->f64 = 0` -- where gcc builds two bases.  Neither
 * offset fits a store immediate (strb reaches 31, strh reaches 62), so both
 * need a computed base; the ROM's second one is reload_cse_move2add rewriting
 * `add rX, a, #100` into `add rX, #15` because it lands in THE SAME HARD
 * REGISTER that already holds a+85.  Ours lands in a different register and
 * move2add has nothing to track.  Plus one `.short 0x0000` pool pad (a
 * consequence of the odd count), `strh r3,[r5,#8]` one slot off, and the two
 * branch displacements.  All 7 relocations are the right symbols.
 *
 * ===== BATCH 345: THE PARK'S "NEXT" IS NOW A SHARP QUESTION =====
 *
 * The old NEXT read: "a spelling that gets the chain without the extra in-loop
 * reference.  On the arithmetic that means the walk must not touch the pooled
 * zero at depth."  That is now LOCATED, and the mechanism is not the walk:
 *
 *   * THE POINTER-WALK SPELLING DOES PRODUCE THE ROM'S CHAIN.  Confirmed:
 *     `q = (unsigned char *)a + 0x55; *q = 0; q += 15;
 *     *(unsigned short *)q = 0;` emits `adds r2,#85 / strb / adds r2,#15 /
 *     strh`.  Figure 121, gap STILL +1, aligned 108 (81.2%).
 *   * WHAT THE WALK COSTS IS NOT THE WALK.  It makes the HALFWORD store's zero
 *     unify with the POOLED zero (the `ldr rN,[pc] @ 0` the ROM also has),
 *     giving that value a third in-loop reference.  REG_N_REFS is loop-depth
 *     weighted (flow.c:5114-5116), so one extra reference at depth is worth
 *     two, and `allocno_compare` (global.c:600-618) then puts the zero above
 *     the loop index: `i` goes to r8 and the zero to r7, the REVERSE of the
 *     ROM.  Cost: `mov r1,r8 / lsls r3,r1,#2` for `arr[i]` and a four-insn
 *     loop increment `movs r3,#1 / add r8,r3 / mov r1,r8 / cmp r1,#1` against
 *     the ROM's `adds r7,#1 / cmp r7,#1`.
 *   * AND THAT HALF IS SOLVED.  Giving the halfword store its OWN named
 *     `unsigned short hz = 0` breaks the unification: `i` goes back to r7, the
 *     pooled zero back to r8, the chain is produced, and the figure is 108 of
 *     133 -- the lowest device-free figure measured.  THE REMAINING COST IS ONE
 *     DUPLICATED `movs r3,#0`, because `hz` is a second zero pseudo where the
 *     ROM shares one immediate zero between the byte and halfword stores.
 *
 * SO THE QUESTION IS NOW EXACTLY: one zero pseudo serving BOTH walk stores that
 * is NOT the pooled one.  Every spelling that names a single zero for both is
 * POISON -- `int z`, `unsigned short hz`, `unsigned char hz`, z set before the
 * chain, z set between the stores: ALL FIVE measure gap +5 (128 of 133).  This
 * reproduces and sharpens the park's own "naming the zero ... 139".
 *
 * *** AND 108 IS NOT AN IMPROVEMENT.  *** Aligned it is 120 of 133 (90.2%)
 * against this body's 122 (91.7%), so the raw drop 109 -> 108 is phase.  That is
 * why the body here is UNCHANGED.  Record the 108 as a measurement about the
 * blocker, not as a better body.
 *
 * ===== THE TWO ROUTES ARE MUTUALLY EXCLUSIVE, AND THE FORMULA PRICES IT =====
 *
 * Carried forward, and re-confirmed: it is NOT scheduling --
 * -fno-schedule-insns, -fno-schedule-insns2 and -fno-strength-reduce all still
 * emit two base copies.  `.17.lreg` arithmetic from the earlier round: the walk
 * gives the pooled zero one extra in-loop use, REG_N_REFS 6 -> 8, floor_log2
 * 2 -> 3, priority 12/92 = 0.130 -> 24/92 = 0.261, OVERTAKING the loop index at
 * 27/142 = 0.190.  Eleven walk spellings all held the zero at 8 refs.  Batch 345
 * adds the fix for exactly that term (the named halfword zero) and the new cost
 * it exposes.
 *
 * ===== MEASURED NEGATIVES =====
 *
 * AUTOSWEEP: both cheap axes closed.  Flag axis 8/8; anchor axis all 26
 * boundaries, best 107 at dsize +4 (MISALIGNMENT, not an improvement -- the
 * tool's own guard says so), 2 did not compile.
 *
 * *** THE -fno-schedule-insns2 ROW IS A TRAP AND IS NOW PRICED. ***  autosweep
 * reported "83 of 133" against 109 and flagged it.  Measured: gap still +1,
 * dsize 0 -- it only equalises the STREAM length (133/133) by absorbing the
 * extra instruction into a pool pad.  ALIGNED it is 102 of 133 (76.7%) against
 * this body's 122.  The figure fell 26 and the code got 20 worse.  Do not
 * re-chase it.
 *
 * Flag axis on the walk+hz body: -fno-strict-aliasing 114 at gap +6, -O1 127 at
 * gap -5, -fno-gcse 130 at gap -4, -ffixed-r7 139 at gap +8, everything else
 * exactly 108.
 *
 * Source probes, batch 345, all gap +1 unless stated:
 *   - walk off `&a->f55` / off `(unsigned char *)a + 0x55` / `q = q + 15`  121 each
 *   - one pointer local re-assigned to a+0x55 then a+0x64                 121
 *   - `*(unsigned short *)(q + 15) = 0` with no `q += 15`                  gap +2
 *   - `a->f64 = 0` BEFORE `a->f55 = 0`                                     121
 *   - byte store walked, halfword store left as the field                  109 (inert)
 *   - halfword store as `*(unsigned short *)((unsigned char *)a + 0x64)`   108
 *   - both stores as cast expressions with no local                        gap +2
 *   - the halfword store split into two byte stores     gap 0 BUT pool 9/8 and
 *     dsize -4: it DELETES the pool word the ROM has.  A wrong program-shape,
 *     not a landing.
 *   - `s = a->f50` after the walk / `a->f68 = c` before it                 121 each
 *   - loop as `i < 2` / `i` unsigned / a `short *` local for the halfword   121 each
 *   - a named zero for the two LATE byte stores only                       121 (inert)
 *
 * ===== TWO LEVERS THAT DID LAND, carried forward =====
 *   * THE TAIL WANTS `arr[k]->...` DIRECTLY, NOT A REUSED LOCAL.  Reassigning
 *     `a = arr[0]; ... a = arr[1];` makes `a` a global allocno gcc will not
 *     clobber, so `arr[1]->f23 = 2` costs `mov r2,r0 / adds r2,#35` where the
 *     ROM destroys r1 in place.  27 -> 17.
 *   * THE TWO TAIL BLOCKS WANT DIFFERENT STATEMENT ORDER.  Block 0 is f6c then
 *     f8_b; block 1 is f8_b, then f6c, then f23.  Both symmetric orderings
 *     measured 17 and 22; the ASYMMETRIC one is 13.  Do not assume two similar
 *     blocks want the same order.
 *
 * NEXT: one zero pseudo for both walk stores that is not the pooled zero.  Do
 * NOT re-screen the flag axis, the anchor axis, or the eleven walk spellings.
 */
struct SpriteSlot {
    unsigned short size;
    unsigned short vramOffset;
};

struct Spr {
    unsigned char pad0[5];
    unsigned char f5_a : 5;
    unsigned char f5_b : 1;
    unsigned char f5_c : 2;
    unsigned char pad6;
    unsigned char f7_a : 6;
    unsigned char f7_b : 2;
    unsigned short f8_a : 10;
    unsigned short f8_b : 2;
    unsigned short f8_c : 4;
    unsigned char pad_a[0x12];
    unsigned char f1c;
    unsigned char f1d;
    unsigned char pad1e[8];
    unsigned char f26;
    unsigned char pad27;
    unsigned char *f28;
};

struct Actor {
    unsigned char pad0[8];
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0xb];
    unsigned char f23;
    unsigned char pad24[0x2c];
    struct Spr *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[0xe];
    unsigned short f64;
    unsigned char pad66[2];
    struct Actor *f68;
    void *f6c;
};

extern struct SpriteSlot gSpriteSlots[];
extern unsigned char *iwram_3001f30;

extern struct Actor *__CreateActor(int kind, int x, int y, int z);
extern void __Sprite_SetAnim(struct Spr *s, int n);
extern void __Func_8003f3c(int n);
extern void OvlFunc_883_200dd14(void);
extern void OvlFunc_883_200dcc4(void);

void OvlFunc_883_200dd68(struct Actor *c)
{
    struct Actor *arr[2];
    struct Actor *a;
    struct Spr *s;
    unsigned char *w;
    int i;

    w = iwram_3001f30;
    for (i = 0; i <= 1; i++) {
        a = __CreateActor(0x1a, c->f8, c->fc, c->f10);
        arr[i] = a;
        if (a != 0) {
            a->f14 = c->f14;
            s = a->f50;
            a->f55 = 0;
            a->f64 = 0;
            a->f68 = c;
            if (s != 0) {
                __Sprite_SetAnim(s, 0);
                s->f26 = 0;
                __Func_8003f3c(s->f1c);
                s->f1c = *(unsigned short *)(w + 0x46);
                s->f1d |= 1;
                s->f8_a = gSpriteSlots[s->f1c].vramOffset >> 5;
                s->f5_b = 0;
                s->f5_c = 1;
                s->f7_b = 2;
                *(s->f28 + 0x16) = 0;
            }
        }
    }
    arr[0]->f6c = OvlFunc_883_200dd14;
    arr[0]->f50->f8_b = 1;
    arr[1]->f50->f8_b = 1;
    arr[1]->f6c = OvlFunc_883_200dcc4;
    arr[1]->f23 = 2;
}
