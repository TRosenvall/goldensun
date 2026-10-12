/* Func_8093304 (SetDialoguePortrait) -- 0x08093304,
 * asm/rom_8a000/rom_93304_a_a_a_a_a_a.s
 *
 * 15 of 37 stream encodings, AND THE INSTRUCTION COUNTS NOW AGREE: 29 against
 * the ROM's 29, 88 bytes against 88, relocations IDENTICAL.  So for the first
 * time this park's figure is a DISTANCE and not phase.  It was 29 of 37 at
 * 28/29 instructions -- ONE SHORT -- for six batches.  Pin-free and
 * device-free (`tools/shimcount.py` prints nothing on this body).
 *
 * Verify with:
 *     docker run --rm -v "$PWD:/work" -w /work goldensun-build python3 \
 *       tools/objcmp.py src/non_matching/rom_8a000/8093304.c \
 *       asm/rom_8a000/rom_93304_a_a_a_a_a_a.s --func Func_8093304
 *     docker run --rm -v "$PWD:/work" -w /work goldensun-build python3 \
 *       tools/aligncmp.py src/non_matching/rom_8a000/8093304.c \
 *       asm/rom_8a000/rom_93304_a_a_a_a_a_a.s Func_8093304 -v
 *   Both figures are real now; use aligncmp while iterating.
 *
 * Split shape: tools/piece.py -- the .s holds ONE .thumb_func_start, so no
 * split and the whole file converts.  tools/datacheck.py: no rows, no data.
 *
 * ============================================================
 * BATCH 344, BRIEF B -- THE GAP IS CLOSED.  GAP ARITHMETIC FIRST.
 *
 * 16-bit instructions, counted per REGION rather than in total, because the
 * net -1 was THREE separate effects (batch 344's brief warns of exactly this
 * and this park is a clean instance):
 *
 *   region                 ref   OLD park   v1a arms   this body
 *   head (push..bne)         7       7          7          7
 *   sentinel arm             6       5  (-1)     6          6
 *   non-sentinel arm        11      10  (-1)    11         11
 *   join (add..bx)           5       6  (+1)     4  (-1)    5
 *   TOTAL                   29      28          28         29
 *
 * The old park was -1/-1/+1 netting -1.  Two independent mechanisms, and
 * fixing either one alone leaves the count wrong.
 *
 * MECHANISM 1 -- the two arms.  The ROM carries the join offset 0x12f6 in a
 * register ACROSS the join: `ldr r1,=0x12f6` on the sentinel path and
 * `ldr r1,=0x12f4 / add r1,#2` on the other.  That is only reachable if the
 * offset is a SOURCE-LEVEL VARIABLE read at the join -- it is not something a
 * pass does for you.  Checked, with the citation: gcse cannot have put it
 * there, because `want_to_gcse_p` (gcse.c:1193-1209) returns 0 for CONST_INT,
 * and in Thumb a large integer STAYS a CONST_INT all the way to the minipool
 * (THUMB_LEGITIMATE_CONSTANT_P, arm.h:1807-1810, accepts any CONST_INT), so no
 * pass ever sees a hoistable pool-load expression.  Writing the join offset as
 * the literal 0x12f6 -- what the park did from batch 338 -- puts the `ldr`
 * in the JOIN and deletes the arms' `a += 2` as dead.  That is the -1/-1/+1.
 *
 * MECHANISM 2 -- and this is what the park could not get past.  Once the join
 * reads the VARIABLE, its address is `(plus reg reg)`, which
 * THUMB_GO_IF_LEGITIMATE_ADDRESS accepts for any mode of size <= 4
 * (arm.h:2133-2148), so combine folds the `add` into the store and the join
 * goes to 4 instructions: `strh r1, [r5, r2]`.  The park recorded this as the
 * other half of a conflict it called unresolvable.  It is resolvable: a
 * use-site `volatile` on the join store alone keeps the `add`.
 *
 *     join spelling                                     figure   gap
 *     literal 0x12f6 (the batch-338 park body)          29 of 37   -1
 *     the variable, 10 spellings (see below)            27 of 37   -1
 *     the variable + use-site `volatile`                16 of 37    0
 *
 * Why the arms do NOT fold while the join does, measured from the .12.life and
 * .20.ce2 dumps: in the sentinel arm the offset register is overwritten by the
 * stored value (`mov r2,#0` kills the 0x12f4 that the `add` read), and in the
 * other arm it is modified by `add r1,#2` between the `add` and the `strh`.
 * Both cases put a write to a source operand between i2 and i3, which is
 * precisely what combine's can_combine_p refuses.  The join has nothing
 * between them, which is why it is the only one of the three that folds.
 * `volatile` is the use-site form documented at docs/elevation.md:6212 and
 * :10827; it is not a device and shimcount does not count it.
 *
 * TEN JOIN SPELLINGS MEASURED, all 27 of 37 at gap -1 (so this is not a
 * spelling question -- the fold is structural):
 *   `*(short *)((unsigned int)base + a) = b` | `a += (unsigned int)base` then
 *   `*(short *)a` | `c = (unsigned int)base; c += a` | `*(short *)(base + a)`
 *   (pointer arithmetic) | `do { } while (0);` between the add and the store |
 *   `c = a + (unsigned int)base` | a dead `a = c;` after the store | a block
 *   with a `short *p` | `b &= 0xffff;` before the store | the parked
 *   literal.  ONLY the `volatile` cast moves it.
 *
 * ---- WHAT THE REMAINING 15 IS, and it is ONE THING ----
 *
 * r1 and r2 are SWAPPED against the ROM in both arms and the join, and nothing
 * else differs.  ROM: r1 = the join offset, r2 = the style value (and in the
 * sentinel arm r2 doubles as 0x12f4, in the other arm as &.L9fc28).  Ours has
 * them the other way round.  Every differing encoding is that swap except the
 * sentinel arm's `ldr =0x12f6`, which the ROM schedules second and sched2 puts
 * fourth here.
 *
 * THE ALLOCATION ARITHMETIC, from the dumps rather than from the formula.
 * `.18.greg` prints `;; 5 regs to allocate: 36 32 34 35 33` -- that list is
 * allocno_order AFTER the qsort (global.c:542 sorts, :547 dumps), so 34 (the
 * offset) is allocated before 35 (the value).  Dispositions `34 in 2  35 in 1`.
 * With 36 -> r3 and 32 -> r0 already placed, REG_ALLOC_ORDER (arm.h:989) is
 * `3, 2, 1, 0, ...`, so whichever of the two is allocated FIRST takes r2 and
 * the second takes r1.  The ROM needs the VALUE allocated first.  There are no
 * hard-reg preferences on either (the greg dump prints preferences only for
 * allocno 32), so find_reg is pure REG_ALLOC_ORDER and the whole question is
 * the sort.
 *
 * AND THE OBVIOUS PRIORITY LEVER DOES NOT REACH IT -- this is the finding the
 * next agent should start from.  allocno_compare (global.c:597-620) is
 * floor_log2(n_refs)*n_refs/live_length*10000*size.  From `.12.life`:
 *     34 (offset)  10 refs across 15 insns  -> 3*10/15 = 2.00
 *     35 (value)    8 refs across 15 insns  -> 3* 8/15 = 1.60
 * Moving the 0x83<<2 scratch out of the offset variable into its own local
 * takes 34 to `6 refs across 11 insns` -> 2*6/11 = 1.09, which is BELOW 35's
 * 1.60 -- and the sort order is STILL `36 32 34 35 33` and the dispositions are
 * STILL `34 in 2  35 in 1`.  So the .12.life refs are NOT the numbers
 * allocno_compare sees (they are recomputed between .12.life and lreg) and the
 * .12.life figures must not be used to predict this sort.  Getting the true
 * n_refs/live_length at greg time is the next step; `-dg` prints conflicts and
 * preferences but not those two fields.
 *
 * MEASURED INERT, with figures (all at gap 0 unless noted):
 *   all SIX declaration orders of {a,b,c} ................ 16, every one
 *   0x83<<2 scratch moved to its own local ............... 16
 *   0x83<<2 scratch carried by the value variable ........ 16
 *   0x83<<2 scratch carried by the address temp .......... 16
 *   sentinel's 0x12f4 moved to its own local ............. 16
 *   sentinel storing the literal 0 instead of the value .. 16
 *   &.L9fc28 moved to its own local ...................... 18
 *   the value copied through a temp at the join .......... 16
 *   table lookup split to `b += c; b = *(unsigned char *)b`
 *     (raises the value's refs by 2), both decl orders ... 16
 *   offset set BEFORE the `if` (lengthens its range) ..... 19
 *   other arm re-loading 0x12f6 instead of `a += 2` ...... 29, gap -1
 *   offset's increment split to `a = d + 2` .............. 30, gap -1
 *   ALL EIGHT per-file flag groups on this body .......... 16, 16, 16, 16,
 *     16, 16, and 18 for -O1 and 18 for -fno-schedule-insns2
 *   a SECOND anchor at all 20 remaining boundaries ....... 15, every one
 *
 * The anchor this body does carry was found by sweeping all 25 boundaries of
 * the volatile body: 24 of 25 read 16 and the two that read 15 are the two
 * sides of `a += 2`.  It fences sched2 between `c = base + a` and `a += 2`,
 * where the ROM has `add r3,r5,r1 / add r1,#2 / strh r0,[r3]` and gcc wants
 * `add / strh / add`.  Device-free and emits nothing (haifa-sched.c:3714,
 * :3735-3736); Func_80a7440 landed carrying one.
 *
 * BOTH CHEAP AXES WERE CLOSED ON THE OLD BODY FIRST (tools/autosweep.py):
 * all eight flag groups 29 or 32, all 21 anchor placements 29.
 */
extern unsigned char *iwram_3001e8c;
extern unsigned int gState;
extern unsigned char L9fc28[] __asm__(".L9fc28");
extern int Func_8092ba8(int slot);
extern int GetSpriteVoice(int a);

void Func_8093304(unsigned int slot)
{
    unsigned char *base;
    unsigned int a;
    unsigned int b;
    unsigned int c;
    int id;

    base = iwram_3001e8c;
    if (slot == 0x80000000) {
        b = 0x12f4;
        a = 0x12f6;
        c = (unsigned int)base + b;
        b = 0;
        *(short *)c = b;
    } else {
        id = GetSpriteVoice(Func_8092ba8(slot));
        c = (unsigned int)&gState;
        a = 0x83;
        a <<= 2;
        c += a;
        c = *(unsigned char *)c;
        b = (unsigned int)L9fc28;
        b = *(unsigned char *)(b + c);
        a = 0x12f4;
        c = (unsigned int)base + a;
        do { } while (0);
        a += 2;
        *(short *)c = id;
    }
    c = (unsigned int)base + a;
    *(volatile short *)c = b;
}
