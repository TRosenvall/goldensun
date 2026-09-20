/* Cluster Func_801c7fc..Func_801c7fc extracted from goldensun/asm/rom_15000/rom_1aeec_c_a_c_a.s.
 *
 * Total .text for this TU = 164 bytes (= 0xa4). Never attempted before batch 274.
 * No pins, no flags.
 *
 * NOTE FOR RE-SCREENING: the reference keeps its literal pool INSIDE the function, so
 * tryc warns and reports one extra label line (82 against 81). objcmp confirms the bytes
 * and relocations, pool included.
 *
 * ================ WHICH BIV'S INCREMENT COMES LAST DECIDES THE PREHEADER ================
 *
 * This is the round's sharpest finding and it was worth 57 differing on one statement swap.
 *
 * `j++` MUST COME AFTER `count++` IN THE BODY. loop.c PREPENDS bivs to its list as it finds
 * them, so the biv whose increment appears LAST in the source is processed FIRST and gets
 * its giv initialiser emitted first. With `j++` last, the `moves` giv init (`mov r7, r0`)
 * leads the inner preheader, `u` dies there, and every reload in the function then picks
 * the ROM's r0/r3 instead of r4/r3.
 *
 * So when a residue is "every reload register is wrong" in a function with two induction
 * variables, the question is which increment is written last -- not anything about the
 * values themselves.
 *
 * FOUR MORE LEVERS:
 *   - `ids[14]` comes from `sub sp, #0x28` MINUS THREE SPILL WORDS. Not a round 16.
 *   - ONE EXIT, not an early `return count;` -- the early return materialises a separate
 *     `mov r0, #0`.
 *   - `out[count]` MUST BE SUBSCRIPTED BY THE BIV, not reached through an
 *     `o = out + count` pointer: the giv initialiser is then `(plus (mult count 4) out)`,
 *     which is the ROM's `add r6, r3, r0` operand order. A pointer gives the operands the
 *     other way round.
 *   - THE PRE-TEST LOAD MUST BE `u->moves[j].id` WITH `j = 0;` WRITTEN FIRST, not
 *     `u->moves[0].id`. With the literal 0 the front end folds `&u->moves[0]` into a
 *     pseudo, cse2 rewrites the giv initialiser as a copy of it, `u` dies at the load, and
 *     reload emits `add r0, #0x58 / ldrh [r0]`. Indexed by `j` the memref stays
 *     `(mem (plus u 88))`, `u` stays live, and reload produces the ROM's
 *     `mov r3, #0x58 / ldrh r3, [r0, r3]` plus the separate `mov r7, r0 / add r7, #0x58`.
 *
 * MEASURED (rom 81): first candidate 83/71; the struct form 81/34; a single-`while`
 * condition 71/65; `for (j = 0; j <= 0x1f; j++)` 82/40; a variable byte offset 82/60;
 * the `moves[j]` pre-test 82/57; the `j++` swap exact. Inert or worse: four statement
 * orders at the top, -fno-gcse and -fno-schedule-insns both 34, -fno-strength-reduce 61,
 * -fno-expensive-optimizations 77/64.
 */
struct Entry {
    unsigned short id;
    unsigned short move;
};

struct Move {
    unsigned short id;
    unsigned short pad;
};

struct Unit {
    unsigned char pad0[0x58];
    struct Move moves[32];
};

extern int _Func_80796c4(unsigned short *buf);
extern struct Unit *_GetUnit(int id);
extern void *_GetMoveInfo(int move);

int Func_801c7fc(struct Entry *out)
{
    unsigned short ids[14];
    int count;
    int n;
    int i;
    int j;
    int id;
    int m;
    unsigned short *p;
    struct Unit *u;

    count = 0;
    n = _Func_80796c4(ids);
    if (count < n) {
        p = ids;
        for (i = 0; i < n; i++) {
            id = *p++;
            u = _GetUnit(id);
            j = 0;
            m = u->moves[j].id & 0x3fff;
            while (m != 0) {
                _GetMoveInfo(m);
                out[count].id = id;
                out[count].move = m;
                count++;
                j++;
                if (j > 0x1f)
                    break;
                m = u->moves[j].id & 0x3fff;
            }
        }
    }
    return count;
}
