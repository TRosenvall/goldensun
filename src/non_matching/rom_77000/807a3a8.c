/* Func_807a3a8 -- 0x0807a3a8 -- asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_c_a_a.s
 *
 * BLOCKER: loop shape of the search loop.  68 encodings of 87 differ (objcmp:
 * "ENCODINGS differ in 68 place(s) (ref 87, ours 87)") -- same length as the
 * ROM, but the search loop is laid out differently, so almost everything after
 * the call is offset.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_77000/807a3a8.c asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_c_a_a.s --func Func_807a3a8
 *
 * WHAT IT DOES: pick the table bank by `(unsigned)unit <= 7 ? 0 : 1`
 * (Func_8077330), search the 4-byte entries at rec+8 (count at rec+0x108) for
 * one with [0]==a and [1]==b; on a hit decrement the count, set found=1 and
 * shift the tail of the table down one word.  Returns found.
 *
 * EXACT ALREADY: the prologue (r8/r9/r10 saves), the bank select and the call.
 *
 * WHAT THE ROM HAS that this candidate does not:
 *   - the search loop is STRENGTH-REDUCED with three separate givs:
 *     r1 = ent+4i (the [0] byte), r0 = rec+9+4i (the [1] byte -- based on
 *     REC, not ent, which is why loop.c cannot combine it with r1), and
 *     r5 = 4i, copied into o (r6) each iteration for the shift loop.
 *   - it has a PRE-CHECK: a copy of the whole i=0 iteration, including the
 *     found-block (`sub/str/mov r9` then `b L3`), in front of the loop.
 *   GiveInnateMove (solved in the same batch) has exactly this shape, and its
 *   mechanism was measured there: the for-loop's exit code (test + body) must
 *   be MORE than 20 insns at jump1, so jump.c's duplicate_loop_exit_test
 *   leaves it alone and loop.c strength-reduces it; after loop.c the exit code
 *   has shrunk below 20 and the jump pass that runs after loop (visible in
 *   the .09.cse2 dump as a new NOTE_INSN_LOOP_VTOP) makes the pre-check copy.
 *   Every spelling tried here is duplicated at jump1 instead (or, with the
 *   bound as a memory load, the test is rotated to the bottom by
 *   expand_end_loop and only the test is copied), so the loop is either
 *   "ignored due to multiple entry points" (no reduction) or reduced without
 *   the pre-check.  tools: `-dL` dump + grep "multiple entry"; count the
 *   exit code in .02.jump.
 *   - the shift loop is entered in the MIDDLE (`b L4` past `lsl r6, r4, #2`)
 *     so its first iteration reuses o from the search; gcc reproduced this
 *     only in the nested variant (shift inside the found-block, F3 shape),
 *     which in turn kills the search loop's reduction ("Increment value can't
 *     be calculated" when the inner loop writes i).
 *
 * TRIED, all worse or no closer (tryc differing / objcmp):
 *   shift nested inside the found block, sharing i (65 insns, 75 differ);
 *   same with a separate j (84 differ); search + `if (found)` shift (F2, 86);
 *   o = i*4 named in the search loop; [1] as rec[o+9] / (rec+9)[o] /
 *   ent[o+1]; struct Ent field access; count as `int *cnt` vs recomputed
 *   `*(int *)(rec + 0x108)`; `continue`-style and nested-if searches; while
 *   loop; `*cnt = *cnt - 1` bulk.  None moved the jump1 duplication decision.
 *
 * NEXT: find a body spelling whose RTL is >20 insns at jump1 but that cse1
 * folds (GiveInnateMove got this for free from an HImode struct-field store
 * expanding into and/ior subreg ops) -- then the ROM's pre-check and
 * reduction should both fall out.
 */
struct E { unsigned char a, b, c, d; };
extern unsigned char *Func_8077330(int bank);

int Func_807a3a8(int unit, int a, int b)
{
    unsigned char *rec;
    unsigned char *ent;
    int found;
    int i;
    int o;

    found = 0;
    rec = Func_8077330((unsigned int)unit <= 7 ? 0 : 1);
    ent = rec + 8;
    for (i = 0; i < *(int *)(rec + 0x84 * 2); i++) {
        o = i * 4;
        if (a == ent[i * 4] && b == rec[i * 4 + 9]) {
            *(int *)(rec + 0x84 * 2) -= 1;
            found = 1;
            break;
        }
    }
    if (found) {
        for (; i < *(int *)(ent + 0x100); ) {
            *(int *)(ent + o) = *(int *)(ent + o + 4);
            if (++i >= *(int *)(ent + 0x100))
                break;
            o = i * 4;
        }
    }
    return found;
}
