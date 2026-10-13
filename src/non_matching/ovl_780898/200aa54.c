/* OvlFunc_883_200aa54 -- PARKED, objcmp 40 of 243 (stream), aligncmp 29 in 16 hunks.
 * Reference asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_a_a_a.s.
 *
 * LENGTH, SIZE AND POOL ALL AGREE NOW: ref 184 16-bit instructions against ours
 * 184, 604 bytes against 604, 59 32-bit entries against 59.
 *
 * THE TWO HEADERS THIS REPLACES WERE BOTH WRONG ABOUT THEIR OWN FIGURE, AND THE
 * ERROR WAS LOAD-BEARING.  They read "Size 604 both and count 243 both, so this
 * IS a distance" and "ref 243 against ours 243, so LENGTH AND SIZE ARE BOTH
 * EXACT".  243 is objcmp's STREAM length -- 16-bit instructions plus 32-bit
 * entries (pool words AND `bl` calls) plus pads.  The 16-bit instruction counts
 * were 184 against 183, and our stream carried a trailing `.short 0x0000` pad
 * absorbing the difference, which is why both the byte size and the stream
 * length looked equal.  objcmp prints the real counts, and its own pad note, on
 * the two lines DIRECTLY ABOVE the figure both headers quoted.
 *
 * What that cost: both headers described "THE TWO REMAINING SHORTFALLS (each 1
 * instruction, both plateaus)" and attributed the whole residue to one cse.c
 * hoist.  Counted per region there were FIVE sites in THREE mechanisms, and two
 * of them cancel exactly -- see the table below.  Everything aimed at the cse
 * hoists was therefore aimed at an instruction-neutral site.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_780898/200aa54.c \
 *       asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_a_a_a.s \
 *       --func OvlFunc_883_200aa54
 *   docker run ... python3 tools/aligncmp.py \
 *       src/non_matching/ovl_780898/200aa54.c \
 *       asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_a_a_a.s \
 *       OvlFunc_883_200aa54 -v
 *
 * ============================================================
 * PER-REGION SPLIT, COUNTED SEPARATELY (batch 347 brief D)
 *
 *   site                                   inherited   now
 *   1  second gState byte read (ref[9:16])      -1       0   <-- CLOSED
 *   2  actor 0x22 / 0x55 pointer pair           -1      -1
 *   3  the 0x815 hoist, first site              +1      +1
 *   4  the 0x815 hoist, second site              0       0
 *   5  the 0x308 hoist (both sites together)     0       0   <-- NEUTRAL, not a cost
 *   net                                         -1       0
 *
 * So the 0x308 hoist costs NO instructions at all -- it pays `adds r0,r5,#0` at
 * the first site and saves a `movs`/`lsls` pair at the second -- and the 0x815
 * hoist costs exactly one.  Both still cost a callee-saved register, which is
 * why our prologue reads `push {r5, r6, lr}` against the ROM's `push {r5, lr}`.
 *
 * ============================================================
 * WHAT CLOSED THE LENGTH, AND WHY (batch 347)
 *
 * *** THE TWO gState BYTE READS MUST USE LITERAL OFFSETS, NOT AN OFFSET
 * VARIABLE. ***  The reference has
 *       ldr r1,=0x205 / adds r3,r2,r1 / adds r1,#1 / ldrb r0,[r3]
 *                     / adds r3,r2,r1 / ldrb r1,[r3]
 * -- a SEPARATE three-operand add before each load, and `adds r1,#1` for the
 * second constant.  The inherited body spelled the reads through an `off`
 * variable, which makes the address `(plus (reg base) (reg off))`: that IS a
 * legal Thumb ldrb address, so combine folds the add into the load and we get
 * `ldrb r1,[r1,r3]` -- one instruction short.  With literal offsets the address
 * is `(plus (reg base) (const_int 517))`, which no Thumb ldrb can encode, so
 * combine cannot fold it and the separate add survives.  The `adds r1,#1` then
 * falls out of reload_cse_move2add (reload1.c:8840-8916) because both constants
 * land in the same hard register.
 *
 * This is the EXACT MIRROR of the offset-variable idiom this overlay's landed
 * ovl_30_c_c_a_a_a_b.c uses for its reg+reg `ldrsh r3,[r3,r1]` loads, and both
 * are still needed in this one function: the halfword reads keep the `off`
 * variable (they WANT the fold), the two byte reads must not have one.
 * Measured: 183 -> 184 instructions, objcmp 215 -> 40, aligned 54 in 27 hunks
 * -> 29 in 16.  Writing the two reads inline in the __SetUIColor argument list
 * is bit-identical to writing them into `a` and `b` first.
 *
 * ============================================================
 * THE ONE REMAINING SHORTFALL: THE 0x22 / 0x55 ACTOR POINTER PAIR
 *
 *       rom   adds r1,r5,#0 / adds r1,#0x22 / strb / adds r3,r5,#0
 *             / movs r2,#0 / adds r3,#0x55 / strb
 *       ours  adds r2,r5,#0 / adds r2,#0x22 / strb / adds r2,#0x33 / ... / strb
 *
 * AND IT IS NOT A SPELLING -- IT IS reload_cse_move2add'S SECOND BRANCH,
 * reload1.c:8917-8990, which rewrites a repeated `(set REGX (REGY))
 * (set REGX (plus REGX A))` pattern into a delta when `reg_base_reg[regno] ==
 * REGNO (src)`.  It fires for us because BOTH pointers land in r2; the
 * reference puts them in r1 and r3, so there is no known base in the
 * destination and each pointer is rebuilt from the actor.  Measured on THIS
 * body, FIVE spellings are BIT-IDENTICAL -- two named pointers, `actor[0x22]`
 * array form, `*(actor + 0x22)`, a mixed form, and a zero carrier -- so the
 * source cannot reach it and the lever is hard-register allocation.
 * (The old header recorded "two named pointers ... 179" as WORSE; that figure
 * was measured on the pre-close body and has expired -- it is inert here.)
 *
 * ============================================================
 * THE 0x815 / 0x308 HOISTS: cse.c, AND NOW KNOWN TO BE NEARLY FREE
 *
 * The ROM rebuilds each repeated flag id at each site; we build it once in a
 * callee-saved register and copy:
 *   0x815, pooled:   rom   ldr r0,=0x815  ... (again) ldr r0,=0x815
 *                    ours  ldr r6,=0x815 / adds r0,r6,#0  ... adds r0,r6,#0
 *   0x308, shifted:  rom   movs r0,#0xc2 / lsls r0,#2  ... the same pair again
 *                    ours  movs r5,#0xc2 / lsls r5,#2 / adds r0,r5,#0
 *                          ... adds r0,r5,#0
 * PASS: cse.c, measured against six flags -- -fno-cse-skip-blocks AND
 * -fno-rerun-cse-after-loop each remove BOTH hoists; -fno-gcse,
 * -fno-cse-follow-jumps, -fno-expensive-optimizations and -fno-schedule-insns2
 * leave them untouched.  NINE spellings are inert against them, because cse.c
 * ends a block only at a CODE_LABEL and cse_skip_blocks walks past the
 * conditional jump between the two uses.
 * FLAG FIGURES ARE DIAGNOSTIC ONLY and both predate the length close; re-screen
 * -fno-rerun-cse-after-loop on THIS body before quoting any of them.
 *
 * ============================================================
 * WHAT WAS READ OUT OF THE ROM AND IS LOAD-BEARING
 *
 *  - IT RETURNS int 0.  `mov r0,#0 / add sp,#8 / pop {r5} / pop {r1} / bx r1`:
 *    the return address is popped into r1, which names a return value, and both
 *    exits funnel to the same `mov r0,#0`.
 *  - THE gState HALFWORD READ IS THE OVERLAY'S ESTABLISHED IDIOM, already
 *    landed in src/overlays/rom_780898/ovl_30_c_c_a_a_a_b.c: an `unsigned int`
 *    base, an offset built as `0xe1 << 1`, and the load as
 *    `*(short *)((char *)p + off)` with `off` re-zeroed -- that zero variable
 *    is the ROM's `mov r1,#0` and the load is `ldrsh r3,[r3,r1]`.  KEEP the
 *    offset variable for these; it is the byte reads that must not have one.
 *  - THE THREE-WAY SPRITE CONSTANT MUST BE WRITTEN AS A STORE IN EVERY ARM.
 *    The ROM has one `lsl r3,#17` and one `str r3,[r5,#8]` at a join reached by
 *    `b` from each arm.  Selecting into a local and storing once after the join
 *    is TWO SHORT; a `goto` join is inert.  Writing the store in all three arms
 *    and letting CROSS-JUMPING merge the tails gives the ROM's shape.
 *  - THE TILE-COPY PAIR SHARES ONE CALLEE-SAVED `1`.  `mov r5,#1` serves the
 *    6th argument of the first __CopyMapTiles and both stacked arguments of the
 *    second, so it is one named local; the first call's 5th argument (2) is a
 *    plain literal in r3.  __Func_8010704's two stacked arguments sit in r3/r2,
 *    CALLER-saved, so those are literals too.
 *  - `__MapActor_GetActor(0x15)` IS CALLED TWICE IN A ROW in the ROM, once for
 *    the saved pointer and once as the argument of __Actor_SetSpriteFlags.  A
 *    call is not a CSE candidate, so writing it twice is what the ROM has.
 *  - THE 0x14/0x15 ACTOR SLOT IS `0x14 + (__GetFlag(0x87a) != 0)`.
 *  - THE 0x109/0x823 PAIR IS AN if/else WITH THE 0x109 BODY SECOND.
 *  - THE 0x5b STORE WRITES THE 0x815 FLAG VARIABLE, NOT A LITERAL ZERO.
 *
 * SHIMS: none (tools/shimcount.py: no pins, no .equ shims, no "+r" barriers).
 * SPLIT: text-only; tools/datacheck.py prints nothing.  The .s holds TWO
 * functions (OvlFunc_883_20095dc and this one), so landing needs a split.
 *
 * NEXT: the actor-pointer pair is an allocation question, so try the brief's
 * routes 4 and 5 (move a statement across a block boundary; lengthen a range to
 * overlap a hard register) rather than more spellings -- five are already known
 * bit-identical.  Then re-screen the cse flags on the closed-length body.
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
        a = *(unsigned char *)(base + 0x205);
        b = *(unsigned char *)(base + 0x206);
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
