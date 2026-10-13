/* Func_801fe2c (0x0801fe2c) -- NON-MATCHING, 94 of 110 encodings.
 *   BODY UNCHANGED by batch 346 brief A; the header below is what changed.
 *   ref 103 instructions, ours 102  (-1).  SIZE ref 232 bytes, ours 228.
 *   ALIGNED RESIDUE 49 of 110 in 21 hunks (tools/aligncmp.py) -- that is the
 *   number to beat, NOT 94.  The stream is 110 against 108: one missing 16-bit
 *   instruction plus the alignment pad it forced, so 94 is mostly PHASE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801fe2c.c \
 *     asm/rom_15000/rom_1fe2c_a.s --func Func_801fe2c
 *
 * asm/rom_15000/rom_1fe2c_a.s (1 function, no data section, so landing needs
 * NO split).  No pins, no devices.
 *
 * *** WHERE THE -1 IS.  THREE SITES, NOT ONE. ***  Counted region by region off
 * the two listings (16-bit instructions only; the six 32-bit stream entries are
 * four `bl`s and two pool words):
 *
 *   region                                        ref  ours  gap
 *   A  entry + the pre-loop `*(k+p) != -1` guard   23    24   +1
 *   B  loop pre-header + the body's first-arg load 19    16   -3
 *   C  loop body (the three calls, sprite setup,
 *      the two coordinate stores)                  32    32    0
 *   D  increment group + bottom-test entry         11    12   +1
 *   E  bottom test + StartTask + epilogue          18    18    0
 *                                                 103   102   -1
 *
 * A net -1 made of +1/-3/+1 is why single-site reasoning has not worked here.
 * Region B is the real deficit and it is the park's own gcse story; regions A
 * and D are each one instruction of SURPLUS that the deficit is hiding.
 *
 * THE REGION-B DEFICIT, CONFIRMED.  The previous header's diagnosis REPRODUCES:
 * gcse moves the body's `*(signed char *)(k + p)` load up into BOTH test blocks,
 * where it becomes a zero-extending `ldrb` (ours, region A index 19 and region
 * D), and the body then pays `lsl r0, #24` / `asr r0, #24` to sign-extend it.
 * The ROM instead re-loads with `ldrsb r0, [r1, r2]` in the pre-header and keeps
 * `0x2c` live in r8, paying `movs r3, #44` / `mov r8, r3` / `mov r2, sl` /
 * `mov r1, r8` -- five instructions we do not emit.  So region B is short
 * because the load was hoisted, exactly as written up.
 *
 * THE REGION-A SURPLUS IS NEW and it is the opposite sign, which is why it was
 * never seen: the ROM builds -1 as `movs r2, #1` / `negs r2, r2` (two
 * instructions) while we build it as `subs r2, #1` (one) on the r2 that still
 * holds the `i = 0` constant.  cse is tracking that 0 and -1 is cheaper as
 * 0 - 1 than as a fresh constant.  Any fix for region B that does not also
 * restore the ROM's two-instruction -1 will LAND ONE INSTRUCTION LONG.
 *
 * *** THE FLAG AXIS IS CLOSED ON THE ALIGNED METRIC, NOT JUST POSITIONALLY. ***
 * This matters because the park's own mechanism is a gcse one and -fno-gcse
 * reads only "97 of 110", which looks like a positional artifact worth
 * re-checking.  It is not -- measured by hand via OBJCMP_EXTRA, with aligncmp:
 *   base                           94 of 110   103/102   232/228   aligned 49 / 21 hunks
 *   -fno-gcse                      97 of 110   103/100   232/224   aligned 53 / 23 hunks
 *   -fno-schedule-insns2           96 of 110   103/102   232/228   aligned 55 / 22 hunks
 *   -fno-rerun-cse-after-loop      71 of 110   103/104   232/232   aligned 55 / 20 hunks
 *   -fno-strength-reduce          107 of 110   103/108   232/240   aligned 89 / 17 hunks
 * ALL FOUR ARE WORSE ALIGNED.  -fno-rerun-cse-after-loop is the interesting
 * negative: it equalises the BYTE SIZE and flips the sign of the gap (ours one
 * instruction LONG), and its 71 is the lowest figure any flag produces here --
 * and its aligned residue is 55 against 49.  Do not chase it.
 * autosweep.py also measured -O1 105, -ffixed-r7 108, -fno-strict-aliasing 94
 * (no effect), -fno-rerun-loop-opt 94 (no effect).
 *
 * ANCHOR AXIS CLOSED: 14 of 14 statement boundaries, all inert or worse
 * (autosweep.py, batch 346, device-free, NOT capped).
 *
 * MEASURED THIS BATCH, on top of the body below:
 *   `base = iwram_3001f2c;` MOVED BELOW `i = 0; k = ...;`
 *                 -> 85 of 110, ref 103 / ours 103, SIZE EQUAL 232/232,
 *                    but ALIGNED 51 in 21 hunks (worse than 49).
 *     THE LENGTH GAP ON THIS FUNCTION IS CLOSABLE BY STATEMENT ORDER ALONE and
 *     it is not progress -- it is the brief's Func_8093af8 trap exactly: the
 *     figure falls from 94 to 85 because the phase offset disappears while the
 *     aligned residue gets WORSE.  Kept as p1/a4.c.  Anyone who closes the
 *     length here must check aligncmp before believing it.
 *   `i = 0;` after `k = ...;`                -> aligned 51 (worse)
 *   `!= (signed char)0xff` for `!= -1`       -> BIT-IDENTICAL to base
 *   naming the body's first argument in a local -> BIT-IDENTICAL to base
 *   naming -1 in a local and comparing against it -> aligned 62 (much worse)
 *   the 4th parameter typed `signed char *`  -> does not compile (pointer
 *     arithmetic on `k + p` with both pointer-typed); the park's "x2" row
 *     reached 35 on an OLDER metric and its body is not recoverable from the
 *     header, so treat that number as unverified.
 *
 * THE MUTUAL EXCLUSION, carried forward from the previous header and still the
 * standing finding.  An explicit `int k` index biv gives the ROM's
 * `ldrsb [k, p]` addressing -- but it makes the loop-top test and the body load
 * syntactically identical, so they get commoned and you gain a split `ldrb` +
 * `lsl`/`asr` pair.  Writing `p[0x2c + i]` instead kills the commoning, but then
 * loop.c folds `p` into the giv's additive term and the addressing is lost.
 * vA = 41, vB = 36 on the old tryc --align metric, and NO SPELLING AMONG 22 GOT
 * BOTH.  `t3_c3.c.08.loop`:
 *
 *     Insn 31: giv reg 41 src reg 38 benefit 2 lifetime 2 replaceable ncav mult 1 add (reg/v:SI 35)
 *     Insn 33: giv reg 42 src reg 38 benefit 4 lifetime 1 replaceable mult 1 add (plus:SI (reg/v:SI 35)
 *     giv of insn 33 not worth while, 34 vs 67.
 *     giv at 31 reduced to (reg:SI 130)
 *
 * reg 35 is the `p` parameter.  With `p[0x2c + i]`, loop.c ALWAYS folds the
 * invariant `p` into the giv's additive term, so `p` can never survive as a
 * separate base register from that spelling.  That is a pass property, not a
 * spelling gap -- BUT note what it does and does not bound: it bounds the
 * `p[0x2c+i]` FAMILY, not the function.  The previous header's "NEXT: nothing
 * source-level" over-reached from it.
 *
 * AND THE SIGN-EXTEND RESIDUE IS CREATED IN combine, not in the source.
 * `vB.c.13.combine` shows `ashiftrt` 5 -> 1 and `mem:QI` 5 -> 6 across that one
 * pass: combine fuses four of five `(sign_extend (mem))` into `ldrsb` and cannot
 * fuse the fifth, because the load and its shifts are in DIFFERENT BASIC BLOCKS
 * by then.
 *
 * REMAINING DIFFERING PAIRS, re-read on the installed body this batch:
 *   sign-extend split   3  rom `ldrsb r0,[r1,r2]` in the pre-header / ours a
 *                          hoisted `ldrb r0,[r3,r4]` in each test block plus
 *                          `lsl r0,#0x18` / `asr r0,#0x18` in the body
 *   the -1 build        1  rom `movs r2,#1 / negs r2,r2` / ours `subs r2,#1`
 *   k/p register swap   ~4 rom k:r8 p:sl (so it re-materialises 0x2c) /
 *                          ours k:sl p:r8-then-r4 (so it does not)
 *   coordinate sums     4  rom `ldr r1,[sp,#0xc]` before the `ldrh`, then
 *                          `add r3,r1,r3` / ours `ldrh` first then `add r3,r3,r2`
 *   register choice    ~3  rom `movs r1,#13` / ours `movs r4,#13`, and knock-ons
 *
 * NEXT, and the two routes nobody has taken.  (1) REGION A IS A SEPARATE,
 * SMALLER PROBLEM than region B and it has its own sign: find a spelling that
 * makes gcc build -1 the ROM's way (two instructions) without touching the
 * loop, measure region A alone, and only then attack region B -- because every
 * region-B fix so far has been scored against a total that region A was
 * cancelling.  (2) The commoning that region B blames is a LOAD MOTION, so the
 * thing to try is not another spelling of the two loads but a reason the two
 * BLOCKS cannot share: the pre-loop guard comes from duplicate_loop_exit_test,
 * which runs before gcse, so the shape of the GUARD is reachable even though
 * the shape of the load is not.
 * Do NOT re-screen: the flag axis (now closed on the aligned metric, see the
 * table above), the anchor axis, or `p[0x2c+i]`-family spellings.
 */
extern unsigned char *iwram_3001f2c;
extern void *_CreateSprite(void *resource);
extern void _Sprite_SetAnim(void *sprite, int anim);
extern void StartTask(void *task, int prio);
extern void Func_801ff58(void);
extern void *_Func_808b3d0(int a, int b);

struct S {
    unsigned char pad0[9];
    unsigned char b0 : 2;
    unsigned char b2 : 2;
    unsigned char b4 : 4;
    unsigned char pad1[0x1c];
    unsigned char f26;
};

struct P {
    unsigned char pad[0xc];
    unsigned short x;
    unsigned short y;
};

void Func_801fe2c(struct P *a, int b, int c, unsigned int p)
{
    unsigned char *base;
    struct S *spr;
    int i;
    signed char *k;

    base = iwram_3001f2c;
    i = 0;
    k = (signed char *)0x2c;
    while (i < 4 && *(signed char *)(k + p) != -1) {
        spr = (struct S *)_CreateSprite(_Func_808b3d0(*(signed char *)(k + p), *(signed char *)(0x33 + p)));
        if (spr != 0) {
            _Sprite_SetAnim(spr, 1);
            spr->f26 = 0;
            spr->b2 = 0;
        }
        *(void **)(base + 0x114 + i * 4) = spr;
        *(short *)(base + 0x134 + i * 2) = ((b + a->x + i * 3) << 3) + 0x10;
        *(short *)(base + 0x144 + i * 2) = ((a->y + c) << 3) + 0x10;
        *(int *)(base + 0x154 + i * 4) = 0x80 << 9;
        i++;
        k++;
    }
    StartTask(Func_801ff58, 0xc8 << 4);
}
