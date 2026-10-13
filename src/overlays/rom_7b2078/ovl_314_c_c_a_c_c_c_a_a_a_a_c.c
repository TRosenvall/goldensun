/* OvlFunc_926_2008e94 -- 0x02008e94.  MATCHING: byte-identical.
 *
 *   ok OvlFunc_926_2008e94          106 encodings
 *   OK whole file -- 236 bytes, 106 encodings and 7 relocations identical
 *
 * Landed in batch 347 (brief A) from a park that read 88 of 106.  Its piece
 * asm/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_a_a_c.s holds exactly ONE
 * function and datacheck reports no data, so it CONVERTS WHOLE -- no split, and
 * piece.py prints no `!! EXPORTS` line for it.  Pin-free; device-free, and
 * deliberately so (see THE UNION, below).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_a_a_c.c \
 *     asm/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_a_a_c.s --whole
 *
 * ===== WHAT THE PARK HAD WRONG ==========================================
 *
 * The park header said "COUNT DIFFERS (ref 106, ours 104) -- so this positional
 * figure measures MISALIGNMENT, not distance" and then, two lines later,
 * "Re-classified in batch 322; the figure IS a distance."  THE SECOND SENTENCE
 * WAS FALSE.  ref 100 instructions against our 98; 236 bytes against 232.  The
 * stream and byte figures are 2 and 4 apart rather than 3 and 6 because this
 * function has an ODD 16-bit instruction count ahead of its aligned five-word
 * pool, so the assembler's 2-byte pad absorbs exactly one instruction:
 * 93 sixteen-bit instructions + 1 pad + 7 `bl` + 5 pool words = 106 entries,
 * 188 + 48 = 236 bytes.  objcmp prints the instruction counts on the figure
 * line.
 *
 * The park then concluded, from a NET figure read in two flag configurations
 * ("99 lines against 101 ... with it 102 against 101"), that "this function is
 * not a clean member of the aliasing class", that answering it needed a typed
 * struct, that no such type exists, that a struct "would probably not help
 * anyway", and finally that the question "is not answerable from the listing
 * alone".  EVERY STEP OF THAT FOLLOWED FROM READING A NET NUMBER.  Counting the
 * regions separately dissolves it:
 *
 *   region                        ref  ours  delta
 *   prologue                        7     7    0
 *   entry assignments               6     6    0
 *   animation loop                 13    13    0
 *   POST-LOOP BLOCK                13     9   -4     <- pure aliasing
 *   call 1 block                   14    14    0
 *   call 2 block                   11    10   -1     } one allocation
 *   call 3 block                   16    18   +2     } swap, +3 net
 *   call 4 block                   13    14   +1     }
 *   epilogue                        7     7    0
 *                                 ---   ---   ---
 *                                 100    98   -2
 *
 * So the net -2 is -4 from aliasing and +2 from allocation, in different
 * regions, needing OPPOSITE fixes.  The aliasing half is not ambiguous at all:
 * all four of the post-loop's missing instructions are one cause, and the
 * "some re-reads survive and some do not" puzzle was the +3 from the call
 * blocks leaking into the total.
 *
 * ===== THE POST-LOOP -4: ONE STORE, FOUR INSTRUCTIONS ===================
 *
 * The ROM's post-loop block is
 *      ldr r3,[r7,#0x50] / mov r5,#0 / strh r5,[r3,#0x1e] /
 *      ldr r3,[r7,#0x10] / mov r4,#0xc0 / lsl r4,#13 / add r3,r4 /
 *      str r3,[r7,#0x10] / mov r3,#0x80 / lsl r3,#24 / str r3,[r7,#0x40] /
 *      mov r0,#0xe3 / bl __PlaySound
 * and we were missing FOUR of those:
 *   * `ldr r3,[r7,#0x10]` -- the RE-READ of w[4].  Without it gcc reuses the
 *     value the loop body loaded and FOLDS the two addends: 0x80<<9 plus
 *     0xc0<<13 became a single `mov r2,#200 / lsl r2,#13` (0xc8<<13).
 *   * `mov r3,#0x80 / lsl r3,#24 / str r3,[r7,#0x40]` -- the whole
 *     `w[0x10] = 0x80<<24` store, deleted as redundant against the loop's.
 * Both deletions need the intervening halfword store NOT to alias an `int`,
 * which at -O2 it does not: `lang_get_alias_set` keys on the ACCESS type.
 * Spelling the actor as a UNION restores the dependence --
 * `c-common.c:3340-3345` walks the COMPONENT_REF/ARRAY_REF chain and returns
 * alias set 0 the moment an operand's type is a UNION_TYPE, so every access
 * below conflicts with every other and both deletions stop.  -4 -> 0, and the
 * block then matches the ROM instruction for instruction.
 *
 * THE UNION IS NOT A DEVICE, and that took one extra step.  The first body that
 * landed used a one-off `union Pun { unsigned short h[16]; int i[8]; }` whose
 * `i` member is never read -- which is exactly the never-read union member
 * docs/elevation.md's device taxonomy forbids as a RESULT (tools/shimcount.py
 * does not detect that class, so it reported the body clean; do not rely on it
 * for this).  Making the ACTOR ITSELF the union reads both members and is also
 * the more likely original: every access in the function goes through it.
 * Byte-identical either way.
 *
 * ===== THE CALL-BLOCK +3: DO NOT NAME THE CONSTANTS =====================
 *
 * With the aliasing fixed the body read 101 against 100, and the whole surplus
 * was one allocation swap.  The ROM's five long-lived homes are
 *     r7 = the actor, r5 = 0, r6 = {0xc0<<12 for calls 1-2, then 0x80<<9 for
 *     calls 3-4}, r8 = {0x80<<24, then 0x3333}, sl = 0xfff80000
 * -- the LOW callee-saved r6 goes to the pair of values that are used as STACK
 * arguments (`str r6,[sp,#4]`, which needs a low register) and the high
 * registers to 0x3333 and 0xfff80000.  The park's named locals inverted that:
 * 0x3333 took r6 and 0x80<<9 took sl, costing one move to create and one to
 * read back, and saving one at call 2.
 *
 * NAMING THE CONSTANTS IS THE CAUSE.  `calls.c:855` already precomputes any
 * call argument whose `rtx_cost (value, SET) > 2` into its own pseudo, so a
 * named local adds nothing except a live range that spans the call sites and
 * fixes which allocno races which.  Writing all four as LITERALS at every use
 * site lets the precomputed pseudos be born and die per block, and the
 * allocator then reproduces the ROM's assignment exactly.  This is the shared
 * brief's lever 1 in its negative direction -- "without a call, naming is a
 * regression" -- except here the calls are what make the names harmful.
 *
 * MEASURED, 400 CELLS over {name, do not name} x {definition slot} for the four
 * constants (pre-loop / post-loop / before call 1 / before call 3), semantically
 * valid cells only:
 *   no name at all                                        100 insns, 5 homes, MATCH
 *   `d1` named at `mid` or `c1`, rest literal             100, 5 homes, MATCH
 *   `d2` named at c3, +/- `d1`                            100, 5 homes, 9 differing
 *   any cell naming `s` or `q`                            >= 101, never a match
 * AND A TRAP WORTH THE SAME AS THE RESULT: 120 OTHER cells reach 100
 * instructions with only FOUR long-lived homes, trading a callee-saved register
 * for two prologue/epilogue instructions.  Their ALIGNED residue is better than
 * the five-home body's (62 of 106 equal against 56) and they are all wrong.
 * `hisaved` -- the count of `mov rN, <high reg>` in the prologue -- separates
 * them, and nothing else in the sweep did.
 *
 * ALSO MEASURED INERT: 3,672 cells of declaration order x definition slot on
 * the named-local bodies; none reached 100 instructions with five homes.
 * Merging the two short-lived constants into one two-role local gives one
 * allocno with six references and still puts it in a high register.
 *
 * Progress is monotonic here on both scales: 88 of 106 and aligned 50/91/18
 * -> 0 of 106 and aligned 106/0/0.
 */
union Actor {
    int w[32];
    unsigned short h[64];
};

extern unsigned char *__MapActor_GetActor(int slot);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void OvlFunc_common0_10c(int a, int b, int c, int d,
                                int e, int f, int g, int h);

void OvlFunc_926_2008e94(void)
{
    union Actor *a;
    unsigned int i;
    int fr;
    int z;

    a = (union Actor *)__MapActor_GetActor(0x13);
    i = 0;
    fr = 8;
    do {
        __WaitFrames(fr);
        a->w[4] += 0x80 << 9;
        a->w[0x10] = 0x80 << 24;
        i++;
        fr -= 2;
    } while (i <= 3);
    z = 0;
    ((union Actor *)*(char **)&a->w[0x14])->h[15] = z;
    a->w[4] += 0xc0 << 13;
    a->w[0x10] = 0x80 << 24;
    __PlaySound(0xe3);
    OvlFunc_common0_10c(a->w[2], a->w[3], a->w[4] + (0xc0 << 12), 0xffff3334, z, 0x3333, z, z);
    OvlFunc_common0_10c(a->w[2], a->w[3], a->w[4] + (0xc0 << 12), 0xcccc, z, 0x3333, z, z);
    OvlFunc_common0_10c(a->w[2] + 0xfffa0000, a->w[3], a->w[4] + 0xfff80000, 0x3333, z, (0x80 << 9), z, z);
    OvlFunc_common0_10c(a->w[2] + (0xc0 << 11), a->w[3], a->w[4] + 0xfff80000, 0x3333, z, (0x80 << 9), z, z);
}
