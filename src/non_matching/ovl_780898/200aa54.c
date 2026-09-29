/* OvlFunc_883_200aa54 -- NON-MATCHING, 215 of 243 encodings differ.
 * Unattempted before batch 298.  Reference asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_a_a_a.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_780898/200aa54.c \
 *       asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_a_a_a.s --func OvlFunc_883_200aa54
 *
 * Size 604 both and count 243 both, so this IS a distance; aligncmp reads 196 of 243
 * (54 differing in 27 hunks), and the relocations are the same 50 symbols in the
 * same ORDER with offsets 2-6 bytes early -- i.e. a size shift, not a layout
 * difference.  PIN-FREE.  Split needed.
 * BLOCKER: cse.c, NOT gcse -- two repeated flag ids hoisted into callee-saved
 * registers.  -fno-cse-skip-blocks and -fno-rerun-cse-after-loop each remove both;
 * -fno-gcse does not.  That is the third time in three batches a hold that looked
 * like gcse was cse's, so check .03.cse before .07.gcse.  Flag figures are
 * DIAGNOSTIC only: under -fno-rerun-cse-after-loop it reads 211 aligned but 241
 * instructions, so neither state is exact and no flag row is warranted.
 * What closed the LENGTH: writing the three-way sprite constant as a store in EVERY
 * arm and letting cross-jumping merge the tails -- the cross-jumping hazard used
 * forwards for once, rather than avoided.
 */
/* OvlFunc_883_200aa54 -- asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_a_a_a.s,
 * 0x0200aa54, 229 ROM lines.  A map-setup cutscene entry: reads save bits
 * 0x109, 0x204, 0x210, 0x308 and sets 0x308.
 *
 * NON-MATCHING: 215 encodings of 243 differ (objcmp), production flags.
 *
 * 215 IS NOT A DISTANCE, AND THE TWO FIGURES THAT ARE: objcmp prints no SIZE
 * line -- 604 bytes in both -- and ref 243 against ours 243, so LENGTH AND SIZE
 * ARE BOTH EXACT.  But RELOCATIONS still differ, by 2-6 bytes of offset, so the
 * stream is locally shifted and the index-by-index count saturates.  The
 * alignment-tolerant measure is the real one: 196 of 243 aligned-equal, 54
 * differing in 27 hunks (tools/aligncmp.py).
 *
 * THE RE IS COMPLETE.  All FIFTY relocations are the same symbols in the same
 * order -- 45 calls and 5 word constants -- and every call target, flag id,
 * actor slot, tile-copy argument and field offset reproduces.
 *
 * SHIMS: none (tools/shimcount.py: no pins, no .equ shims, no "+r" barriers).
 * SPLIT: text-only; tools/datacheck.py prints nothing.  The .s holds TWO
 * functions by the anchored .thumb_func_start pattern (OvlFunc_883_20095dc and
 * this one), so landing needs a split.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/overlays/200aa54.c \
 *     asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_a_a_a.s \
 *     --func OvlFunc_883_200aa54
 *
 * ============================================================
 * THE DOMINANT RESIDUE IS ONE MECHANISM, AND IT IS A cse.c PASS, NOT gcse.
 *
 * TWO REPEATED FLAG IDS GET HOISTED INTO CALLEE-SAVED REGISTERS.  The ROM
 * rebuilds each one at each site; we build it once and copy:
 *
 *   0x815, pooled:   rom   ldr r0,=0x815  ... (again) ldr r0,=0x815
 *                    ours  ldr r6,=0x815 / adds r0,r6,#0  ... adds r0,r6,#0
 *   0x308, shifted:  rom   movs r0,#0xc2 / lsls r0,#2  ... (again) the same pair
 *                    ours  movs r5,#0xc2 / lsls r5,#2 / adds r0,r5,#0 ... adds r0,r5,#0
 *
 * That is also why our prologue reads `push {r5, r6, lr}` against the ROM's
 * `push {r5, lr}`, and why the whole gState block sits one register across
 * (ours r1/r2 where the ROM has r2/r1) -- one extra callee-saved allocno.
 *
 * PASS: cse.c.  Measured against six flags: -fno-cse-skip-blocks AND
 * -fno-rerun-cse-after-loop each remove BOTH hoists; -fno-gcse,
 * -fno-cse-follow-jumps, -fno-expensive-optimizations and -fno-schedule-insns2
 * leave them untouched.  So this is NOT the gcse cprop class that
 * asm/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c_c_b.o books, and -fno-gcse is
 * the wrong rule for it.  It IS the class the tree's ten-odd
 * -fno-rerun-cse-after-loop rules book.
 *
 * NINE SPELLINGS WERE TRIED AGAINST IT AND ALL ARE INERT: the 0x308 pair as a
 * nested `if`, as a `goto` to a shared exit, the 0x815 pair with the first test
 * stored into a local, and six orderings around them.  The two uses are always
 * in the same cse block because cse.c ends one only at a CODE_LABEL and
 * cse_skip_blocks walks past the conditional jump between them.
 *
 * FLAG-CONDITIONAL FIGURE, LABELLED AS SUCH: with -fno-rerun-cse-after-loop
 * this candidate reads 211 of 243 aligned-equal, 36 differing in 15 hunks --
 * better structurally -- but the instruction count drops to 241, because the
 * flag costs two instructions of its own elsewhere.  NEITHER STATE IS EXACT, so
 * this is a park either way and the flag is NOT a recommendation yet.  Anyone
 * adding the Makefile rule must close the two shortfalls below first.
 *
 * ============================================================
 * THE TWO REMAINING SHORTFALLS (each 1 instruction, both plateaus).
 *
 *  1. THE SECOND gState BYTE READ GETS FOLDED INTO A REGISTER-OFFSET LOAD.
 *        rom   adds r3, r2, r1 / ldrb r1, [r3]
 *        ours  ldrb r1, [r1, r3]
 *     combine merges `p = base + off; *p` when `off` was just incremented.
 *     Inert: bumping `off` after the first load (worse, 228), a second pointer
 *     variable, an `unsigned char *` carrier instead of an `unsigned int`
 *     (that one DID fix the first load, 179 -> 186 aligned).
 *  2. THE SECOND ACTOR BYTE POINTER IS DERIVED FROM THE FIRST.
 *        rom   adds r1,r5,#0 / adds r1,#0x22 / strb / adds r3,r5,#0 /
 *              movs r2,#0 / adds r3,#0x55 / movs r1,#0xc8 / strb
 *        ours  adds r2,r5,#0 / adds r2,#0x22 / strb / adds r2,#0x33 / ... / strb
 *     The ROM rebuilds each pointer from the actor; gcc spots 0x55 = 0x22+0x33.
 *     Inert: two named pointer variables, `q = actor; q += 0x55;`.
 *
 * ============================================================
 * WHAT WAS READ OUT OF THE ROM AND IS LOAD-BEARING.
 *
 *  - IT RETURNS int 0.  `mov r0,#0 / add sp,#8 / pop {r5} / pop {r1} / bx r1`:
 *    the return address is popped into r1, which names a return value, and both
 *    exits funnel to the same `mov r0,#0`.
 *  - THE gState READ IS THE OVERLAY'S ESTABLISHED IDIOM, already landed in
 *    src/overlays/rom_780898/ovl_30_c_c_a_a_a_b.c: an `unsigned int` base, an
 *    offset built as `0xe1 << 1`, and the load as
 *    `*(short *)((char *)p + off)` with `off` re-zeroed -- that zero variable is
 *    the ROM's `mov r1,#0` and the load is `ldrsh r3,[r3,r1]`.  Read THREE
 *    times in this function; the first keeps the base alive (`adds r3,r2,r1`,
 *    three-operand) because 0x205/0x206 are read from it, the other two
 *    consume it (`add r3,r2`).
 *  - THE THREE-WAY SPRITE CONSTANT MUST BE WRITTEN AS A STORE IN EVERY ARM,
 *    AND THIS IS WHAT CLOSED THE LENGTH.  The ROM has one `lsl r3,#17` and one
 *    `str r3,[r5,#8]` at a join reached by `b` from each arm.  Selecting the
 *    constant into a local and storing once after the join is TWO INSTRUCTIONS
 *    SHORT (241): gcc hoists each `movs r3,#imm` above its own `cmp` and
 *    inverts the branch, dropping both `b`s.  A `goto` to an explicit join is
 *    inert.  Writing `*(int *)(actor + 8) = K << 17;` in all three arms and
 *    letting CROSS-JUMPING merge the tails gives the ROM's shape exactly --
 *    241 -> 243, size exact.  This is the cross-jumping warning used FORWARDS:
 *    the ROM merges here, so the source must present arms to merge.
 *  - THE TILE-COPY PAIR SHARES ONE CALLEE-SAVED `1`.  `mov r5,#1` serves the
 *    6th argument of the first __CopyMapTiles and both stacked arguments of the
 *    second, so it is one named local; the first call's 5th argument (2) is a
 *    plain literal in r3.  __Func_8010704's two stacked arguments sit in r3/r2,
 *    CALLER-saved, so those are literals too -- naming them costs a register.
 *  - `__MapActor_GetActor(0x15)` IS CALLED TWICE IN A ROW in the ROM, once for
 *    the saved pointer and once as the argument of __Actor_SetSpriteFlags.  A
 *    call is not a CSE candidate, so writing it twice is what the ROM has.
 *  - THE 0x14/0x15 ACTOR SLOT IS `0x14 + (__GetFlag(0x87a) != 0)`, which is the
 *    `neg r0,r3 / orr r0,r3 / lsr r0,#31` idiom.  Here the plain `!= 0` gives
 *    it, because __GetFlag returns an opaque int and gcc cannot see a single
 *    bit -- the statement-level-branch rule in docs/elevation.md applies to
 *    masked bit tests, not to this.
 *  - THE 0x109/0x823 PAIR IS AN if/else WITH THE 0x109 BODY SECOND.  The ROM
 *    lays the 0x823 test and its body first and `.L2bc8` after, so the source
 *    tests `__GetFlag(0x109) == 0` and puts the 0x5b store in the `else`.
 *  - THE 0x5b STORE WRITES THE 0x815 FLAG VARIABLE, NOT A LITERAL ZERO:
 *    `strb r5, [r0]` reuses the register holding that __GetFlag result, which
 *    is provably 0 on this path.
 *
 * MEASURED, 12 spellings (aligned-equal of 243):
 *   store in every arm for the three-way            196   <-- this file
 *   `unsigned char *` carrier for the byte reads     186
 *   conditional select into a local + one store      179, and 241 instructions
 *   `goto` join for the three-way                    179
 *   two named pointers for the 0x22 / 0x55 stores    179
 *   `goto` shared exit for the 0x308 block           196 (inert)
 *   second pointer variable for the byte read        196 (inert)
 *   `q = actor; q += 0x55;`                          196 (inert)
 *   literal stacked args for __Func_8010704          inert
 *   dropping the first tile-copy's named 5th arg     inert
 *
 * NEXT: the two 1-instruction shortfalls above, then re-screen with
 * -fno-rerun-cse-after-loop.  If they close, this is a Makefile-rule landing of
 * the same shape as the ten existing -fno-rerun-cse-after-loop rules; if they
 * do not, it stays a cse.c park.
 */
extern int gState;
extern unsigned char gScript_883__0200e248[];
extern void OvlFunc_883_200d72c(void);
extern void OvlFunc_883_200da94(void);
extern void OvlFunc_883_200da40(void);
extern void OvlFunc_883_200b4c8(void);
extern void OvlFunc_883_200db48(int a);
extern void OvlFunc_883_200d950(void);
extern void OvlFunc_883_200bfb0(void);
extern void OvlFunc_883_2008d70(void);

extern void __SetUIColor(int a, int b);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *actor, int flags);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetBehavior(int slot, int s);
extern void __StartTask(void (*fn)(void), int arg);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8091ff0(int a);
extern void __Func_800fe9c(void);
extern void __WaitFrames(int n);

int OvlFunc_883_200aa54(void)
{
    unsigned char *actor;
    unsigned char *q;
    unsigned int base;
    unsigned int p;
    unsigned int off;
    int a;
    int b;
    int v;
    int f;
    int n;

    base = (unsigned int)&gState;
    off = 0xe1;
    off <<= 1;
    p = base + off;
    off = 0;
    if (*(short *)((char *)p + off) == 0x10) {
        off = 0x205;
        q = (unsigned char *)(base + off);
        off += 1;
        a = *q;
        q = (unsigned char *)(base + off);
        b = *q;
        __SetUIColor(a, b);
        OvlFunc_883_200b4c8();
        return 0;
    }
    if (__GetFlag(0xfd0) == 0) {
        if (__GetFlag(0x87a) == 0)
            OvlFunc_883_200db48(0x1a);
        else
            OvlFunc_883_200db48(0x14);
    }
    n = 1;
    __CopyMapTiles(2, 0x66, 0x54, 0x29, 2, n);
    __CopyMapTiles(1, 0x66, 0x53, 0x29, n, n);
    actor = __MapActor_GetActor(0x14 + (__GetFlag(0x87a) != 0));
    __Actor_SetSpriteFlags(actor, 0);
    if (__GetFlag(0x314) != 0)
        *(int *)(actor + 8) = 0xb5 << 17;
    else if (__GetFlag(0x316) != 0)
        *(int *)(actor + 8) = 0xc5 << 17;
    else
        *(int *)(actor + 8) = 0xbd << 17;
    *(int *)(actor + 0x10) = 0x92 << 18;
    *(int *)(actor + 0xc) = 0xc0 << 16;
    OvlFunc_883_200d950();
    q = actor + 0x22;
    *q = 3;
    q = actor + 0x55;
    *q = 0;
    __StartTask(OvlFunc_883_200da94, 0xc8 << 4);
    if (__GetFlag(0x87a) == 0) {
        if (__GetFlag(0x815) != 0) {
            actor = __MapActor_GetActor(0x15);
            __Actor_SetSpriteFlags(__MapActor_GetActor(0x15), 0);
            *(int *)(actor + 0x18) = 0x28f;
            *(int *)(actor + 0x1c) = 0x28f;
        }
        if (__GetFlag(0x808) != 0) {
            __MapActor_SetPos(0xf, 0, 0);
            __MapActor_SetPos(0x10, 0, 0);
            __MapActor_SetPos(0x11, 0, 0);
        }
        f = __GetFlag(0x815);
        if (f == 0) {
            if (__GetFlag(0x109) == 0) {
                if (__GetFlag(0x823) != 0) {
                    __MapActor_SetPos(0x16, 0x80 << 17, 0xe4 << 17);
                    *(void **)(__MapActor_GetActor(0x16) + 0x6c) = OvlFunc_883_200d72c;
                    __MapActor_SetBehavior(0x16, (int)gScript_883__0200e248);
                }
            } else {
                *(char *)(__MapActor_GetActor(0x16) + 0x5b) = f;
                __ClearFlag(0x241);
            }
            p = (unsigned int)&gState;
            off = 0xe1;
            off <<= 1;
            p += off;
            off = 0;
            if (*(short *)((char *)p + off) != 0x10 && __GetFlag(0x87a) == 0)
                __StartTask(OvlFunc_883_200da40, 0xc8 << 4);
        }
        if (__GetFlag(0x308) == 0) {
            p = (unsigned int)&gState;
            off = 0xe1;
            off <<= 1;
            p += off;
            off = 0;
            if (*(short *)((char *)p + off) == 0x11) {
                OvlFunc_883_200bfb0();
                __SetFlag(0x308);
            }
        }
    }
    if (__GetFlag(0x109) != 0) {
        if (__GetFlag(0x204) != 0) {
            __Func_8010704(0x31, 0x35, 8, 4, 0x14, 0x32);
        }
        if (__GetFlag(0x210) != 0)
            OvlFunc_883_2008d70();
    }
    __Func_8091ff0(0xaa);
    __Func_800fe9c();
    __WaitFrames(1);
    return 0;
}
