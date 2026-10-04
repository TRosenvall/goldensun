/* Func_80a8578  --  NON-MATCHING, 11 of 60 encodings
 *                   (ref 60, ours 60 -- COUNT EQUAL, so the figure IS a
 *                    distance.  SIZE EXACT at 140 bytes.  RELOCATIONS CLEAN.)
 *
 *   RE-MEASURED batch 322, brief I.  The parks 11 of 60 stands.  What is new is
 *   the DECOMPOSITION, the deciding rung priced against the compiler source, and
 *   a correction to the parks split claim.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a8578.c \
 *     asm/rom_a1000/rom_a7380_c_c.s --func Func_80a8578
 *
 * SPLIT SHAPE -- THE PARK IS WRONG HERE.  It says "single-function file, so it
 * would convert WHOLE with no split".  tools/datacheck.py:
 *
 *     asm/rom_a1000/rom_a7380_c_c.s
 *         data sections : .rodata
 *         functions     : Func_80a8578
 *         EXPORTS       : .Laf2fc  (already global -- NOT the set a split needs)
 *         -> converting a function here needs a TEXT/DATA SPLIT
 *         Func_80a8578  reads no data label -> split needs NO new export
 *
 * So a landing needs a TEXT/DATA split (the .rodata must keep its own object),
 * and `exports` is empty because this function reads no data label.  PINS: 0.
 *
 * ------------------------------------------------------------------------
 * THE 11, PER INDEX.  TWO independent causes, and one index belongs to both.
 *
 *   CAUSE A, 8 indices (5, 9, 11, 12, 15, 16, 22, 25) plus half of 41:
 *     one register exchange.  ROM has the second parameter `b` in r7 and the
 *     `base + 0x21a` pointer `p` in r6; we have them the other way.
 *   CAUSE B, 2 indices (38, 39) plus half of 41:
 *     rom   bl Func_8004938 / add r5, r0 / ldr r0, =0xbe6 / add r1, r5
 *                           / add r0, r7, r0
 *     ours  bl Func_8004938 / ldr r3, =0xbe6 / add r5, r0 / add r1, r5
 *                           / add r0, r6, r3
 *
 * Index 41 (`add r0, r7, r0` against `add r0, r6, r3`) carries BOTH, which is
 * why 8 + 2 + 1 = 11 and why pinning `p` to r6 reads 3, not 2.
 *
 * ------------------------------------------------------------------------
 * CAUSE A -- THE RUNG, AND THE MARGIN IS 0.87 PERCENT
 *
 * `.18.greg` says `;; 5 regs to allocate: 37 33 36 35 32`, so these are GLOBAL
 * allocnos and the denominator is `allocno[].live_length` -- which is exactly
 * the "across N insns" number `.17.lreg` prints.  `global.c:605`:
 *
 *     pri = floor_log2(n_refs) * n_refs / live_length * 10000 * size
 *
 * (and `local-alloc.c:1496` uses floor_log2 TOO -- the difference between the
 * two formulae is the DENOMINATOR, `death - birth` against `live_length`, not
 * floor_log2.  See FINDINGS.md; the old note at the top of this park quoting the
 * local formula was not wrong about floor_log2, only about which pass decided.)
 *
 *     allocno  role   n_refs  live_length  priority
 *       37     unit      4        11        7272
 *       33     b         7        37        3783   <-- allocated first, gets r6
 *       36     p         3         8        3750   <-- gets r7
 *       35     base      2         6        3333
 *       32     a         2        44         454
 *
 * Those five values reproduce greg published order `37 33 36 35 32` EXACTLY, in
 * that sequence, which is the proof the arithmetic is the right arithmetic.
 *
 * THE MARGIN BETWEEN 33 AND 36 IS 33 UNITS OUT OF 3783 -- 0.87 PERCENT.  The
 * park called it "a margin no spelling has moved"; it is a margin of less than
 * one percent, and the condition to flip it is exact:
 *
 *     priority(36) > priority(33)
 *       <=>  3 / live_length(p)  >  14 / 37  =  0.3784
 *       <=>  live_length(p)  <=  7          (it is 8)
 *
 * and equivalently n_refs(b) <= 6 (2*6/37 = 3243) would do it, as would
 * live_length(b) >= 71, or n_refs(p) = 4 (floor_log2 jumps to 2: 8/8 = 10000).
 * SHORTENING p LIVE RANGE BY ONE INSN IS THE WHOLE REMAINING QUESTION.
 *
 * AND THE FLIP IS SUFFICIENT.  Traced through find_reg: `unit` (37) goes first
 * and takes r5; r4 is skipped for both because -fcall-used-r4 makes it
 * call-clobbered and both cross calls.  If 36 went before 33, p conflict set
 * (r0, r3, r5, sp, lr) leaves r6 and r7, REG_ALLOC_ORDER picks r6, and b then
 * has r6 taken and lands on r7.  That is the ROM, exactly.
 *
 * WHERE p LIVE RANGE COMES FROM, so the next reader can aim at it.  `.12.life`:
 * p (reg 36) is born at insn 34 in basic block 4, stays live to the end of
 * block 4 (insns 37, 39, 40-call, 42, 45, 48, 49, 50-jump = 7), is NOT live in
 * block 5 (the `b = 8` arm), and DIES AT INSN 64, the FIRST insn of block 6.
 * 7 + 1 = 8.  Note the RTL reads `*p` BEFORE `unit[0xf]`; the ROM final order
 * (unit[0xf] at index 24, `*p` at 25) is sched2 transposing them.  So the
 * single insn to remove is one of block 4 seven, not anything in block 6.
 *
 * MEASURED AND INERT, all 11, and SCREENED BY .17.lreg not by the figure --
 * these have allocator inputs BIT-IDENTICAL to this body, so they are edits
 * that NEVER REACHED THE ALLOCATOR, not evidence the lever is dead:
 *   naming 0x80 in a local, before or after the allocation call
 *   naming `b + 0xbe6` in a local
 *   `p = base + 0x21a` written twice (CSE folds it back)
 *   `*p` spelled `p[0]`
 *   `b >= 4` for `b > 3`
 *   `b - 1 == 0` for `b == 1`
 *   naming the 5 of _Func_8019908 in a local
 *   `((int *)unit)[0x49]` for `*(int *)(unit + 0x124)`
 *   declaration order: p first, buf first
 *
 * MEASURED, REACHED THE ALLOCATOR, AND STILL 11 -- these DID move the allocno
 * set and still did not reorder 33 against 36:
 *   first byte read as `base[0x21a]` with p born in the else arm only
 *       -> p becomes reg 42; greg order `37 33 42 35 32`.  Still b first.
 *   p declared in an inner `{}` scope
 *       -> p becomes reg 39, unit becomes 36; greg order `36 33 39 35 32`.
 *   `_Func_8079008(p[0], 1 + unit[0xf])`
 *
 * MEASURED AND WORSE:
 *   hoisting `unit[0xf] + 1` into a local        11, but p live_length 8 -> 10
 *       and greg order becomes `37 33 35 36 32` -- p drops BELOW `base`.  This
 *       is the one edit proven to move p live_length, and it moves it the
 *       WRONG WAY.  The lever is live; the sign is wrong.
 *   hoisting `*(int *)(unit + 0x124)` into a local         22, RELOCDIFF
 *   inverting the `== 0x63` branch                         23, RELOCDIFF
 *   an int temp for the `== 0x63` comparison               44, RELOCDIFF
 *   dropping the named `base`                              19
 *   `b > 3 && c == 0` instead of `c == 0 && b > 3`         15
 *   `b` as `unsigned int`                                  12
 *   `b` copied into a second local                         12
 *   naming the -1 of _Func_8017aa4                         14
 *   moving the allocation call to the top of the function   62, size +8
 *   (from the park) naming `b + 0xbe6` BEFORE the alloc call  62 insns, 144 b
 *   (from the park) the parameter pinned into r7              53 insns, 124 b
 *
 * ------------------------------------------------------------------------
 * CAUSE B -- THE PARK DIAGNOSIS IS REFUTED.  IT IS NOT A SCHEDULER PREFERENCE.
 *
 * The park says "We hoist the pool load above the assignment and it therefore
 * has to use r3", and files it with batch 265 sched2 result on Func_80270d8 --
 * a bare load with no dependence floating up into a gap.  The causality is the
 * other way round.  `.12.life` block 7 already has the ROM order:
 *
 *     (insn 102 (set (reg 38) (reg 0 r0)))            buf = result
 *     (insn 105 (set (reg 56) (const_int 3046)))      0xbe6
 *     (insn 107 (set (reg 57) (plus (reg 33) (reg 56))))
 *     (insn 109 (set (reg 0 r0) (reg 57)))
 *
 * so 105 is ALREADY below 102 before sched2, and the ROM keeps it there.  In the
 * ROM, reg 56 is allocated **r0** -- `ldr r0, =0xbe6` -- and r0 is read by insn
 * 102.  A write to r0 at 105 therefore carries an ANTI-DEPENDENCE on 102 and
 * sched2 CANNOT hoist it.  In our compile reg 56 gets r3, which no insn in the
 * gap touches, so the load is dependence-free and floats.
 *
 * THE ORDER IS A CONSEQUENCE OF THE REGISTER, NOT THE REVERSE.  So the question
 * is why reg 56 does not get r0, given that r0 dies at insn 102 and reg 57 (the
 * sum) DOES get r0 in both compiles.
 *
 * ONE EXPLANATION TRIED AND CLOSED, with its evidence.  `local-alloc.c:1392-1462`
 * holds a `fake_birth`/`fake_death` lifetime extension whose stated purpose is
 * "to discourage the register allocator from creating false dependencies" --
 * i.e. the compiler deliberately avoiding exactly the r0 choice the ROM made.
 * That looked like the answer and IT IS NOT: the gate is
 *
 *     flag_schedule_insns_after_reload && !optimize_size && !SMALL_REGISTER_CLASSES
 *
 * and `config/arm/arm.h:1061` reads `#define SMALL_REGISTER_CLASSES TARGET_THUMB`,
 * which is TRUE for this build.  BOTH fake-lifetime calls are therefore skipped
 * and every allocation here uses the real birth/death.  Do not spend a round on
 * this; it is shut.
 *
 * WHAT IS LEFT, as the concrete next step.  reg 56 has NO copy to a hard reg, so
 * it gets no `qty_phys_copy_sugg`, so `find_free_reg` walks REG_ALLOC_ORDER
 * {3,2,1,0,...} and takes r3 -- which is free across its two-insn range 105..107.
 * reg 57 DOES have a suggestion (insn 109 copies it to r0) and takes r0; 56 and
 * 57 have touching lifetimes and could share r0 under the two-slots-per-insn
 * birth/death encoding, which is what the ROM did.  So the ROM compile must have
 * had r3 -- and r2 and r1 -- OCCUPIED across insns 105..107, pushing reg 56 down
 * REG_ALLOC_ORDER to r0.  In our RTL the three argument fills (r1 = buf at 111,
 * r2 = 0x80 at 113) and the `-1` in r3 all sit AFTER 107.  THE NEXT QUESTION IS
 * THEREFORE: what spelling materialises the `-1` of _Func_8017aa4 (or an argument
 * fill) BEFORE the 0xbe6 load in the RTL, without adding an insn?  Naming the -1
 * in a local measures 14, so not that spelling -- but that is one probe, not a
 * bound.  Stated as evidence, not as a conclusion.
 *
 * WHICH CAUSE IS CLOSER: cause A.  It is 9 of the 11 indices, its rung is priced
 * to within 0.87 percent, and the target is a one-insn change to a live range in
 * one basic block.  Cause B is 3 indices and needs a compiler-configuration
 * question answered first.
 */
#include "gba/types.h"

extern unsigned char *iwram_3001f2c;
extern u8 *_GetUnit(s32 id);
extern int _Func_8079008(int a, int b);
extern void _Func_8019908(int a, int b);
extern void *Func_8004938(unsigned int size);
extern void _Func_801965c(int a, void *buf, int n);
extern void _Func_8017aa4(void *buf, int b, int c, int d);
extern void free(void *p);

void Func_80a8578(int a, int b, int c)
{
    u8 *base;
    u8 *p;
    u8 *unit;
    void *buf;

    base = iwram_3001f2c;
    if (c == 0 && b > 3)
        b++;
    if (b == 1) {
        p = base + 0x21a;
        unit = _GetUnit(*p);
        if (unit[0xf] == 0x63) {
            b = 8;
        } else {
            _Func_8019908(_Func_8079008(*p, unit[0xf] + 1) - *(int *)(unit + 0x124), 5);
        }
    }
    buf = Func_8004938(0x100);
    _Func_801965c(b + 0xbe6, buf, 0x80);
    _Func_8017aa4(buf, a, 0, -1);
    free(buf);
}
