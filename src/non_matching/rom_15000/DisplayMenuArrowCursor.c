/* DisplayMenuArrowCursor (EmitPartySprites) -- 0x0801aeec.
 * NON-MATCHING: 16 encodings of 133 differ (objcmp).
 *
 * *** CLAIM CORRECTED IN BATCH 305, AND THE IMPROVEMENT IT NAMED IS NOT IN THIS BODY. ***
 * This line read "6 encodings of 133 differ.  WAS 16; batch 297a took it to 6 with one
 * lever (below)".  parkcheck re-measures the body at 16, so the 6 was never written into
 * the C -- the batch 282/283 failure mode a third time: a header advanced while its body
 * was left behind.  The lever is still described below and should reproduce the 6; treat
 * it as UNAPPLIED, not as established.
 *
 * WHY IT HID FOR SO LONG, and this is the transferable part: the recipe's candidate slot
 * held the PLACEHOLDER `<this file>` rather than a path, so parkcheck could not parse it
 * and reported UNCHECKABLE -- and an UNCHECKABLE park is never re-measured, so a header
 * lying about its body cannot be caught.  Five other parks had the same placeholder and
 * were fixed in the same pass.  A stale or unparseable recipe does not merely hide a
 * figure; it disables the only check on that figure.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/DisplayMenuArrowCursor.c \
 *     asm/rom_15000/rom_1aeec_a_a_a_a_a.s --func DisplayMenuArrowCursor
 *
 * THE PREVIOUS RECIPE WAS STALE AND THAT IS WHY THIS PARK WAS NEVER RANKED.  It
 * named asm/rom_15000/rom_1aeec_a_a_a_a.s, which does not exist; the reference is
 * rom_1aeec_a_a_a_a_a.s (one more `_a`).  tools/parkcheck.py therefore reported
 * UNCHECKABLE "reference not found" and the park stayed invisible to every
 * ranking pass.  It is one of 32 parked frontier functions in that state.
 *
 * 6 IS A TRUE DISTANCE: ref 133 encodings / 292 bytes / 6 relocations, ours 133 /
 * 292 / 6.
 *
 * SPLIT SHAPE: NONE NEEDED.  The reference holds exactly ONE function (anchored
 * `grep -cE '^[[:space:]]*\.?thumb_func_start'` = 1) and tools/datacheck.py
 * prints nothing for it -- no data section.  So this is a plain WHOLE-FILE
 * conversion.  The two data labels it reads, `.L342f8` and `.L33ef8`, live in
 * asm/rom_15000/rom_1aeec_c_c_b.s and are ALREADY `.global` there, so NO new
 * export and NO linker alias is required.
 *
 * SHIMS: none.  No register pins, no barriers, no .equ, no volatile, no flag.
 *
 * ================== THE LEVER: 16 -> 6, TWO LOCALS OF DIFFERENT WIDTH =========
 *
 * The park's residue 1 (8 of its 16) was that each arm loaded `a[i].f0` TWICE
 * where the ROM loads once and compares a COPY:
 *     rom   ldrh r2, [r6, #0x3c] / mov r3, r2 / ldr r1, =.L342f8 / cmp r3, #0
 *     ours  ldrh (compare) ... ldrh (add)
 * The park had already traced WHY -- the compare load is `(set (reg:HI) (mem:HI))`
 * and the add load is `(zero_extend:SI (mem:HI))`, two different RTL expressions
 * that cse cannot merge -- and had tried "an int/uint/short/u16 local for the
 * compare, for the add, for both".  What it had NOT tried is TWO LOCALS OF
 * DIFFERENT WIDTH AT ONCE, which is what the ROM's load+copy pair actually is:
 *
 *     d = m->a[1].f0;        /* int  -- the zero_extend:SI load, the ADD's operand
 *     e = d;                 /* u16  -- the HImode value, the COMPARE's operand
 *     src = L342f8;
 *     if (e != 0)
 *         o->x = o->x + d;
 *
 * `e = d` is the ROM's `mov r3, r2`.  Measured, each a single drop from 16:
 *     int d for the add,  memory compare                  57  (133 enc)
 *     u16 d for the add,  memory compare                 113  (141 enc)
 *     int d for the compare, memory add                   57  (133 enc)
 *     u16 d for the compare, memory add                   16  (133 enc)  = park
 *     int d + u16 e = d, compare on e, add on d            6  (133 enc)  <- THIS
 *     int d + u16 e re-read from memory, compare on e      6  (133 enc)  (same)
 *     u16 d + int c = d, compare on c, add on d           57  (133 enc)
 * So the direction matters: the WIDE local must carry the ADD and the NARROW one
 * the COMPARE.  The reverse (narrow carries the add) is 57.  Both spellings of
 * the narrow local -- a copy `e = d` and a second read `e = m->a[1].f0` -- give 6,
 * which is the evidence that cse does merge them once the modes line up.
 *
 * ================== THE REMAINING 6, AND THE PASS RESPONSIBLE ================
 *
 * Read off the side-by-side.  Two sites, and they are COUPLED:
 *
 *  (1) FOUR encodings, indices 21-25: ONE SCHEDULING ROTATION plus the two
 *      register names that follow from it.
 *          ref   21 ldrh r1,[r6,r2]   22 mov r5,r3   23 mov ip,r0  25 mov r3,ip
 *          ours  21 mov r5,r3         22 mov ip,r0   23 ldrh r3,[r6,r2]  25 mov r1,ip
 *      Index 27 `and r3, r1` is IDENTICAL in both -- the AND is commutative and
 *      both sides reach it -- so the whole of (1) is: WHERE the `ldrh` sits and,
 *      as a consequence, which register it targets.  In the ROM the load runs
 *      BEFORE `mov r5, r3`, so r3 still holds the base and the load must take r1;
 *      in ours the load runs after, r3 is dead, and the load takes r3.
 *
 *      PASS: sched2.  `-fno-schedule-insns` changes nothing (sched1 is inert
 *      here); `-fno-schedule-insns2` gives 35 of 133, so the ROM is itself
 *      scheduled and this is a rank/order difference inside sched2.  The order
 *      follows INSN_LUID, i.e. statement order: `o = &m->a[i].oam;` is expanded
 *      before `o->x = m->a[i].x;`, so o's two insns get the lower uids.
 *
 *      WHY SOURCE ORDER DOES NOT REACH IT.  To give the load a lower uid it has
 *      to be an earlier statement, and naming it costs two instructions:
 *          int x = m->a[i].x; ... o->x = x;          123 of 135 enc
 *          unsigned short x   version                120 of 135 enc
 *          int x and int y both hoisted              133 of 137 enc
 *      Writing the stores through `m->a[i].oam.x` so that `o` is computed later
 *      is far worse (132-143, 143-147 enc), and a `struct Arrow *a = &m->a[i];`
 *      intermediate is 25-26 at the right length.  `o->y` before `o->x` is 52.
 *      Declaration order does not touch it: five orders of
 *      {o, src, f, d, e} all measure exactly 6.
 *
 *  (2) TWO encodings, indices 35 and 37: the `i` copy out of r8.
 *          ref   mov r1, r8 / strb r3,[r5,#4] / cmp r1, #0
 *          ours  mov r2, r8 / strb r3,[r5,#4] / cmp r2, #0
 *      `i` lives in r8 (hi), so every touch needs a low reload register, and
 *      which one it gets is allocate_reload_reg's ROUND-ROBIN over spill_regs
 *      from last_spill_reg (reload1.c:5003, updated at 4937) -- a function of how
 *      many reload-register allocations happened EARLIER in the function, which is
 *      why it is coupled to (1): index 25 is itself a hi->lo reload.  This is the
 *      same mechanism src/non_matching/rom_a1000/80a524c.c and
 *      src/non_matching/rom_b5000/80b6d30.c both document.  Inert, measured:
 *      `if (i)`, `if (i != 0)`, `if (0 != i)` and a local `n = i` are all 6;
 *      `if (i > 0)` is 7.
 *
 * So the expected shape of a close is: fix (1) and (2) falls out with it.
 *
 * MEASURED INERT beyond the above (all still 6): the five declaration orders,
 * the four `i`-test spellings, the `n = i` local.
 * -- worked in scratch_elev/b297a/t6
  *
 * *** BATCH-305 CORRECTION: THIS FILE ATTRIBUTES A RESIDUE TO sched1, AND sched1 DOES NOT
 * *** RUN IN THIS BUILD.  Verified with -da at production flags: the dump sequence is
 * *** 17.lreg 18.greg 19.flow2 20.ce2 23.sched2 25.jump2 26.mach -- there is NO sched1 dump,
 * *** because flag_schedule_insns is off at -O2 here, so only the post-reload scheduler runs.
 * *** Re-attribute to sched2 (rank_for_schedule), to combine, or to the ALLOCATION that fixed
 * *** the order.  Relatedly, any "-fno-schedule-insns is inert" note below rules nothing out:
 * *** that flag controls a pass that never runs.  The sched2 tie-break is priority ->
 * *** dependent count (more wins) -> INSN_LUID (lower wins), and LUID preserves EXPAND order.
 * *** See "sched1 DOES NOT RUN IN THIS BUILD" in docs/elevation.md.
*/

/* DisplayMenuArrowCursor (EmitPartySprites) -- NON-MATCHING.
 * NON-MATCHING: 16 encodings of 133 differ (objcmp).
 * asm/rom_15000/rom_1aeec_a_a_a_a.s.
 *
 * objcmp: "XX ENCODINGS differ in 16 place(s) (ref 133, ours 133)" -- SIZE EXACT.
 * tryc --align: 11 instruction(s) in disagreeing regions, of 130.
 *
 * Verify with:
 *   python3 tools/objcmp.py <this> asm/rom_15000/rom_1aeec_a_a_a_a.s --func DisplayMenuArrowCursor
 *
 * RESIDUE:
 *  1. (8 of the 11) each arm loads a[1].f0 / a[0].f0 TWICE (ldrh ... cmp ... ldrh);
 *     the ROM loads once and compares a COPY (`ldrh r2 / mov r3,r2 / cmp r3`).
 *     Traced in .00.rtl: the compare load is `(set (reg:HI) (mem:HI))` and the add
 *     load is `(zero_extend:SI (mem:HI))`, so cse cannot merge them.  The ROM's
 *     pattern is the one our own tail `if (a[i].f0) a[i].f0--;` produces (two
 *     HImode loads merged into a copy).  Tried: an int/uint/short/u16 local for
 *     the compare, for the add, for both, assigned inside the if; field declared
 *     `short`; OamSprite with u16 bitfield containers -- 11 at best, most 15-34.
 *  2. (3) x load order / AND operand (`mov r3,r12; and r3,r1`) and the i copy
 *     (`mov r1,r8` vs ours r2).  A local for x: inert or worse.
 */
struct OamSprite {
    unsigned char pad[4];
    unsigned int y:8;
    unsigned int affineMode:2;
    unsigned int objMode:2;
    unsigned int mosaic:1;
    unsigned int bpp:1;
    unsigned int shape:2;
    unsigned int x:9;
    unsigned int matrixNum:5;
    unsigned int size:2;
    unsigned int tileNum:10;
    unsigned int priority:2;
    unsigned int paletteNum:4;
};

struct Arrow {
    unsigned short f0;
    unsigned short f2;
    unsigned short slot;
    unsigned short tile;
    unsigned short x;
    short y;
    unsigned char pad0c[0x14];
    struct OamSprite oam;
    unsigned char pad2c[8];
};

struct Menu {
    unsigned char pad00[8];
    struct Arrow a[2];
    unsigned char pad70[0x394 - 0x70];
    unsigned short f394;
    unsigned short f396;
    unsigned short f398;
    unsigned short pad39a;
    unsigned short f39c;
};

extern unsigned char L342f8[] __asm__(".L342f8");
extern unsigned char L33ef8[] __asm__(".L33ef8");
extern unsigned int iwram_3001800;
extern int UploadSpriteGFX(int slot, int n, void *src);
extern int _GetFlag(int id);
extern void Func_8003dec(void *p, int n);

void DisplayMenuArrowCursor(struct Menu *m, int i)
{
    struct OamSprite *o;
    unsigned char *src;
    int f;

    f = (iwram_3001800 >> 2) & 7;
    if (m->a[i].f2 == 0)
        return;
    o = &m->a[i].oam;
    o->x = m->a[i].x;
    o->y = m->a[i].y;
    if (i != 0) {
        src = L342f8;
        if (m->a[1].f0 != 0)
            o->x = o->x + m->a[1].f0;
    } else {
        src = L33ef8;
        if (m->a[0].f0 != 0)
            o->x = o->x - m->a[0].f0;
    }
    o->tileNum = UploadSpriteGFX(m->a[i].slot, 0x80, src + f * 0x80);
    if (_GetFlag(0x103)) {
        if (*(unsigned short *)((unsigned char *)m + 0x2e2) == 1)
            o->objMode = 1;
        else
            o->objMode = 0;
    }
    Func_8003dec(o, 0xee);
    if (m->a[i].f0 != 0)
        m->a[i].f0--;
}
