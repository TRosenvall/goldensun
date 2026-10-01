/* OvlFunc_964_200a59c  --  0x0200a59c
 *
 * NON-MATCHING, 3 of 1062   (objcmp --func, PRODUCTION FLAGS)
 *   SIZE   ref 2680 bytes, ours 2688            INEXACT, +8
 *   COUNT  ref 1062 encodings, ours 1065        INEXACT, +3
 *   objcmp's "differ in 977 place(s)" is SATURATED and must not be read as a
 *   distance; the pair above is the figure.
 *   aligncmp  705 aligned-equal, 66.4% of ref, 539 differing in 146 hunks.
 *   shimcount  0 register pins, 0 .equ shims, 0 "+r" barriers, 0 empty asm.
 *              No fakematch.txt row is needed.
 *
 * Verify with:
 *   docker run --rm -v "$PWD:/work" -w /work goldensun-build sh -c \
 *     'GCC296_DIR=/opt/gcc296 AGBCC_DIR=/opt/agbcc python3 tools/objcmp.py \
 *        scratch_elev/b311d/PARK_OvlFunc_964_200a59c.c \
 *        scratch_elev/b311d/ref_OvlFunc_964_200a59c.s --func OvlFunc_964_200a59c'
 *   docker run --rm -v "$PWD:/work" -w /work goldensun-build sh -c \
 *     'GCC296_DIR=/opt/gcc296 python3 tools/aligncmp.py \
 *        scratch_elev/b311d/PARK_OvlFunc_964_200a59c.c \
 *        scratch_elev/b311d/ref_OvlFunc_964_200a59c.s OvlFunc_964_200a59c'
 *   ref_OvlFunc_964_200a59c.s in this directory is the function sliced out of
 *   asm/overlays/rom_7ed0a0/ovl_30_c_c_c_c_c_b.s; it is not a tree file.
 *
 * SPLIT SHAPE.  None yet, and landing this one needs a second split.
 * asm/overlays/rom_7ed0a0/ovl_30_c_c_c_c_c_b.s is ALREADY the b-half of a split
 * made when the file's first four functions landed as
 * src/overlays/rom_7ed0a0/ovl_30_c_c_c_c_c_a.c.  That b-half holds this one
 * function AND the whole file's .data, which exports four named globals plus
 * THIRTEEN four-digit `.L` labels (.L31f0 .L3230 .L3248 .L3350 .L336c .L33ec
 * .L340c .L342c .L3474 .L3654 .L3a74 .L3c0c .L3ef4) as `.global`, each an
 * `.incbin` slice of overlays/rom_7ed0a0/orig.bin.  So converting this function
 * means splitting the .data off into a third object and adding it to the linker
 * script's .text AND .data runs.  Do not attempt that until the function is
 * byte-exact.
 *
 * ASM-LABEL CAPTURE HAZARD: CHECKED AND CLEAR, by generating the candidate's .s
 * and comparing label sets rather than by reasoning about digits.  gcc's counter
 * on this candidate reaches .L67 (plus .LCB47..".LCB984" and .Lfe1); the
 * intersection with the thirteen needed externs above is EMPTY.  Re-run that
 * comparison after any edit that changes the number of basic blocks -- the
 * five all-decimal names in that list (.L3230 .L3248 .L3350 .L3474 .L3654) are
 * safe only because gcc's counter does not get near them, not because they
 * contain a hex letter.
 *
 * WHAT IT IS.  The fifth and last function of the original object: a
 * 993-instruction cutscene entry point, and structurally a DOUBLE DISPATCH.  It
 * stamps 0x204 into the iwram script word, optionally re-homes the party, and
 * then switches on the sub-area id gState[0xe1] -- through one of TWO jump
 * tables, chosen by whether the area id gState[0xe0] is _AREA_ac.  The first
 * table has 13 entries (sub-areas 1..13), the second 18 (0..17).  Every arm
 * returns 0 through one shared epilogue, which is why the reference reaches it
 * with `bl .L2fda` (the assembler's long-branch form) from eleven places.
 *
 * TWO IDIOMS COPIED, NOT RE-DERIVED, both from the landed sibling
 * ovl_30_c_c_c_c_c_a.c and both load-bearing here:
 *   - `__MapActor_GetActor(n)->f8 >> 20` and `->f10 >> 20` written as TWO
 *     SEPARATE calls per coordinate pair.  The reference fetches the same actor
 *     twice, once per coordinate; fetching it once into a local drops a call.
 *   - the six-argument callees' two stack arguments NAMED as locals where the
 *     reference builds both into separate registers before storing either.
 *
 * THREE DEFECTS FOUND AND CLOSED, with figures.
 *
 *   1. THE AREA IDS ARE SYMBOLS, NOT LITERALS -- the single biggest find, and it
 *      came from elevation.md's "a pooled constant that FITS a thumb immediate
 *      is a symbol tell".  The reference reaches 0xac, 0xad and 0xb0 with
 *      `ldr rN, =`, and Thumb `cmp Rn,#imm8` covers 0..255, so gcc had no reason
 *      to pool any of them.  All three were already in area.sym.  Spelling the
 *      three tests and the two stores as `(int)&_AREA_ac` / `_AREA_ad` /
 *      `_AREA_b0` is what makes the entry block and the second table's first arm
 *      come out right.  This gives ours an R_ARM_ABS32 where the ROM has a bare
 *      literal -- the relocation-FORM non-residue, benign.
 *
 *   2. A POOLED **ONE**, not a pooled zero.  `*(short *)(g + 0x242) = 1;`
 *      compiles to `ldrh r2, .L59` -- the literal 1 in the pool -- because
 *      `*thumb_movhi_insn` lists its memory alternative BEFORE its immediate
 *      one.  The reference has `mov r2, #1`.  An int carrier (`one = 1;` then
 *      store `one`) closes it at both of the two sites.  This is the batch-306
 *      pooled-zero defect with a NON-ZERO value, which matters: elevation.md's
 *      rule that `ldr rN, =0` is not a symbol tell while any other small pooled
 *      value is one would, read literally, send you hunting area.sym for a
 *      symbol whose value is 1.  The discriminator is the STORE WIDTH, not the
 *      value: a halfword store of any small constant can pool it.
 *
 *   3. ONE gState REFERENCE FOLDED to `=gState+450`.  The `gState[0xe1] != 5`
 *      test deep inside the second table's 4/5 arm is far from the function's
 *      base-pointer local, and gcc folded base+offset into one pool word where
 *      the reference re-loads `=gState` and rebuilds the offset.  A fresh local
 *      base assigned at that point (`h = gState;`) restores the reference's
 *      shape.  This is the known gState-fold lever; what is worth recording is
 *      that ONE function can need the local base TWICE, because the first local's
 *      live range ends at the dispatch.
 *
 *      1+2+3 together:  +56 / +26  ->  +44 / +21.
 *
 * AND THEN THE SURPRISE, WHICH IS THE OPPOSITE OF A LEVER: FOUR NAMED LOCALS
 * HAD TO BE **UN-NAMED**.  Replacing `m = 0xfe` (five `&= m` sites in the
 * 13/14 arm) with the literal at each site went +44/+21 -> +12/+5, 61.6% ->
 * 65.1% aligned, 171 -> 151 hunks.  Un-naming `e = 3` took it to +8/+3; three
 * further un-namings (`m = 2`, `f = 0xfe`, in the 4/5 arm) were byte-neutral but
 * are kept un-named because they cost nothing and they make the prologue
 * argument honest.  The combined candidate is +8/+3 at 66.4% aligned, 146 hunks,
 * AND ITS PROLOGUE NOW MATCHES THE REFERENCE EXACTLY --
 * `push {r5,r6,r7,lr} / mov r7,r8 / push {r7}`, one high register saved, where
 * the all-named version saved two (r8 and sl).
 *
 * This is the band doc's "changing the SET of long-lived quantities" lever
 * running in the SUBTRACTIVE direction, and the band doc only ever measured it
 * additively ("count the reference's parked constants, then make your candidate
 * hold that many" -- +20/+10 -> +4/+1 by naming eight and assigning them at the
 * top).  On this function the reference holds FEWER long-lived quantities than
 * the naive candidate, so the same lever is "name FEWER".  The band-entry rule
 * should be stated neutrally: MAKE THE COUNT EQUAL, in whichever direction.
 *
 * ONE PROBE THAT WAS WORSE, recorded so it is not retried: un-naming the
 * function pointer (`fp = OvlFunc_964_2009a98`, three `->f6c =` sites in the 4/5
 * arm) measured +16/+7 against +8/+3.  So the subtractive lever is not
 * "un-name everything" -- it tracks the reference's own parked set, and that
 * pointer IS parked in the reference (r5).
 *
 * ONE PROBE THAT WAS BYTE-IDENTICAL: storing the literal 0 instead of the
 * __GetFlag result in the 4/5 arm's eight `->f55` / `->f64` stores.  The
 * reference stores r7, the flag's own return register, at all eight -- but that
 * is cse1's `record_jump_equiv` substituting a register it has PROVED equal to
 * 0 on the `beq` edge, not evidence that the source stored the flag.  Both
 * spellings give the identical object.  Per the discipline an inert spelling is
 * UNTESTED, not disproved; what IS established is that this residue is not here.
 *
 * ================== THE BLOCKER, ATTRIBUTED, AND IT IS EXCEPTIONLESS ==========
 *
 * The whole remaining +8/+3 is the band's constant-commoning mechanism, and this
 * function let it be pinned down exactly.  See repro_commoning.c in this directory
 * for the four-function reproduction; the short form is:
 *
 *   At RTL expand time, a CONST_INT that Thumb can load with one `mov #imm8` is
 *   written DIRECTLY INTO THE HARD ARGUMENT REGISTER.  Any constant that cannot
 *   be -- a wide `mov`+`lsl` pair, or a pool load -- is routed through a FRESH
 *   PSEUDO and then copied to the argument register.  cse.c's
 *   `invalidate_for_call` invalidates HARD REGISTERS ONLY.  So the narrow class
 *   is rebuilt at every call site, and the pseudo classes survive every call and
 *   are commoned by cse1 inside the basic block.  Register allocation must then
 *   park that pseudo somewhere call-saved, and past r5/r6/r7 it goes to r8-r11,
 *   which Thumb-1 cannot use as operands.
 *
 * MEASURED ON THIS FUNCTION, and the right instrument is the COPY COUNT, one
 * grep: `mov rARG, rSAVED` is a commoned value entering argument position.
 *
 *   cand  size  count  copies   prologue high-register saves
 *   v1    +56    +26     46     r8 AND sl  (two)
 *   v2    +44    +21     46     r8 AND sl  (two)
 *   v4    +12     +5     33     r8 only -- MATCHES THE REFERENCE
 *   v7     +8     +3     33     r8 only
 *   v10    +8     +3     33     r8 only        <- this file
 *   ref     --     --     10     r8 only
 *
 * The count fell 46 -> 33 at exactly the edit that fixed the prologue, which is
 * the same edit that moved size and count from +44/+21 to +12/+5.  It is coarse
 * at the end -- it cannot tell v4 from v10 -- so use it to find the structural
 * gap and then go back to size-and-count.
 *
 * A COUNT THAT MISLED ME, RECORDED BECAUSE IT NEARLY REACHED THIS LINE.  I first
 * measured commoning by which register each wide constant is BUILT into, and got
 * reference 41 builds all into r0-r3 with ZERO callee-saved, against ours 27
 * builds with SIX callee-saved -- and read it as "the references never common a
 * wide argument constant".  Those numbers are real and that reading is WRONG.
 * Counting builds counts the wrong thing: a value built ONCE and copied EIGHT
 * times is one build and eight copies.  Tracing a single value through
 * OvlFunc_888_200888c caught it -- 0x90<<5 there is built into r5 three times
 * and fed to arguments eight times.  The references DO common; 959_200b054
 * commons harder than we do, 57 copies out of one r5 on a `push {r5,lr}`
 * prologue.  repro_commoning.c carries the retraction and the corrected table.
 *
 * So the residue is not "commoning versus none" but HOW MANY DISTINCT VALUES
 * get commoned: the reference concentrates 10 copies in r5 and r8, ours spreads
 * 33 across r5, r6, r7 and r8.  The 14 wide builds we do not make still show up
 * in the immediate multiset as 11 missing `mov` bases and 11 missing `lsl`
 * shifts, ours-short-never-over -- the signature brief D predicted, and here
 * every deficit is a __MapActor_SetPos coordinate: 0x88 0x98 0xba 0xbc 0xbe 0xc2
 * 0xc6 0xc8 0xcc 0xe8, shifts 16/17/18.  The reference rebuilds `0xba<<18` and
 * `0x88<<16` SEVEN INSTRUCTIONS APART IN ONE BASIC BLOCK with one call between,
 * while cse1 commons them for us; that pair alone rules out distance,
 * call-boundary and block-extent explanations.
 *
 * NO SOURCE SPELLING REACHES THE REMAINDER, and now there is a reason rather
 * than a tally of failures: the pseudo is created by the EXPANDER from the
 * constant's VALUE and its ARGUMENT POSITION, before any pass a source shape
 * could steer, and the commoning then operates on a pseudo the source never
 * named.  That PREDICTS the band doc's two negative results instead of merely
 * recording them -- lever 2 (one-variable-per-region) and declaration order are
 * byte-identical because "region-scoping cannot reach a pseudo the compiler
 * invented" is exactly this pseudo.  It also predicts the subtractive result
 * above: the only thing source CAN change is how many OTHER long-lived
 * quantities compete with the commoned pseudos for r5/r6/r7, which is why
 * un-naming four locals bought the prologue and then stopped buying anything.
 *
 * NOT A FLAG CASE, and not a REG_ALLOC_ORDER case.  The band doc's twelve
 * settings are inert for the reason above (they tune cse's reach ACROSS blocks;
 * this is intra-block).  `-ffixed-r8..r11` is the known masking flag and is not
 * retried here -- this function's REFERENCE has the high-save prologue and
 * spends r8 itself, so denying the high bank would deny the reference's own
 * shape.
 *
 * REMAINING NON-RESIDUE, stated so the next reader does not chase it: most of
 * the 146 hunks are POOL PLACEMENT.  The reference carries the entry block's
 * five pool loads all the way to offset 0x3d4 (its hand-written `.pool_aligned`
 * sits after the first table's arms); gcc dumps the same words at 0x244.  Every
 * `ldr [pc,#N]` in the function then differs, and so do the jump tables' own
 * `.word` offsets.  That is why aligncmp reads only 66.4% while objcmp says +3
 * encodings, and it is why the size-and-count pair is the ranking axis here.
 * The POOL CONTENT is correct: every numeric literal and every symbol matches
 * the reference entry for entry (checked with cmpconst.py; the apparent
 * differences in its output are hex-versus-decimal spellings of the same
 * values).
 */
struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x23 - 0x14];
    unsigned char f23;
    unsigned char pad24[0x55 - 0x24];
    unsigned char f55;
    unsigned char pad56[0x59 - 0x56];
    unsigned char f59;
    unsigned char pad5a[0x64 - 0x5a];
    short f64;
    unsigned char pad66[0x6c - 0x66];
    void (*f6c)(void);
};

extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;
extern int _AREA_ac;
extern int _AREA_ad;
extern int _AREA_b0;

extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8092950(int a, int b);
extern void __Func_8092b08(int a, int b);
extern void __CutsceneWait(int n);
extern void __StartTask(void (*f)(void), int n);
extern void __Func_8091494(int a);
extern void OvlFunc_964_2009abc(int a);
extern void OvlFunc_964_2009fdc(void);
extern void OvlFunc_964_200a3a0(void);
extern void OvlFunc_964_200a410(void);
extern void OvlFunc_964_200a480(void);
extern void OvlFunc_964_200a52c(void);
extern void OvlFunc_964_2008e20(void);
extern void OvlFunc_964_2008ec8(void);
extern void OvlFunc_964_2009a98(void);

int OvlFunc_964_200a59c(void)
{
    unsigned char *g;
    unsigned char *h;
    int e, f, m, v, one;
    void (*fp)(void);

    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x81 << 2;
    g = gState;
    if (*(short *)(g + (0xe0 << 1)) == (int)&_AREA_ac
     || *(short *)(g + (0xe0 << 1)) == (int)&_AREA_ad) {
        __Func_8091494(0);
        one = 1;
        *(short *)(g + 0x242) = one;
        *(short *)(g + (0x90 << 2)) = (int)&_AREA_ac;
    }
    if (*(short *)(g + (0xe0 << 1)) == (int)&_AREA_ac) {
        switch (*(short *)(g + (0xe1 << 1))) {
        case 1:
        case 2:
            if (__GetFlag(0x982)) {
                e = 5;
                f = 8;
                __CopyMapTiles(0x79, 4, 0x4a, 9, e, f);
                    f = 2;
                __CopyMapTiles(0x12, 0x53, 9, 0x49, e, f);
                __CopyMapTiles(0x12, 0x51, 9, 0x4b, e, f);
                __CopyMapTiles(0x12, 0x53, 9, 0x4d, e, f);
                __CopyMapTiles(0x12, 0x53, 9, 0x4f, e, f);
                __CopyMapTiles(0x12, 0x53, 0xb, 0x4e, e, f);
                __CopyMapTiles(0x12, 0x53, 0xd, 0x4f, e, f);
                return 0;
            }
            if (!__GetFlag(0x983))
                return 0;
            e = 5;
            f = 8;
            __CopyMapTiles(0x79, 0xd, 0x4a, 9, e, f);
            f = 2;
            __CopyMapTiles(0x12, 0x55, 0xb, 0x4a, e, f);
            __CopyMapTiles(0x12, 0x53, 0xd, 0x4b, e, f);
            __CopyMapTiles(0x12, 0x55, 0xb, 0x4c, e, f);
            __CopyMapTiles(0x12, 0x53, 0xb, 0x4e, e, f);
            __CopyMapTiles(0x12, 0x53, 0xd, 0x4f, e, f);
            return 0;
        case 3:
        case 4:
            OvlFunc_964_200a3a0();
            v = 0;
            __MapActor_GetActor(8)->f55 = v;
            __MapActor_GetActor(9)->f55 = v;
            __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
            __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
            fp = OvlFunc_964_2008ec8;
            __MapActor_GetActor(8)->f6c = fp;
            __MapActor_GetActor(9)->f6c = fp;
            __StartTask(OvlFunc_964_2008e20, 0xc8 << 4);
            return 0;
        case 5:
        case 6:
        case 7:
            if (__GetFlag(0x982)) {
                e = 0x1e;
                f = 8;
                __Func_80105d4(0x17, 0x11, 1, 2, e, f);
            }
            if (!__GetFlag(0x983))
                return 0;
            e = 0x20;
            f = 0xa;
            __Func_80105d4(0x17, 0x11, 1, 2, e, f);
            return 0;
        case 8:
        case 9:
            OvlFunc_964_200a410();
            v = 0;
            __MapActor_GetActor(0xa)->f55 = v;
            __MapActor_GetActor(0xb)->f55 = v;
            __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
            __Actor_SetSpriteFlags(__MapActor_GetActor(0xb), 0);
            fp = OvlFunc_964_2008ec8;
            __MapActor_GetActor(0xa)->f6c = fp;
            __MapActor_GetActor(0xb)->f6c = fp;
            __StartTask(OvlFunc_964_2008e20, 0xc8 << 4);
            return 0;
        case 10:
        case 11:
            __Actor_SetSpriteFlags(__MapActor_GetActor(0x12), 0);
            __Actor_SetSpriteFlags(__MapActor_GetActor(0x13), 0);
            __MapActor_SetAnim(0x12, 2);
            __Actor_SetSpriteFlags(__MapActor_GetActor(0x14), 0);
            __Actor_SetSpriteFlags(__MapActor_GetActor(0x15), 0);
            __Func_8092950(0x14, 0xf);
            __Func_8092950(0x15, 0xf);
            if (__GetFlag(0x971)) {
                e = 1;
                f = 3;
                __CopyMapTiles(0x3b, 8, 0x31, 8, e, f);
                f = 8;
                __Func_8010704(0x33, 8, 1, 1, 0x31, f);
                __MapActor_GetActor(0x12)->f23 |= 2;
                __MapActor_SetAnim(0x12, 3);
                __Func_8010704(0x2d, 4, 1, 1, 0x2e, f);
                __MapActor_SetPos(0x12, 0xba << 18, 0x88 << 16);
                __MapActor_GetActor(0x12)->fc = 0xfff00000;
                __MapActor_SetPos(0x14, 0xba << 18, 0x88 << 16);
            }
            if (__GetFlag(0x80 << 2)) {
                __Func_8092950(0x14, 0);
                __MapActor_SetAnim(0x14, 5);
            }
            if (__GetFlag(0x202))
                __MapActor_SetAnim(0x13, 2);
            if (__GetFlag(0x972)) {
                e = 1;
                f = 3;
                __CopyMapTiles(0x3b, 8, 0x2d, 0xe, e, f);
                f = 0xe;
                __Func_8010704(0x33, 8, 1, 1, 0x2d, f);
                __MapActor_GetActor(0x13)->f23 |= 2;
                __MapActor_SetAnim(0x13, 3);
                __Func_8010704(0x2d, 4, 1, 1, 0x30, f);
                __MapActor_SetPos(0x13, 0xc2 << 18, 0xe8 << 16);
                __MapActor_GetActor(0x13)->fc = 0xfff00000;
                __MapActor_SetPos(0x15, 0xc2 << 18, 0xe8 << 16);
                __SetFlag(0x202);
            }
            if (!__GetFlag(0x201))
                return 0;
            __Func_8092950(0x15, 0);
            __MapActor_SetAnim(0x15, 5);
            return 0;
        case 12:
        case 13:
            OvlFunc_964_200a480();
            v = 0;
            __MapActor_GetActor(0xc)->f55 = v;
            __MapActor_GetActor(0xd)->f55 = v;
            __Actor_SetSpriteFlags(__MapActor_GetActor(0xf), 0);
            __Actor_SetSpriteFlags(__MapActor_GetActor(0x10), 0);
            __Actor_SetSpriteFlags(__MapActor_GetActor(0x11), 0);
            __Actor_SetSpriteFlags(__MapActor_GetActor(0xc), 0);
            __Actor_SetSpriteFlags(__MapActor_GetActor(0xd), 0);
            __Actor_SetSpriteFlags(__MapActor_GetActor(0xe), 0);
            fp = OvlFunc_964_2008ec8;
            __MapActor_GetActor(0xc)->f6c = fp;
            __MapActor_GetActor(0xd)->f6c = fp;
            __MapActor_GetActor(0xe)->f6c = fp;
            __StartTask(OvlFunc_964_2008e20, 0xc8 << 4);
            return 0;
        }
        return 0;
    }
    switch (*(short *)(g + (0xe1 << 1))) {
    case 1:
    case 2:
    case 3:
        one = 1;
        *(short *)(g + 0x242) = one;
        *(short *)(g + (0x90 << 2)) = (int)&_AREA_b0;
        __ClearFlag(0x12f);
        __Func_8092950(0x11, 6);
        __Func_8092950(0x12, 6);
        if (__GetFlag(0x974))
            __MapActor_SetPos(0x11, 0xb6 << 18, 0x9c << 17);
        if (__GetFlag(0x975))
            __MapActor_SetPos(0x12, 0xba << 18, 0x9c << 17);
        OvlFunc_964_200a52c();
        return 0;
    case 4:
    case 5:
        v = __GetFlag(0x109);
        if (!v) {
            __MapActor_GetActor(0xa)->f55 = v;
            __MapActor_GetActor(0xb)->f55 = v;
            __MapActor_GetActor(0xa)->fc = 0xffd00000;
            __MapActor_GetActor(0xb)->fc = 0xffd00000;
            __MapActor_GetActor(0xa)->f23 |= 2;
            __MapActor_GetActor(0xb)->f23 |= 2;
            __MapActor_GetActor(0xa)->f59 &= 0xfe;
            __MapActor_GetActor(0xb)->f59 &= 0xfe;
            __MapActor_GetActor(0xa)->f64 = 3;
            __MapActor_GetActor(0xb)->f64 = 3;
            __Func_8092b08(0xa, 1);
            __Func_8092b08(0xb, 1);
            __MapActor_GetActor(0xc)->f55 = v;
            __MapActor_GetActor(0xd)->f55 = v;
            __MapActor_GetActor(0xe)->f55 = v;
            __Actor_SetSpriteFlags(__MapActor_GetActor(0xc), 0);
            __Actor_SetSpriteFlags(__MapActor_GetActor(0xd), 0);
            __Actor_SetSpriteFlags(__MapActor_GetActor(0xe), 0);
            __MapActor_GetActor(0xc)->f64 = v;
            __MapActor_GetActor(0xd)->f64 = v;
            __MapActor_GetActor(0xe)->f64 = v;
            h = gState;
            if (*(short *)(h + (0xe1 << 1)) != 5)
                return 0;
            __MapActor_GetActor(0xa)->fc = 0xffe00000;
            __MapActor_GetActor(0xb)->fc = 0xffc00000;
            __MapActor_GetActor(0xa)->f64 = 2;
            __MapActor_GetActor(0xb)->f64 = 4;
            __MapActor_SetPos(0xc, 0xc8 << 16, 0x98 << 16);
            __MapActor_GetActor(0xc)->f64 = 0xb;
            fp = OvlFunc_964_2009a98;
            __MapActor_GetActor(0xc)->f6c = fp;
            __MapActor_GetActor(0xc)->f23 |= 2;
            __MapActor_SetPos(0xd, 0xc8 << 16, 0x98 << 16);
            __MapActor_GetActor(0xd)->f64 = 0xc;
            __MapActor_GetActor(0xd)->f6c = fp;
            __MapActor_GetActor(0xd)->f23 |= 2;
            __MapActor_SetPos(0xe, 0x88 << 16, 0x98 << 16);
            __MapActor_GetActor(0xe)->f64 = 0xa;
            __MapActor_GetActor(0xe)->f6c = fp;
            __MapActor_GetActor(0xe)->f23 |= 2;
            __CutsceneWait(2);
            __SetFlag(0x80 << 2);
            __SetFlag(0x201);
            __SetFlag(0x202);
        }
        OvlFunc_964_2009abc(0);
        return 0;
    case 6:
    case 7:
        __Func_8092b08(8, 1);
        v = 0;
        __MapActor_GetActor(8)->f55 = v;
        __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
        __Func_8092b08(9, 1);
        __Func_8092950(9, 0xf);
        __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
        __MapActor_GetActor(9)->f55 = v;
        if (!__GetFlag(0x81 << 2))
            return 0;
        __Func_8092950(9, 0);
        __MapActor_SetAnim(9, 5);
        __Func_8010704(0x1a, 8, 1, 1,
                       __MapActor_GetActor(9)->f8 >> 20,
                       __MapActor_GetActor(9)->f10 >> 20);
        fp = OvlFunc_964_2008ec8;
        __MapActor_GetActor(9)->f6c = fp;
        __MapActor_GetActor(8)->f6c = fp;
        return 0;
    case 8:
    case 9:
    case 10:
    case 11:
        if (__GetFlag(0x982)) {
            e = 0x10;
            f = 0x1e;
            __Func_80105d4(0xa, 0x1e, 1, 2, e, f);
        }
        if (__GetFlag(0x983)) {
            e = 0x16;
            f = 0x1e;
            __Func_80105d4(0xa, 0x1e, 1, 2, e, f);
        }
        __SetFlag(0x973);
        return 0;
    case 12:
        e = 8;
        f = 0x71;
        __Func_8010704(8, 0x31, 1, 1, e, f);
        OvlFunc_964_2009fdc();
        __StartTask(OvlFunc_964_2008e20, 0xc8 << 4);
        return 0;
    case 13:
    case 14:
        __CutsceneWait(1);
        if (__GetFlag(0x984)) {
            e = 0x20;
            f = 0x2e;
            __Func_80105d4(0x18, 0x3b, 1, 2, e, f);
            __MapActor_SetPos(0x13, 0xcc << 17, 0xc6 << 18);
            __MapActor_SetPos(0x14, 0xbc << 17, 0xc6 << 18);
            __MapActor_SetPos(0x15, 0xcc << 17, 0xbe << 18);
            __MapActor_SetPos(0x16, 0xbc << 17, 0xbe << 18);
            __MapActor_SetPos(0x17, 0xc4 << 17, 0xc2 << 18);
        }
        __MapActor_GetActor(0x13)->f55 &= 0xfe;
        __MapActor_GetActor(0x14)->f55 &= 0xfe;
        __MapActor_GetActor(0x15)->f55 &= 0xfe;
        __MapActor_GetActor(0x16)->f55 &= 0xfe;
        __MapActor_GetActor(0x17)->f55 &= 0xfe;
        __Func_8092950(0x13, 4);
        __Func_8092950(0x14, 1);
        __Func_8092950(0x15, 4);
        __Func_8092950(0x16, 0xa);
        __Func_8092950(0x17, 0);
        __MapActor_SetAnim(0x13, 2);
        __MapActor_SetAnim(0x17, 2);
        __Func_8010704(0x14, 0x38, 1, 1,
                       __MapActor_GetActor(0x13)->f8 >> 20,
                       __MapActor_GetActor(0x13)->f10 >> 20);
        __Func_8010704(0x14, 0x38, 1, 1,
                       __MapActor_GetActor(0x14)->f8 >> 20,
                       __MapActor_GetActor(0x14)->f10 >> 20);
        __Func_8010704(0x14, 0x38, 1, 1,
                       __MapActor_GetActor(0x15)->f8 >> 20,
                       __MapActor_GetActor(0x15)->f10 >> 20);
        __Func_8010704(0x14, 0x38, 1, 1,
                       __MapActor_GetActor(0x16)->f8 >> 20,
                       __MapActor_GetActor(0x16)->f10 >> 20);
        __Func_8010704(0x14, 0x38, 1, 1,
                       __MapActor_GetActor(0x17)->f8 >> 20,
                       __MapActor_GetActor(0x17)->f10 >> 20);
        return 0;
    case 17:
        e = 0x31;
        f = 0x6b;
        __Func_8010704(0x31, 0x2b, 1, 1, e, f);
        OvlFunc_964_2009fdc();
        return 0;
    }
    return 0;
}
