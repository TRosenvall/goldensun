/* Func_8026e80 -- 0x08026e80.  MATCHING: byte-identical.
 *
 *   OK Func_8026e80 -- 296 bytes, 136 encodings and 11 relocations identical
 *
 * Landed in batch 347 (brief A) from a park that read 90 of 136.  Pin-free,
 * device-free (tools/shimcount.py: no .equ, no register declaration, no
 * barrier, no volatile, no .sym).
 *
 * SPLIT REQUIRED.  Its piece asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a.s holds
 * four functions and the other three are still parked:
 *     python3 tools/split_s.py asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a.s Func_8026e80
 *   -> rom_23178_a_a_a_a_c_a_a_a_a.s  (3 function(s), 3562 lines)
 *   -> rom_23178_a_a_a_a_c_a_a_a_b.s  (1 function(s),  152 lines)   <- this .c
 * The target is the LAST function in the file, so there is no _c part.
 * datacheck: no exported non-function symbols (piece.py prints no
 * `!! EXPORTS` line for this piece).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_15000/rom_23178_a_a_a_a_c_a_a_a_b.c \
 *     asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a_b.s --whole
 * (before the split, against the unsplit piece: `--func Func_8026e80`.)
 *
 * ===== WHAT THE PARK HAD WRONG, AND WHY IT MATTERED =====================
 *
 * The park header asserted "Re-classified in batch 322; the figure IS a
 * distance" two lines after correctly saying the counts differ.  THEY DIFFER:
 * ref 131 instructions against our 129, 296 bytes against 292.  The stream
 * figure 136/134 and the byte figure are both 2/4 apart because this function
 * has an ODD 16-bit instruction count before an aligned literal pool, so the
 * assembler's 2-byte pad absorbs exactly one instruction.  objcmp prints the
 * instruction counts on the same line now.
 *
 * Because the figure was believed to be a distance, 90-of-136 was read as a
 * statement about the RESIDUE and the park went looking for one global cause.
 * It named LAYOUT and wrote "NEXT: nothing found for the layout".  Per-region
 * counting shows the gap was TWO SITES OF ONE, in two different regions, and
 * that the bulk of the 90 was not a length defect at all:
 *
 *   region                                      ref  ours  delta
 *   entry + null guard                            7     7    0
 *   .L26e8e  a != b block                        14    14    0
 *   .L26eac  store + call                         3     3    0
 *   .L26eb4  loop pre-header                      4     4    0
 *   .L26ebc  loop body                            9     9    0
 *   .L26ed0  latch                                4     4    0
 *   Func_80219c8 .. q[0x52] test                 15    15    0
 *   .L26ef8 + .L26f0e  (the halfword chain)  11+19=30   29   -1
 *   .L26f34  join                                 2     2    0
 *   .L26f38                                       9     9    0
 *   .L26f4c  decrement                             5     4   -1
 *   .L26f56  minute test                         16    16    0
 *   .L26f7a  final call                           9     9    0
 *   epilogue                                      4     4    0
 *
 * Aligned residue (tools/aligncmp.py): 91 of 136 in 18 hunks -> 0.
 *
 * ===== THE FIVE LEVERS, IN THE ORDER THEY PAID ==========================
 *
 * 1. THE DECREMENT IS A TWO-ADDRESS TIE, NOT A LAYOUT EFFECT.  The ROM spends
 *    `sub r3, r6, #1 / str r3, [r7,#0x4c] / mov r6, r3` where we had
 *    `sub r5, r5, #1 / str r5`.  regmove ties an insn's output to an input
 *    operand only if that input DIES there, so storing `n - 1` as its own
 *    expression and then re-deriving `n` keeps `n` live across the subtract and
 *    the tie fails -- which is what puts the ROM's extra `mov` back:
 *        *(int *)(p + 0x4c) = n - 1;
 *        n = n - 1;
 *    (129 -> 130 instructions.)  `n--` and `*(...) = --n` are both INERT: they
 *    keep the tie.
 *
 * 2. THE LAYOUT, AND THE PARK WAS RIGHT ABOUT IT.  The ROM's extra instruction
 *    is the `b .L26f34` that closes an out-of-line block; our inline `&&` chain
 *    falls through instead.  Writing the last two conditions as a separate
 *    labelled block placed TEXTUALLY BEFORE the `n < 0` block, reached by a
 *    backward goto and closed by `goto join;`, reproduces the ROM's block order
 *    exactly: [q[0x52] test, beq] [clear; return] [tail] [nblock] [join].
 *    130 -> 131 instructions AND 130 -> 13 of 136 in one step.
 *
 * 3. THE PARK TESTED THE WRONG BASE POINTER.  The ROM's last two halfword
 *    comparisons are `ldrh r3, [r2,#0xc]` and `[r2,#0xe]` where r2 is **e**,
 *    the ewram_2002024 record -- not q.  All four tests are consecutive
 *    halfwords of ONE record at +8/+0xa/+0xc/+0xe.  The park body read
 *    `q + 0xc` / `q + 0xe`: a different program that happened to be the same
 *    length, which also forced `q` to stay live into the tail block and is why
 *    four hunks read `r1` against the ROM's `r2`.  6 -> 1 of 136.
 *
 * 4. THE SLOT BYTE MUST NOT BE NAMED.  `t = *(q + 0x50);` then `(t ^ 1)` leaves
 *    the xor result in the loaded byte's register (`eor r2, r2, r3`); inlining
 *    the read leaves it in the CONSTANT's register (`mov r3,#1 / eor r3, r2`),
 *    as the ROM does.  12 -> 6 of 136.
 *
 * 5. THE LAST ENCODING IS optabs.c:658-666.  `adds r0, r5, r0` against our
 *    `adds r0, r0, r5`.  expand_binop swaps a commutative pair when
 *    `target == op1`, so `t = b + t` with `t` as the destination can NEVER
 *    produce `(plus b t)` -- and gcc-2.96 has no other reordering that reaches
 *    two pseudos: combine.c:3494-3500 and :9845-9852 move only CONSTANTs and
 *    non-'o' operands, so two REGs are never swapped there.  Routing the left
 *    operand through a copy makes the test false:
 *        u = t;
 *        t = b + u;
 *        *(int *)(p + 0x2c) = t;
 *    local-alloc coalesces the `u = t` copy away.  Byte-identical.
 *    Four spellings of that copy all land (a fresh `u`, a reused `d`, a reused
 *    `b`, and `*(int *)(p + 0x2c) + u` relying on CSE); the fresh `u` is the
 *    one kept.  Writing `b + t` into a fresh temp gets the ORDER right and the
 *    DESTINATION wrong (`adds r3, r5, r0`, 2 differing); `t += b` and
 *    `t = t + b` get the destination right and the order wrong (1 differing).
 *
 * MEASURED INERT HERE: all twelve declaration positions of `q` (the r1-vs-r2
 * hunks were lever 3, not an allocation choice); `1 ^ t` against `t ^ 1`;
 * `(t ^ 1) * 0x18` against `((t ^ 1) * 3) << 3`; `unsigned int t`.
 * MEASURED WORSE: `n`, `s`, `i` as the destination of `b + t` (18, 2, 146).
 *
 * The park's own "MEASURED (rom 144 lines)" table counted LINES of a body that
 * already carried the `&&` chain, so its "142, 125" and "141, 87 (inert)" rows
 * measured nothing about the chain.  Its `t--` row IS correct and survives in
 * the body below -- when a ROM derives a small constant from a value a compare
 * has just proved, write the derivation.
 */
extern unsigned char *iwram_3001f34;
extern unsigned char *iwram_3001e74;
extern unsigned char ewram_2002024[];
extern void _Func_80b8fd4(void);
extern void Func_8003dec(unsigned char *a, int b);
extern void Func_80219c8(void *a);
extern int Func_8021c34(void);
extern void _PlaySound(int id);
extern void Func_801ea08(int a, int b, int c, int d, int e);

void Func_8026e80(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *e;
    unsigned char *r5;
    unsigned char *r6;
    int a;
    int b;
    int d;
    int t;
    int u;
    int n;
    int s;
    int i;

    p = iwram_3001f34;
    if (p == 0)
        return;
    a = *(int *)(p + 0x28);
    b = *(int *)(p + 0x2c);
    if (a != b) {
        d = a - b;
        t = d / 3;
        if (t == 0) {
            t--;
            if (d >= 0)
                t = 1;
        }
        u = t;
        t = b + u;
        *(int *)(p + 0x2c) = t;
        _Func_80b8fd4();
    }
    r5 = p;
    r6 = p;
    r5 += 0x24;
    i = 2;
    do {
        if (*r5++ != 0)
            Func_8003dec(r6, 0xf0);
        i--;
        r6 += 0xc;
    } while (i >= 0);
    Func_80219c8((void *)0x6006680);
    if (*(int *)(p + 0x50) == 0)
        return;
    q = iwram_3001e74;
    if (*(q + 0x52) == 0)
        goto nblock;
    *(int *)(p + 0x4c) = 0;
    return;
tail:
    if (*(unsigned short *)(e + 0xc) == 0x56 && *(unsigned short *)(e + 0xe) == 0x53) {
        *(int *)(p + 0x4c) = 0xe1 << 2;
        n = 0xe1 << 2;
    }
    goto join;
nblock:
    n = *(int *)(p + 0x4c);
    if (n >= 0)
        goto have;
    e = ewram_2002024 + ((((int)*(q + 0x50) ^ 1) * 3) << 3);
    if (*(unsigned short *)(e + 8) == 0x45 && *(unsigned short *)(e + 0xa) == 0x44)
        goto tail;
join:
    if (n < 0)
        return;
have:
    if (*(int *)(p + 0x44) == 0 && *(int *)(p + 0x48) == 0) {
        *(int *)(p + 0x44) = Func_8021c34();
        n = *(int *)(p + 0x4c);
    }
    if (n > 0) {
        *(int *)(p + 0x4c) = n - 1;
        n = n - 1;
    }
    if (n < 0)
        return;
    s = (n + 0x3b) / 0x3c;
    if (s != 0 && s * 0x3c == n)
        _PlaySound(0x6c);
    if (*(int *)(p + 0x44) != 0)
        Func_801ea08(s, 2, *(int *)(p + 0x44), 0x10, 8);
}
