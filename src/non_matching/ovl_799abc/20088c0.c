/* ================ BATCH 303 ADDENDUM -- FOUR SPELLINGS MEASURED AND REJECTED ==========
 * Re-measured the park as installed: 7 of 135, ref 284 bytes == ours, count exact,
 * relocations silent.  Confirmed independently of the agent that produced it.
 *
 * THE RESIDUE, read from aligncmp (4 encodings in 3 hunks; objcmp's 7 counts the
 * downstream register renames):
 *     ref   681b  ldr  r3, [r3, #0]      <- loads INTO the address register
 *     ref   151d  asrs r5, r3, #20       <- and shifts straight out to a fresh reg
 *     ours  681a  ldr  r2, [r3, #0]      <- loads to a spare, keeps the base alive
 *     ours  1512  asrs r2, r2, #20       <- shifts IN PLACE, four slots later
 *     ref   182a  adds r2, r5, r0   vs   ours 1812  adds r2, r2, r0
 * The ROM clobbers its own address register, which says the base is dead at the load.
 *
 * SO THE OBVIOUS READING IS "DO NOT SHARE A BASE", AND IT IS WRONG.  Measured:
 *   drop the `cam` local, read iwram_3001e70 directly at both sites   131 of 135, 280 B
 * That is SHORTER than the ROM by two instructions -- gcc commons the two global
 * reads into one base ANYWAY (they are 4 bytes apart), so removing the local removes
 * an instruction the ROM has.  The ROM both keeps a base AND clobbers it.
 *
 * THE SECOND READING -- "the value is born earlier, so it gets its own register" --
 * is also wrong.  Three placements of the `cx`/`cz` pair, all worse:
 *   immediately after `e = __MapActor_GetActor(slot)`     137 of 135, 296 B (+6 insns)
 *   immediately before the two `>>= 20` shifts             56 of 135, 288 B (+2 insns)
 *   before the box-table arithmetic                       139 of 135, 296 B (+6 insns)
 * Every earlier birth lengthens the live range and costs instructions, so the
 * live-range lever has no purchase here either.
 *
 * WHAT THAT LEAVES.  The instruction COUNT is already exact at 135, so nothing is
 * missing or extra -- this is purely which register the load targets and when the
 * shift issues.  The agent ruled out the scheduler by flag (-fno-schedule-insns
 * inert, -fno-schedule-insns2 +55) and declaration order by exhaustion (16 orders,
 * byte-identical, because `w` is the only memory local so there are no spill slots
 * for the expand_decl lever to move).  A pin on the camera register measured WORSE
 * (14) -- the eighth instance of that result.
 *
 * WORTH MORE THAN THE PARK: ONE SOLUTION LANDS SEVENTEEN FUNCTIONS.  The family is
 * seventeen members at 132 instructions, all identical modulo relocated symbols, and
 * the transfer is PROVEN not predicted -- all seventeen were generated from one
 * recipe and each measures the same 7 encodings in the same 3 hunks.  A separate
 * 71-instruction function shares the `_2008ba4` address suffix and is NOT in the
 * family; do not include it.  tools/dupfuncs.py reports this group as x15 because it
 * demands byte identity and does not canonicalise relocated symbol or label names --
 * the canonical scan finding 17 is the better instrument, and dupfuncs' own summary
 * ("8 duplicate groups, 21 would come free") is therefore a LOWER bound.
 *
 * SUPERSEDES AN OLDER PARK.  src/non_matching/ovl_780898/20088c0.c parks
 * OvlFunc_883_20088c0 -- the same routine, named StampPlayerFootprintSolid -- at 101
 * of 142 from a first attempt.  This recipe is at 7 and is the base to build on; that
 * park's leads (the model id is a DOUBLE indirection, the search table is indexed by
 * a mutated offset) are already incorporated here.  Its "fifteen copies" figure and
 * its claim of larger x18/x17 groups are stale: those groups are elevated and gone.
 * ==================================================================================
 * OvlFunc_905_20088c0 -- overlay rom_799abc, ovl_30_a_a_a_c_c_c_c_a_a.
 *
 * NON-MATCHING, 7 of 135 encodings differ.
 *
 * MEASUREMENT (objcmp is the authority; figures below are objcmp's, not aligncmp's):
 *     python3 tools/objcmp.py src/non_matching/ovl_799abc/20088c0.c \
 *         asm/overlays/rom_799abc/ovl_30_a_a_a_c_c_c_c_a_a.s --func OvlFunc_905_20088c0
 *   ->  XX ENCODINGS differ in 7 place(s) (ref 135, ours 135)
 *       first at index 92: ref 681b  ours 681a
 *   SIZE is EXACT (284 = 284; objcmp prints no SIZE line, so it agreed).
 *   INSTRUCTION COUNT is EXACT (135 = 135).  Both exact, so 7 is a TRUE
 *   DISTANCE and not a saturated count.
 *   ALL SEVEN RELOCATIONS are identical in symbol and order -- objcmp prints no
 *   RELOCATIONS line: __MapActor_GetActor, __Func_8010704, OvlFunc_905_2008244 x2,
 *   iwram_3001e70, .L1594, .L15ac.
 *   aligncmp, reported SEPARATELY because it is a different metric:
 *     python3 tools/aligncmp.py src/non_matching/ovl_799abc/20088c0.c \
 *         asm/overlays/rom_799abc/ovl_30_a_a_a_c_c_c_c_a_a.s OvlFunc_905_20088c0
 *   ->  aligned-equal 132 (97.8% of ref)   differing/ins/del 7 in 3 hunks
 *
 * Verify with (in the build container):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_799abc/20088c0.c \
 *       asm/overlays/rom_799abc/ovl_30_a_a_a_c_c_c_c_a_a.s --func OvlFunc_905_20088c0
 *
 * MAKEFILE FLAGS: NONE.  objcmp.cflags_for() returns adjust=[] -- plain -O2.
 *   Do NOT add a flag row.  Measured, all WORSE than plain -O2 (7):
 *     -fno-schedule-insns2       62      -fno-gcse                 92
 *     -fno-rerun-cse-after-loop 125      -fno-strength-reduce      24
 *     -fno-schedule-insns         7 (inert)
 *   -fno-rerun-cse-after-loop also breaks SIZE (292 vs 284), so this park is
 *   NOT a CSE_CFLAGS candidate.  That is positive evidence the residue is not a
 *   CSE-pass artifact.
 *
 * SPLIT SHAPE: NONE -- WHOLE, convert directly.  No linker edit.
 *   tools/asmfacts.py asm/overlays/rom_799abc/ovl_30_a_a_a_c_c_c_c_a_a.s
 *     -> WHOLE       convert directly
 *   tools/datacheck.py: silent (no data section in this .s)
 *   LABELS NEEDING .global: NONE.  .L1594 and .L15ac are already
 *   `.global` in asm/overlays/rom_799abc/ovl_30_c_c_c_c_c_b.s
 *   (grepped).  No export to add, no asm file to touch.
 *
 * tools/shimcount.py src/non_matching/ovl_799abc/20088c0.c -> 0 shims
 *   (no register-asm locals, no inline asm, no volatile).
 *
 * THIS IS ONE OF SEVENTEEN BYTE-IDENTICAL COPIES.  The family, the derivation
 * and the transfer proof are in scratch_elev/b303a/NOTES.md.  This exact source
 * measures 7 of 135 with SIZE and COUNT exact against ALL SEVENTEEN references;
 * only the function name, the single callee and the two table symbols change.
 *
 * THE RESIDUE (3 hunks, 7 encodings), attributed to a gcc pass:
 *   ref   ldr r3,[r3,#0] / asrs r5,r3,#20   ... / adds r2,r5,r0
 *   ours  ldr r2,[r3,#0] /  ... asrs r2,r2,#20 / adds r2,r2,r0
 *   The ROM's first camera read lands IN its own address register (r3) and the
 *   >>20 must therefore move the value OUT to r5 immediately, because r3 is
 *   reused for the second camera address.  Ours loads into r2 instead, so the
 *   shift can be done destructively in place AFTER the second read.  Same
 *   instruction count, three slots apart.
 *   PASS: local-alloc / global.c register assignment (`find_reg`), not the
 *   scheduler -- -fno-schedule-insns and -fno-schedule-insns2 do not move it
 *   (the first is inert, the second costs 55 more).  The load destination is a
 *   `find_reg` choice; the shift's position follows from it.
 *
 * LEVERS THAT PAID, with figures (aligned-equal % of ref, and objcmp distance
 * once SIZE and COUNT went exact):
 *   1. loop shape -- `w.idx = 7` AFTER the compare, not before .. 58.5 -> 65.9
 *      The ROM's peeled first iteration never stores the 7 sentinel, so the
 *      assignment cannot dominate the comparison.
 *   2. field-by-field copy, not `w.p = e->p` ................... (in the above)
 *      A struct assignment emits ldmia/stmia; the ROM has three ldr/str pairs.
 *   3. x/z assigned then BOTH shifted, interleaved ............. 65.9 -> 67.4
 *      The ROM stores the unshifted w.p.x once.  Only an aliasing read between
 *      the assignment and the shift forces that store, and the z assignment is
 *      that read.
 *   4. NAMED LOCALS `ex`/`ez` holding e->p.x and e->p.z ........ 67.4 -> 97.8
 *      THE DECISIVE LEVER, and it is what took SIZE and COUNT to exact.
 *      `w`'s address is taken (it is an aggregate), so every store to w may
 *      alias e->p.x and gcc must RELOAD it.  The ROM instead captures both
 *      values at first load (mov ip,r3 / mov lr,r0) and reuses them.  With
 *      `ex`/`ez` as locals the reload disappears, the extra register pressure
 *      goes with it, and the actor pointer stops needing its own copy
 *      (`adds r4,r0,#0` vanished) -- the ROM keeps it in r0 throughout.
 *      +2 instructions and -4 bytes -> BOTH EXACT in one change.
 *   5. `unsigned int idx` in the work struct .................... 8 -> 7
 *      `if (w.idx > 6)` is `bls` in the ROM, `ble` with a signed field.
 *
 * LEVERS THAT WERE MEASURED AND ARE INERT (untested spellings are not
 * disproved, but these ARE tested):
 *   - DECLARATION ORDER IS COMPLETELY INERT HERE.  Eight orders of the ten
 *     locals, and eight more of five, all produce byte-identical output.  The
 *     `expand_decl` spill-slot lever has NO purchase on this function: there
 *     are no spill slots.  `w` is the only memory local and its frame position
 *     is fixed by its own size; every scalar is a global allocno.
 *   - camera-read spelling: 9 variants (cz-first 13, int* index 7, raw-word
 *     local 7, one-base pointer 7, args pre-summed 7, split shifts 18,
 *     expression-inline 10, operands swapped 8, two-statement 7) -- none beat 7.
 *   - `register int cx __asm__("r5")` pinning cx to the ROM's register: 14,
 *     WORSE.  Pinning fights the allocator instead of steering it.
 *
 * SHAPE NOTES for whoever picks this up:
 *   - `struct Work` models ONE 20-byte aggregate at sp+8 reached through r7:
 *     idx at +0, a 4-byte hole at +4, and x/y/z at +8/+0xc/+0x10.  That single
 *     base pointer is why the ROM computes `add r7,sp,#8` on BOTH sides of the
 *     peeled compare.  The trailing pad takes the frame to the ROM's 0x20.
 *   - The offsets 0x13c/0x140 are spelled `(0x9e << 1)` / `(0xa0 << 1)` to get
 *     `mov r3,#0x9e / lsl r3,#1`.  Thumb cannot build 0x13c in one insn.  The
 *     elevation.md warning that gcc FOLDS `(0xfa << 1)` into the pool word does
 *     NOT apply: the base here is a value LOADED from iwram_3001e70 at run time,
 *     so there is no pool word to fold into.
 *   - This function forwards its own first argument: r0 is untouched between
 *     entry and `bl __MapActor_GetActor`.  A no-argument call compiles the same,
 *     so the signature is not decided by the encodings -- `int slot` is the
 *     reading, not a measurement.
 *   - types reused verbatim from the LANDED sibling
 *     src/overlays/rom_799abc/ovl_30_a_a_a_c_c_a.c, which names these very two
 *     tables.  A landed file, not a park.
 */
struct Model { unsigned char pad00[0x28]; short *f28; };
struct Pos { int x, y, z; };
struct Ent { unsigned char pad00[8]; struct Pos p; unsigned char pad14[0x3c]; struct Model *f50; };
struct Rect { int x0, z0, x1, z1; };
struct Work { unsigned int idx; int pad04; struct Pos p; int pad14; };
extern unsigned char *iwram_3001e70;
extern int L1594[] __asm__(".L1594");
extern struct Rect L15ac[] __asm__(".L15ac");
extern struct Ent *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_905_2008244(int a, int b, int c, int d, int e, int f);

int OvlFunc_905_20088c0(int slot)
{
    unsigned char *cam;
    struct Ent *e;
    struct Work w;
    unsigned int i;
    int xe, ze, ex, ez;
    int cx, cz;
    cam = iwram_3001e70;
    e = __MapActor_GetActor(slot);
    for (i = 0; i <= 5; i++) {
        if (*e->f50->f28 == L1594[i]) {
            w.idx = i;
            break;
        }
        w.idx = 7;
    }
    if (w.idx > 6)
        return 0;
    w.p.x = ex = e->p.x;
    w.p.y = e->p.y;
    w.p.z = ez = e->p.z;
    ze = (((L15ac[w.idx].z0 < 0) ? -L15ac[w.idx].z0 : L15ac[w.idx].z0) + ((L15ac[w.idx].z1 < 0) ? -L15ac[w.idx].z1 : L15ac[w.idx].z1)) >> 4;
    xe = (((L15ac[w.idx].x0 < 0) ? -L15ac[w.idx].x0 : L15ac[w.idx].x0) + ((L15ac[w.idx].x1 < 0) ? -L15ac[w.idx].x1 : L15ac[w.idx].x1)) >> 4;
    w.p.x = ex + (L15ac[w.idx].x0 << 16);
    w.p.z = ez + (L15ac[w.idx].z0 << 16);
    w.p.x >>= 20;
    w.p.z >>= 20;
    cx = *(int *)(cam + (0x9e << 1)) >> 20;
    cz = *(int *)(cam + (0xa0 << 1)) >> 20;
    __Func_8010704(w.p.x, w.p.z, xe, ze, cx + w.p.x, cz + w.p.z);
    OvlFunc_905_2008244(0, w.p.x, w.p.z, xe, ze, 0xff);
    OvlFunc_905_2008244(2, w.p.x, w.p.z, xe, ze, 0xff);
    return 1;
}
