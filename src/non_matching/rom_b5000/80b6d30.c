/* Func_80b6d30 (AssignBattlePositions)  --  0x080b6d30
 *
 * STILL NON-MATCHING, **4 of 119 encodings** (ref 119 / ours 119, first
 * differing index 23).  PIN-FREE, SHIM-FREE, FLAG-FREE.  Batch 321 brief E
 * RE-MEASURED and CONFIRMED the figure, ran 37 crossed variants over the lever
 * class the park had never touched (declarations and signatures), and
 * independently DERIVED residue (1)'s impossibility from cse.c rather than
 * inferring it.  THE BODY BELOW IS UNCHANGED from the parked one.
 *
 * The only function in asm/rom_b5000/rom_b5a0c_c_c_c_a_c.s and no data section
 * (datacheck.py prints nothing), so landing would be a plain whole-file
 * conversion with no export and no split.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/80b6d30.c \
 *     asm/rom_b5000/rom_b5a0c_c_c_c_a_c.s --func Func_80b6d30
 *
 * THE RESIDUE IS TWO PLACES, FOUR ENCODINGS, and both are confirmed:
 *
 *   (1) index 23:   rom `mov r4, sl`              ours `movs r4, #0`
 *   (2) indices 82-84:
 *       rom  `lsl r3, r5, #12 / orr r3, r7 / mov sl, r3`
 *       ours `lsl r2, r5, #12 / orr r2, r7 / mov sl, r2`
 *
 * ===== BATCH 321: RESIDUE (1) IS NOW PROVED, NOT ARGUED =====
 *
 * The park argued from COST and notreg_cost that the constant wins on ties.
 * That is correct as far as it goes but it is not the whole decision, and the
 * missing half makes the result STRONGER rather than weaker.  Read in gcc-2.96's
 * own cse.c, the full chain at `j = ret` is:
 *
 *   a. `ret = 0` records BOTH `(const_int 0)` and ret's pseudo in ONE
 *      equivalence class, and `insert` keeps a class sorted by CHEAPER with the
 *      cheapest FIRST.  Its own comment says it: "a constant is the only thing
 *      that can be cheaper than a register".  So the class head is the constant.
 *
 *   b. At `j = ret`, cse_insn walks that class and PRUNES every candidate that
 *      is already in the table -- `src = 0` for the register and then
 *      `src_folded = 0` for the constant ("Prefer items not in the hash table
 *      to ones that are when they are equal cost").  BOTH are pruned, so
 *      src_cost and src_folded_cost both stay at 10000 and NEITHER is what
 *      decides anything.
 *
 *   c. The substitution therefore comes from the hash-table entry, and `elt` was
 *      set to `elt->first_same_value` -- the class HEAD -- which by (a) is the
 *      constant, at `src_elt_cost == 0`.  The fold is unconditional.
 *
 * So the escape is not "make the register cheaper than the constant", it is
 * "keep the register off the head of its own equivalence class", and the only
 * thing in CHEAPER that can beat a `(const_int 0)` at cost 0 is CHEAP_REG, which
 * needs `REG_USERVAR_P && REGNO < FIRST_PSEUDO_REGISTER` -- a HARD-REGISTER USER
 * VARIABLE.  That is the pin the park already measured at 102 of 119.  **There
 * is no pin-free C source that reaches residue (1) inside one basic block**, and
 * the ROM's own layout puts `ret = 0` (indices 18/21) and `j = ret` (index 23)
 * in one block with only `bl Func_80c2384` between them -- and a call does not
 * invalidate a pseudo in cse's table, which the park verified from the other
 * direction by moving `j = ret` across it (still 4).
 *
 * The documented escape remains a CONTROL-FLOW BOUNDARY, as in the corpus
 * exemplar src/rom_b5000/rom_b8228_c_a_c_c_a_c_a_c_b.c whose `for (i = ret; ...)`
 * sits in an ELSE arm at .L3.  This function has no boundary to put there.
 *
 * ===== BATCH 321: THE DECLARATION LEVER CLASS IS MEASURED, AND IT IS FLAT =====
 *
 * docs/humanization.md section 3 says a pin is evidence about a DECLARATION.
 * This park's body is a textbook instance of that signature -- `*(short *)(s +
 * a)` raw-offset arithmetic four times, and `extern unsigned char
 * ewram_2018000[];` with no dimensions -- and NOTHING in the class had ever been
 * tried here.  37 crossed variants via tools/crossfire.py, depth 2, two edit
 * sets.  Reference memory profile ldr=7 ldrb=4 ldrsh=4 str=1 strh=2.
 *
 * EXACTLY INERT at 4 of 119, ref 119 / ours 119 (so these are distances, and
 * each is a candidate prerequisite that pays nothing on its own OR in any pair):
 *
 *   * `extern unsigned char ewram_2018000[][0x4000];` WITH the use rewritten as
 *     `(int)ewram_2018000[i]` -- pattern 4, the inner dimension included.  This
 *     is the dividend case the brief asks to be reported: BETTER-TYPED SOURCE,
 *     IDENTICAL BYTES.  (Either half ALONE is 5 of 119, i.e. one worse -- the
 *     declaration and the use have to move together.)
 *   * splitting `if (v == 0x1dc || v == 0x1e3) continue;` into two sequential
 *     `if`s -- pattern 1.
 *   * `i * 0x4000` for `i << 14`; dropping the `(int)` cast on the ewram
 *     argument; `_PreloadSpriteGFX`'s second parameter declared `void *`;
 *     `_GetUnit`'s prototype withheld; `ret` declared `unsigned int`; `v`
 *     declared `unsigned int`.  And every PAIR of the above.
 *
 * FAR WORSE, all with a COUNT flag (so the figure measures misalignment, not
 * distance) -- recorded so nobody repeats them:
 *   * `short *s` with the four accesses as `s[i+2]` / `s[i+3]`: 79-94 at 115-123
 *     instructions, with MEM divergence.  Pattern 2 does NOT apply here: the
 *     ROM's `ldrsh r3, [r6, r2]` register-offset form is what the `off`/`a`
 *     idiom produces, and narrowing the pointer type replaces it with scaled
 *     addressing.  The landed sibling Func_80b6cdc's idiom is correct as written.
 *   * hoisting the arg-4 call `Func_80c23a0(u[0x128])` into a temp: 101 at 123.
 *
 * READ THE FLATNESS AS THE FINDING (brief 321): 29 of 37 rows tie the base
 * exactly, across three different dimensions.  The lever is not in declarations,
 * types or callee signatures.  Given the derivation above, that is expected:
 * residue (1) is decided on COST inside cse1's equivalence class, which no type
 * written in C can move, and residue (2) is a reload-register INDEX.
 *
 * ===== RESIDUE (2), inherited and still right =====
 *
 * `ret` lives in sl, a hi register, so Thumb must compute `(i << 12) | v` in a
 * lo register and copy; the copy is a RELOAD-created insn.  The park corrected an
 * earlier misreading that is worth keeping: `.18.greg`'s "Using reg 3 for reload
 * 0" is printed by find_reg, which selects which hard register to SPILL, not
 * which register a reload gets.  The reload register is chosen later, per insn,
 * by allocate_reload_reg walking `spill_regs` ROUND-ROBIN from `last_spill_reg`,
 * precisely so consecutive reloads leapfrog.  So the register at index 82 is a
 * function of the COUNT of reload-register allocations made EARLIER in the
 * function -- which is why every respelling of that statement is inert.
 *
 * ===== THE INERT LIST, carried forward =====
 *
 * From earlier batches, all still 4 or far worse: `for (j = ret; ...)`, `j = ret`
 * before and after the call, `ret = j = 0`, `j = ret = 0`, `j = 0; ...; ret = j`;
 * commuted `v | (i << 12)`; `!j` for `j == 0`; a block-scoped temp for `i << 12`;
 * `ret = i << 12; ret |= v`; declaration order (`ret` last, `ret` first, `j`
 * before `ret`); `i` before `j`; `o` at function scope; the tail test as
 * `if (v != 0x1dc && v != 0x1e3) break;`; `(int)ewram_2018000 + (i << 14)`.
 * NOT inert and not useful: `a = off; a += 4;` -> 75; hoisting `u[0x128]` into a
 * local -> 59; moving `ret = 0` after the call -> 10; `+` for `|` -> 56.
 * Flags: `-fno-gcse`, `-fno-cse-follow-jumps`, `-fno-rerun-cse-after-loop`,
 * `-fno-strength-reduce` all leave it at 4, so NO flag group applies.
 * The cse-defeating barrier family is measured in full and all of it costs two
 * instructions, because `ret` is in a HI register: "+r" 96, "+h" 96, "+g" 98-103,
 * "+l" 100-104, at 121-123 encodings against 119.  "+h" not helping is the
 * important one -- gcc-2.96 copies through a low register either way.
 *
 * SHIMS -- NONE.  register class 0, .equ class 0, other __asm__ 0.
 *
 * NEXT, IF ANYONE TAKES IT UP: residue (1) needs a control-flow boundary between
 * `ret = 0` and `j = ret` that cse1 SEES and that a later pass REMOVES.  Nothing
 * in C is known to do that -- jump optimisation runs at pass 02, BEFORE cse1 at
 * pass 03, so a branch gcc can fold is already gone when cse1 runs, and one it
 * cannot fold survives to the output.  State that as the open question rather
 * than as a lever.
 */
extern unsigned char *_GetUnit(int id);
extern int Func_80c23c0(int a);
extern int Func_80c2384(int a);
extern int Func_80c23a0(int a);
extern int _PreloadSpriteGFX(int a, int b, int c, int d);
extern char *iwram_3001e74;
extern unsigned char ewram_2018000[];

int Func_80b6d30(int slot)
{
    char *s;
    unsigned char *u;
    int flag;
    int v;
    int ret;
    int j;
    int i;
    int off;
    int a;

    s = iwram_3001e74;
    u = _GetUnit(slot);
    flag = Func_80c23c0(u[0x128]);
    ret = 0;
    v = Func_80c2384(u[0x128]);
    for (j = ret; j <= 1; j++) {
        if (u[0x129] != 0)
            continue;
        for (i = 0; i <= 5; i++) {
            off = i * 2;
            a = off + 4;
            if (*(short *)(s + a) != 0)
                continue;
            if (flag != 0)
                break;
            if (i > 4)
                continue;
            a = off + 6;
            if (*(short *)(s + a) == 0)
                break;
        }
        if (i == 6)
            break;
        if (_PreloadSpriteGFX(i, (int)(ewram_2018000 + (i << 14)), v + j,
                              Func_80c23a0(u[0x128])) == 0)
            return 0;
        if (j == 0)
            ret = (i << 12) | v;
        {
        int o = i * 2;
        a = o + 4;
        *(short *)(s + a) = slot;
        if (flag == 0) {
            a = o + 6;
            *(short *)(s + a) = slot;
        }
        }
        if (v == 0x1dc || v == 0x1e3)
            continue;
        break;
    }
    return ret;
}
