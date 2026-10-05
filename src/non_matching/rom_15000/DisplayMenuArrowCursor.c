/* DisplayMenuArrowCursor -- PARK HOLDS AT 6 of 133.  Batch 328 brief D.
 * The park's bound now covers BOTH directions, with the second direction's
 * arithmetic AND its measurement, instead of only the first.
 *
 *   6 differing encodings of 133.  ref 133, ours 133 -- EQUAL.  SIZE EQUAL,
 *   INSTRUCTION COUNT EQUAL, RELOCATIONS EQUAL (objcmp prints no SIZE, no
 *   INSTRUCTION COUNT and no RELOCATIONS line), first differing index 21
 *   (ref 5ab1 `ldrh r1,[r6,r2]`, ours 1c1d `adds r5,r3,#0`).  So the figure IS
 *   a distance.  SPLIT SHAPE: none.  PINS 0.  Figure independently re-derived.
 *
 * VERIFY: python3 tools/objcmp.py src/non_matching/rom_15000/DisplayMenuArrowCursor.c asm/rom_15000/rom_1aeec_a_a_a_a_a.s --func DisplayMenuArrowCursor
 *
 * ========== THE SECOND DIRECTION, CLOSED THE SAME WAY AS THE FIRST ==========
 *
 * Batch 327 closed the direction "stretch an existing 10000-priced block-1
 * quantity (52, 72 or 74) DOWN across insn 55": QTY_CMP_PRI divides by the
 * live span, so stretching one pushes it past the span its ref count can
 * afford and it ends up allocated AFTER pseudo 56, where `post_mark_life`
 * (local-alloc.c:2039) can no longer exclude r3.
 *
 * THE OPPOSITE DIRECTION WAS NEVER COSTED: extend PSEUDO 56 UPWARD so that the
 * quantities already priced at 10000 overlap IT.  The arithmetic says it
 * should work.  56 has R = 4 and size 1, so QTY_CMP_PRI = 80000 / L:
 *     L = 9  (today)  8888        L = 31 (death moved 75 -> 86)  2580
 * At 2580 the allocation order becomes 52, 72, 74 (10000), 55 (2727),
 * 56 (2580), 61 (2500) -- and qty 55 (`idx + 16`, born 53, DIES 86) then
 * OVERLAPS 56 and is allocated BEFORE it, so `post_mark_life` excludes 55's
 * register across 56's whole range.  That is exactly the exclusion the park
 * has been looking for, and it needs no new quantity at all.
 *
 * SO THE REQUIREMENT IS: move pseudo 56's death from insn 75 to insn >= 86 at
 * zero instruction cost, keeping R = 4 (R = 5 needs L >= 37 to stay under
 * 55's 2727).  Insn 86 is the recomputed `m + (idx + 16)` for the y read, so
 * in source terms the X VALUE must still be live after the y store -- i.e. the
 * arms must consume the x VALUE rather than re-read the bitfield.
 *
 * MEASURED, crossed over four edits at depth 4 (tools/crossfire.py), and it
 * fails for the SAME reason the first direction did -- the bitfield:
 *     `int xv = m->a[i].x; o->x = xv;` with the arms still on `o->x`
 *                                          6   EXACTLY INERT (xv folds)
 *     the declaration of `xv` alone        6   EXACTLY INERT
 *     arms read `xv + d` (if arm only)   129   at 135 insns, RELOC, COUNT
 *     arms read `xv - d` (else arm only) 133   at 137 insns, RELOC, COUNT
 *     BOTH arms read xv, with `xv =`     105   RELOC, INSNS
 *     BOTH arms read xv, without `xv =`  119   RELOC, INSNS
 * Every row that actually extends 56 carries RELOC and a length flag.  The
 * reason is semantic, not accidental: `o->x = o->x + d` is a READ-MODIFY-WRITE
 * of a 9-bit field and the ROM performs that read (`ldrh`); feeding the arms a
 * saved value deletes it.  The results are arithmetically identical mod 512,
 * so these are not wrong programs -- they are DIFFERENT programs, doing less
 * memory work than the ROM, which is the trap crossfire's MEM/COUNT screen
 * exists for.
 *
 *   >> BOUND, now symmetric and with its evidence attached.  Pseudo 56 cannot
 *      be made to lose r3 in `find_free_reg` (local-alloc.c:1934, :2026):
 *      pulling an overlapping quantity DOWN to insn 55 costs it its price
 *      (span in the denominator, batch 327's table), and pushing 56's own
 *      death UP to insn 86 costs an `ldrh` the ROM performs.  BOTH ends of the
 *      only arithmetic that works are held by the same fact -- the
 *      destination is a BITFIELD, so no intermediate is free.
 *      WHAT WOULD RETIRE IT: a block-1 quantity priced above 8888 that
 *      overlaps insn 55 and costs no instruction, OR a way to keep the x value
 *      live past insn 86 while still emitting the arms' halfword read. <<
 *
 * ========== WHAT THE LANDED MODULE-MATES SAID ==========
 *
 * This function's struct is CONFIRMED by the module, not merely plausible.
 * `tools/upstream_module.py DisplayMenuArrowCursor` -> rom_15000/rom_1aeec.s,
 * 34 landed siblings.  src/rom_15000/rom_1aeec_c_a_a_a_a_a_a_b.c (Func_801c0dc)
 * carries the SAME `struct OamSprite` field for field, and in this batch
 * Func_801c154 -- also in this module -- LANDED BYTE-IDENTICAL as nothing but
 * `o->x = x; o->y = y; Func_8003dec(o, 0xfc);` on that struct.  So the x:9 /
 * y:8 layout and the `Func_8003dec(o, N)` tail are both module facts now.
 * That is also the warning: on Func_801c154 the park had built a hand-rolled
 * struct and spent six batches on the consequences.  Here the struct is right
 * and the residue is genuinely the allocator.
 */
/* DisplayMenuArrowCursor (EmitPartySprites) -- 0x0801aeec, PARK.
 * STILL NON-MATCHING, 6 of 133 encodings -- RE-MEASURED batch 326B.  WAS 16.
 *
 * VERIFY: python3 tools/objcmp.py src/non_matching/rom_15000/DisplayMenuArrowCursor.c asm/rom_15000/rom_1aeec_a_a_a_a_a.s --func DisplayMenuArrowCursor
 *
 * --func 6 of 133, SIZE EQUAL, ENCODING COUNT 133 = 133, INSTRUCTION COUNT
 * EQUAL (objcmp prints no SIZE and no INSTRUCTION COUNT line), relocations
 * equal -- so the figure IS a distance.  --whole: 6 of 133 and the TU is this
 * function alone.  datacheck.py: no output, exit 0.  SPLIT SHAPE: NONE.
 * PINS: 0.
 *
 * ======================================================================
 * *** THE PARK'S "NEXT MOVE" IS UNREACHABLE.  IT NAMES THE WRONG ALLOCATOR. ***
 * ======================================================================
 *
 * The park (and batch 325's brief, and this batch's) said: "`.18.greg` here says
 * `;; 12 regs to allocate`, so this is GLOBAL-alloc and `allocno_compare`'s
 * floor_log2(R)*R/L applies -- NOT local-alloc's `qty_compare`, which has no
 * floor_log2", and concluded that "the lever to try next is the allocno priority
 * itself", via the REG_N_REFS loop-depth-weighting trick.
 *
 * **The pseudo that has to move is not a global allocno at all.**  Read out of
 * the dumps rather than inferred:
 *
 *   .17.lreg  (insn 55 (set (reg:SI 56) (zero_extend:SI (mem/s:HI ...))))
 *   .17.lreg  Register 56 used 4 times across 9 insns IN BLOCK 1; set 2 times
 *   .18.greg  ;; 12 regs to allocate: 67 44 166 37 34 165 32 36 64 57 33 35
 *
 * **56 is not in that list.**  It is block-local, so local-alloc (pass 17)
 * assigns it a hard register BEFORE global-alloc ever runs, and
 * `allocno_compare` cannot reach it.  The 12-allocno line is true and
 * irrelevant: it describes the OTHER twelve pseudos.
 *
 * Confirmation that the twelve are priced exactly as `allocno_compare` says --
 * every one reproduced by hand from `.17.lreg`'s `used N times across M insns`
 * (n_refs read from the dump, never counted in the C, because all four
 * REG_N_REFS increment sites in flow.c add `pbi->bb->loop_depth + 1`):
 *   67 18/36=20000  44 4/7=11428  166 3/4=7500  37 4/14=5714  34 13/76=5131
 *   165 3/6=5000  32 13/99=3939  36 4/53=1509  64 4/54=1481  57 4/56=1428
 *   33 5/92=1086  35 3/54=555
 * -- which is the printed order, exactly.  The formula and the loop weighting
 * are therefore confirmed here; they are just pointed at the wrong pseudo.
 *
 * ======================================================================
 * THE ACTUAL DECIDER, READ IN local-alloc.c
 * ======================================================================
 *
 * `find_free_reg` (local-alloc.c:1934) walks hard registers in REG_ALLOC_ORDER
 * (local-alloc.c:2026, `int regno = reg_alloc_order[i];`; arm.h:989 gives
 * 3,2,1,0,12,14,4,5,6,7,8,10,9,11) and takes the first that is not in
 * `first_used`.  **r3 is tried first and it is FREE**, so pseudo 56 gets r3.
 * That is the whole cause: not a priority loss, an EMPTY EXCLUSION SET.
 *
 * Local quantities are ordered by `QTY_CMP_PRI` (local-alloc.c:1496), which is
 * the same shape as `allocno_compare` -- floor_log2(n_refs)*n_refs*size /
 * (death-birth) * 10000, so the park's claim that qty_compare "has no
 * floor_log2" is ALSO wrong -- with ties broken on the quantity number
 * (`qty_compare_1`, :1519).  Block 1's quantities price as
 *     52: 2/2  = 10000    72: 2/2  = 10000    74: 2/2 (HI) = 10000
 *     56: 4/9  =  8888    55: 3/11 =  2727    61: 2/8 (HI) =  2500
 * 56 is fourth in line and the three above it do not overlap it, so r3 is still
 * free when 56's turn comes.
 *
 * **SO THE REQUIREMENT IS EXACT: r3 must be in `first_used` across insn 55's
 * birth..death, which (local-alloc running before global-alloc, where almost no
 * hard register is live in block 1) means ANOTHER BLOCK-1 LOCAL QUANTITY
 * PRICING ABOVE 8888 MUST OVERLAP IT.**  Then 56 falls through to r2 -- held by
 * the scaled index -- and on to the ROM's r1.
 *
 * MEASURED AGAINST THAT REQUIREMENT, and all of it fails the same way: the
 * natural way to manufacture a 2-ref/2-insn (=10000) quantity spanning the x
 * load is to hoist the y value into a named local, and **a named intermediate
 * for a BITFIELD store does not fold -- it costs instructions.**
 *     y value hoisted above the x store, `int yv`      131   at 137 insns vs 133
 *     the same, `short yv`                             131   137
 *     the same, `unsigned char yv`                     129   135
 *     the same, `yv` declared first among the locals   131   137
 *     the same, hoisted above `o = &m->a[i].oam`       132   137
 *     x and y stores simply swapped                     52   133, RELOC
 * Splitting the `o->x` pseudo so its ref count (and so its 8888) drops -- the
 * only other way to reorder that queue -- fails identically:
 *     arms read `m->a[i].x + d` instead of `o->x + d`  120   135
 *     arms read `m->a[1].x` / `m->a[0].x`              104   135
 *     the arms merged with a negated `d`               132   125
 *
 * Every one carries COUNT and MEM, i.e. a different program, not a worse score.
 * **That is the park's flat cross confirmed in the one dimension it had not
 * tried, and it is now a bound WITH ITS EVIDENCE: `o->x` is set twice (the
 * initial store and the arms' read-modify-write share pseudo 56, which is why
 * it is 4 refs / 9 insns), and nothing that splits or shortens it survives the
 * bitfield.**
 *
 * ======================================================================
 * THE 6 IS TWO ALLOCATION DECISIONS, 4 + 2
 * ======================================================================
 *
 *   idx 21/22/23/25 -- WHICH REGISTER THE x LOAD LANDS IN.  4 encodings.
 *     ref  ldrh r1,[r6,r2] / adds r5,r3,#0 / mov ip,r0 / ... / mov r3,ip
 *     ours adds r5,r3,#0   / mov ip,r0     / ldrh r3,[r6,r2] / ... / mov r1,ip
 *     The ROM issues the load FIRST; ours is held by an ANTI-DEPENDENCE on
 *     insn 412 (`adds r5,r3,#0`), a reload-inserted copy that READS r3 -- insn
 *     55's own destination.  (412 is absent from .17.lreg and present in
 *     .18.greg, so reload made it, as the park says.)  With the load in r1
 *     there is no edge from 412, 55 is ready at t=4 and WINS there.  So all
 *     four are downstream of the one local-alloc choice above; the scheduling
 *     is a symptom.
 *   idx 35/37 -- `mov r1,r8` / `cmp r1,#0` against ours `mov r2,r8` /
 *     `cmp r2,#0`.  2 encodings.  The hi->lo reload scratch for `i` in r8.
 *     Independent of the above (they survive the r1 pin unchanged) and
 *     untouched by every edit in this file.  Per batch 325's settled three-layer
 *     reading, the action here is to change which pseudos are LIVE at that insn,
 *     not the spelling of the site.
 *
 * ===================== PROVED WITH AN INSTRUMENT (a device) =================
 *
 *     register int xv __asm__("r1");  xv = m->a[i].x;  o->x = xv;
 *
 *     park body                    6   first=21
 *     + the r1 pin                 9   first=35
 *
 * and the pinned build's index list shows 21, 22, 23 AND 25 all gone.  So **4 of
 * the 6 are ONE ALLOCATION DECISION** and not a scheduling or addressing
 * problem.  The pin is NOT shipped: it costs 3 fresh encodings in the later
 * blocks (86/87, 110/111, 114/115/116), so it reads 9 against the body's 6.
 * Its number is recorded as a figure ABOUT THE BLOCKER, per the device rule.
 *
 * *** THIS RETIRED BOTH OF THE PARK'S ORIGINAL BIG SWEEPS *** -- 5 spellings of
 * the x read crossed against 3 of the y read (15 variants), then 32 variants of
 * statement position, reported as "a 32-VARIANT CROSS FLOORS AT 6, so 6 is not a
 * one-at-a-time artefact".  Both were aimed at rungs BELOW the deciding one,
 * which is why both floored.  The park's claim that idx 35/37 are "coupled to
 * (1)" is also wrong: they survive the pin unchanged.
 *
 * ===================== PIN-FREE ROUTES TRIED, 19 + 10 VARIANTS ==============
 *
 * From batch 322A, unchanged and still reproducing:
 *   reuse `e` 6 inert | reuse `d` 118 | reuse `f` 130 | x read hoisted above
 *   `o =` 123 | the same reusing d/f 122/130 | x read below the y store 52 |
 *   `ar = &m->a[i]` for `o` only 6 inert | all three reads through `ar` 25 |
 *   a second OamSprite pointer 6 inert | y store through `m->a[i].oam.y` 132 |
 *   x store through `m->a[i].oam.x` 140 | `int xv` named, no pin 6 inert.
 * Plus the ten of batch 326B listed above.
 *
 * **Everything that leaves the function intact is exactly inert at 6; everything
 * that moves the x value's identity destroys it.**
 *
 * NEXT: a block-1 local quantity pricing above 8888 that overlaps insn 55 and
 * costs no instruction.  Nothing in two batches has produced one, and the
 * bitfield is why -- every candidate intermediate has to be a real register.
 * Worth trying from the OTHER side: anything that makes one of the three
 * existing 10000-priced quantities (52, 72, 74) OVERLAP insn 55, rather than
 * creating a fourth.
 *
 * The park's own 16 -> 6 lever is unchanged and reproduces: a WIDE `int d` for
 * the zero_extend:SI load that feeds the ADD and a NARROW `unsigned short e = d`
 * for the HImode value that feeds the COMPARE.  `e = d` is the ROM's `mov r3,r2`.
 * The sibling DisplayMenuArrowCursor2 (30 of 140) keeps this struct layout.
 *
 * ======================================================================
 * BATCH 327 BRIEF D -- 6 of 133 RE-DERIVED.  THE "WORTH TRYING FROM THE OTHER
 * SIDE" SUGGESTION ABOVE IS NOW CLOSED **WITH ARITHMETIC**, NOT ANOTHER SWEEP.
 * ======================================================================
 * objcmp --func: 6 of 133 (ref 133, ours 133), first at index 21 (ref 5ab1
 * `ldrh r1,[r6,r2]`, ours 1c1d `adds r5,r3,#0`).  No SIZE, no RELOCATIONS, no
 * INSTRUCTION COUNT line -- the figure IS a distance.  Both find_free_reg
 * citations stand.
 *
 * BLOCK 1's SIX LOCAL QUANTITIES, read out of .17.lreg WITH THEIR RTL ROLES
 * (the insns, not just the header line):
 *   qty  refs/span  QTY_CMP_PRI  born..dies  what it is
 *    52     2 / 2      10000       45 -> 47   `m + idx` (&m->a[i]); feeds o = that + 40
 *    72     2 / 2      10000       86 -> 88   `m + (idx+16)` RECOMPUTED for the y read
 *    74     2 / 2 HI   10000       88 -> 104  the loaded m->a[i].y
 *    56     4 / 9       8888       55 -> 75   the x value: set at 55
 *                                             (zero_extend (mem:HI (plus reg32 reg55))),
 *                                             re-set at 62 (& 0x1ff), used at 75 (ior)
 *    55     3 / 11      2727       53 -> 86   `idx + 16`
 *    61     2 / 8 HI    2500       64 -> 69   the old o->x halfword
 *
 * MECHANISM RE-CONFIRMED IN THE COMPILER: QTY_CMP_PRI is
 * floor_log2(n_refs)*n_refs*size / (death - birth) * 10000 (local-alloc.c:1496-1498);
 * qty_compare_1 ties on the LOWER quantity number (:1519-1521); find_free_reg
 * takes the first reg_alloc_order regno not in `first_used` (:2023-2032); and
 * `post_mark_life (regno, mode, 1, born_index, dead_index)` (:2039) is what puts
 * an ALREADY-ALLOCATED quantity's register into the exclusion set -- and only
 * across its OWN birth..death.  So the exclusion needs an overlapping quantity
 * allocated EARLIER, i.e. priced strictly above 8888 (or tied at 8888 with a
 * lower qty number).
 *
 * THE BOUND, WITH THE ARITHMETIC.  Need floor_log2(R)*R/L > 0.8888:
 *     R = 2 -> L <= 2     R = 3 -> L <= 3     R = 4 -> L <= 8
 *     R = 5 -> L <= 11    R = 6 -> L <= 13
 * Applied to the three existing 10000-priced quantities:
 *   - 52 is born at insn 45 and must live to at least 55.  The insns in its way
 *     are 45, 47, 53, 55, so L >= 4 with at best R = 3 => 1*3/4 = 7500, BELOW
 *     8888.  Reaching R = 4 inside L <= 8 means using `m + idx` four times in
 *     eight insns, and the only available fourth use is to address the x load
 *     off it as (plus reg52 16) -- which replaces the ROM's register-offset
 *     `ldrh r1,[r6,r2]` with an immediate-offset `ldrh`, i.e. a different
 *     program AT THE VERY ENCODING BEING FIXED.
 *   - 72 is born at 86 and must be born before 75.  Pulling it up means one
 *     base pointer serving both the x and the y read: R = 3 over L ~ 53..88, a
 *     few thousand at best.  (That is the already-measured `ar = &m->a[i]`
 *     family: 25, and 122/130.)
 *   - 74 is the loaded y halfword and cannot be born before the load that
 *     defines it without moving the y read -- the refuted named-intermediate
 *     family, 129-132 at 135-137 insns.
 *
 *   >> BOUND: QTY_CMP_PRI DIVIDES BY THE LIVE SPAN, so any way of stretching an
 *      existing 10000-priced block-1 quantity across insn 55 necessarily pushes
 *      its span past the L its ref count can afford, and it ends up allocated
 *      AFTER pseudo 56 -- where post_mark_life can no longer exclude r3.  The
 *      only shape the arithmetic permits is a FRESH 2-ref/2-insn quantity born
 *      and dying inside 55..75, which is exactly the route batch 326 refuted ten
 *      ways: the destination is a BITFIELD, so a named intermediate never folds
 *      and costs +2 to +4 instructions (COUNT and MEM on every row).
 *      Evidence attached: the table above is read from .17.lreg, the formula
 *      from local-alloc.c:1496, the exclusion from :2039.
 *      WHAT WOULD RETIRE IT: a block-1 quantity with 4 refs inside 8 insns (or
 *      5 inside 11) spanning insn 55 that costs no instruction.  None of the
 *      block's six quantities can be reshaped into one.
 *
 * PARK HOLDS AT 6 of 133.  The suggested direction is closed; no new variants
 * were measured this round because the arithmetic rules out the whole class.
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
