/* OvlFunc_933_2009638 -- park. The subject line must stay FIRST: tools that identify a
 * park by its subject (piece.py, autosweep.py, shortrefs.py, scratchfill.py)
 * read line 1, and a note prepended above it makes them resolve the wrong
 * function or no function at all. Notes go UNDER this line. Added in batch
 * 341 after the pre-fix autosweep was found sweeping wrong subjects.
 */
/* PARKED -- OvlFunc_933_2009638, objcmp 21 of 251 (stream), aligncmp 21 in 14 hunks
 *
 * THE INSTRUCTION COUNTS NOW AGREE: ref 216, ours 216 (16-bit encodings, pads
 * excluded).  Pool words 8 against the reference's 9, so bytes are 568 against
 * 572 -- the ONE remaining shortfall, and it is one pool word, not one
 * instruction.
 *
 * *** THE HEADER THIS REPLACES WAS WRONG ABOUT ITS OWN FIGURE, AND THAT ERROR
 * WAS LOAD-BEARING. ***  It read:
 *     "the SIZE and the instruction count BOTH match (ref 251 encodings, ours
 *      251), so the --align figure IS a distance"
 * 251 is objcmp's STREAM length -- instructions plus 32-bit entries (pool words
 * AND `bl` calls) plus pads.  The 16-bit instruction counts were 216 against
 * 215, and objcmp prints them, plus its own note that a pad is absorbing the
 * difference, on the two lines DIRECTLY ABOVE the figure the header quoted.
 * Because the claim told every previous agent the residue could be read
 * straight, it also made the park mis-call its own observation: the "SECOND,
 * SMALLER OPEN ITEM ... one extra branch-to-the-next-label ... balanced
 * elsewhere so the total stays 251" was neither balanced nor a
 * branch-to-the-next-label.  It was +1 against a -2, and it was
 * create_fix_barrier's jump over a literal pool dumped mid-function.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7bc690/2009638.c \
 *     asm/overlays/rom_7bc690/ovl_4e4_c_c_c_a.s --func OvlFunc_933_2009638
 *   docker run ... python3 tools/aligncmp.py \
 *     src/non_matching/ovl_7bc690/2009638.c \
 *     asm/overlays/rom_7bc690/ovl_4e4_c_c_c_a.s OvlFunc_933_2009638 -v
 *
 * PER-REGION SPLIT, COUNTED SEPARATELY (batch 347 brief D).  Region 1 is the
 * prologue through the `str r2,[r3]` that writes 0x100; region 2 is the three
 * __Func_80105d4 arms and the epilogue.
 *     region 1   ref 33   inherited 32 (-1)   now 33 (0)
 *     region 2   ref 183  inherited 183       now 183
 * ALL of the gap was in region 1 and NONE of it in the arms.  Progression:
 * objcmp 245 -> 21, aligned 43 in 15 hunks -> 21 in 14 hunks.
 *
 * LANDING NEEDS A SPLIT.  ovl_4e4_c_c_c.s holds TWO functions
 * (OvlFunc_933_2009638, OvlFunc_933_2009874) plus a trailing `.section .data`
 * with Events_TolbiSpring, .L1f48 and .L1f70.  datacheck.py: this function
 * "reads no data label -> split needs NO new export", and the three data
 * symbols are ALREADY .global.  The two linker lines are
 *   overlays/rom_7bc690/overlay.ld:42  asm/.../ovl_4e4_c_c_c.o(.text)
 *   overlays/rom_7bc690/overlay.ld:58  asm/.../ovl_4e4_c_c_c.o(.data)
 * and the data must stay with the 2009874 half.
 *
 * ========================= WHAT IS SETTLED =========================
 *
 * 1. FOUR POOLED AREA IDS, ALL ALREADY IN area.sym AT 175-178.  The ROM does
 *    `ldr r3, =0x5c / cmp r2, r3` for each of 0x5c, 0x59, 0x5a, 0x5b where an
 *    eight-bit `cmp` would do.  `(int)&_AREA_5c` etc. is required; plain
 *    literals are 4 pool words SHORT.  Their pool words carry R_ARM_ABS32
 *    relocations where the ROM object has bare 0x59..0x5c -- the recorded
 *    "a relocation FORM is not a residue"; take it to `make compare`.
 *
 * 2. THE gState BASE IS A LOCAL POINTER OFF A STRUCT OBJECT, not an array.
 *    `typedef struct { unsigned char _bytes[704]; } GlobalState; extern
 *    GlobalState gState;` with `gs = (unsigned char *)&gState;` gives the ROM's
 *    `ldr r0, =gState` once plus a reg+reg `add r2, r0, r3` per offset.
 *    `extern char gState[]` instead folds each address into a
 *    `ldr r2, =gState+556` pool entry and is 13 instructions SHORT.
 *
 * 3. THE ZERO IS A NAMED LOCAL -- but NOT because it is narrow.  An `int`
 *    carrier for it is BIT-IDENTICAL to `short zero` on this body (measured,
 *    batch 347).  The old header's "MUST BE A NAMED NARROW LOCAL" is half
 *    refuted: named yes, narrow no.
 *
 * 4. THE TWO PER-ARM CONSTANTS ARE LOCALS.  0x7e (two arms) and 0x7c (third)
 *    live in r5 across every __Func_80105d4 call in their arm; one `int c`
 *    per arm reproduces that.
 *
 * 5. THE TWO ARMS SHARE A TAIL BY CROSS-JUMPING, not by source structure.
 *    .L17f2 is gcc's jump2 merge of the last call of the 0x59 arm and the last
 *    of the 0x5a arm; both are written out in full below.
 *
 * 6. *** NEW, BATCH 347: EACH STORED VALUE NEEDS AN `int` CARRIER AND EACH
 *    ADDRESS NEEDS ITS OWN POINTER LOCAL, AND THE ADDRESS STATEMENT MUST COME
 *    FIRST. ***  Both halves are load-bearing and neither works alone:
 *
 *    (a) A HImode pooled value forces the early pool and the spurious `b`.
 *        Spelling a stored value as a bare literal into a `short` lvalue makes
 *        gcc emit `ldrh r3, .L10` -- *thumb_movhi_insn alt 1, pool_range 64
 *        against SImode's 1020 (arm.md:4353 vs :3856).  Two HI pool loads at
 *        offsets 0x08 and 0x1c pin the pool inside 64 bytes, so
 *        create_fix_barrier dumps it at 0x40 and jumps over it.  An `int`
 *        carrier per value makes both loads SImode and the pool moves to the
 *        reference's own 0x218; the spurious branch disappears.
 *
 *    (b) THE CONSTANT-DELTA CHAIN IS reload_cse_move2add, NOT cse, AND ITS
 *        PRECONDITION IS A HARD REGISTER.  reload1.c:8840-8916 rewrites a
 *        second (set REGX (CONST_INT B)) into (set REGX (plus REGX (B-A)))
 *        -- for THE SAME REGX only (:8884-8893), only while
 *        reg_set_luid[regno] > last_label_luid (:8872, so a CODE_LABEL between
 *        the two kills it), and only when
 *        rtx_cost (new_src, PLUS) < rtx_cost (src, SET) (:8905).
 *        The reference gets `adds r3,#0x2c` and `subs r2,#0xc0` and NOT the
 *        other four deltas purely because each store's ADDRESS lands in the
 *        register that held the previous constant (`add r2,r0,r3` then
 *        `add r3,r0,r2`), clobbering it every other store.  One shared pointer
 *        local is ONE pseudo and so ONE hard register, which is why the
 *        inherited body chained four times (`add r0,#2`, `add r0,#2`,
 *        `sub r0,#0x6e`) and pooled the VALUE instead of the offset.
 *        THREE separate pointer locals split the pseudo (brief route 1) and the
 *        reference's alternation falls out.
 *
 *        THE OLD HEADER NAMED THE WRONG PASS, IN THE COSTLY DIRECTION.  It
 *        said "the pass responsible is cse (constant reuse via a related
 *        value), not reload".  cse.c:2074 use_related_value is documented and
 *        coded for expressions "of type CONST" -- symbolic -- and tests
 *        GET_CODE (x) == CONST, so it cannot relate two CONST_INTs at all.
 *        The pass is post-reload cse, i.e. exactly the reload that was ruled out.
 *
 * ========================== WHAT IS OPEN ===========================
 *
 * ONE ALLOCATION CHOICE AND ONE POOL WORD, and they are the same thing.
 * Region 1 now matches the reference at 28 of 33 slots.  The remaining
 * decision is the SECOND offset, 0x22e:
 *     ref   puts it in r2 -- the register that held the first store's address,
 *           so move2add has no known constant there and 0x22e comes from the
 *           POOL (the reference's ninth pool word)
 *     ours  puts it in r3, which still holds 0x258, so move2add fires as
 *           `subs r3, #0x2a` and we are one pool word and four bytes short
 * The base register is r1 where the reference has r0, and the two are
 * plausibly one choice.  Routes NOT yet tried: a CODE_LABEL between the two
 * constants (reload1.c:8872 is the only non-register gate), and -ffixed-r*.
 *
 * MEASURED INERT ON THIS BODY (batch 347; all 216 insns / pos 21 / aligned 21):
 *   - DECLARATION ORDER IS EXACTLY INERT.  Five permutations of the nine locals
 *     are BIT-IDENTICAL.  That axis is closed.
 *   - an `int` carrier for the zero instead of `short zero`;
 *   - the value written `0x96 << 2` or `0x22c + 0x2c` (the front end folds both);
 *   - a second base copy `g2 = (unsigned char *)&gState;`;
 *   - `((short *)gs)[0x117]` for the middle store.
 *
 * MEASURED WORSE (batch 347):
 *   - source-level deltas `*(gs+o) = o + 0x2c` / `o2 - 0xc0` WITHOUT the
 *     carriers: 214 (-2), aligned 51 -- the class the old header recorded as
 *     "offsets as one reused int o variable (5 short, 34)";
 *   - the same with an inner-scope o2: 212 (-4);
 *   - the o-delta with ONE shared q: 215 (-1), aligned 39 -- it does buy the
 *     ninth pool word and 572 bytes, but costs `adds r3,r2,#0` because the
 *     reg+reg store `strh r3,[r1,r2]` keeps the offset live;
 *   - 0x22e written `0x116 * 2`: 212 (-4), bytes -12, pos back to 245;
 *   - all three addresses computed up front: 218 (+2) but aligned 30 in only
 *     NINE hunks -- fewest seen, wrong length, so a prerequisite candidate;
 *   - the middle store left inline while the outer two use pointers: 215 (-1);
 *   - `p` computed before the three stores: 216 but pos 30, aligned 26.
 *
 * SHIMS: NONE (shimcount reports no pins and no fakematch-class shim).
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern unsigned char iwram_3001ebc[];
extern GlobalState gState;   /* GlobalState @ 0x02000240 */

extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8091ff0(int n);
extern int __StartTask(void (*fn)(void), int n);
extern void OvlFunc_933_2008cd0(void);
extern void OvlFunc_933_2009c1c(void);
extern void OvlFunc_933_20084e4(void);
extern int _AREA_59;
extern int _AREA_5a;
extern int _AREA_5b;
extern int _AREA_5c;

int OvlFunc_933_2009638(void)
{
    unsigned char *gs;
    short *p;
    void *base;
    short zero;

    int v1;
    int v2;
    short *q1;
    short *q2;
    short *q3;
    gs = (unsigned char *)&gState;
    q1 = (short *)(gs + 0x22c);
    v1 = 0x258;
    *q1 = v1;
    q2 = (short *)(gs + 0x22e);
    zero = 0;
    *q2 = zero;
    q3 = (short *)(gs + 0x230);
    v2 = 0x119;
    *q3 = v2;
    p = (short *)(gs + 0x1c0);
    if (*p != (int)&_AREA_5c) {
        base = *(void **)iwram_3001ebc;
        *(int *)((char *)base + 0x1c0) = 0x100;
        OvlFunc_933_2009c1c();
        __StartTask(OvlFunc_933_2008cd0, 0xc8 << 4);
        if (*p == (int)&_AREA_59) {
            int c = 0x7e;
            __Func_80105d4(0x16, 7, 4, 2, 0x40, c);
            __Func_80105d4(8, 0xa, 4, 2, 0x44, c);
            __Func_80105d4(0x17, 0x15, 4, 2, 0x48, c);
            __Func_80105d4(0x10, 0x2a, 4, 2, 0x4c, c);
            __Func_80105d4(0x24, 0x2c, 4, 2, 0x50, c);
            __Func_80105d4(0xe, 0x37, 4, 2, 0x54, c);
        } else if (*p == (int)&_AREA_5a) {
            int c = 0x7e;
            __Func_80105d4(0x2a, 5, 4, 2, 0x40, c);
            __Func_80105d4(0x14, 0xb, 4, 2, 0x44, c);
            __Func_80105d4(0xe, 0xc, 4, 2, 0x48, c);
            __Func_80105d4(0x38, 0x12, 4, 2, 0x4c, c);
            __Func_80105d4(7, 0x16, 4, 2, 0x50, c);
            __Func_80105d4(0x2c, 0x17, 4, 2, 0x54, c);
            __Func_80105d4(0x26, 0x18, 4, 2, 0x58, c);
            __Func_80105d4(0x1a, 0x1c, 4, 2, 0x5c, c);
            __Func_80105d4(0x11, 0x23, 4, 2, 0x60, c);
            __Func_80105d4(0x32, 0x24, 4, 2, 0x64, c);
            __Func_80105d4(0x22, 0x2b, 4, 2, 0x68, c);
            __Func_80105d4(6, 0x2e, 4, 2, 0x6c, c);
            __Func_80105d4(0x1b, 0x37, 4, 2, 0x70, c);
            __Func_80105d4(0x2b, 0x38, 4, 2, 0x74, c);
        } else if (*p == (int)&_AREA_5b) {
            int c = 0x7c;
            __Func_8091ff0(0xa9);
            __Func_80105d4(8, 0xe, 4, 4, 0x40, c);
            __Func_80105d4(6, 0x12, 4, 4, 0x44, c);
            __Func_80105d4(0xa, 0x15, 4, 4, 0x48, c);
        }
        OvlFunc_933_20084e4();
    }
    return 0;
}
