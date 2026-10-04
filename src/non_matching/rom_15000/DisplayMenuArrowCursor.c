/* DisplayMenuArrowCursor (EmitPartySprites) -- 0x0801aeec, PARK.
 * STILL NON-MATCHING, 6 of 133 encodings -- RE-MEASURED batch 322A.  WAS 16.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/DisplayMenuArrowCursor.c \
 *     asm/rom_15000/rom_1aeec_a_a_a_a_a.s --func DisplayMenuArrowCursor
 *
 * --func 6 of 133, size identical.  --whole: **6 of 133 and the TU is this
 * function ALONE** -- no missing sibling, no FUNCTIONS/ORDER line, relocations
 * equal.  tools/datacheck.py on the reference: no output, exit 0.  SPLIT SHAPE:
 * NONE, confirmed again this batch.  PINS: 0.
 *
 * *** THE PARK'S BLOCKER IS WRONG, AND THE DUMP SAYS SO. ***
 *
 * The park says insn 55 (the `ldrh` of `m->a[i].x`) "IS NOT IN THE READY LIST
 * UNTIL t=5 (latency 2 from 53)", that it is therefore "NOT reachable by any
 * rank_for_schedule tie-break", and that to close the rotation "its ADDRESS must
 * stop depending on the `adds r2,#0x10`".  `.23.sched2`, production flags,
 * `-fsched-verbose=6`, basic block 1:
 *
 *     ;;    insn  code  bb  dep  prio  cost   units
 *     ;;      45     5   0    0    12     1   core : 106 55 53 412
 *     ;;     412   173   0    1    11     1   core : 106 55 47
 *     ;;      53     5   0    1    12     1   core : 106 86 55
 *     ;;      55   157   0    3    11     2   core : 106 104 86 78 62
 *     ;;      57   180   0    1    11     1   core : 106 408 419
 *     ;;  Ready list (t = 3):  68 57 412 53
 *     ;;    --> scheduling insn <<<53>>>
 *     ;;    Ready list after queue_to_ready:  68 57 412       <-- 53 RESOLVED NOTHING
 *     ;;  Ready list (t = 4):  68 57 412
 *     ;;    --> scheduling insn <<<412>>>
 *     ;;    dependences resolved: insn 55 into ready          <-- 412 DID
 *
 * 53's cost column is **1**, so the address dependence alone would have had 55
 * ready at t=4.  What holds it is the THIRD of its three dependences: insn 412
 * is `adds r5,r3,#0`, the reload-inserted `o = &m->a[i].oam` copy, and it
 * **reads r3** -- which is insn 55's own DESTINATION register.  An
 * ANTI-DEPENDENCE, cost 0, manufactured by the ALLOCATOR.
 *
 * The ROM's load lands in **r1** (`5ab1 ldrh r1,[r6,r2]`); ours lands in **r3**
 * (`5ab3`).  With the load in r1 there is no edge from 412 and it is ready at
 * t=4 -- and it WINS there, which the park could not have predicted from the
 * three-rung ladder.  From `rank_for_schedule` in
 * ~/gs_project/camelot-gcc/gcc-2.96/gcc/haifa-sched.c:
 *
 *       link = find_insn_list (tmp, INSN_DEPEND (last_scheduled_insn));
 *       if (link == 0 || insn_cost (last_scheduled_insn, link, tmp) == 1)
 *         tmp_class = 3;
 *
 * the **cost escape is tested BEFORE the note kind**, so insn 55 -- a true data
 * dependent of the just-issued 53 -- still classifies 3, not 1.  Priority ties
 * 11/11/11 against 412 and 57, CLASS ties 3/3/3, and the **dependent-count** rung
 * then favours 55 **five to three** (`106 104 86 78 62` against `106 55 47` and
 * `106 408 419`).  Breaking the anti-dependence is sufficient.
 *
 * ===================== PROVED WITH AN INSTRUMENT (a device) =================
 *
 *     register int xv __asm__("r1");  xv = m->a[i].x;  o->x = xv;
 *
 *     park body                    6   first=21
 *     + the r1 pin                 9   first=35
 *
 * and the pinned build's own index list shows **21, 22, 23 AND 25 all gone**.
 * So **4 of the 6 encodings are ONE ALLOCATION DECISION** -- which hard register
 * the x load lands in -- and not a scheduling or an addressing problem.  The pin
 * is NOT shipped: it costs 3 fresh encodings in the later blocks (86/87,
 * 110/111, 114/115/116), so it reads 9 against the body's 6.  Its number is
 * recorded as a figure ABOUT THE BLOCKER, per the device rule.
 *
 * *** THIS RETIRES BOTH OF THE PARK'S BIG SWEEPS. ***  The park crossed 5
 * spellings of the x read against 3 of the y read (15 variants) and then 32
 * variants of statement position, and reported "a 32-VARIANT CROSS FLOORS AT 6,
 * so 6 is not a one-at-a-time artefact".  Both sweeps were aimed at rungs BELOW
 * the deciding one, which is exactly why both floored.  The park's conclusion
 * that idx 35/37 are "coupled to (1)" is also wrong: they SURVIVE the pin
 * unchanged, so they are independent.
 *
 * ===================== PIN-FREE ROUTES TRIED, 19 VARIANTS ===================
 *
 * Allocation happens at passes 17/18, sched2 at 23, and in the PRE-sched2 order
 * 412 precedes 55 -- so `&m->a[i]` is already dead when the load is born and its
 * register is free to reuse.  Chicken and egg: the schedule decides the overlap
 * and the allocation decides the schedule.  Two landed-sibling levers were aimed
 * at it and neither bit:
 *
 *   src/rom_15000/rom_15e8c_c_c_a_b.c -- "a variable assigned twice is never a
 *   local-alloc quantity", so reuse an existing GLOBAL allocno for the x value:
 *       reuse `e`                                  6    inert
 *       reuse `d`                                118    dsize +8, RELOCDIFF
 *       reuse `f`                                130    dsize -12, RELOCDIFF
 *   make the x value LIVE ACROSS the `o = ...` copy so the ranges must not share:
 *       x read hoisted above `o =`               123    dsize +4, RELOCDIFF
 *       the same reusing `d` / `f`           122/130    RELOCDIFF
 *       x read below the y store                  52    RELOCDIFF
 *   change what insn 412 reads:
 *       `ar = &m->a[i]` for `o` only               6    inert
 *       all three reads through `ar`              25
 *       a second OamSprite pointer                 6    inert
 *       the y store through `m->a[i].oam.y`      132    dsize +20
 *       the x store through `m->a[i].oam.x`      140    dsize +24
 *       `int xv` named, no pin                     6    inert
 *
 * **Everything that leaves the function intact is exactly inert at 6; everything
 * that moves the x value's identity destroys it.**  That is a flat cross in the
 * allocation dimension, so the next move is NOT another spelling of the read --
 * it is the allocno priority itself.  The lever to try next is the one in
 * src/rom_15000/rom_15e8c_a_a.c: REG_N_REFS is loop-depth weighted and
 * source-reachable, and a statement duplicated into both arms of an `if` raises
 * it BEFORE flow1 measures it while jump2's cross-jumper merges the duplicate
 * back out after reload.  `.18.greg` here says `;; 12 regs to allocate`, so this
 * is GLOBAL-alloc and `allocno_compare`'s floor_log2(R)*R/L applies -- NOT
 * local-alloc's `qty_compare`, which has no floor_log2.
 *
 * IDX 35/37 are a separate residue: `mov r1,r8` / `cmp r1,#0` against our
 * `mov r2,r8` / `cmp r2,#0`, the hi->lo reload round-robin for `i` in r8.  They
 * are untouched by everything above.
 *
 * The park's own 16 -> 6 lever is unchanged and reproduces: a WIDE `int d` for
 * the zero_extend:SI load that feeds the ADD and a NARROW `unsigned short e = d`
 * for the HImode value that feeds the COMPARE.  `e = d` is the ROM's `mov r3,r2`.
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
    int d;
    unsigned short e;

    f = (iwram_3001800 >> 2) & 7;
    if (m->a[i].f2 == 0)
        return;
    o = &m->a[i].oam;
    o->x = m->a[i].x;
    o->y = m->a[i].y;
    if (i != 0) {
        d = m->a[1].f0;
        e = d;
        src = L342f8;
        if (e != 0)
            o->x = o->x + d;
    } else {
        d = m->a[0].f0;
        e = d;
        src = L33ef8;
        if (e != 0)
            o->x = o->x - d;
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
