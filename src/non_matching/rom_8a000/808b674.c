/* InitMapActors -- PARKED, **PIN-FREE, 50 DIFFERING ENCODINGS OF 195**.
 * The subject line must stay FIRST: piece.py, autosweep.py, shortrefs.py and
 * scratchfill.py identify a park by line 1.  Notes go UNDER this line.
 *
 * ref 195 / ours 195 stream encodings; **172 / 172** 16-bit instructions -- IN
 * PHASE, a TRUE DISTANCE.  dsize 0.  Relocations differ in ONE entry and it is
 * a PURE OFFSET SHIFT, not a missing symbol: `bl Func_808b9f8` at 0x4a in the
 * reference against 0x48 here -- the same call, two bytes earlier, because one
 * instruction moved.  No symbol appears or disappears.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/808b674.c \
 *     asm/rom_8a000/rom_8b674_a_a.s --func InitMapActors
 *
 * ============== BATCH 341 D: WHAT THE ONE PIN WAS BUYING ==============
 *
 * PINNED FIGURE 14 of 195, same 172/172, relocations CLEAN, with
 * `register unsigned char *g __asm__("r8");` on the declaration of `g`.  That
 * one line is the ONLY difference between this body and the 14 -- re-add it to
 * rebuild the pinned alternate.  NOT INSTALLED: pin policy prefers pin-free.
 *
 * THE PIN BUYS EXACTLY ONE THING -- an r7/r8 ROLE SWAP BETWEEN `g` AND `id`:
 *   rom   ldr r3,[pc] / ldr r3,[r3] / mov sl,r0 / mov r8,r3   <- g  -> r8
 *         ... ldr r7,[r3,#0]                                  <- id -> r7
 *   ours  ldr r3,[pc] / mov sl,r0 / ldr r7,[r3]               <- g  -> r7
 *         ... ldr r3,[r3,#0] ... mov r8,r3                    <- id -> r8
 * and every offset access follows: ROM `movs r5,#128 / lsls r5,#2 / add r5,r8`
 * against ours `movs r1,#128 / lsls r1,#2 / adds r5,r7,r1`.  Same instruction
 * COUNT (3 each), same total (172), different registers.
 *
 * ***CORRECTION: THE PUSH LIST DOES NOT CHANGE.***  The old header said
 * "without it g is allocated r7, the push list changes, and the file reads 50 of
 * 195 WITH DIRTY RELOCATIONS".  The 50 and the RELOCDIFF reproduce exactly; the
 * push list does NOT -- both sides emit
 * `push {r5,r6,r7,lr} / mov r7,sl / mov r6,r8 / push {r6,r7}`, byte for byte.
 * r8 and sl are saved either way.  The dirty relocation is the 2-byte `bl`
 * shift above, not a symbol defect.
 *
 * WHY r7 AND NOT r8, read in the compiler: REG_ALLOC_ORDER is
 * `3, 2, 1, 0, 12, 14, 4, 5, 6, 7, 8, 10, 9, 11, 13, 15, ...`
 * (config/arm/arm.h:989-995), so r7 is reached BEFORE r8 and whichever of the
 * two quantities is allocated FIRST takes r7.  Order is decided by
 * allocno_compare (global.c:597-621):
 *     pri = floor_log2 (n_refs) * n_refs / live_length * 10000 * size
 * `g` currently wins.  For the ROM's assignment `id` must win.
 *
 * ***AND THE REFERENCE COUNT IS NOT REACHABLE FROM THE C LEVEL HERE.***  Six
 * spellings that lower `g`'s C-level reference count are ALL BYTE-IDENTICAL at
 * 50 with dsize 0 and the same first differing index:
 *     `*(struct Rec *)(g + 0x218)` written off `r + 0x18`              50
 *     `w = (int)g + 0xc` written `w = lim + 0xc`                       50
 *     `gl = g` written `gl = (unsigned char *)lim`                     50
 *     all three together                                              50
 *     `g` held as an `unsigned int` with casts at every use            50
 * cse normalises every one of them back to the same RTL, so allocno[].n_refs is
 * a property of the post-cse RTL and NOT of the source spelling.  Only two
 * variants moved at all, both worse: `g = iwram_3001ebc;` after the `id` load
 * 51, and `r = g + 0x200;` before the `id` load 166 (-4 bytes).
 * THE BOUND IS STATED WITH ITS EVIDENCE: six spellings, byte-identical output,
 * dsize 0 -- it is not a claim that no lever exists, it is a claim that the
 * reference-count lever does not reach this pair.
 *
 * PRECEDENT FOR THE CLASS, if the owner elects the pinned form: 27 landed .c
 * files already carry a `register ... __asm__` on a HIGH register (r8-r11/sl/
 * fp/ip), e.g. src/overlays/rom_7a7298/ovl_30_c_c_c_c_c_c_c_c_b.c:149
 * `register int lim __asm__("r11");`.  A hard register cannot be named in C any
 * other way, which is why this class exists at all.
 *
 * MEASURED AND CLOSED THIS BATCH (do not re-screen): autosweep BOTH AXES.
 * Flags -- `-fno-gcse`, `-fno-strict-aliasing`, `-fno-strength-reduce`,
 * `-fno-rerun-cse-after-loop`, `-fno-rerun-loop-opt` all EXACTLY INERT at 14;
 * `-fno-schedule-insns2` 60, `-O1` 124, `-ffixed-r7` 197.  Anchors -- all 40
 * statement boundaries inert or worse.
 *
 * ref: asm/rom_8a000/rom_8b674_a_a.s -- ONE function, no data section; whole-file
 * conversion, no split (tools/datacheck.py prints nothing).
 */
/* InitMapActors -- park. The subject line must stay FIRST: tools that identify a
 * park by its subject (piece.py, autosweep.py, shortrefs.py, scratchfill.py)
 * read line 1, and a note prepended above it makes them resolve the wrong
 * function or no function at all. Notes go UNDER this line. Added in batch
 * 341 after the pre-fix autosweep was found sweeping wrong subjects.
 */
/* BACKFILLED FIGURE (batch 337) -- NOT MATCHING, 14 differing encodings of 195.
 *
 * This park carried NO PARSEABLE FIGURE AND NO RECIPE, so it was invisible to
 * every targeting instrument in this tree: absent from the figure bands,
 * NO CLAIM in parkcheck, unrankable in reports/frontier.md.  170 of 675 park
 * files were in that state; 26 of them had a compilable body, which means a
 * figure was one compile away.  tools/backfill.py found them and this is the
 * measurement.
 *
 * ref 195 / ours 195 stream encodings; 172/172 16-bit instructions -- IN PHASE, so this is a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/808b674.c \
 *     asm/rom_8a000/rom_8b674_a_a.s --func InitMapActors
 *
 * THE FIGURE IS EVIDENCE; ANY DIAGNOSIS BELOW IS A HYPOTHESIS.  Across pass two
 * a park's stated blocker has been wrong roughly 40 times in 42.
 */
/* InitMapActors -- 0x0808b674.  PARKED at 14 of 195, relocations CLEAN.
 * ref: asm/rom_8a000/rom_8b674_a_a.s  (ONE function, no data section; whole-file
 *      conversion, no split -- tools/datacheck.py prints nothing)
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/808b674.c asm/rom_8a000/rom_8b674_a_a.s --whole
 *
 * RE-DERIVED batch 326, brief D: `14 of 195 differ (ours 195), first at index 30`
 * with NO SIZE line, NO INSTRUCTION COUNT line and NO RELOCATIONS line -- so size
 * equal, 195 instructions against 195, relocations clean.  `--func` agrees.
 *
 * SHIM: still ONE, `register unsigned char *g __asm__("r8")`, re-confirmed
 * load-bearing: without it g is allocated r7, the push list changes, and the file
 * reads 50 of 195 WITH DIRTY RELOCATIONS.  Needs a fakematch.txt row if it lands.
 *
 * ================= THE 14 DECOMPOSES INTO FOUR RUNS, 2+5+2+5 =================
 *   A  2  indices 30/32: `mov r2,#0` and `mov ip,r8` transposed.  A sched2 tie on
 *         INSN_LUID; loop.c hoists the loop body's zero to the END of the
 *         preheader, so it is always later than the `lim` copy.  Park's negatives
 *         re-confirmed: naming the zero is 185-186 and +4 bytes; dropping `lim` is
 *         166 and -4 bytes; `w` before `lim` is 15.
 *   B  5  the `gl` cluster.  `gl = g;` before the subscript is the ONE spelling
 *         that reproduces the ROM's REGISTER-OFFSET LOAD (`g` is a HI hard
 *         register, so `(mem (plus (reg r8) (reg n)))` fails thumb's
 *         GO_IF_LEGITIMATE_ADDRESS and expand legitimises it with an `add`; a
 *         plain lo-register COPY is a pseudo, the REG+REG address is legitimate,
 *         and reload emits the ROM's `mov r0, r8` itself).  What is left is the
 *         rotation: ROM has gl in r0 and the 0xf6 constant in r1, we have r2 and
 *         r0.  `register unsigned char *gl __asm__("r0")` measures 13, so the gl
 *         register alone is worth ONE and the other four are the 0xf6 chain.
 *         `gl = g;` BEFORE `n = id*4+0x14;` is 15.  Naming `s + 0x1ec` is inert.
 *   C  2  indices 79/80: `ldr r2, =0xfffff` against our `ldr r1`.  NOT a reload --
 *         an ordinary insn of the signed-division expansion, register chosen by
 *         local-alloc.  The OTHER `ldr r4, =0xfffff` of the same pair already
 *         matches, so the two halves of the division chain disagree only here.
 *   D  5  the `a`/`b` pointer cluster -- rewritten below.
 *
 * ========== RUN D: THE 14 CONTAINS TWO COINCIDENTAL MATCHES ==========
 *
 * The reference names its symbols (rom_8b674_a_a.s:115 and :118):
 *   rom   ldr r0,=gBuffer | lsl r3,#2 | add r2,r3,r0 | ldr r4,=ewram_200fe00 | mov r0,#0xf0 | lsl r0,#1 | add r1,r3,r4
 *   ours  ldr r2,=gBuffer | lsl r3,#2 | add r1,r3,r2 | mov r0,#0xf0 | ldr r2,=ewram_200fe00 | lsl r0,#1 | add r2,r3,r2
 *
 * So IN THE ROM `a = &gBuffer[i]` IS IN r2 AND `b = &ewram_200fe00[i]` IN r1;
 * in this body they are the other way round.  The two `ldrb r3,[rN,#2]` tests
 * that follow are not in the diff AND THAT IS A COINCIDENCE: the ROM tests r2,
 * which is its `a`; we test r2, which is our `b`.  Two differences cancel.
 * Measured batch 326, all three at 195 instructions against 195:
 *
 *   body                                     pool order        a / b    tested  figure  reloc
 *   this body: a computed first, b tested     gBuffer (ROM)    r1 / r2    b        14    clean
 *   a computed first, A TESTED FIRST          gBuffer (ROM)    r1 / r2    a        16    clean
 *   b computed first, a tested first          ewram (WRONG)    r2 / r1    a        14    DIRTY
 *
 * The 16-body's diff is this body's diff PLUS EXACTLY THE TWO `ldrb` LINES --
 * same four runs, same single run-D cause.
 *
 * ==> THE INSTALLED 14 IS A LOCAL OPTIMUM POINTING AWAY FROM THE FIX.  Correct
 *     the a/b registers on top of THIS body and the test order becomes wrong, so
 *     run D goes 5 -> 2 and the figure 14 -> 11.  Correct them on top of the
 *     16-body and run D goes 7 -> 0 and the figure 16 -> 9.  THE REJECTED 16 IS
 *     THE BASE TO BUILD ON.  Its source is this body with the condition written
 *     `a->flags == 0xfd && b->flags == 0xfd`.
 *
 * WHY THE POINTERS GET r1 AND r2, read in the compiler.  `REG_ALLOC_ORDER`
 * (config/arm/arm.h:989-995) is  r3, r2, r1, r0, ip, lr, r4, r5, r6, r7, ...
 * r3 holds the scaled index, so r2 IS THE NEXT CHOICE FOR ANY SHORT-LIVED LOCAL
 * QUANTITY, and local-alloc -- which runs before global-alloc -- hands r2 to the
 * `ldr rX,=<symbol>` pool pseudo of BOTH loads.  Whichever pointer is computed
 * first has a live range covering the OTHER load, so it conflicts with r2 and
 * global-alloc gives it r1; the second-computed pointer is then free to take r2.
 * Measured as an invariant across all three bodies above: THE FIRST-COMPUTED
 * POINTER ALWAYS GETS r1 AND THE SECOND ALWAYS GETS r2.  The ROM is first -> r2,
 * second -> r1, inverted, and its two pool values are in r0 and r4 -- the 4th and
 * 7th entries of REG_ALLOC_ORDER.
 *
 * ==> THE REQUIREMENT, SHARPENED: in the ROM's compilation r2 AND r1 were BOTH
 *     unavailable to local-alloc for the two pool pseudos at that point.  That
 *     replaces the park's "get local-alloc to put the SECOND pool pseudo
 *     somewhere other than r2", which was only half of it -- the FIRST pool
 *     pseudo is in r2 in our output too, and the ROM has it in r0.
 * The ROM also emits `ldr r4,=ewram_200fe00` BEFORE `mov r0,#0xf0` where we emit
 * it after, so its ewram pool value is live two insns longer than ours --
 * consistent with having been given a register nothing else wanted.
 *
 * REFUTED EARLIER AND STILL REFUTED: "the one with the SHORTER live length is
 * allocated first and takes r2".  `.18.greg` prints `;; 19 regs to allocate`, so
 * this is the GLOBAL allocator and the denominator is allocno[].live_length out
 * of `.17.lreg`; the priority is IRRELEVANT because r2 is not a choice -- the
 * conflict decides.  The dispositions are BIT-IDENTICAL between the 14-body and
 * the 16-body (`42 in 1`, `43 in 2` in both): only the allocno ORDER line moves.
 * Measured and NOT it: base pointers separating the symbol reference from the add
 * in all four assign/test combinations -- 14, 16, 16, 30, two with dirty
 * relocations.  Pinning a to r2 is not a diagnostic: it turns relocations DIRTY
 * and reads 18.
 *
 * -- batch 326 work in scratch_elev/b326/D (v_ima/, rtl_imaA/, rtl_imaB/)
 */
struct Tile {
    unsigned char f0;
    unsigned char f1;
    unsigned char flags;
    unsigned char f3;
};

struct Rec {
    int w[6];
};

union hw { short h; int i; };

extern unsigned char *iwram_3001ebc;
extern unsigned char **iwram_3001e70;
extern unsigned char gState[];
extern struct Tile gBuffer[];
extern struct Tile ewram_200fe00[];
extern struct Rec L9f810[] __asm__(".L9f810");
extern void Func_808b9f8(void);
extern void LoadMapActors(unsigned char *a, int b);
extern int _Func_8011f54(int a, int x, int z);
extern void _Actor_SetSpriteFlags(unsigned char *a, int n);
extern void _Actor_SetAnim(unsigned char *a, int n);
extern unsigned char *_CreateActor(int id, int x, int y, int z);
extern void _Camera_SetTarget(unsigned char *cam, unsigned char *a);
extern unsigned char *_Sprite_AddLayer(void *s, int n);

void InitMapActors(unsigned char *arg)
{
    unsigned char *g;
    unsigned char *s;
    unsigned char *s0;
    unsigned char *s2;
    unsigned char *r;
    unsigned char *gl;
    unsigned char *act;
    unsigned char *cam;
    unsigned char *lay;
    struct Tile *a;
    struct Tile *b;
    int id;
    int i;
    int n;
    int w;
    int lim;
    int h;
    int y;
    int m;

    g = iwram_3001ebc;
    s0 = gState;
    id = *(int *)(s0 + 0x1f4);
    r = g + 0x200;
    *(struct Rec *)r = L9f810[0];
    *(struct Rec *)(g + 0x218) = L9f810[1];
    lim = (int)g;
    w = (int)g + 0xc;
    do {
        *(int *)w = 0;
        w -= 4;
    } while (w >= lim);
    Func_808b9f8();
    s = gState;
    m = 0xffff;
    ((union hw *)(r + 2))->h = m;
    ((union hw *)r)->h = id;
    *(int *)(r + 8) = *(int *)(s + 0x1dc);
    *(int *)(r + 0xc) = 0;
    *(int *)(r + 0x10) = *(int *)(s + 0x1e4);
    *(short *)(r + 0x14) = *(int *)(s + 0x1e8);
    LoadMapActors(r, id);
    LoadMapActors(arg, 8);
    n = id * 4 + 0x14;
    gl = g;
    act = *(unsigned char **)(gl + n);
    *(char *)(act + 0x22) = *(unsigned short *)(s + 0x1ec);
    i = *(int *)(act + 8) / 0x100000
        + *(int *)(act + 0x10) / 0x100000 * 128;
    a = &gBuffer[i];
    b = &ewram_200fe00[i];
    if (*(int *)(s + 0x1e0) != 0 && b->flags == 0xfd && a->flags == 0xfd) {
        *(char *)(s + 0x1f2) = 1;
        h = _Func_8011f54(0, *(int *)(act + 8),
                          *(int *)(act + 0x10) - 0x100000) - 0x200000;
        y = *(int *)(act + 0xc);
        y += h;
        *(int *)(act + 0xc) = y;
        *(int *)(act + 0x14) = y;
        *(char *)(act + 0x55) = 0;
        _Actor_SetSpriteFlags(act, 0);
        _Actor_SetAnim(act, 0xc);
    } else {
        s2 = gState;
        *(char *)(s2 + 0x1f2) = 0;
    }
    cam = _CreateActor(0x80 << 8, *(int *)(act + 8), *(int *)(act + 0xc),
                       *(int *)(act + 0x10));
    *(int *)(cam + 0x14) = *(int *)(act + 0x14);
    _Camera_SetTarget(cam, act);
    if (*(short *)(g + 0x19e) == 3) {
        lay = _Sprite_AddLayer(*(void **)(act + 0x50), 0x17);
        *(char *)(lay + 5) = 0xf;
        *(char *)(lay + 6) = 9;
    }
    *iwram_3001e70 = cam + 8;
    *(unsigned char **)(g + 0x1e0) = cam;
}
