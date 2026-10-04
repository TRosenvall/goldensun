/* Func_8019908 (RegisterCallback)  --  0x08019908   [rom_15000]
 *
 * MATCH, pin-free.  0 differing, 27 of 27 encodings, 60 bytes, 1 relocation.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_15000/rom_1908c_c_a_c_c_c_b.c \
 *     asm/rom_15000/rom_1908c_c_a_c_c_c_b.s --whole
 *   ->  ok Func_8019908  27 encodings
 *       OK whole file -- 60 bytes, 27 encodings and 1 relocations identical
 *
 * SPLIT SHAPE: none needed.  asm/rom_15000/rom_1908c_c_a_c_c_c_b.s holds ONE
 * `.thumb_func_start` (Func_8019908) and nothing else -- no .section, .data,
 * .rodata, .word, .byte or .incbin.  `tools/datacheck.py` on it exits 0 with no
 * output, so this is a WHOLE-FILE conversion to
 * src/rom_15000/rom_1908c_c_a_c_c_c_b.c and the object path does not change.
 * No `tools/split_s.py --dry-run` applies.  PINS: 0.  No flag group: no explicit
 * and no wildcard Makefile rule names this stem, so the generic `asm/%.o: src/%.c`
 * rule with plain GCC296_CFLAGS builds it -- which is what objcmp used.
 *
 * ---------------------------------------------------------------------------
 * THE PARK'S DIAGNOSIS WAS A SYMPTOM, NOT THE CAUSE -- AND THE WHOLE RESIDUE
 * WAS ONE STATEMENT SWAP.  `i = 0;` MOVES AHEAD OF `n = 8;`.  9 -> 0.
 *
 * The park (src/non_matching/rom_15000/8019908.c, "SETTLED") called the blocker
 * "the order in which the two parameters are copied to callee-saved registers"
 * and recorded the ROM's `mov r7,r1 / ldr r1,[r3] / mov r6,r0` against our
 * `mov r6,r0 / ldr r0,[r3] / mov r7,r1`.  A per-index differ shows that is
 * DOWNSTREAM.  All nine differing indices are ONE cause:
 *
 *      ROM   b -> r1, i -> r0        ours   b -> r0, i -> r1
 *
 * and nothing else.  Indices 3, 9, 10, 14, 16, 19 are that rename verbatim
 * (`ldr r1,[r3]` / `adds r2,r1,r3` / `ldrh r3,[r4,r1]` / `strh r7,[r4,r1]` /
 * `adds r0,#1` / `cmp r0,r5`).  Indices 2 and 6 -- the parameter copies the park
 * blamed -- are forced by it: whichever of r0/r1 receives `b` must have its
 * incoming parameter saved FIRST, so `mov r7,r1` leads iff `b` lands in r1.  The
 * park's own control ("copying both parameters into locals assigned in the ROM's
 * order, 9 differing, unchanged") is exactly what a symptom does when you edit it.
 * Instruction ORDER and POOL ORDER were already the ROM's in all 27 slots; the
 * park never had an ordering problem at all.
 *
 * THE DECIDING RUNG IS global_alloc's PRIORITY, AND THE MARGIN WAS 1.2%.
 * `.18.greg` prints `;; 7 regs to allocate:`, so this is global_alloc and the
 * formula DOES carry floor_log2 (local-alloc's qty_compare does not -- see the
 * batch-321 correction).  From `.17.lreg`, with priority = floor_log2(N_REFS) *
 * N_REFS / LIVE_LENGTH and N_REFS loop-weighted (+= loop_depth + 1):
 *
 *   park order (b,q,n,i,p)        p 2*6/9  = 1.333   q 3*8/26 = 0.923
 *                                 i 2*7/18 = 0.778   b 2*5/13 = 0.769   <-- 1.2%
 *       => `;; 7 regs to allocate: 35 36 37 34 38 32 33`, i ahead of b
 *
 *   this file (b,q,i,n,p)         p 2*6/9  = 1.333   q 3*8/26 = 0.923
 *                                 b 2*5/13 = 0.769   i 2*7/20 = 0.700
 *       => `;; 7 regs to allocate: 35 36 34 37 38 32 33`, b ahead of i
 *
 * Only `i`'s LIVE LENGTH moved, 18 -> 20: defining `i` two insns earlier in the
 * preheader lengthens its range and costs it the tie.  `b`'s own numbers are
 * untouched at 5/13.  REG_ALLOC_ORDER (config/arm/arm.h:989) is
 * { 3, 2, 1, 0, 12, 14, 4, 5, 6, 7, ... }; r3 is held by the two block-local
 * allocnos local-alloc placed (regs 39 and 42, both "in block N" and absent from
 * the greg list), and find_reg's PASS 0 skips `allocno[].regs_someone_prefers`
 * -- r0 and r1 are precisely what the two parameter copies prefer (`;; 32
 * preferences: 0`, `;; 33 preferences: 1`), so r0/r1 are only reachable in pass
 * 1, in alloc order r1 then r0.  Third-allocated therefore takes r1 and
 * fourth-allocated r0, and the ROM's b/i assignment falls straight out.
 *
 * MEASURED: all 60 legal permutations of the five preheader statements (120
 * minus the 60 where `p` precedes the `b` it reads).  ELEVEN match exactly --
 * bqinp, binpq, binqp, biqnp, bnipq, bniqp, ibnpq, ibnqp, ibqnp, inbpq, inbqp --
 * and the field is otherwise {2:5 rows, 6:5, 8:12, 9:10, 10:5, 11:2, 12:5, 13:1,
 * 14:3}.  The park's own bqnip is the 9.
 *
 * WHY bqinp AND NOT ONE OF THE OTHER TEN.  It is the only matching order with no
 * tie anywhere in the priority list: margins 44% / 20% / 10%.  `binpq` also
 * matches but puts p and q at 1.200 EACH, broken only by allocno number, which is
 * not a margin anyone should ship.  bqinp is also the one-token edit to the park.
 *
 * KEPT FROM THE PARK, all three re-verified by this file matching:
 *   - THE FUNCTION IS VOID.  The epilogue `pop {r5,r6,r7} / pop {r0} / bx r0`
 *     pops lr into r0, which gcc cannot do if r0 carries a return value.
 *   - the id table is reached with the WALKING OFFSET as the addressing base,
 *     `*(unsigned short *)(q + (int)b)`, q walking 0x12dc.. and b the iwram
 *     pointer; the natural subscript does not give `ldrh r3,[r4,r1]`.
 *   - the loop bound 8 lives in a named local (`n`), not as a literal.
 *
 * THE TWIN CONFIRMS THE TABLE.  src/rom_15000/rom_1908c_c_a_c_c_b.c
 * (ClearCallbackTable, Func_80198dc) clears the same pair: the int slots at
 * iwram_3001e8c+0x12bc and the short ids at +0x12dc, 8 entries, 0x12dc-0x20 ==
 * 0x12bc.  Its `union CbSlot` alias-set lever is NOT needed here and was
 * measured inert: the ROM's `str` / `strh` pair is already in order at indices
 * 13/14 in every one of the 60 variants, because here the two stores are in a
 * block that ENDS at the `break` and sched2 has no second store to reorder
 * against.  That pin (`register int off __asm__("r4")`) is also not needed here;
 * this body is pin-free.
 */
extern unsigned char *iwram_3001e8c;

void Func_8019908(int cb, int id)
{
    unsigned char *b;
    int *p;
    unsigned char *q;
    int i;
    int n;

    b = iwram_3001e8c;
    q = (unsigned char *)0x12dc;
    i = 0;
    n = 8;
    p = (int *)(b + 0x12bc);
    do {
        if (*(unsigned short *)(q + (int)b) == 0) {
            *p = cb;
            *(unsigned short *)(q + (int)b) = id;
            break;
        }
        i++;
        p++;
        q += 2;
    } while (i != n);
}
