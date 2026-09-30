/* BattleMain (0x080b63c8) -- NON-MATCHING, 618 of 684 encodings differ (objcmp,
 * PRODUCTION FLAGS).  645 instructions; at this size objcmp's count SATURATES, so LEAD
 * WITH aligncmp.
 *
 * SIZE IS EXACT (1380 == 1380 -- objcmp prints no SIZE line).  INSTRUCTIONS 684 vs 682,
 * TWO SHORT, so size-and-count is NOT both exact and 618 still saturates.
 * THE RANKING VIEW IS aligncmp: 464 of 684 ALIGNED-EQUAL (67.8%), 274 differing/inserted/
 * deleted in 118 HUNKS.
 * RELOCATIONS: 138 against 138, and READ THE SYMBOL SEQUENCE SEPARATELY FROM THE OFFSETS.
 * THE SEQUENCE IS 133 OF 138 IN ORDER; the five out-of-place entries are THREE POOL-WORD
 * BLOCKS DUMPED EARLY, nothing else:
 *     `Func_80008d4`                                     ours index 17, ref 35
 *     `_RPGRandom` / `gState` transposed                  ours 53, ref 54
 *     `iwram_3001f58` + `Func_80b7738` + `Func_80008d4`   ours 82, ref 114
 * 134 of 138 OFFSETS differ, every one by a uniform +2 or +4 downstream of a pool dump.
 * No CALL and no data symbol is missing, extra, or wrong -- the call sequence itself is
 * exact end to end, which on a 645-instruction reconstruction is what says the program is
 * the ROM's program.
 * > I FIRST RECORDED THIS AS "all relocations match" AND IT WAS WRONG: my grep filter was
 * > `SIZE|ENCODINGS|first at`, which DROPS objcmp's `RELOCATIONS differ` line, so the
 * > absence of the line in my own transcript was an absence of the grep, not of the
 * > defect.  Never conclude relocations match from a filtered objcmp run.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_b5000/80b63c8.c \
 *       asm/rom_b5000/rom_b5a0c_c_c_a_a_a_c_a.s --func BattleMain
 *   docker run ... python3 tools/aligncmp.py src/non_matching/rom_b5000/80b63c8.c \
 *       asm/rom_b5000/rom_b5a0c_c_c_a_a_a_c_a.s BattleMain
 *
 * SPLIT SHAPE (measured with tools/split_s.py --dry-run; NOT yet performed):
 *     asm/rom_b5000/rom_b5a0c_c_c_a_a_a_c_a.s holds TWO functions, BattleMain FIRST.
 *       ->  rom_b5a0c_c_c_a_a_a_c_a_b.s   BattleMain      (718 lines)  <- the target
 *       ->  rom_b5a0c_c_c_a_a_a_c_a_c.s   Func_80b6a60     (75 lines)
 *     FINAL INSTALLED PATH (match): src/rom_b5000/rom_b5a0c_c_c_a_a_a_c_a_b.c
 *     `make compare` must be green after the split and before any .c is written.
 *     CORRECTION TO RECON_BattleMain.txt: it says the split writes `_a` and `_b`.
 *     split_s.py actually writes `_b` and `_c` (it does not reuse the parent's own
 *     suffix), so the target is `_b` and the installed path is the one above.  The same
 *     off-by-one is in src/non_matching/rom_9000/800f2f8.c's recipe.
 * datacheck.py: nothing.  No data section, so no code/data split.
 * `.global` REQUIREMENTS: NONE -- `.thumb_func_start` in include/macros.inc emits
 *     `.global` for both functions, and this file names no `.L` label.
 * stage1.ld names the object EXACTLY ONCE, line 1539, `(.text)` only.  No `(.rodata)`.
 *
 * SHIMS -- PIN-FREE.  shimcount reports NOTHING: no register pins, no inline asm of this
 * file's own, no fakematch-class shim, so NO fakematch.txt row is due.  The only asm in
 * the translation unit is include/dma.h's DMA3_CLEAR, which is the tree's shared header.
 * BOTH indirect calls go through gcc's ORDINARY veneer, `bl _call_via_r3` -- the form gcc
 * emits from a C function pointer, not the inline `.call_via` macro.  A screen of this
 * function for inline `.call_via` sites returns ZERO.
 *
 * ================= LEVERS THAT PAID, IN ORDER, WITH FIGURES =================
 * The first candidate already read SIZE EXACT, count 682 vs 684, 67.4% aligned in 124
 * hunks, with only two relocation defects.  Written straight off the disassembly with
 * RECON_BattleMain.txt's structural reading; the recon's two named blockers both resolved.
 *
 * 1. THE RECON'S "48 UNEXPLAINED FRAME BYTES" IS SETTLED IN THE DIRECTION IT DID NOT
 *    EXPECT, AND ITS OWN CHEAP DISCRIMINATOR IS WHAT SETTLED IT.  The recon asked whether
 *    three DMA3_CLEAR expansions give three `u32 value;` slots (12 bytes) or one.  THEY
 *    GIVE ONE.  gcc-2.96's inline expander allocates the inlined local as a stack TEMP,
 *    and a temp slot is reusable, so all three expansions share one 4-byte slot -- which
 *    is exactly what the ROM shows (`add r5, sp, #0x10` ONCE, `str r7, [r5]` three times,
 *    r5 held across all three).  So the declared region is 4 bytes of `value` plus 48
 *    bytes of something else, and `int pad[12]` closes it: 4 + 48 = 52 = 0x10..0x44.
 *    THE 48 BYTES ARE STILL UNIDENTIFIED, but they are now bounded exactly and they are
 *    an AGGREGATE, because batch 306's rule says an unused scalar gets no slot.  The frame
 *    arithmetic that has to hold is
 *        4 (outgoing 5th arg for Func_80c0a24) + 12 (three spills) + 52 (declared) = 0x44.
 *
 * 2. `_call_via_r2` -> `_call_via_r3`, BOTH SITES, FROM NAMING THE CALL'S ARGUMENT.
 *    elevation.md's "Naming a call's ARGUMENT fixes the `_call_via_rN` register" is
 *    recorded on Func_809397c; it transfers verbatim.  `f((int)en, 0xa0 << 1)` with the
 *    pointer assigned at function scope puts the pointer in r2; giving the size its own
 *    named local FIRST and declaring the pointer in an INNER BLOCK adjacent to the call
 *    pushes the pointer load after the argument computation and selects r3.  That removed
 *    the only WRONG-SYMBOL defect (`_call_via_r2` appeared where the ROM has
 *    `_call_via_r3`, twice); the sequence defects that remain are pool placement, above.
 *
 * 3. ONE VARIABLE PER REGION, on nine function-level scalars at once -- 124 -> 118 hunks
 *    and objcmp 628 -> 615.  `i`, `cnt`, `ui`, `h`, `idx`, `s`, `k`, `koff`, `p` and
 *    `era` were declared at function level; each one's live_length is the SUM of its
 *    disjoint ranges, and ten of them inflated the allocno set enough to cost three extra
 *    spill slots.  Brace-scoping each to the region that uses it is batch 306's
 *    complement lever, fourth instance.
 *
 * 4. REUSE A VARIABLE TO INHERIT ITS REGISTER, read straight off the ROM's register map.
 *    r9 carries `view + 0xc` and THEN `st + 0x41` -- two disjoint ranges, both
 *    `unsigned char *`, so ONE declared variable.  Writing them as one (`vp`) is worth
 *    274 differing against 278 and 464 aligned against 463.  The other half of the same
 *    reading: the ROM has TWO DISTINCT ZERO QUANTITIES, r7 (the three p4 stores plus the
 *    DMA `value` store) and r5 (the three view stores, then `sRPGRNGState = 0`), so one
 *    `zero` variable serving both jobs is wrong and they must be declared separately.
 *
 * 5. ONE VARIABLE, NOT TWO, CARRIES BOTH CALL RESULTS -- the recon's single most
 *    load-bearing find, confirmed.  `x = Func_80c1ffc(*(int *)st)` lands in r6 and the
 *    `*pf != 0` block CLOBBERS r6 with `x = _GetFlagByte(0xfc << 2)`; the later
 *    `Func_80c02a4(x, enc)` reads whichever ran.  Two source variables cannot produce it.
 *    This is lever 1 in its original form and it was right in the recon before any
 *    measurement existed.
 *
 * 6. THE SIZE CONSTANT 0x7c8 IS ONE VARIABLE ACROSS THE galloc AND THE CLEAR.  The ROM
 *    builds `mov r5,#0xf9 / lsl r5,#3` once and passes r5 to `galloc_ewram(0x36, n)` and
 *    then to the indirect clear.  `n` is the name; two literals would rebuild it.
 *
 * ================= THE RESIDUE, ATTRIBUTED =================
 * (A) THREE EXTRA SPILL SLOTS -- the frame is 0x50 against the ROM's 0x44 and this is the
 *     whole of the remaining structural gap.  Below the declared region the ROM has 4
 *     bytes of outgoing argument and THREE spills (sp+4 = `pf`, sp+8 = `p4`, sp+0xc =
 *     `enc`); this candidate has SIX.  The ROM fits fourteen long-lived quantities into
 *     seven callee-saved registers by giving each register a chain of disjoint tenants:
 *       r5  n -> &value -> st+0x2ec -> cnt -> ui
 *       r6  en -> x -> st+0x52 -> the enemy-loop byte offset
 *       r7  the zero constant -> the RETURN VALUE
 *       r8  st            (live end to end)
 *       r9  view+0xc -> st+0x41 -> the constant 1 in the link-wait loop
 *       r10 view -> &iwram_3001f64
 *       r11 st+0x45
 *     Levers 3 and 4 above recovered two of those chains.  THE NEXT THREE TO WRITE AS
 *     REUSE, in the order the ROM makes them cheapest, are r5's `n`/`cnt`/`ui` chain
 *     (all int, all disjoint), r6's `en`/`x` pair, and r7's `zero`/`ret` pair -- and note
 *     r7's pair is the one that ALSO explains the interleaved `mov r7,#1 / ... / neg r7,r7`
 *     in the lost-battle tail, which elevation.md already records as the signature of one
 *     variable rather than two returns.
 *     RULED OUT: declaration ORDER is not the handle here.  Unlike ActorCmd_Player (whose
 *     six slots ARE declaration-ordered), the slot ASSIGNMENT is already the ROM's for
 *     the three slots we share; the defect is the COUNT of spilled allocnos, which
 *     allocno_compare reaches only through n_refs and live_length.
 *
 * (B) TWO MISSING INSTRUCTIONS, and they are a consequence of (A), not independent: each
 *     extra spill deletes a `mov rlo, rhi` pair somewhere and adds a `str`/`ldr` pair, so
 *     the count moves in both directions and nets to -2.  Size is already exact, which is
 *     the tell that nothing is structurally absent.
 *
 * (C) POOL PLACEMENT -- the bulk of the 118 hunks and the three displaced relocation
 *     blocks named at the top.  All three of ours dump EARLIER than the ROM's: the
 *     `Func_80008d4` word lands between the two `_Func_8016018` calls where the ROM
 *     carries it to after `_Func_8077330`, and the three-word block lands at index 82
 *     against the ROM's 114.  arm_reorg dumps a pending pool when the furthest `ldr
 *     [pc,#N]` would go out of Thumb's 1KB range, so a denser stretch of code dumps
 *     later, not earlier -- and ours is DENSER by the three spill/reload pairs of (A) and
 *     SHORTER by the two instructions of (B).  So (C) is downstream of (A) and is NOT an
 *     independent finding; the ROM's own `.pool_aligned` markers sit after 0xb6650,
 *     0xb66ea and 0xb6994, and those are the three positions to check once the frame is
 *     0x44.  The branch displacements (`b.n` / `beq.n` off by one halfword) follow.
 *
 * THINGS THE ROM PINS THAT ARE EASY TO GET WRONG, all verified here:
 *   - THE FIFTH galloc_ewram RESULT IS DISCARDED (`galloc_ewram(0xb, 0xa0 << 2)`); r0 is
 *     overwritten by the next call's argument before any use, so the callee stores it
 *     globally.  Assigning it to a variable costs a register.
 *   - THE COPY LOOP ENDS `bls`, i.e. an UNSIGNED latch: `i = 0; do { ...; i++; } while
 *     (i <= 0x7c7);`.  Do NOT name 0x7c7 -- loop.c hoists it into r4 unprompted, and the
 *     same goes for the constants 3 and 1 in the link-wait loop (r7 and r9 in the ROM).
 *     Reading the condition code before choosing the compare is the cheapest correctness
 *     signal in the file; sibling copy loops in this bank end `ble`.
 *   - `add r5, #1` sits BEFORE `bl WaitFrames` in the link-wait loop, so the increment is
 *     written before the call and sched2 will not move it across one.
 *   - THE LOSE TAIL'S `REG_DISPCNT = 1` IS A POOL WORD (`ldr r3, .Lb6994  @ 1`) while the
 *     init block's `REG_DISPCNT = 1` is `mov r2, #1`.  Same store, two encodings: the
 *     first is an HImode literal that *thumb_movhi_insn force_const_mem's, the second
 *     reaches the register as an SImode constant because the SAME variable also feeds
 *     `*(int *)(p4 + 0x14) = 1`.  So the tail needs `unsigned short dz = 1;` and the init
 *     needs `int one = 1;` shared with the word store.  Spelling either the other way is
 *     a size-and-count defect.  `REG_BLDCNT = 0` is the same class (`ldr r5, =0`) and its
 *     `unsigned short z` is then reused for `*(st + 0x45) = z`, which is why that store is
 *     `strb r5` off the halfword register rather than a fresh `mov r3,#0`.
 *   - THE FOUR-WAY `goto done` TAIL IS FOUR DIFFERENT RETURN VALUES and cross-jumping does
 *     not touch it: `*(int *)(st + 0x538)` twice (0xb6954 and 0xb696e), `-1` (0xb69b0) and
 *     `0x3e7` (0xb6a00).  The two that share a value still differ in their emitted tails
 *     (one calls `_SetFlag`), which is what keeps jump.c off them -- batch 306's rule that
 *     cross-jumping compares EMITTED TAILS, read in the safe direction for once.
 *   - `Func_80b9b30`'s second argument is `0xa` WHEN i == 0 and `0` otherwise, not the
 *     reverse: the ROM is `mov r1,#0xa / cmp r7,#0 / beq .Lb67fc / mov r1,#0`, so the
 *     `beq` SKIPS the zero.
 */
#include "dma.h"

extern unsigned char gState[];
extern unsigned int sRPGRNGState;
extern unsigned short iwram_3001f64;
extern unsigned char *iwram_3001f28;
extern unsigned char iwram_3001f58;
extern unsigned char ewram_2018000[];

extern void *galloc_ewram(int tag, int size);
extern void Func_80008d4(int dst, int size);
extern void ClearTasks(void);
extern void _SetFlag(int id);
extern int _GetFlag(int id);
extern int _GetFlagByte(int id);
extern void InitMatrixStack(void);
extern int _Func_808b248(void);
extern void _InitActors(int n);
extern void _Func_8016018(int n);
extern int Func_80c1ffc(int enc);
extern void WaitFrames(int n);
extern void Func_80b6378(void);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void _PlaySound(int id);
extern void SetSoundFXMode(int m);
extern void Func_80b5a0c(void);
extern void Func_80b75dc(void);
extern void Func_80b5c08(void);
extern void Func_80b5d3c(void);
extern int *_Func_8077330(int n);
extern void _Func_801ef08(int n);
extern void Func_80b7f9c(void);
extern void Func_80b6c90(void);
extern void Func_80c08a8(void);
extern void AnimTransitionIn(int a, int b, int c);
extern void Func_80c0a24(int a, int b, int c, int d, int e);
extern void Func_80c0cec(int a, int b, int c, int d);
extern void Func_80b5b14(int n);
extern void Func_80c24b0(void);
extern int AllocUploadSpriteGFX(int n);
extern int _RPGRandom(void);
extern void Func_80c02a4(int x, int enc);
extern void Func_80b9b2c(void);
extern void _Func_801f200(int n);
extern void Func_8003f3c(int h);
extern void Func_800488c(void);
extern void Func_80048a0(void);
extern int Func_80b9934(void *p);
extern int Func_80b8574(void *p);
extern int Func_80b9b30(void *p, int n);
extern int Func_80b874c(void *p);
extern int Func_80b6b40(int a, int b);
extern int Func_80b6148(void);
extern void Func_80bf674(void);
extern void Func_80bf678(void);
extern void Func_80b7e7c(void);
extern int _Func_8017658(int a, int b, int c, int d);
extern int _Func_8017364(void);
extern void _CloseUIBox(int h, int n);
extern void Func_80bb7c0(int a, int b);
extern void Func_80b63b0(void);
extern void _InitEnemyUnit(int a, int b, int c);
extern void _Func_80198dc(void);
extern void _Func_8019908(int a, int b);
extern void _Func_80175a0(int id);
extern void WaitTextPrompt(void);
extern void Func_80c2724(void);
extern void Func_8003b70(int n);
extern void Func_8003ce0(void);
extern void Func_80042c8(int n);
extern int Func_80b6a60(int n);
extern void Func_80b5b18(void);
extern void Func_80bf5a8(void);
extern void Func_80c08e0(void);
extern void Func_80b5864(void);
extern void Func_80b7738(void);

int BattleMain(int r0)
{
    int pad[12];
    int enc;
    unsigned char *view;
    unsigned char *st;
    unsigned char *en;
    unsigned char *vp;
    unsigned char *p4;
    unsigned char *pf;
    unsigned char *flg;
    int n;
    int zero;
    int one;
    int x;
    int ret;
    int cnt;
    unsigned short z;

    enc = r0;
    view = galloc_ewram(0xc, 0x4c);
    st = galloc_ewram(9, 0x82c);
    n = 0xf9 << 3;
    en = galloc_ewram(0x36, n);
    p4 = galloc_ewram(0x2c, 0x20);
    galloc_ewram(0xb, 0xa0 << 2);
    vp = view + 0xc;
    {
        void (*f)(int, int) = Func_80008d4;
        f((int)en, n);
    }
    ClearTasks();
    zero = 0;
    *(int *)(p4 + 4) = zero;
    *(int *)p4 = 0x80 << 6;
    one = 1;
    *(int *)(p4 + 0x14) = one;
    *(int *)(p4 + 0x18) = zero;
    *(int *)(p4 + 0x1c) = zero;
    REG_DISPCNT = one;
    _SetFlag(0x103);
    _SetFlag(0x169);
    InitMatrixStack();
    DMA3_CLEAR(view, 0x4c);
    DMA3_CLEAR(st, 0x82c);
    *(int *)(st + 0x54) = -1;
    *(int *)st = enc;
    DMA3_CLEAR(galloc_ewram(0x25, 0xc), 0xc);
    *(unsigned short *)(st + (0xc9 << 3)) = _Func_808b248();
    galloc_ewram(4, 0xe0 << 4);
    galloc_ewram(3, 0xc0 << 3);
    _InitActors(4);
    if (_GetFlag(0xb7 << 1))
        _Func_8016018(1);
    else
        _Func_8016018(0);
    {
    int k = 0;
    *(int *)(vp + 4) = 0x80 << 15;
    *(int *)vp = k;
    *(int *)(vp + 8) = k;
    *(int *)(view + 4) = 0xb4 << 16;
    *(int *)(view + 8) = 0x80 << 15;
    *(int *)view = k;
    *(unsigned short *)(view + 0x36) = 0xa0 << 6;
    *(unsigned short *)(view + 0x34) = 0xa0 << 7;
    *(int *)(view + 0x20) = 0x80 << 17;
    }
    x = Func_80c1ffc(*(int *)st);
    if (_GetFlag(0xb6 << 1)) {
        pf = st + 0x44;
        *pf = 1;
        gState[0x22b] = 4;
    } else {
        pf = st + 0x44;
    }
    if (*pf) {
        unsigned char *era;
        int k;
        sRPGRNGState = 0;
        k = 0;
        era = st + 0x52;
        while ((iwram_3001f64 & 3) != 3) {
            k++;
            WaitFrames(1);
            if (k > 0x18) {
                *era = 1;
                break;
            }
        }
        *(unsigned char *)(st + 0x50) = (REG_SIOCNT << 26) >> 30;
        {
            unsigned char *d = iwram_3001f28;
            unsigned char *s2 = ewram_2018000;
            int i = 0;
            do {
                *d = *s2;
                i++;
                s2++;
                d++;
            } while (i <= 0x7c7);
        }
        x = _GetFlagByte(0xfc << 2);
        Func_80b6378();
        *(unsigned char *)(st + 0x42) = 0;
    }
    StartTask(Func_80b5864, 0xc7f);
    {
    int koff = 0xf7 << 1;
    int s = *(short *)(gState + koff);
    if (s) {
        _PlaySound(s);
        if (_GetFlag(0xb6 << 1)) {
            _PlaySound(0x37);
            SetSoundFXMode(4);
        }
    } else {
        _PlaySound(0x33);
        _PlaySound(0x4c);
    }
    }
    Func_80b5a0c();
    Func_80b75dc();
    Func_80b5c08();
    Func_80b5d3c();
    {
    int *p = _Func_8077330(0);
    if (*p) {
        vp = st + 0x41;
        *vp = 3;
    } else {
        vp = st + 0x41;
        *vp = 1;
    }
    }
    _Func_801ef08(9);
    Func_80b7f9c();
    Func_80b6c90();
    Func_80c08a8();
    AnimTransitionIn(1, *(unsigned short *)(st + (0xc9 << 3)), 0);
    Func_80c0a24(0xa0 << 16, 0xa0 << 15, 0, 0, 0x80 << 10);
    Func_80c0cec(0, 0, 0, 0xbe);
    Func_80b5b14(1);
    z = 0;
    REG_BLDCNT = z;
    Func_80c24b0();
    *(int *)(st + 0x54) = AllocUploadSpriteGFX(0x80);
    flg = st + 0x45;
    *flg = z;
    if (_GetFlag(0xb7 << 1))
        goto one_up;
    if (gState[0x22b] != 0)
        goto after;
    if ((_RPGRandom() & 0xf) != 0)
        goto two_up;
one_up:
    *flg = 1;
    goto after;
two_up:
    if ((_RPGRandom() & 0x1f) == 0)
        *flg = 2;
after:
    Func_80c02a4(x, enc);
    *(int *)(p4 + 0x14) = 0;
    iwram_3001f58 = 0;
    StartTask(Func_80b7738, 0xc8 << 4);
loop:
    Func_80b9b2c();
    Func_80b5d3c();
    {
    int *p = _Func_8077330(0);
    if (*p)
        *vp = 3;
    else
        *vp = 1;
    }
    *(int *)p4 = 0xa0 << 6;
    *(int *)(p4 + 4) = 0x3c;
    _Func_801f200(*vp);
    en = st + (0xbb << 2);
    {
        int sz = 0xa0 << 1;
        void (*f)(int, int) = Func_80008d4;
        f((int)en, sz);
    }
    Func_8003f3c(*(int *)(st + 0x54));
    if (!_GetFlag(0xb5 << 1)) {
        Func_800488c();
        Func_80048a0();
        cnt = Func_80b9934(en);
        Func_800488c();
        Func_80048a0();
    } else {
        cnt = Func_80b8574(en);
    }
    *(int *)(st + 0x54) = AllocUploadSpriteGFX(0x80);
    _Func_801f200(*vp);
    if (cnt < 0)
        goto lose;
    {
    int i = 0;
    if (i < cnt) {
        int idx = 0xbb << 2;
        do {
            int h = *(short *)(st + idx);
            Func_800488c();
            Func_80048a0();
            if (!_GetFlag(0xb5 << 1)) {
                n = 0xa;
                if (i != 0)
                    n = 0;
                if (Func_80b9b30(st + idx, n) == 1)
                    goto won;
            } else {
                if (Func_80b874c(st + idx) == 1)
                    goto won;
            }
            Func_800488c();
            Func_80048a0();
            if (Func_80b6b40(1, 0) == 0)
                goto lost;
            if (Func_80b6b40(2, 0) == 0) {
                if ((unsigned int)h <= 7 && *(int *)(st + (0xa7 << 3)) == 1)
                    *(unsigned short *)(st + 0x3e) = 3;
                goto ended;
            }
            if (Func_80b6148() < 0)
                goto lose;
            i++;
            idx += 0x10;
        } while (i < cnt);
    }
    }
    *flg = 0;
    Func_80bf674();
    Func_80bf678();
    Func_80b7e7c();
    if (*pf) {
        if (Func_80b6148() < 0)
            goto lose;
    } else {
        WaitFrames(0x14);
    }
    if (!_GetFlag(0xb7 << 1))
        goto loop;
    {
    int ui = _Func_8017658(0xc47, 0, 4, 1);
    while (_Func_8017364() == 0)
        WaitFrames(1);
    _CloseUIBox(ui, 1);
    WaitFrames(1);
    ui = _Func_8017658(0xc48, 0xa, 4, 1);
    Func_80bb7c0(0x5c, 0x18);
    _CloseUIBox(ui, 1);
    WaitFrames(1);
    }
    goto loop;
ended:
    Func_80b63b0();
    if (!_GetFlag(0xb7 << 1)) {
        if (*pf)
            _PlaySound(0x3a);
        if (*(int *)(st + (0xa7 << 3)) != 0) {
            _PlaySound(0x3a);
            if (*(unsigned short *)(st + 0x3e) <= 1) {
                int idx = (*(unsigned short *)(st + 0x3c) << 1) + 0x10;
                _InitEnemyUnit(0x80, *(unsigned short *)(st + idx), 0x1a);
                _Func_80198dc();
                _Func_8019908(0x80, 1);
                _Func_80175a0(*(unsigned short *)(st + 0x3e) + 0x838);
                WaitTextPrompt();
            }
        }
        Func_80c2724();
    }
    _PlaySound(0x11);
    Func_8003b70(0x1e);
    Func_8003ce0();
    ret = *(int *)(st + (0xa7 << 3));
    goto done;
lose:
    Func_80b63b0();
    Func_80042c8(0);
    {
    unsigned short dz = 1;
    REG_DISPCNT = dz;
    }
    ret = *(int *)(st + (0xa7 << 3));
    _SetFlag(0xfa << 2);
    goto done;
lost:
    Func_80b63b0();
    _PlaySound(0x3b);
    _Func_80198dc();
    _Func_8019908(gState[0xfc << 1], 1);
    if (Func_80b6a60(0) == 1)
        _Func_80175a0(0x83d);
    else
        _Func_80175a0(0x837);
    WaitTextPrompt();
    _PlaySound(0x11);
    ret = -1;
    Func_8003b70(0x1e);
    Func_8003ce0();
    goto done;
won:
    _PlaySound(0x11);
    Func_8003b70(0x1e);
    Func_8003ce0();
    ret = 0x3e7;
done:
    Func_80b5b18();
    Func_80bf674();
    Func_80bf5a8();
    gState[0x22b] = 0;
    StopTask(Func_80b7738);
    Func_80c08e0();
    return ret;
}
