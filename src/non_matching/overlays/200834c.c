/* OvlFunc_883_200834c  --  0x0200834c, asm/overlays/rom_780898/ovl_30_a_a_c_a.s
 *
 * Source asm: goldensun/asm/overlays/rom_780898/ovl_30_a_a_c_a.s
 *
 * THE LARGEST FAMILY IN THE TREE: THIRTEEN byte-identical copies, 139
 * instructions each, one per overlay. tools/find_twins.py has ranked it first
 * by payoff since the tool existed and this is the first attempt on it.
 *
 *   OvlFunc_883_200834c  OvlFunc_905_200834c  OvlFunc_913_200834c
 *   OvlFunc_914_200834c  OvlFunc_915_200834c  OvlFunc_923_2008630
 *   OvlFunc_924_2008630  OvlFunc_946_200834c  OvlFunc_948_200834c
 *   OvlFunc_957_200834c  OvlFunc_958_2008630  OvlFunc_959_200834c
 *   OvlFunc_964_200834c
 *
 * BLOCKER CLASS: instruction scheduling -- ONE LOAD, hoisted.
 * Status: 144 lines against 144, 28 differing, and all 28 are in one
 * twenty-instruction window that cascades from a single placement.
 *
 * FindPushableFacingPlayer. Outer loop over entity slots 8..0x41, inner loop
 * over the six pushable model ids. For a candidate whose model id matches, it
 * builds the footprint rectangle around the candidate's centre and tests
 * whether the tile one step ahead of the player falls inside it; index bit 0
 * selects the log's axis and rejects the case where the player stands at the
 * end of the log, so a log can only be pushed broadside.
 *
 * WHAT IS RIGHT, and it is nearly everything. Both streams are 144 lines. The
 * stack frame is the same size and every spill slot lands on the same offset
 * (sp+0 the model-id cursor, +4 the id, +8 the slot, +0xc/+0x10/+0x14 the three
 * out-pointers). r8-r11 hold the same values, the `ldmia r1!, {r3}` cursor over
 * the id table and the `add r0, #0x10` walk over the rectangles both fall out
 * of plain array indexing, every branch matches, and the epilogue is identical.
 *
 * WHAT IS WRONG IS ONE LOAD:
 *
 *     rom    ... asr r7, r1, #4      <- tx finished
 *            mov r1, r9 / ldr r1, [r1, #0x10]   <- THEN the player's z
 *     ours   ... mov r4, r9 / ldr r4, [r4, #0x10]  <- z loaded five slots early
 *            ... asr r7, r3, #4      <- tx finished after it
 *
 * gcc hoists the `pl->z` read above the whole tx computation. Because it does,
 * z is live across that block and needs its own register, so the two
 * `mov rLow, r9` copies that Thumb requires for a high-register base go to
 * DIFFERENT low registers where the ROM reuses r1 -- and every register name in
 * the window shifts with it. Nothing writes memory between the two reads, so
 * the scheduler is free to move it and there is no aliasing barrier to invoke.
 *
 * MEASURED. Source shapes, all against the same reference:
 *
 *   inline `(pl->x >> 16)` / `(pl->z >> 16)`            144 / 29
 *   `((short)s + (pl->z >> 16))`, operands swapped      144 / 31
 *   tx and tz split into add-then-shift statements      143 / 99
 *   `s` re-read and re-shifted per half (this file)     144 / 28   <- best
 *   `s = (s << 16) >> 16` between the two               144 / 29
 *   the model-id pointer named in a local               144 / 28
 *   `e->f50->f28[0]` instead of `*e->f50->f28`          144 / 28
 *   explicit walking pointers over both tables          149 / 133
 *
 * Flags, on the best shape:
 *
 *   -O2                          144 / 28   <- floor
 *   -fno-cse-follow-jumps        144 / 28     (also -fno-cse-skip-blocks,
 *   -fno-force-mem               144 / 28      -fno-caller-saves, -fno-peephole,
 *   -fno-peephole2               144 / 28      -fno-delayed-branch,
 *   -fno-function-cse            144 / 28      -fno-thread-jumps,
 *   -fno-optimize-sibling-calls  144 / 28      -fno-schedule-insns,
 *   -fno-gcse                    144 / 28      -fno-strict-aliasing)
 *   -fno-schedule-insns2         144 / 56   worse
 *   -fno-expensive-optimizations 144 / 72   worse
 *   -fno-rerun-cse-after-loop    145 / 122  worse
 *   -fno-strength-reduce         151 / 133  worse
 *   -O1                          150 / 138  worse
 *
 * Thirteen flags leave it exactly where it is, which is itself the finding:
 * this is not a pass that can be switched off, it is the scheduler's ordinary
 * ready-list choice between two independent instructions.
 *
 * WHAT WOULD MOVE IT is a reason for gcc not to hoist the second load -- a
 * store between the two reads that could alias it, or a call. The ROM has
 * neither, so if the original source had one it was optimised away, and no
 * spelling tried puts one back without adding an instruction.
 *
 * THE THREE TABLES ARE NAMED HERE BY ADDRESS, the convention from batch 80.
 * They exist in every one of the thirteen overlays at its own address and are
 * already `.global .L6190` / `.L61d0` / `.L61e8` in
 * asm/overlays/rom_780898/ovl_30_c_c_c_c_c_c_c_c_c.s; elevating this family
 * means renaming them per overlay. Contents, for the record:
 *
 *   gStep    16 words, packed (dz:dx) as two shorts, +/-16 by facing octant
 *   gModelId  6 words: 0xcf 0xcd 0xe4 0xe5 0x12a 0x129
 *   gBox      6 rectangles: (-32,-8,32,8) (-8,-32,8,32) (-32,-16,32,0)
 *             (-8,-32,8,32) (-32,-8,32,8) (-8,-32,8,32)
 *
 * RE-ATTEMPTED IN BATCH 91 with batch 89's finding that two assignments come out
 * in the OPPOSITE order to their source order. Writing `tz` before `tx` -- three
 * spellings, including one that loads the step word once and one that names both
 * player coordinates -- gives 122 to 125 differing of ~142, far worse than the
 * 28 below. So the source order really is tx then tz, and the hoist is the
 * scheduler rather than anything the assignment order reaches.
 *
 * ALSO: this cluster is SEVENTEEN functions by shape, not thirteen.
 * tools/find_shape.py --clusters (batch 88) ranks it second in the tree at 2304
 * payoff. find_twins.py sees only the thirteen that are byte-identical.
 *
 * NOTE FOR ANYONE RE-SCREENING THIS FILE: it reports 46 differing, not 28. The
 * extra eighteen are the three table symbols and the label renumbering behind
 * them -- tryc.py normalises the ROM's `.L6190` to `L<n>` because it looks like
 * a local label, and an address-named extern does not normalise to the same
 * token. Substituting `L6190`/`L61d0`/`L61e8` for the three names reproduces
 * the 28 quoted above. Do not read the difference as a regression.
 *
 * The C below is believed correct and is what should be used the moment the
 * scheduling question is answered. Thirteen functions come with it.
 */

struct Model { unsigned char pad00[0x28]; short *f28; };

struct Ent {
    unsigned char pad00[6];
    unsigned short facing;
    int x;
    unsigned char pad0c[4];
    int z;
    unsigned char pad14[0x3c];
    struct Model *f50;
};

struct Rect { int x0, z0, x1, z1; };

extern unsigned char iwram_3001ebc[];
extern int gStep_883__0200e190[];
extern int gModelId_883__0200e1d0[];
extern struct Rect gBox_883__0200e1e8[];
extern struct Ent *__MapActor_GetActor(int slot);

struct Ent *OvlFunc_883_200834c(int *facingOut, int *slotOut, int *modelOut)
{
    struct Ent **tbl;
    struct Ent *pl;
    struct Ent *e;
    unsigned int slot, i;
    int s, tx, tz, x0, z0, x1, z1, ex, ez, id;

    tbl = (struct Ent **)(*(char **)iwram_3001ebc + 0x14);
    pl = __MapActor_GetActor(0);
    *facingOut = pl->facing >> 12;
    for (slot = 8; slot <= 0x41; slot++) {
        e = tbl[slot];
        id = *e->f50->f28;
        for (i = 0; i <= 5; i++) {
            if (id != gModelId_883__0200e1d0[i])
                continue;
            *modelOut = i;
            s = gStep_883__0200e190[*facingOut] >> 16;
            tx = ((pl->x >> 16) + s) >> 4;
            s = (short)gStep_883__0200e190[*facingOut];
            tz = ((pl->z >> 16) + s) >> 4;
            ex = *(short *)((char *)e + 0xa);
            x0 = (ex + gBox_883__0200e1e8[i].x0) >> 4;
            ez = *(short *)((char *)e + 0x12);
            z0 = (ez + gBox_883__0200e1e8[i].z0) >> 4;
            x1 = (ex + gBox_883__0200e1e8[i].x1) >> 4;
            z1 = (ez + gBox_883__0200e1e8[i].z1) >> 4;
            if (x0 > tx)
                continue;
            if (tx >= x1)
                continue;
            if (z0 > tz)
                continue;
            if (tz >= z1)
                continue;
            if (i & 1) {
                if (x0 == (pl->x >> 20))
                    continue;
                *slotOut = slot;
                return e;
            } else {
                if (z0 == (pl->z >> 20))
                    continue;
                *slotOut = slot;
                return e;
            }
        }
    }
    return 0;
}
