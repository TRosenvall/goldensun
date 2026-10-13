/* Func_8022768 -- NON-MATCHING, 77 encodings of 87 differ (objcmp).
 *
 * ref 84 instructions / 180 bytes / 87 stream entries
 * ours 83 instructions / 180 bytes / 87 stream entries   -- gap -1 INSTRUCTION
 * aligned-equal 34 of 87 (39.1%).  NO IMPROVEMENT FOUND THIS BATCH.
 * Body unchanged; header rewritten with the missing instruction LOCATED.
 *
 * *** CORRECTION TO THIS PARK'S OWN FIGURE LINE. ***  It said "77 encodings of
 * 87 differ ... same length, but the count is inflated by a register
 * permutation".  Half right.  The STREAM and the BYTE SIZE are equal (87/87 and
 * 180/180) and the INSTRUCTIONS ARE NOT: 84 against 83.  A 2-byte alignment pad
 * is absorbing the missing instruction, which is why every length check the
 * park ran came back clean.  objcmp now says this out loud:
 *   "NOTE: size and encoding count MATCH -- a pad is absorbing the difference,
 *    so the positional figure below measures MISALIGNMENT, not distance."
 * So 77 is NOT a distance and never was; the park was one instruction short the
 * whole time and nothing it ran could see it.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/8022768.c \
 *     asm/rom_15000/rom_21dfc_a_c_c_c_a.s --func Func_8022768
 *
 * LANDING SHAPE: *** NEEDS A TEXT SPLIT. ***  asm/rom_15000/rom_21dfc_a_c_c_c_a.s
 * holds TWO functions -- MenuBar (no park, no sibling .c) and this one.
 * datacheck.py reports nothing (no data section involvement).
 * split_s.py --dry-run asm/rom_15000/rom_21dfc_a_c_c_c_a.s Func_8022768:
 *     would write asm/rom_15000/rom_21dfc_a_c_c_c_a_a.s  (1 function, 1163 lines)
 *     would write asm/rom_15000/rom_21dfc_a_c_c_c_a_b.s  (1 function,   97 lines)
 *     would REMOVE asm/rom_15000/rom_21dfc_a_c_c_c_a.s
 *     would rewrite stage1.ld
 * -> this function lands alone as src/rom_15000/rom_21dfc_a_c_c_c_a_b.c.
 * No `.global` export is needed: the split is text-only.
 * PIN-FREE, DEVICE-FREE.
 *
 * ============ THE GAP IS ENTIRELY PRE-LOOP, AND IT IS NOT ONE SITE ============
 *
 * REGION SPLIT, counted instruction by instruction off both listings.  Every
 * probe aimed at either loop is aimed at a region that already matches:
 *
 *                               ROM   ours
 *     pre-loop (entry..outer top) 48     47    <-- THE WHOLE GAP IS HERE
 *     outer body, before inner     7      7
 *     inner loop                  10     10
 *     outer body, after inner     12     12
 *     epilogue                     7      7
 *                               ----   ----
 *                                 84     83
 *
 * Inside that pre-loop region there are TWO OPPOSED sub-items of 2 each, which
 * is why the net is 1 and why a single-site hypothesis is wrong here (the
 * brief's rule: a net gap of +-1 is not evidence of one site):
 *
 *   (i)  THE ROM ROUND-TRIPS `pal` THROUGH r12, COSTING +2.
 *          ROM   ldr r2,[sp,#0x1c] / mov ip,r2 / mov r5,ip /
 *                mov r3,#1 / and r5,r3 / lsl r5,#12 / mov ip,r5
 *          ours  ldr r3,[sp,#28] / mov r2,#1 / and r3,r3,r2 /
 *                lsl r3,r3,#12 / mov r8,r3
 *        `pal` is in a HIGH register on BOTH sides -- r12 in the ROM, r8 in
 *        ours -- so this is NOT "the ROM uses a high register and we do not",
 *        which is what the park's blocker line implies.  Both commit five high
 *        registers (ROM r8/r9/r10/r12/r14 = base/mask/2/pal/dirty-ptr; ours
 *        r8/r9/r10/r12/r14 = pal/2/base/mask/dirty-ptr).  The difference is WHICH
 *        value got r12: r12 is reload's scratch here, so the value that lands in
 *        it cannot be ANDed and shifted in place and has to come down to r5 and
 *        go back.  Ours puts the MASK in r12 instead, and the mask is only ever
 *        read, so it never pays the round trip.
 *
 *   (ii) OURS RE-HOMES THE INDUCTION VARIABLES TWICE, COSTING +2.
 *        The ROM re-homes the arguments ONCE, in the prologue --
 *          mov r4,r0 (x) / mov r0,r3 (h) / mov r7,r2 (w)
 *        -- and then runs the loops directly on r0 (h, down-counter) and r1 (y,
 *        incrementing giv), which is the register y ARRIVED in.  Ours re-homes
 *        in the prologue (mov r5,r2 / mov r4,r3) and AGAIN in the preheader
 *        (mov r0,r4 / mov r6,r1), copying h a second time and copying y out of
 *        its incoming register.  That second pair is the park's
 *        "register allocation of the incoming arguments" blocker, and it is the
 *        correct diagnosis -- it is just worth +2, not +1, and it is cancelled
 *        by (i).
 *
 * SO THE TWO HALVES MUST BE MOVED TOGETHER OR THE COUNT WILL NOT TELL YOU
 * ANYTHING.  Fixing (ii) alone makes the function 2 instructions SHORT, not
 * right; fixing (i) alone makes it 1 LONG.  This is a compositionality case, and
 * the brief's "two is not N" applies -- neither sub-item has been probed alone.
 *
 * ============ WHAT THE ROM DOES, READ OFF THE REFERENCE ============
 *
 * Confirmed, and all of it already in the park's "reachable" list:
 *   * outer loop is REVERSED -- `sub r0,#1 / add r1,#1 / cmp r0,#0 / bne`, so h
 *     is a down-counter in r0 and y is an incrementing giv in r1;
 *   * inner loop is ALSO a reversed down-counter tested `!= 0`, entered by
 *     `mov r2,r7 / cmp r2,#0 / beq` -- w copied into the counter register
 *     BEFORE the entry test, which our `cmp r5,#0 / ble` does not do;
 *   * the map walk is a BYTE OFFSET in r6 -- `lsl r3,r4,#1 / lsl r2,r1,#6 /
 *     add r6,r2,r3` seeded, `add r6,#0x40` per outer iteration, dereferenced as
 *     `add r4,r6,r5` with r5 = base.  (The park lists "a raw byte-offset off
 *     instead of &bg->map[y][x]" as measured-worse; that negative was taken
 *     against a body whose outer loop was not reversed, so it has expired --
 *     re-run it.)
 *   * the dirty bit is `2 << ((unsigned)y >> 2)` with the 2 held in r10 and the
 *     shift UNSIGNED (`lsr r3,r1,#2`), as the park has it;
 *   * `ldr r3,=0xffffefff / mov r9,r3` is hoisted into the OUTER loop body,
 *     INSIDE the `w != 0` guard -- not to the function preheader.
 *
 * ============ MEASURED NEGATIVES, batch 345 brief B ============
 *
 * Both aimed at raising the committed high-register count so `pal` is pushed to
 * r12.  The reasoning is right and neither spelling reaches it:
 *   1. naming the dirty pointer and the constant 2 as loop invariants
 *      (`unsigned char *dp = &bg->dirty; int two = 2;` before the outer loop,
 *      body `*dp |= two << ...`):  aligned 34 -> 33 of 87, gap unchanged at -1.
 *      WORSE.  work/p3_a.c
 *   2. the same plus naming the mask (`int mask = ~0x1000;`):  aligned 34 of 87,
 *      exactly inert, gap unchanged.  work/p3_b.c
 * So gcc already hoists all three; naming them changes nothing, and the high-
 * register count is not set by how many invariants the SOURCE names.
 *
 * AUTOSWEPT (batch 345, --max 60).  BOTH CHEAP AXES CLOSED.  ANCHOR AXIS: 15 of
 * 15 boundaries inert at 77 (4 produced no row -- untested, not inert).
 * FLAG AXIS: `-fno-rerun-loop-opt` reads 71 of 87 and is a *** FALSE LEAD ***,
 * measured -- it equalises the 16-bit instruction count, which is why the figure
 * falls, and the ALIGNED residue goes 34 -> 29 of 87, i.e. WORSE, with the
 * instruction still missing.  This is batch 344's qualifier reproduced exactly.
 * -fno-strict-aliasing and -fno-rerun-cse-after-loop are exactly inert at 77;
 * -fno-schedule-insns2 (79), -O1 (93), -fno-gcse (91), -ffixed-r7 (93) and
 * -fno-strength-reduce (93) are all worse.
 *
 * NEXT: the question is NOT a priority question and not a source-naming
 * question.  It is "what makes greg commit a fifth high register here", and the
 * reachable route is route 5 of the five (`global.c:1630-1642`, :897-909,
 * :1016) -- LENGTHEN a range so it overlaps a hard register and PRUNES ITS OWN
 * preference.  The candidate range is `pal`: it is live from the prologue to the
 * last inner-loop iteration and never redefined, so anything that makes it
 * cross the call-clobbered set differently moves it.  Check `.18.greg` for how
 * many allocnos get a hard reg >= 8 and compare against the ROM's five; do NOT
 * read `.12.life`'s refs/live-length, which batch 344 measured as not
 * predicting `allocno_compare`'s sort at all.
 *
 * WHAT IT DOES.  Clip (x, y, w, h) to the 30x20 BG map at iwram_3001e8c, then
 * for each row set or clear tile-attribute bit 12 (palette bank) from flag&1 and
 * mark the row group dirty in the byte at +0xea3.  The y clip compares against
 * 29 but clamps to 20 - y (0x14), as the ROM does.  Called with that signature
 * by Func_802281c (EXACT, same .s).
 *
 * INERT / WORSE, inherited and NOT re-measured (each was taken against a body
 * whose outer loop may differ from this one -- treat as "inert against the body
 * of the batch that measured it"): a do/while outer loop with h--/y++ (78);
 * `for (i = w; i != 0; i--)` inner (w lands in r12); unsigned i (kills
 * reversal); `for (i = 0; i != w; i++)` (69 but no reversal); p[i] indexing;
 * short vs unsigned short map; mask spelled 0xefff or `&= ~0x1000; |= pal`;
 * pal named/unnamed/reusing the flag parameter.
 */
struct Bg {
    short map[32][32];
    unsigned char pad[0xea3 - 0x800];
    unsigned char dirty;
};

extern struct Bg *iwram_3001e8c;

void Func_8022768(int x, int y, int w, int h, int flag)
{
    struct Bg *bg = iwram_3001e8c;
    int pal = (flag & 1) << 12;
    short *p;
    int i, j;

    if (x < 0) {
        w += x;
        x = 0;
    }
    if (x + w > 29)
        w = 30 - x;
    if (y < 0) {
        h += y;
        y = 0;
    }
    if (y + h > 29)
        h = 20 - y;
    if (w > 0) {
        for (j = 0; j < h; j++) {
            p = &bg->map[y + j][x];
            for (i = 0; i < w; i++) {
                *p = (*p & ~0x1000) | pal;
                p++;
            }
            bg->dirty |= 2 << ((unsigned int)(y + j) >> 2);
        }
    }
}
