/* Func_801776c -- NON-MATCHING, 6 encodings of 137.  SIZE EXACT, INSTRUCTION COUNT EXACT
 * (137 = 137), ALL RELOCATIONS EXACT (objcmp prints no RELOCATIONS block).  One of the
 * closest parks in the corpus.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_15000/801776c.c \
 *     asm/rom_15000/rom_15e8c_c_c_a.s --func Func_801776c
 *
 * ITS .s HOLDS ONLY THIS AND Func_8017658 AND IS CLEAN OF DATA, SO THE FILE CONVERTS WHOLE
 * THE MOMENT BOTH MATCH.  This one is six encodings away; the sibling is the harder half.
 *
 * BLOCKER: sched2 (post-reload) ALONE.  `-fno-schedule-insns` is INERT and
 * `-fno-schedule-insns2` is WORSE (35), which locates it to the second scheduling pass
 * rather than the first.  The 6 are ONE three-instruction window -- `and r7,rK` should sit
 * AFTER the two x/y stores -- plus its 3 register-name consequences.
 *
 * WHAT GOT IT HERE, 110 -> 13, AND IT IS REUSABLE: FOUR LEAF ARMS ALL ASSIGNING THE SAME
 * VARIABLE LET CROSS-JUMPING PUT ONE STORE AT THE JOIN; A SHARED TEMP WITH ONE ASSIGNMENT
 * AFTER DOES NOT.  With a temp, gcc additionally inverts branch polarity and speculates
 * each arm's body above its own test.  Same family as batch 284's "write a shared exit tail
 * twice" and batch 285's cross-jumped swap.
 *
 * ALSO LOAD-BEARING, and worth the index: DECLARATION-INITIALISER FORM IS NOT THE SAME AS
 * AN ASSIGNMENT -- `int x = 0, y = 0;` read 17 where `x = 0; y = 0;` read 6.
 */
/* Func_801776c (RunTextBoxModal) -- NON-MATCHING BY SIX ENCODINGS OF 137 against
 * asm/rom_15000/rom_15e8c_c_c_a.s.  SIZE EXACT.  INSTRUCTION COUNT EXACT
 * (137 = 137).  ALL RELOCATIONS EXACT -- objcmp prints no RELOCATIONS block.
 *
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this> \
 *     asm/rom_15000/rom_15e8c_c_c_a.s --func Func_801776c
 *
 * NO SHIM, NO asm, NO PIN.  Plain C throughout.
 *
 * THE SIX ARE ONE THREE-INSTRUCTION WINDOW IN THE PROLOGUE BLOCK PLUS ITS THREE
 * REGISTER-NAME CONSEQUENCES:
 *
 *     rom  ... mov r10,r0 / str r5,[sp,#0x18] / str r5,[sp,#0x14] / and r7,r2
 *     ours ... mov r10,r0 / and r7,r1         / str r2,[sp,#0x18] / str r2,[sp,#0x14]
 *
 * The multiset is identical; only the order differs, and with it which pseudo
 * gets r2 (the ROM gives r2 to the constant 1 and r5 to the zero; we give r2 to
 * the zero and r1 to the 1, which also renames the `strb` inside the flags&2
 * arm).  -fno-schedule-insns is INERT here, so this is sched2, post-reload.
 *
 * FOUR LEVERS LANDED, and the first two are new and general.
 *
 * 1. A TWO-WORD OUT-PARAMETER ZEROED AT THE TOP AND ADDRESS-TAKEN LATER IS A
 *    `long long`, NOT AN ARRAY OR A STRUCT.  THE TELL IS `mov r3,#0 / mov r4,#0`
 *    -- A DImode ZERO IN A REGISTER PAIR.  The ROM zeroes sp+4/sp+8 in the FIRST
 *    basic block yet gives that object the BOTTOM frame slot, which no array or
 *    struct can do: both are addressable from their first reference, so
 *    `q[0] = 0` or `s.a = 0` allocates the slot immediately and it lands at the
 *    TOP (0x14), with h/w/y/x pushed down.  `long long s; s = 0;` keeps it in a
 *    DImode PSEUDO -- hence the two zero registers -- and the only `addressof` is
 *    the `(int *)&s` at the _Func_8094154 call, so the slot is assigned LAST and
 *    lands at 0x4, with x/y/w/h at 0x18/0x14/0x10/0xc.  Frame layout exact.
 *    125 -> 108.
 *
 * 2. A SYMBOL+OFFSET FOLDS INTO ONE POOL WORD UNLESS THE BASE IS A NAMED
 *    POINTER LOCAL.  The ROM wants
 *    `ldr r3,=gState / mov r2,#0xfa / lsl r2,#1 / add r3,r2`; plain
 *    `*(int *)(gState + 0x1f4)` gives `ldr r3,=gState+500`, and so does
 *    `*(int *)((int)gState + 0x1f4)` -- the integer cast does NOT stop the fold.
 *    `gsb = gState;` as a statement, then `*(int *)(gsb + 0x1f4)`, gives the
 *    ROM's split exactly.  (`int gp = (int)gState;` is much worse, 138: the base
 *    wins a callee-saved register and everything shifts.)
 *
 * 3. FOUR LEAF ARMS ALL ASSIGNING `y` LET CROSS-JUMPING PUT THE ONE STORE AT THE
 *    JOIN; A SHARED `t` WITH `y = t` AFTER DOES NOT.  With `t` gcc also inverts
 *    the branch polarity and speculates each arm's body above its own test.
 *    Writing `y = y + 4` / `y = y + 0xc` / `y = t - 5` / `y = t + 4` gives the
 *    ROM's `beq / body / b join` shape and a single `str r3,[sp,#0x14]` at the
 *    join.  110 -> 13.  (`y += 4` with the else arm also in place on `y` is 84 --
 *    in-place on an ADDRESS-TAKEN local costs loads.)
 *
 * 4. THE POINTER TO THE `long long` MUST BE INLINE, NOT A NAMED LOCAL, OR THE
 *    FRAME ADDRESS IS SCHEDULED ONE SLOT EARLY.  `_Func_8094154(v, (int *)&s)`
 *    with `((int *)&s)[1]` for the read puts `add r5,sp,#4` AFTER `add r3,r2`,
 *    as the ROM has it; a named `int *r = (int *)&s;` puts it before.  8 -> 6.
 *    (Precomputing the gState word into a local, `v = *(int *)(gsb + 0x1f4);`,
 *    reads 6 as well.)
 *
 * DROP LADDER:
 *   first candidate                                     132 differing
 *   `long long s` (frame layout)                         108
 *   named `z` for the three tail zeros                   108, but 133 lines from
 *                                                        136 -- kills the pooled
 *                                                        `ldr r2,=0x0` that the
 *                                                        halfword store creates
 *   `gsb` (gState split)                                 110 with the right shape
 *   four-arm `y =` (cross-jumped store)                  13
 *   prologue statement order, 24 permutations            best 8; `s = 0;` BETWEEN
 *                                                        `x = 0;` and `y = 0;` is
 *                                                        load-bearing and `m` must
 *                                                        be in the first three
 *   inline `(int *)&s`                                   6
 *   declaration order, 20 permutations                   ALL INERT
 *   `m = flags % 2` / named `one = 1` / `int m = 0`      ALL 6 (inert)
 *   `x = y = 0`                                          13
 *   `int x = 0, y = 0` / `long long s = 0` initialisers   17 -- initialisers are
 *                                                        NOT the same as
 *                                                        assignments here
 *   `m = flags & 1` inlined at both call sites           140
 *   -fno-schedule-insns / -fno-gcse                      INERT (so it is sched2)
 *   -fno-rerun-cse-after-loop                            19 (worse)
 *   -fno-schedule-insns2                                 35 (worse)
 *
 * BLOCKER: SCHEDULING (sched2, post-reload), with the register naming that
 * follows from it.  Everything reachable from source order was swept.
 */
extern unsigned char *iwram_3001e8c;
extern unsigned char gState[];
extern void TextBox(int id, int *x, int *y, int *w, int *h);
extern void _Func_8094154(int a, int *q);
extern void *Func_8017658(int id, int x, int y, int m);
extern int Func_8017364(void);
extern int Func_8017394(void *box);
extern void CloseUIBox(void *box, int m);
extern void WaitFrames(int n);

void Func_801776c(int id, unsigned int flags)
{
    unsigned char *p;
    long long s;
    int h;
    int w;
    int y;
    int x;
    int m;
    int t;
    int z;
    unsigned char *gsb;
    int *r;
    void *box;

    p = iwram_3001e8c;
    m = flags & 1;
    x = 0;
    s = 0;
    y = 0;
    if (flags & 2)
        *(char *)(p + 0x12f9) = 1;
    TextBox(id, &x, &y, &w, &h);
    x = (0x1e - w) >> 1;
    y = (0xc - h) >> 1;
    if (flags & 8) {
        y = y + 4;
    } else if (flags & 0x40) {
        y = y + 0xc;
    } else {
        gsb = gState;
        _Func_8094154(*(int *)(gsb + 0x1f4), (int *)&s);
        t = ((int *)&s)[1] >> 3;
        if (t > 9)
            y = t - 5;
        else
            y = t + 4;
    }
    box = Func_8017658(id, x, y, m);
    if (box != 0) {
        while (Func_8017364() == 0)
            WaitFrames(1);
        if (flags & 0x20)
            *(char *)(iwram_3001e8c + 0xea6) = 1;
        if (!(flags & 4)) {
            CloseUIBox(box, m);
            while (Func_8017394(box) == 0)
                WaitFrames(1);
        }
    }
    z = 0;
    *(char *)(p + 0x12f9) = z;
    *(short *)(p + 0x12f4) = z;
    *(short *)(p + 0x12f6) = z;
    WaitFrames(3);
}
