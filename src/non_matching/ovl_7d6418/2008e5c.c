/* OvlFunc_951_2008e5c -- NON-MATCHING, 918 of 971.
 *
 * tools/objcmp.py at PRODUCTION flags: 918 encodings differ of the reference's
 * 971.  NOT saturated -- ours is 969 against 971.  Separated axes, never summed:
 *     INSTRUCTIONS  ours  946  ref  947   (-1)
 *     POOL WORDS    ours   23  ref   24   (-1)   [objcmp count minus insns]
 *     size          ours 2116  ref 2124   (-8)
 *     aligncmp      528 of 971 aligned (54.4%), 185 hunks
 *     HIGH REGISTERS  ours r8/r9/r10, ref r8/r9/r10 -- THE PUSH LIST MATCHES
 *     high-reg mentions  ours 100  ref 94
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7d6418/2008e5c.c \
 *     asm/overlays/rom_7d6418/ovl_30_c_c_c_c_a.s --func OvlFunc_951_2008e5c
 *
 * ================================================================
 * SPLIT SHAPE -- AND tools/datacheck.py UNDER-REPORTS IT.  DO NOT FOLLOW ITS
 * RECIPE AS WRITTEN; THE LINK WILL FAIL.
 * ================================================================
 * TWO functions in the file (`OvlFunc_951_2008e5c`, `OvlFunc_951_20096a8`) plus
 * `.data` AND `.bss`, so a TEXT/DATA SPLIT is required and the data must keep
 * its own object.  `python3 tools/split_s.py <ref> OvlFunc_951_2008e5c
 * --dry-run` REFUSES (quoted, not run destructively):
 *
 *     REFUSING to split ...: 2 local label(s) would cross files.
 *       _b.s references .L2054, defined in _c.s
 *       _b.s references .L2057, defined in _c.s
 *
 * and datacheck.py prints the same pair:
 *     OvlFunc_951_2008e5c   reads .L2054, .L2057
 *     *** SPLIT MUST EXPORT: .global .L2054 .global .L2057
 *
 * THAT SET IS INCOMPLETE.  `.L20d0` is ALSO read by this function -- `ldr r2,
 * =.L20d0` at reference line 497 -- and again by `OvlFunc_951_20096a8` at line
 * 1055.  It is declared `.lcomm .L20d0, 0x60` at line 1166 and there is NO
 * `.global .L20d0` anywhere in the file: the file's `.global` block covers
 * `.L2070`, `.L20c0`, `.L2130`, `.L2134`, `.L2138` and skips `.L20d0`.  A
 * `.lcomm` symbol does not reach the object's symbol table, so a split that
 * exports only datacheck's pair link-fails on `.L20d0`.
 *
 * THE CORRECT EXPORT SET FOR THIS SPLIT IS THREE SYMBOLS:
 *     .global .L2054
 *     .global .L2057
 *     .global .L20d0
 * A `.global` emits no bytes; add all three and verify `make compare` green
 * BEFORE the split and again after it, so a layout mistake stays separable from
 * a bad decompilation.  (That discipline is what
 * src/overlays/rom_7d6418/ovl_30_c_c_c_c_b.c's own header records for the
 * sibling split of this same overlay, which was done BY HAND for exactly this
 * reason -- split_s.py keeps trailing data with the function it follows and
 * would have dragged the whole overlay's data into the .c.)
 *
 * WHY datacheck MISSES IT, so the tool can be fixed rather than worked around:
 * it reports a label only when the label is DEFINED by a `LABEL:` line in a
 * data section it walked.  `.L2054` and `.L2057` are defined that way (each an
 * `.incbin` slice).  `.L20d0` is defined by a `.lcomm` DIRECTIVE and never
 * appears as `LABEL:`, so the definition-side scan never sees it and the
 * reference-side hit has nothing to pair with.  Every `.lcomm` symbol in the
 * tree that is NOT already `.global` is exposed to the same miss.
 *
 * python3 tools/shimcount.py reports no shims for the file.
 *
 * ================================================================
 * FRAME, READ WITH THE THREE GREPS -- PLUS A FOURTH THE TRIAD NEEDS
 * ================================================================
 *   `sub sp, #imm`  -> `sub sp, #0x18` (line 11), `add sp, #0x18` (1029).
 *   `mov rX, sp` + `add rX, #K`  -> NONE.
 *   `add rX, sp` + a LOAD        -> NONE.
 *
 * On the triad as stated that reads as "0x18 of pure spill, no aggregate", and
 * that is WRONG IN ONE THIRD.  The brief's third grep is meant to separate a
 * spill from ARGUMENT STAGING, but it only looks for `add rX, sp`, and this
 * function stages its outgoing arguments through a BARE `[sp]` with no address
 * ever formed:
 *
 *     str r2, [sp]  (850)  str r2, [sp]  (860)  str r1, [sp]  (879, 895)
 *     str r6, [sp]  (924, 931, 938, 945, 952, 974, 981, 988, 995, 1002)
 *
 * FOURTEEN stores to `[sp]` and NOT ONE load from it.  Those are the fifth
 * argument of `OvlFunc_951_2008dd0`, a 5-argument call whose fifth word goes on
 * the stack -- outgoing argument space, not frame.  The REAL spills are the
 * ones with a MATCHING LOAD:
 *     [sp, #4]  str 302 / ldr 309, str 333 / ldr 340, str 788 / ldr 791+800
 *     [sp, #8]  str 797 / ldr 799
 * both saving a low register across `_divsi3_RAM` and the `_call_via_r3` to
 * `Func_8000948`.  SO: of 0x18, one word is outgoing args and two are spills.
 *
 * THE FOURTH CHECK, which the triad should carry: a `str` to `[sp, #k]` with NO
 * `ldr` from the same slot is OUTGOING ARGUMENT SPACE.  Pairing is the
 * discriminator, not the presence of an `add rX, sp` -- a bare `[sp]` with
 * offset 0 needs no address computation at all, so the staging is invisible to
 * all three existing greps.  Any function calling something with more than four
 * arguments has this, and reading its frame as all-spill will send you hunting
 * spill slots that do not exist.
 *
 * POOLED-CONSTANT MULTISET of the reference (29 pool LOADS, 19 distinct, ZERO
 * in the explicit `ldr rX, .Lnnn @ value` form):
 *   .L2138 x5  .L20c0 x3  Func_8000948 x2  0x3ff x2  .L2054 x2  .L2057 x2
 *   and x1 each: .L2070, .L20d0, .L2134, 0x1999, 0x12d, 0x17f, 0x1ff, 0x7fe,
 *   0xffff, 0xafffff, 0xffffc000, 0xef440000, 0xf5740000
 *
 * THE CONSTANT-SET CHECK (band-800plus section 5) IS WHAT GATED THIS
 * RECONSTRUCTION, and it is the reason the figure above is a figure about this
 * function rather than about a wrong program:
 *     IN REF NOT OURS:  (empty)      -- from the FIRST compile
 *     IN OURS NOT REF:  0x2ffff, 0xfffd0000   -- fixed, see lever table
 * Nothing the reference pools is missing from ours, so all 19 pooled values,
 * both `.L` data tables, the five `.L2138` writes and all 29 loads are right.
 *
 * ================================================================
 * THE COMPARISON CENSUS, WHICH THE BRIEF ASKED FOR AND WHICH CONFIRMS ITS
 * WARNING EXACTLY
 * ================================================================
 *     function              bne  beq  ble  bge  bgt  bhi  total cmp
 *     OvlFunc_951_2008e5c    21    8   19   15   12    2      77
 *     OvlFunc_880_20083cc    30   26    6    1    1    0      65
 *     OvlFunc_882_200b1ac     1    0    0    0    0    0       1
 *
 * 2008e5c is 46 SIGNED-RELATIONAL of 77 and 20083cc is 56 EQUALITY of 65.  They
 * are opposite populations and an `!=`-everywhere rule imported from one would
 * corrupt the other at 46 sites.  Its two `bhi` are also real and are the
 * documented unsigned RANGE idiom, not a spelling accident --
 *     add r3, r2, #0x3ff / cmp r3, #0x7fe / bhi
 * is `(unsigned)(v + 0x3ff) <= 0x7fe`, i.e. v in [-0x3ff, 0x3ff], and writing
 * it as a two-sided signed test is what produces it.  Both of this function's
 * LOOPS, by contrast, terminate on `!=` (`while (--i != 0)` and
 * `while (k != 4)`), so the function wants BOTH spellings and the census is the
 * only thing that says which goes where.
 *
 * ================================================================
 * MEASURED LADDER.  All tools/objcmp.py at production flags.
 * ================================================================
 *   candidate                       insns  poolw  size  objcmp  aligned  highregs
 *   ref                              947     24   2124    --      --       3
 *   v1  plain transcription          933     24   -32     881    48.0%      4
 *   v2  + explicit negate variable   945     24    -8     919    43.6%      4
 *   v3  + merged counter/coordinate  947     24    -4     918    50.2%      4
 *   v4  + field-wise struct copy     948     23    -4     911    53.8%      4
 *   v5  + indexed prologue loop      946     23    -8     918    54.4%   ** 3 **
 *
 * V5 IS THE PARK EVEN THOUGH v4 HAS THE LOWER objcmp AND v3 HAS BOTH COUNTS
 * EXACT, AND THE REASON IS A RUNG BELOW ALL THREE FIGURES: v5 is the only one
 * whose PUSH LIST matches.  The reference saves exactly r8, r9, r10 --
 * `push {r5,r6,r7,lr} / mov r7,r10 / mov r6,r9 / mov r5,r8 / push {r5,r6,r7}`
 * and the mirrored epilogue, with no r11 anywhere.  v1-v4 all save r11 too and
 * pay an extra `mov r7,r8 / push {r7}` pair.  The push list IS the function's
 * register budget, printed; a candidate holding one more long-lived quantity
 * than the reference is arguing about the wrong allocation no matter how its
 * encodings line up.
 *
 * AND v3's "BOTH COUNTS EXACT" WAS A COINCIDENCE HIDING A REAL ERROR, which is
 * discipline rule 3 arriving from a direction the rule does not name.  v3 had
 * insns 947 of 947 and pool 24 of 24, each exact SEPARATELY -- so it survives
 * the "don't compare the sum" test -- and it was still structurally wrong: its
 * `struct Vec` assignment compiled to `ldmia r3!,{r0,r4,r5} / stmia r2!,{r0,r4,r5}`
 * where the reference has three `ldr`/`str` pairs.  Two block-move instructions
 * standing in for six, plus four instructions of extra pointer setup, happened
 * to total 947.  Writing the copy FIELD-WISE (v4) made the loop body match the
 * reference instruction for instruction and moved aligned 50.2% -> 53.8% while
 * both counts went off by one.  SO: separated-axis exactness is necessary and
 * still not sufficient; the loop body either matches or it does not, and that is
 * checkable without any figure.
 *
 * LEVERS THAT PAID
 *
 * 1. THE INDEXED LOOP, TO DENY cse1 A HOISTED ADDRESS -- the batch's single
 *    best edit, and it works by the mechanism the brief names.  Written with a
 *    pointer local,
 *        q = &c->v[2]; do { q[1].x = q[0].x; ... q--; } while (--k);
 *    cse1 commons `&c->v[2]` with the `OvlFunc_951_2008dd0(0xe, &c->v[2], ...)`
 *    argument ~940 instructions later, parks it in r11 for the whole function
 *    (`mov fp, r2` at the top, `mov r1, fp` at both call sites) and that fourth
 *    long-lived quantity is the entire extra push.  `-fno-gcse` does NOT remove
 *    it -- so this is cse1, not `pre_insert_copies`, and the brief's gcse
 *    attribution is the wrong half of the mechanism.  Rewriting the loop to
 *    index,
 *        k = 3; do { c->v[k].x = c->v[k-1].x; ... k--; } while (k != 0);
 *    makes the pointer a product of loop.c STRENGTH REDUCTION, which runs after
 *    cse1 and gcse both, so there is no function-wide expression left to
 *    common.  The reference's own addressing is exactly a strength-reduced
 *    `&c->v[k-1]` -- base+0x1c counting down by 0xc, destination at `+0xc` --
 *    which is the confirmation that this is the original shape.  Worth the
 *    fourth high register, 103 -> 100 high-reg mentions, and 53.8% -> 54.4%.
 *
 * 2. NEGATE A LIVE VALUE RATHER THAN SPELL THE NEGATIVE CONSTANT.  `-(0xc0 <<
 *    10)` as a literal pools BOTH 0xfffd0000 and 0x2ffff (gcc canonicalises the
 *    `>` into a `>=` against value-1 and pools that too) -- the only two
 *    constants in the whole reconstruction that the reference does not have.
 *    Hoisting `t = 0xc0 << 10;` and writing `-t` makes it a runtime `neg r2, r4`
 *    off the live register, exactly as the reference does, and removes both
 *    pool words.  Note this is the OPPOSITE direction to 200b1ac's carrier
 *    lever: there an int local had to be ADJACENT to its store to keep a value
 *    OUT of the pool; here the local has to be HOISTED to keep two values out.
 *    The deciding question is not placement but whether the pooled thing is the
 *    value itself or a negation of it.
 *
 * 3. MERGING THE TWO LOOP COUNTERS into one variable (the reference reuses r8
 *    for the prologue counter, then the clamp high-limit, then the entity-loop
 *    counter) and the x/z "coordinate under test" into one (the reference reuses
 *    r5), v2 -> v3: 43.6% -> 50.2% and insns -2 -> 0.  This is lever 1 paying at
 *    band entry for the SECOND function in this batch, against
 *    band-800plus section 3's "reuse is an endgame lever".  Same distinction as
 *    the 200b1ac park records: merging two CONSTANT ranges removes a quantity
 *    from the set the count measures, merging two POINTER-or-COUNTER ranges
 *    raises the merged variable's reference count and buys it a register.
 *
 * MEASURED INERT OR WORSE -- bounds, every one:
 *
 *   * DECLARATION ORDER IS INERT, BYTE-IDENTICALLY, THREE WAYS.  Declaring the
 *     counter before the camera pointer, splitting the camera pointer's
 *     initialiser out of its declaration, and both together all produce the
 *     SAME object as v5 -- identical insns, pool, size, 918, 528, 185 hunks.
 *     The remaining residue IS a high-register rotation (reference `c` in r9 and
 *     counter in r8; ours `c` in r10 and counter in r9) and declaration order
 *     does not touch it.  Consistent with band-800plus section 3 as narrowed in
 *     batch 311: this function's quantities are placed by GLOBAL alloc and the
 *     order lever only ever reached reload SPILL SLOTS.
 *
 *   * FLAG SWEEP -- NOTHING BEATS THE BASELINE.  tools/flagcmp.py, labelled,
 *     none of these in the claim line:
 *       baseline (production)          918  54.4%  size -8   count -2
 *       -fno-gcse                      915  53.2%  size -4   count  0
 *       -fno-gcse -fno-rerun-cse-...   915  53.2%  size -4   count  0
 *       -fno-strict-aliasing           913  52.5%  size -4   count  0
 *       -fno-schedule-insns2           923  53.2%  size -4   count  0
 *       -fno-cse-follow-jumps          943  52.5%  size  0   count +2
 *       -fno-expensive-optimizations   951  53.2%  size +12  count +5
 *       -fno-rerun-cse-after-loop      949  51.1%  size +8   count +7
 *       -fno-cse-skip-blocks           977  49.5%  size +32  count +18
 *       -fno-omit-frame-pointer        990  56.1%  size +64  count +32
 *     `-fno-omit-frame-pointer` has the best ALIGNED figure in the whole batch
 *     and the worst count; it is a masking flag of the same family as the
 *     `-ffixed-r8..r11` case band-800plus section 7 retracted, and NO ROW
 *     SHOULD BE WRITTEN FOR IT.  `-fno-schedule-insns2` again moves the output,
 *     re-confirming sched2 runs.  NO MAKEFILE ROW IS IMPLIED by any of this.
 *
 *   * `-fno-rerun-cse-after-loop` IS ACTIVELY WORSE HERE (+8 size, +7 count,
 *     3.3 aligned points), AND THIS FUNCTION HAS TWO LOOPS.  Together with the
 *     200b1ac park's byte-identical result on a function that HAS a loop, and
 *     with the batch-312 qualification now standing in band-800plus section 2
 *     (exact on `20095dc` which has NO loop, byte-identical on `200b4c8` which
 *     HAS one), that is four functions and four different outcomes with no
 *     correlation to loop presence whatsoever.  THE FLAG MUST BE SWEPT PER
 *     FUNCTION.  "Has a loop" is not its precondition in either direction, and
 *     docs/elevation.md's section "`-fno-rerun-cse-after-loop` is not a loop
 *     phenomenon" already said the name describes WHEN the pass runs and not
 *     what it acts on.
 *
 * ================================================================
 * WHAT THE RECONSTRUCTION LEANS ON, so the next agent need not re-derive it
 * ================================================================
 * `struct Ovl951Cam` is NOT invented here -- it is lifted verbatim from the
 * already-landed sibling src/overlays/rom_7d6418/ovl_30_c_c_c_c_b.c
 * (`OvlFunc_951_200973c`), which was split out of this same overlay and reaches
 * the same `.L2070`.  Its `struct Vec v[4]` is the history buffer this
 * function's prologue shifts, which is what makes the indexed loop above read
 * naturally.  `struct Ovl951Ent` IS new: `.lcomm .L20d0, 0x60` is four
 * 0x18-byte entries, the reference indexes it as `((k<<1)+k)<<3`, and the
 * `OvlFunc_951_2008dd0(slot, &e->pos, ...)` calls pass `&e->pos` -- so entry
 * +0x00 is a `struct Vec` and +0x0c..+0x16 are six shorts.  Field +0x04 is
 * never touched by this function; it is `pos.y` and exists only because the
 * first twelve bytes are a Vec.
 *
 * TWO SPELLINGS THIS FUNCTION DISTINGUISHES AND A CANDIDATE MUST NOT CONFLATE:
 *   `>> 8` on a signed int  -> a bare `asr r7, r3, #8`        (lines 82, 87)
 *   `/ 0x100` on a signed int -> `cmp #0 / bge / add #0xff / asr #8`  (119-123)
 * The reference uses BOTH, eleven instructions apart, so the division bias
 * sequence is a reliable reading of which operator was written.  Same for
 * `>> 16` at 228/229 and 744/748.
 *
 * `_divsi3_RAM` and `_modsi3_RAM` are CALLED DIRECTLY rather than spelled `/`
 * and `%`, following src/non_matching/overlays/2008e00.c.  That is what gets
 * the reference's own relocation instead of `__divsi3`, so this park has NO
 * alias non-residue to report -- unlike the 200b1ac park, where `%` was written
 * inline on that function's own documented advice and the nine
 * `__umodsi3`/`_umodsi3_RAM` differences are expected.
 *
 * The two `Func_8000948` calls go through `int (*fp)(int); fp = Func_8000948;
 * fp(m);` to get the reference's `ldr r3, =Func_8000948 / bl _call_via_r3`;
 * the mechanism is recorded at src/rom_c9000/rom_de974_c_c_c_c_c_c_c_c_c_c_b.c.
 *
 * ================================================================
 * NEXT
 * ================================================================
 * The residue is ONE high-register rotation plus the frame. The reference puts
 * `c` in r9 and the counter in r8; we put `c` in r10 and the counter in r9 --
 * a shift of one through the whole function, which is most of the 185 hunks.
 * Declaration order does not move it and no flag does either, so the lever is
 * the ALLOCNO PRIORITY of `c` against the counter: `allocno_compare` ranks on
 * live_length/n_refs, and `c` is referenced far more often than the counter
 * while living longer.  Follow `c`'s allocno through `18.greg` -- one quantity,
 * which is band-800plus section 6's condition for dump reading being tractable
 * -- and remember the dumps carry a four-line pointer-address noise floor that
 * the 200b1ac park documents.  Second item: our frame is `sub sp, #8` against
 * the reference's `sub sp, #0x18`, i.e. we spill one word where it spills two
 * and we reserve less; that is downstream of the same rotation.
 */
struct Vec { int x, y, z; };

struct Ovl951Cam {
    /* 0x00 */ short unk0;
    /* 0x02 */ short unk2;
    /* 0x04 */ struct Vec v[4];
    /* 0x34 */ int unk34[3];
    /* 0x40 */ int unk40;
    /* 0x44 */ int unk44;
    /* 0x48 */ int unk48;
    /* 0x4c */ int unk4c;
};

struct Ovl951Ent {
    /* 0x00 */ struct Vec pos;
    /* 0x0c */ short unkc;
    /* 0x0e */ short unke;
    /* 0x10 */ short unk10;
    /* 0x12 */ short unk12;
    /* 0x14 */ short unk14;
    /* 0x16 */ short unk16;
};

extern struct Ovl951Cam L2070 __asm__(".L2070");
extern struct Ovl951Ent L20d0[] __asm__(".L20d0");
extern unsigned char L2054[] __asm__(".L2054");
extern unsigned char L2057[] __asm__(".L2057");
extern int L20c0 __asm__(".L20c0");
extern int L2134 __asm__(".L2134");
extern int L2138 __asm__(".L2138");

extern int Func_8000948(int a);
extern int _divsi3_RAM(int a, int b);
extern int _modsi3_RAM(int a, int b);
extern int __sin(int a);
extern int __cos(int a);
extern void __PlaySound(int id);
extern int __MapActor_GetActor(int slot);
extern void __Actor_SetAnim(int actor, int anim);
extern void OvlFunc_951_2008e44(int a, int b);
extern void OvlFunc_951_2008dd0(int slot, struct Vec *p, int a, int b, int c);

void OvlFunc_951_2008e5c(void)
{
    struct Ovl951Cam *c = &L2070;
    struct Ovl951Ent *e;
    int (*fp)(int);
    int k;
    int t;
    int d;
    int m;
    int s;
    int dx;
    int dz;
    int w;
    int xlo;
    int xhi;
    int zlo;
    int zhi;

    k = 3;
    do {
        c->v[k].x = c->v[k - 1].x;
        c->v[k].y = c->v[k - 1].y;
        c->v[k].z = c->v[k - 1].z;
        k--;
    } while (k != 0);
    if (c->unk2 > 0x1f) {
        c->v[0].x += c->unk40;
        c->v[0].y += c->unk44;
        c->v[0].z += c->unk48;
        if (c->v[0].y > 0) {
            c->unk44 -= 0x4000;
        } else {
            c->v[0].y = 0;
            if (c->unk44 != 0) {
                c->unk44 = 0;
                if (L20c0 == 1) {
                    __Actor_SetAnim(__MapActor_GetActor(0x11), 1);
                } else {
                    __Actor_SetAnim(__MapActor_GetActor(0xc), 1);
                }
            }
            if (c->unk4c > 0) {
                dx = (0xf0 << 15) - c->v[0].x >> 8;
                dz = (0x8e << 15) - c->v[0].z >> 8;
                fp = Func_8000948;
                d = fp(dx * dx + dz * dz);
                s = 0x1999;
                c->unk40 += _divsi3_RAM(s * dx, d);
                c->unk48 += _divsi3_RAM(s * dz, d);
                c->unk40 = c->unk40 * 0xfd / 0x100;
                c->unk48 = c->unk48 * 0xfd / 0x100;
                c->unk4c--;
            } else {
                c->unk40 = c->unk40 * 0xdc / 0x100;
                c->unk48 = c->unk48 * 0xdc / 0x100;
                if ((unsigned int)(c->unk40 + 0x3ff) <= 0x7fe) {
                    c->unk40 = 0;
                }
                if ((unsigned int)(c->unk48 + 0x3ff) <= 0x7fe) {
                    c->unk48 = 0;
                }
                if (c->unk40 == 0 && c->unk48 == 0) {
                    if (L20c0 == 1) {
                        __Actor_SetAnim(__MapActor_GetActor(0x11), 2);
                        OvlFunc_951_2008e44(0xf, 0);
                        OvlFunc_951_2008e44(0xe, 0);
                        OvlFunc_951_2008e44(0xd, 0);
                    } else {
                        __Actor_SetAnim(__MapActor_GetActor(0xc), 2);
                        OvlFunc_951_2008e44(0xa, 0);
                        OvlFunc_951_2008e44(9, 0);
                        OvlFunc_951_2008e44(8, 0);
                    }
                    dx = (0xf0 << 15) - c->v[0].x >> 16;
                    dz = (0x8e << 15) - c->v[0].z >> 16;
                    m = dx * dx + dz * dz;
                    L2134 = 1;
                    if (m <= 0xe0) {
                        L2138 = 0;
                    } else if (m <= 0x9c << 2) {
                        L2138 = 1;
                    } else if (m <= 0x88 << 3) {
                        L2138 = 2;
                    } else if (m <= 0xd2 << 3) {
                        L2138 = 3;
                    } else {
                        L2138 = 4;
                    }
                }
            }
            xlo = 0xc0 << 14;
            xhi = 0xc0 << 16;
            zlo = 0xc0 << 13;
            zhi = 0xf0 << 15;
            w = c->v[0].z;
            if (w < 0xa8 << 14) {
                t = _divsi3_RAM(((0xa8 << 14) - w) * 0x2a, 0x12);
                xlo = t + (0xc0 << 14);
                if (xlo > 0xb4 << 15) {
                    xlo = 0xb4 << 15;
                }
                xhi = (0xc0 << 16) - t;
                if (xhi < 0x96 << 16) {
                    xhi = 0x96 << 16;
                }
            }
            if (w > 0xcc << 15) {
                t = _divsi3_RAM((w - (0xcc << 15)) * 0x2a, 0x12);
                xlo = t + (0xc0 << 14);
                if (xlo > 0xb4 << 15) {
                    xlo = 0xb4 << 15;
                }
                xhi = (0xc0 << 16) - t;
                if (xhi < 0x96 << 16) {
                    xhi = 0x96 << 16;
                }
            }
            w = c->v[0].x;
            if (w < 0xb4 << 15) {
                t = _divsi3_RAM(((0xb4 << 15) - w) * 0x12, 0x2a);
                zlo = t + (0xc0 << 13);
                if (zlo > 0xa8 << 14) {
                    zlo = 0xa8 << 14;
                }
                zhi = (0xf0 << 15) - t;
                if (zhi < 0xcc << 15) {
                    zhi = 0xcc << 15;
                }
            }
            if (w > 0x96 << 16) {
                t = _divsi3_RAM((w - (0x96 << 16)) * 0x12, 0x2a);
                zlo = t + (0xc0 << 13);
                if (zlo > 0xa8 << 14) {
                    zlo = 0xa8 << 14;
                }
                zhi = (0xf0 << 15) - t;
                if (zhi < 0xcc << 15) {
                    zhi = 0xcc << 15;
                }
            }
            if (w < xlo) {
                c->v[0].x = xlo;
                if (c->unk40 < 0) {
                    c->unk40 = -c->unk40 / 2;
                }
                w = xlo;
            }
            if (w > xhi) {
                c->v[0].x = xhi;
                if (c->unk40 > 0) {
                    c->unk40 = -c->unk40 / 2;
                }
            }
            w = c->v[0].z;
            if (w < zlo) {
                c->v[0].z = zlo;
                if (c->unk48 < 0) {
                    c->unk48 = -c->unk48 / 2;
                }
                w = zlo;
            }
            if (w > zhi) {
                c->v[0].z = zhi;
                if (c->unk48 > 0) {
                    c->unk48 = -c->unk48 / 2;
                }
            }
        }
    }
    k = 0;
    do {
        e = &L20d0[k];
        if (e->unk12 > 0) {
            e->unk12--;
        }
        if (e->unk14 > 0) {
            e->unk14--;
        }
        if (k <= 1) {
            s = 0x80 << 9;
            if (e->unk10 == 1) {
                s <<= 1;
            }
            if (e->unk10 == 2) {
                s = (s << 1) + s;
            }
            if (e->unk12 > 0) {
                if (k == 0) {
                    __Actor_SetAnim(__MapActor_GetActor(0x12), 3);
                } else {
                    __Actor_SetAnim(__MapActor_GetActor(0x13), 3);
                }
            } else {
                if (k == 0) {
                    __Actor_SetAnim(__MapActor_GetActor(0x12), 1);
                } else {
                    __Actor_SetAnim(__MapActor_GetActor(0x13), 1);
                }
                if (e->unke == 0) {
                    if (e->unkc == 0) {
                        e->pos.x = e->pos.x + s;
                    } else {
                        e->pos.x = e->pos.x - s;
                    }
                    if (e->pos.x <= 0x80 << 15) {
                        e->unkc = 0;
                        if (k == 1) {
                            e->unke = 0x1e;
                        }
                    }
                    if (e->pos.x > 0xafffff) {
                        e->unkc = 1;
                        if (k == 1) {
                            e->unke = 0x1e;
                        }
                    }
                } else {
                    e->unke--;
                }
            }
        } else if (k == 2) {
            s = -0x40;
            if (e->unk10 == 1) {
                s <<= 1;
            }
            if (e->unk10 == 2) {
                s = (s << 1) + s;
            }
            if (e->unk12 > 0) {
                __Actor_SetAnim(__MapActor_GetActor(0x14), 3);
            } else {
                __Actor_SetAnim(__MapActor_GetActor(0x14), 2);
                e->pos.x = __sin(e->unkc) * 3 * 16 + (0xe0 << 15);
                e->pos.z = __cos(e->unkc) * 5 * 8 + (0x90 << 15);
                e->unkc += s;
                e->unke++;
            }
        } else {
            m = e->unke & 0x1ff;
            s = 0x40;
            if (e->unk10 == 1) {
                s = 0x80;
            }
            if (e->unk10 == 2) {
                s = (s << 1) + s;
            }
            if (e->unk12 > 0) {
                __Actor_SetAnim(__MapActor_GetActor(0x15), 3);
            } else if (m > 0x17f) {
                __Actor_SetAnim(__MapActor_GetActor(0x15), 3);
            } else {
                e->pos.x = __sin(e->unkc) * 0x34 + (0xe0 << 15);
                e->pos.z = __cos(e->unkc) * 3 * 8 + (0x90 << 15);
                e->unkc += s;
                __Actor_SetAnim(__MapActor_GetActor(0x15), 2);
            }
            e->unke++;
        }
        if (e->unk14 == 0 && c->v[0].y == 0) {
            dx = e->pos.x - c->v[0].x >> 16;
            dz = e->pos.z - c->v[0].z >> 16;
            m = dx * dx + dz * dz;
            if (m <= 0x77 && c->unk4c > 0x1e) {
                t = 0xc0 << 10;
                if (k <= 1) {
                    if (e->unkc == 0) {
                        if (c->unk40 < t) {
                            c->unk40 = t;
                            c->unk4c -= 0x64;
                        }
                    } else {
                        if (c->unk40 > -t) {
                            c->unk40 = -t;
                            c->unk4c -= 0x64;
                        }
                    }
                } else {
                    fp = Func_8000948;
                    d = fp(m);
                    c->unk40 = _divsi3_RAM(-dx * t, d);
                    c->unk48 = _divsi3_RAM(-dz * t, d);
                    c->unk4c -= 0x64;
                }
                __PlaySound(0x12d);
                e->unk12 = 0x24;
                e->unk10 = _modsi3_RAM(e->unk10 + 1, 3);
                e->unk14 = 0x1e;
            }
        }
        switch (k) {
        case 0:
            OvlFunc_951_2008dd0(0x12, &e->pos, 0, L2054[e->unk10], (e->unk10 << 4) + 0x10);
            break;
        case 1:
            OvlFunc_951_2008dd0(0x13, &e->pos, 0, L2054[e->unk10], (e->unk10 << 4) + 0x10);
            break;
        case 2:
            OvlFunc_951_2008dd0(0x14, &e->pos, (0x80 << 8) - e->unkc, L2057[e->unk10], (e->unk10 << 4) + 0x10);
            break;
        case 3:
            OvlFunc_951_2008dd0(0x15, &e->pos, 0xffff - e->unkc, L2057[e->unk10], (e->unk10 << 4) + 0x10);
            break;
        }
        k++;
    } while (k != 4);
    c->unk34[0] = c->v[0].x;
    c->unk34[1] = 0;
    c->unk34[2] = c->v[0].z;
    if (L20c0 == 1) {
        OvlFunc_951_2008dd0(0x11, &c->v[0], 0, 0, 0x10);
        OvlFunc_951_2008dd0(0x10, (struct Vec *)c->unk34, 0, 0, 0x10);
        OvlFunc_951_2008dd0(0xf, &c->v[1], 0, 0, 0x10);
        OvlFunc_951_2008dd0(0xe, &c->v[2], 0, 0, 0x10);
        OvlFunc_951_2008dd0(0xd, &c->v[3], 0, 0, 0x10);
        __Actor_SetAnim(__MapActor_GetActor(0xf), 4);
        __Actor_SetAnim(__MapActor_GetActor(0xe), 4);
        __Actor_SetAnim(__MapActor_GetActor(0xd), 4);
    } else {
        OvlFunc_951_2008dd0(0xc, &c->v[0], 0, 0, 0x10);
        OvlFunc_951_2008dd0(0xb, (struct Vec *)c->unk34, 0, 0, 0x10);
        OvlFunc_951_2008dd0(0xa, &c->v[1], 0, 0, 0x10);
        OvlFunc_951_2008dd0(9, &c->v[2], 0, 0, 0x10);
        OvlFunc_951_2008dd0(8, &c->v[3], 0, 0, 0x10);
        __Actor_SetAnim(__MapActor_GetActor(0xa), 4);
        __Actor_SetAnim(__MapActor_GetActor(9), 4);
        __Actor_SetAnim(__MapActor_GetActor(8), 4);
    }
    if (c->unk2 != -1) {
        c->unk2++;
    }
}
