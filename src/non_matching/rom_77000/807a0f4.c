/* Func_807a0f4 -- 0x0807a0f4,
 * asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_a_c_a_a.s (1 function, no data section, so
 * landing needs NO split).
 *
 * NOT MATCHING: 7 differing of 88 encodings, 92 lines against 92 -- LENGTH EXACT, and
 * relocations identical. Candidate below.
 *
 * THE WHOLE RESIDUE IS ONE REGISTER CONTEST, r5 against r6:
 *
 *   idx  rom                  ours
 *    33  add r6, r3, r2       mov r6, r0
 *    34  mov r5, r0           add r5, r3, r2
 *    36  ldrb r0, [r6]        ldrb r0, [r5]
 *    56  ldrb r3, [r6]        ldrb r3, [r5]
 *    60  sub r5, #1           sub r6, #1
 *    61  add r6, #1           add r5, #1
 *    62  cmp r5, #0           cmp r6, #0
 *
 * Every other register matches the ROM -- r7 the id, r8 the best, r9 the best index,
 * r10 the effect, r11 the flag.
 *
 * IT IS A REFERENCE-COUNT CONTEST, NOT A SPELLING, and the arithmetic says so. From
 * .18.greg:
 *
 *   ;; 12 regs to allocate: 38 37 42 41 39 79 40 32 36 33 35 34
 *   ;; Register dispositions: ... 40 in 6 ... 79 in 5
 *
 * Pseudo 79 is the strength-reduced gState byte cursor with FIVE references (its def, two
 * ldrb, and `add r79, r79, 1` counting twice); pseudo 40 is the check_dbra_loop
 * down-counter with FOUR. They are born adjacent in the RTL (insn 275 and insn 286), so
 * their live lengths differ by one. allocno_compare gives 79 `floor_log2(5)*5*4 = 40/len`
 * against 40's `32/len`, and with lengths differing by one over a ~30-insn loop 40 CAN
 * NEVER OVERTAKE 79 at this reference-count pair.
 *
 * So the ROM's allocation needs either the cursor at 4 references or the counter at 5 --
 * a different reference count, not a different spelling. (At 4 against 4 the tie breaks by
 * allocno number, 40 < 79 takes r5, and that IS the ROM. So batch 272's declaration-order
 * lever would apply if the counts were equal; they are not.)
 *
 * WHAT IS ALREADY RIGHT, and all of it was needed to get here:
 *   - the call-argument fill order `r1, r2, r0` needs GiveDjinni and Func_807a458 declared
 *     INT-returning, not void (void gives `r0, r1, r2`).
 *   - `ldr r3, =gState` plus `mov r2, #0xfc / lsl r2, #1 / add` needs the offset as a
 *     NAMED VARIABLE; `&gState + 0xfc*2` folds to `ldr r3, =gState+504`.
 *   - the cursor init must sit AFTER the loop guard, which means the subscript has to be
 *     INSIDE the loop: a giv init lands at loop_start, i.e. after the duplicated exit
 *     test, where a source statement before the loop lands before it.
 *   - `add r3, r7, r2 / ldrb r3, [r0, r3]` needs `o = 0x8c * 2;` as its own statement used
 *     twice.
 *   - the inner loop's `sub r2, #1 / add r0, #1 / add r1, r3` needs `t = *q; q++;
 *     sum += t;` -- 9 differing with `sum += *q++` or with `sum += *q; q++;`.
 *
 * MEASURED, all exactly 7 and no change: four spellings of the gState byte access
 * (`((unsigned char *)&gState)[base+i]`, the int-cast form, the base-added form, and a
 * struct member); `base` as int against unsigned int; `for`, `while` and
 * `i = 0; if (size > i) { do ... }` outer forms -- the last of which is 75, because it
 * breaks the reversal.
 *
 * WORSE: the cursor as a source pointer before the loop (`p += base; p[i]`) is 64, because
 * it keeps `i` live so check_dbra_loop does not reverse; the cursor assigned inside the
 * loop is 49 and 64, with gcc falling to r4 and spilling.
 *
 * FLAGS, all inert at 7: -fno-rerun-cse-after-loop, -fno-strict-aliasing, -fno-regmove,
 * -fno-cse-follow-jumps, -fno-caller-saves, -fno-force-mem, -fno-thread-jumps,
 * -fno-peephole, -fno-delayed-branch, -fno-function-cse. WORSE: -fno-schedule-insns2 27,
 * -fno-gcse 78, -fno-expensive-optimizations 82, -fno-strength-reduce 90. So no existing
 * Makefile flag group lands it either.
 *
 * NEXT: the only routes are to change a reference count (give the cursor a fourth use or
 * the counter a fifth) without changing the emitted instructions, or to stop the cursor
 * being strength-reduced at all -- for which see the `goto`-loop lever recorded in batch
 * 274, which suppresses strength_reduce outright by removing NOTE_INSN_LOOP_BEG. That was
 * not tried here and is the obvious next move.
 */
typedef struct { unsigned char _b[704]; } GlobalState;
extern GlobalState gState;
extern int GetFlag(int flagID);
extern void SetFlag(int flagID);
extern int GetPartySize(void);
extern unsigned char *GetUnit(int id);
extern int GiveDjinni(int who, int id, int effect);
extern int Func_807a458(int who, int id, int effect);

int Func_807a0f4(int id, int effect)
{
    int flag, bestIdx, best;
    unsigned char *u, *q;
    int size, i, sum, k, o, t;
    unsigned int base;

    flag = id * 20 + effect + 0x30;
    bestIdx = 0;
    best = 0x3e7;
    if (GetFlag(flag) != 0)
        return -1;
    size = GetPartySize();
    base = 0xfc * 2;
    for (i = 0; i < size; i++) {
        u = GetUnit(((unsigned char *)&gState)[base + i]);
        o = 0x8c * 2;
        if (u[id + o] <= 9) {
            q = u + o;
            sum = 0;
            for (k = 3; k >= 0; k--) {
                t = *q;
                q++;
                sum += t;
            }
            if (best > sum) {
                best = sum;
                bestIdx = ((unsigned char *)&gState)[base + i];
            }
        }
    }
    if (best == 0x3e7)
        return -2;
    GiveDjinni(bestIdx, id, effect);
    Func_807a458(bestIdx, id, effect);
    SetFlag(flag);
    return bestIdx;
}
