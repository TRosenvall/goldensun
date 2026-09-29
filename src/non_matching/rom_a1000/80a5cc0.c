/* Func_80a5cc0 -- the item/PSI action flow of the party menu, 0x080a5cc0,
 * 316 ROM instructions.
 * NON-MATCHING, 275 of 344 encodings differ.
 *
 * SIZE IS EXACT (800 = 800) but the instruction count is NOT (345 against 344),
 * so 275 is not a true distance -- objcmp's index-by-index count saturates.  The
 * honest figure is tools/aligncmp.py: 290 of 344 aligned-equal (84.3%), 58
 * differing in 37 hunks, and 23 of those 37 hunks are pool/branch offsets behind
 * the six defects listed below.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a5cc0.c \
 *     asm/rom_a1000/rom_a5534_c_a_a.s --func Func_80a5cc0
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py src/non_matching/rom_a1000/80a5cc0.c \
 *     asm/rom_a1000/rom_a5534_c_a_a.s Func_80a5cc0 -v
 *
 * THE SPLIT.  asm/rom_a1000/rom_a5534_c_a_a.s holds SIX functions (Func_80a5788,
 * Func_80a5b94, Func_80a5cc0, Func_80a5fe0, Func_80a602c, Func_80a60d4) and no
 * .section .data; tools/datacheck.py is silent, so the split is pure text and
 * needs no `.global`.  Func_80a5b94 and Func_80a60d4 are already parked from this
 * same file (src/non_matching/rom_a1000/80a5b94.c, 80a60d4.c).
 *
 * SHIMS: ZERO.
 *
 * SIGNATURE READ OFF THE CALLER, not guessed.  src/non_matching/rom_a1000/80a5b94.c
 * calls `r = Func_80a5cc0(&a, &b, &c);` and then uses `c | (a << 10)`, so it is
 * (int *, int *, int *) returning int -- and the MIDDLE pointer is never touched
 * by the callee, which is why only [sp,#8] (arg 0) and [sp,#4] (arg 2) are spilled.
 *
 * WHAT CLOSED 290 OF 344, in the order it paid:
 *
 * 1. THE SWITCH ARMS ARE IN SOURCE ORDER 0, 1, 3, 2, 4 -- NOT 0..4.  The jump
 *    table is [case0, case1, case2, case3, case4] by value, but the ARM BODIES are
 *    emitted in the order the case labels appear in the source, and the ROM emits
 *    case 3's body (the 0xaeb / Func_80a63e4 block) BEFORE case 2's (the
 *    Func_80a5fe0 block).  Writing the cases in numeric order costs the whole tail
 *    of the function; reordering them is what took SIZE from 804 to exactly 800.
 *    Generalisable: on any jump-table function, read the arm order off the BODY
 *    order, never off the table.
 *
 * 2. ONE LOCAL SPANS CASE 3 AND CASE 4.  The ROM keeps Func_80a63e4's return in
 *    r10 in case 3 and the "slot was 9" marker in r10 in case 4 -- a callee-saved
 *    register in both, i.e. ONE variable whose live range covers the loop.  Two
 *    separate locals give two allocnos and lose the `mov r10, r0` / `cmp r10, r3`
 *    pair.
 *
 * 3. CASE 2's Func_80a5fe0 RESULT NEEDS ITS OWN LOCAL.  Sharing the local that
 *    case 1 uses for Func_80a6ccc's return (which IS live across calls, and so
 *    gets a register) inserts a `mov r1, r0` before every compare.  A distinct
 *    short-lived local stays in r0, as the ROM does.
 *
 * 4. THE POINTER LOAD IS DECLARED LAST.  `int state = 0; int done = 0; int r = 0;`
 *    BEFORE `struct State *st = iwram_3001f2c;` puts `mov r5,#0` ahead of the
 *    iwram pool load and makes the whole 8-instruction prologue exact; declared
 *    first, the zero sinks past the load.  Worth 288 -> 290 on its own.  (The
 *    equivalent form -- declare `st` uninitialised and assign it just before the
 *    loop -- measures identically, 290/58/37.)
 *
 * 5. THE ZERO IS ONE CONSTANT, SHARED THREE WAYS.  `state = 0`, `done = 0` and
 *    `r = 0` all come out of the single `mov r5,#0`, and `done` is stored from it.
 *    Nothing had to be written for that; it falls out of initialising all three to
 *    0 in the same declaration group.
 *
 * 6. CASE 4's IF-BRANCH ORDER IS `st->f220 |= 1; state = 1; r = 1;`.  All six
 *    permutations were measured (aligned-equal / differing / hunks):
 *        f,s,r  290 / 58 / 37   <- this file
 *        s,r,f  290 / 59 / 38
 *        f,r,s  287 / 65 / 40
 *        r,s,f  287 / 70 / 37
 *        s,f,r  286 / 67 / 39
 *        r,f,s  286 / 67 / 37   and SIZE 792, see the blocker below
 *
 * ============================================================
 * THE BLOCKERS.  Six sites, all downstream of two mechanisms.
 *
 * BLOCKER 1, +3 encodings: THE CROSS-JUMP OF THE TWO `st->f220 |= 1` TAILS IS
 * RACED BY WHERE THE THIRD `1` LANDS.  Pass: sched2, then jump2's cross-jumping.
 *
 * The ROM has THREE distinct `1`s live in case 4's if-branch and case 3:
 *     mov r2,#1 / mov r11,r2     <- r = 1, SImode
 *     ldr r3, =1                 <- the OR mask, HImode (pooled; see below)
 *     mov r5,#1                  <- state = 1, in the SHARED tail
 * and case 3's block ends `ldrh r2,[r1] / ldr r3,=1 / b .La5f58`, jumping into
 * case 4's `orr r3,r2 / strh r3,[r1] / mov r5,#1 / b` -- a four-instruction
 * cross-jump that only exists because `mov r2,#1 / mov r11,r2` sits BEFORE the
 * mask load, leaving the tail clean.
 *
 * We can have either the three constants or the cross-jump, never both:
 *   - with `r = 1` written AFTER the OR (this file), the two HImode masks common
 *     with each other and stay POOLED exactly as the ROM has them, but sched2
 *     drops `mov r5,#1 / mov fp,r5` into the tail and the cross-jump is lost (+3).
 *   - with `r = 1` written BEFORE the OR, the cross-jump happens exactly as in the
 *     ROM -- and cse then commons the SImode 1 with the HImode mask, so the mask
 *     becomes `mov r3,#1` and BOTH pool words disappear: 341 encodings, 792 bytes,
 *     i.e. -8 bytes which is precisely the two `.word 1`s.
 * The anti-dependency is visible: in the ROM r2 holds the 1 and is then reloaded
 * by `ldrh r2,[r1]`; in ours r2 holds the loaded halfword first and the constant
 * afterwards, so sched2 can only place the constant after the `orr`.
 * `int one = 1;` for all three masks (the src/non_matching/rom_a1000/80a5b94.c
 * lever) is much WORSE, 246/344 -- it lets gcse common everything and the whole
 * tail collapses.
 *
 * BLOCKER 2, 2 encodings + 1 pool word: THE ROM POOLS 0xaf0 AND WE SPLIT IT.
 * Pass: the shiftable-constant split in arm.md.
 *
 *     rom   ldr r1, =0xaf0
 *     ours  mov r1, #0xaf / lsl r1, #4
 *
 * ESTABLISHED HERE, AND IT IS A GENERAL RULE WORTH KEEPING: in gcc-2.96 Thumb an
 * SImode CONST_INT that `thumb_shiftable_const` accepts is ALWAYS split into
 * mov+lsl, and an HImode one is ALWAYS pooled -- and a Thumb `ldrh rN, <pcrel>`
 * assembles to the SAME encoding as `ldr rN, [pc,#x]`, so the reference's
 * `ldr rN, =K` does not tell you which mode it was.  Confirmed in both
 * directions: `gh = 0xaf0` on an `unsigned short` global emits
 * `ldrh r3, .L / strh` with `.word 2800`, while every call-argument form of 0xaf0
 * (plain literal, `unsigned short` parameter, `short` parameter, cast, a local
 * held across a branch, a per-arm `unsigned short` local) emits mov+lsl, because
 * the argument is promoted to SImode before expansion.  The same rule EXPLAINS a
 * clean site in the sibling Func_80a7a34: its `ldr r0, =0x80` is an HImode store
 * constant and reproduces for free.
 * So the ROM's 0xaf0 reaches Func_80a3cf8 as an SImode argument that was never
 * split, and no source spelling found puts it there.  The two neighbouring ids
 * 0xaea and 0xaf1 are not shiftable, pool either way, and match exactly -- which
 * is why only the third arm shows the defect.
 *
 * BLOCKER 3, 4 sites, ~9 encodings: LOW-REGISTER ROLE SWAPS ON SHORT-LIVED TEMPS.
 * Every callee-saved role already matches the ROM (r5 = state, r6 = &b21a,
 * r7 = st, r8 = the -1 of case 1, r9 = the -1 of case 4, r10 = the shared marker,
 * r11 = the return value), so this is local-alloc, not global:
 *   - `st->f174 = 0`: the ROM puts the 0x174 offset in r3 and the address in r2;
 *     ours the other way round (3 encodings).
 *   - case 0's `r = -1`: the ROM copies r3 (the register the `neg` built) where we
 *     copy r0 (the compared return, which cse's record_jump_equiv proved equal).
 *     Both are correct; cse picked a different member of the -1 class.
 *   - case 4's third `r != -1` test: `cmp fp,r3` against our `cmp fp,r2`.
 *   - `_ModifyPP(st->b21a, -info[9])`: the ROM loads b21a into r3 and adds
 *     `mov r0,r3`; ours loads straight into r0 and is one instruction SHORTER.
 *     gcc precomputes both arguments here (arg 2 contains a CALL_EXPR, so
 *     calls.c's precompute_arguments fires); the ROM's copy is reload failing to
 *     coalesce, not an expression shape.
 *
 * ALSO TRIED, AFTER Func_80a8114 LANDED EXACT IN THE SAME BATCH, AND ALL FLAT AT
 * 290/58/37 (or worse) -- record them so nobody repeats the sweep:
 *   - naming the _GetMoveInfo result (`int pp = _GetMoveInfo(...)[9];`) and
 *     naming the returned pointer instead: 290 and 287.
 *   - naming the -1 of case 0 (`int none = -1;` used for both the test and the
 *     assignment), to try to make `mov r11,r3` pick the constant's register: 290.
 *   - `*(unsigned short *)((unsigned char *)st + 0x174) = 0;` for the f174
 *     store, to try to flip its r2/r3 roles: 278 and SIZE 804.  WORSE.
 *   - `int ok = 1; ... r = ok;` in case 4's if-branch, in two positions, and
 *     `st->f220 = st->f220 | 1;` written out: 290, 792/286, 290.
 *
 * AND ONE DIAGNOSTIC THAT DOES NOT APPLY.  Func_80a8114's lever 3 says a
 * whole-function low-register rotation is ONE missing short-lived quantity.
 * That is NOT what this is: the first differing hunk (`st->f174 = 0`) uses two
 * simultaneous low registers in BOTH versions, and the later hunks are r2<->r3
 * and r0<->r3 SWAPS rather than a consistent +1 shift.  So there is no single
 * extra quantity to find; these are independent local-alloc ties.
 *
 * STRUCT.  iwram_3001f2c as this function sees it: 0x24 and 0x2c two UI-box
 * handles, 0x174 u16, 0x178 u16 (the selected item, masked 0x3fff), 0x218 u8
 * (non-zero = a selection exists), 0x21a u8 (the unit), 0x21b u8 (the target
 * slot; 9 is the sentinel), 0x220 u16 (flag word, bit 0), 0x222 u16, 0x25a
 * SIGNED short (read with `ldrsh r0,[r3,r2]`, r2 = 0, which is the Thumb
 * register-offset form a plain `short` field gives for free), 0x268 u8 (the kind:
 * 0, 1 or 2).  0x178 and 0x24 agree with the caller's own view in 80a5b94.c.
 */
extern int _GetFlag(int id);
extern int _GetUnit(int id);
extern void _PlaySound(int id);
extern void WaitFrames(int n);
extern void _CalcStats(int id);
extern unsigned char *_GetMoveInfo(int id);
extern void _ModifyPP(int id, int b);
extern void _Func_8016498(unsigned int win);
extern void _Func_80164ac(unsigned int win);
extern void Func_80a112c(unsigned int win, int id, int a, int b);
extern void Func_80a1d08(int a, int b, int c);
extern void Func_80a3cf8(int a, int b);
extern int Func_80a5fe0(void);
extern int Func_80a602c(int a);
extern int Func_80a63e4(int a);
extern void Func_80a65e4(int a, int b, int c);
extern int Func_80a6ccc(int a);
extern void Func_80a9cbc(void);
extern int Func_80a9f10(int a, int b, int c, int d);
extern void Func_80aa460(int a);

struct State {
    unsigned char pad0[0x24];
    unsigned int f24;
    unsigned char pad28[4];
    unsigned int f2c;
    unsigned char pad30[0x144];
    unsigned short f174;
    unsigned char pad176[2];
    unsigned short f178;
    unsigned char pad17a[0x9e];
    unsigned char b218;
    unsigned char pad219;
    unsigned char b21a;
    unsigned char b21b;
    unsigned char pad21c[4];
    unsigned short f220;
    unsigned short f222;
    unsigned char pad224[0x36];
    short f25a;
    unsigned char pad25c[0xc];
    unsigned char b268;
};
extern struct State *iwram_3001f2c;

int Func_80a5cc0(int *outUnit, int *outB, int *outItem)
{
    int state = 0;
    int done = 0;
    int r = 0;
    struct State *st = iwram_3001f2c;
    int v;
    int w;
    int kind;
    int t;

    while (!done && !_GetFlag(0x150)) {
        switch (state) {
        case 0:
            st->f174 = 0;
            Func_80a3cf8(0, 0xae9);
            if (Func_80a602c(0) == -1) {
                done = 1;
                r = -1;
            }
            _Func_8016498(st->f2c);
            state = 1;
            break;
        case 1:
            WaitFrames(1);
            _GetUnit(st->b21a);
            if (st->b218 == 0) {
                state = 0;
                break;
            }
            switch (st->b268) {
            case 0:
                Func_80a3cf8(0, 0xaea);
                break;
            case 1:
                Func_80a3cf8(0, 0xaf1);
                break;
            case 2:
                Func_80a3cf8(0, 0xaf0);
                break;
            }
            Func_80a9cbc();
            Func_80a112c(st->f24, st->b21a, 0, 0);
            v = Func_80a6ccc(0);
            state = 0;
            if (v == -1)
                break;
            kind = st->b268;
            state = 2;
            if (kind == 0)
                break;
            if (kind == 1) {
                Func_80a65e4(st->b21a, v, 0);
                _Func_80164ac(st->f2c);
                Func_80a1d08(0xae2, -1, -1);
            } else {
                Func_80a65e4(st->b21a, v, 1);
                _Func_80164ac(st->f2c);
                Func_80a1d08(0xae3, -1, -1);
            }
            state = 0;
            break;
        case 3:
            Func_80a3cf8(0, 0xaeb);
            t = Func_80a63e4(0);
            state = 4;
            if (t == -1) {
                st->f220 |= 1;
                state = 1;
            }
            break;
        case 2:
            w = Func_80a5fe0();
            if (w == 1) {
                state = 3;
            } else if (w == 2) {
                st->b21b = 9;
                state = 4;
            } else {
                done = 1;
                r = 1;
                *outUnit = st->b21a;
                *outItem = st->f178 & 0x3fff;
            }
            break;
        case 4:
            t = 0;
            r = Func_80a9f10(st->f178, st->b21a, st->b21b, 0);
            if (st->b21b == 9) {
                st->b21b = st->b21a;
                t = 9;
            }
            if (r != -1)
                _ModifyPP(st->b21a, -_GetMoveInfo(st->f178 & 0x3fff)[9]);
            _CalcStats(st->b21a);
            if (r != -1) {
                Func_80a112c(st->f24, st->b21b, 0, 0);
                Func_80aa460(st->f178 & 0x3fff);
                _Func_80164ac(st->f2c);
                Func_80a1d08(st->f25a + 0xbef, 0, -1);
            } else {
                _PlaySound(0x72);
                _Func_80164ac(st->f2c);
                Func_80a1d08(st->f25a + 0xbef, r, r);
            }
            if (r != -1) {
                st->f220 |= 1;
                state = 1;
                r = 1;
            } else {
                st->f222 = 1;
                if (t == 9) {
                    st->f220 |= 1;
                    state = 1;
                } else {
                    state = 3;
                }
            }
            break;
        default:
            done = 1;
            break;
        }
    }
    if (_GetFlag(0x150))
        r = -1;
    return r;
}
