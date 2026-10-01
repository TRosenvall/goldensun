/* Func_80b920c (0x080b920c) -- NON-MATCHING, 116 of 131 encodings (ours 127).
 * Blocker class: ONE MISSING ALLOCNO -- a loop-invariant mask that LICM refuses.
 * Never attempted before batch 277.
 *
 * asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a_c_c_a.s (1 function after batch 277's two
 * splits, so landing needs NO further split).
 *
 * READ THE COUNT WITH ITS CAUSE, because 116 of 131 looks far worse than it is.
 * THE INSTRUCTION STREAM IS IDENTICAL ONE-FOR-ONE THROUGH THE WHOLE FUNCTION --
 * every mnemonic and every offset lines up. Only register NAMES differ, plus five
 * instructions the ROM has and we do not. This is the count-is-not-a-difference
 * rule in its most extreme form seen so far in this corpus: one root cause,
 * renaming almost everything.
 *
 * THE ROM'S INNER LOOP HOLDS THIRTEEN LIVE VALUES:
 *
 *     u -> r0                      bufA + nb*2 -> r1        out + na*16 -> r2
 *     &bufB[i] -> r4               j -> r5                  cursor -> r6
 *     na -> r8                     nb -> r9                 countdown -> r10
 *     0xffffff00 -> r11            u + 0x43 -> r12          u + 0x13c -> r14
 *     u + 0x138 -> [sp, #0]        (spilled, hence sub sp,#0x10 to our #0xc)
 *
 * Ours has the same twelve MINUS THE MASK, which gcc rematerialises as
 * `ldr r2, =0xffffff00` at the use.
 *
 * PRICED FROM `.08.loop`. The mask is refused:
 *
 *     Insn 101: regno 61 (life 1), move-insn savings 1 -- not desirable
 *
 * On loop pass 2 the inner loop is 31 real insns with no prior moves, which
 * BRACKETS Thumb's `threshold = (loop_has_call ? 1 : 2) * (1 + n_non_fixed_regs)`
 * BELOW 31 for a CALL-FREE inner loop. Read that with the recorded value of 23 for
 * a loop WITH a call: together they bound the n_non_fixed_regs-derived threshold
 * usefully for the next function in this class, which is the transferable part.
 *
 * AND THE OBVIOUS FIX MAKES IT WORSE IN A SPECIFIC WAY. Three placements of a
 * named `int mask` (function top, outer-loop body, inner-loop body) and an
 * `(mask = 0xffffff00) & ...` assignment-expression all measure 120-121: when the
 * mask IS hoisted (`Insn 80: regno 36 (life 11) moved`) it moves FIRST, the
 * threshold stays high for the later movables, and `u + 0x40` gets hoisted and
 * spilled instead -- which the ROM does not do. So the ROM needs the mask accepted
 * as the THIRD movable with life >= 2, and no spelling found puts it there.
 * `move_movables` processes in order and the order is what would have to change.
 *
 * ALSO INERT: four declaration orders; `i = 0` against `i = na`; `bufA[nb++]`;
 * `unsigned int` for the masked load.
 *
 * WHAT IS RIGHT: the ROM initialises this loop's counter from `na`, a live variable
 * already holding zero (`cmp r8, r5`), which is the same lever that was required
 * for the file-mate Func_80b90f8 -- second instance in one `.s`.
 *
 * AND A COUNTER-EXAMPLE TO A SIBLING PARK'S GENERALISATION. src/non_matching/
 * rom_b5000/80b90ac.c concludes that "gcc will not emit a redundant address copy
 * into any callee-saved register, whatever the pressure." The ROM's `mov r4, r6` in
 * this function's inner-loop preheader is the same copy-a-base-into-a-second-
 * register shape AND GCC DOES REPRODUCE IT -- because here it is LICM's
 * hoist-and-copy of `&bufB[i]`, a different mechanism from that park's guard-local
 * copy. The park's r7-against-r8 refinement stands for its own case; the
 * generalisation does not.
 *
 * NEXT: move_movables' ordering, not a spelling. This is the third instance in this
 * `.s` of a residue that is decided inside one pass's cost comparison.
  *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/80b920c.c \
 *     asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a_c_c_a.s --func Func_80b920c
 *
 * RECIPE ADDED IN BATCH 311.  This park carried an "N of M" figure with no way to
 * re-measure it, so its number could never be caught lying -- the dangerous half of
 * what parkcheck used to lump into one UNCHECKABLE verdict.  A tree-wide sweep found
 * eight such parks; this is one of them.  The figure above is NOT re-measured by the
 * act of adding this line: run it.
 *
 * ============ BATCH 316: THE LICM TEST, WITH ITS INEQUALITY ============
 * Still 116 of 131 (ours 127). The blocker is right. The gate is not `savings`
 * on its own -- `.08.loop` shows `savings 1` being accepted:
 *     Insn 130: regno 73 (life 7), move-insn savings 1   MOVED
 *     Insn 101: regno 61 (life 1), move-insn savings 1   not desirable
 *     Insn  82: regno 51 (life 2), savings 2             MOVED
 * `move_movables`' test is `threshold * savings * lifetime >= insn_count`, with
 * threshold 20 and the inner loop at insn_count 31. So the mask needs only
 * `savings * lifetime >= 2` -- LIFETIME 2 WOULD BE ENOUGH, which is this park's
 * "life >= 2" intuition with the inequality behind it. Identified in `.07.gcse`:
 *     insn 101 = (set (reg:SI 61) (const_int -256))                  the mask
 *     insn 149 = (set (reg:SI 81) (plus (reg/v:SI 35) (const_int 64)))  u + 0x40
 *
 * WHY FORCING IT CASCADES, which this park could observe but not explain: a
 * named `int mask` is hoisted on PASS 1 as `regno 43 (life 11) moved`, first.
 * That removes one insn from the inner loop, and on PASS 3 `regno 81 (life 1)`
 * -- `u + 0x40` -- then satisfies `20*1*1 >= insn_count` and is hoisted too,
 * which the ROM does not do (it computes it inline: `adds r3,r0,#0 / adds r3,#64
 * / ldrh`). The cascade is an insn_count threshold crossing and it is ONE INSN
 * WIDE. That is why every forced spelling lands at 117-119 and none at 116.
 *
 * 38 VARIANTS, TWO AXES CROSSED (scratch_elev/b316e/v920c and v920d).
 * BYTE-IDENTICAL at 116: all four `u + 0x40` spellings (`((u16 *)u)[0x20]`,
 * `&u[0x40]`, a named `q = u + 0x40`), `& ~0xff`, `w = load; w &= 0xffffff00;`,
 * `0xffffff00 & load` (gcc canonicalises `const & reg` back to `reg & const`, so
 * OPERAND ORDER DOES NOT separate the constant from its use), `unsigned int` and
 * `long` carriers, and dropping the `!= 0`.
 * REGRESSIONS: the named mask at three placements 117; the load named ahead of
 * the condition 119; the `||` arms swapped 119; mask and load both named 119.
 * The `u`-access axis is COMPLETELY INERT, so the 2x2 cross adds nothing: the
 * mask's `lifetime` is the only live variable and no C spelling moves it.
 * **"move_movables' ordering, not a spelling" STANDS.**
*/
struct Entry {
    unsigned short id;
    unsigned short f2;
    unsigned short move;
    unsigned short f6;
    unsigned short f8;
    unsigned short fa;
    unsigned short fc;
    unsigned short fe;
};

extern void *Func_8004970(int size);
extern int Func_80b6b40(int kind, unsigned short *buf);
extern unsigned char *_GetUnit(int id);
extern int _Func_8027114(struct Entry *e, unsigned short *ids, int n);
extern void free(void *p);

int Func_80b920c(struct Entry *out)
{
    unsigned short *bufA;
    unsigned short *bufB;
    unsigned char *u;
    int cnt;
    int i;
    int j;
    int na;
    int nb;
    int r;
    int res;

    bufA = Func_8004970(0x11);
    bufB = Func_8004970(9);
    cnt = Func_80b6b40(1, bufB);
    na = 0;
    nb = 0;
    for (i = na; i < cnt; i++) {
        u = _GetUnit(bufB[i]);
        for (j = 0; j < u[0x43]; j++) {
            if (u[0x13c] != 0 || (*(int *)(u + 0x138) & 0xffffff00) != 0) {
                out[na].id = bufB[i];
                out[na].move = *(unsigned short *)(u + 0x40);
                out[na].f6 = 8;
                out[na].f8 = 0;
                out[na].fa = 0xc0 << 1;
                na++;
            } else {
                bufA[nb] = bufB[i];
                nb++;
            }
        }
    }
    out += na;
    r = _Func_8027114(out, bufA, nb);
    res = -1;
    if (r >= 0)
        res = na + r;
    free(bufB);
    free(bufA);
    return res;
}
