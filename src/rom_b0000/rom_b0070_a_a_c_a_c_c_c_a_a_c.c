/* Func_80b06c0  --  LANDS BYTE-IDENTICAL.  Batch 317, brief D.
 *
 * Install as:   src/rom_b0000/rom_b0070_a_a_c_a_c_c_c_a_a_c.c
 * Split:        NONE NEEDED -- asm/rom_b0000/rom_b0070_a_a_c_a_c_c_c_a_a_c.s
 *               holds exactly ONE function.  `tools/datacheck.py` on it prints
 *               nothing: no data section.  (.Lb4100 is READ here and DEFINED
 *               elsewhere, so nothing is lost by deleting this .s.)
 * Flags:        NONE -- plain production -O2, no Makefile rule.
 * Pins:         0.   fakematch.txt row: NOT needed.
 *
 * FIGURE: 44 bytes, 21 encodings and 1 relocation IDENTICAL, both
 * `objcmp --func` and `objcmp --whole`.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_b0000/rom_b0070_a_a_c_a_c_c_c_a_a_c.c \
 *     asm/rom_b0000/rom_b0070_a_a_c_a_c_c_c_a_a_c.s --func Func_80b06c0
 *
 * (The park's header named asm/rom_b0000/rom_b0070_a_a_c_a_c_c_c.s, which no
 * longer exists -- the bank has been cut three more times since.  The live
 * path is the one above.)
 *
 * WHAT THE PARK SAID.  Its description of the residue was right and its
 * verdict was wrong.  Measured at 2 of 21 (not "22 against 22"), the two
 * places being indices 1 and 2:
 *
 *     idx1  ref 010b = lsl r3, r1, #4      ours 0109 = lsl r1, r1, #4
 *     idx2  ref 1c59 = add r1, r3, #1      ours 3101 = add r1, #1
 *
 * One correction to the park's reading: Thumb-1 has NO two-operand `lsl`
 * ENCODING.  Both forms are format-1 `lsl Rd, Rs, #imm5`; only Rd differs, and
 * the ROM's `.s` writes `lsl r1, #4` purely as hand-written shorthand.  The
 * `add` IS a real encoding change: format-1 `add Rd,Rs,#imm3` against format-3
 * `add Rd,#imm8`.
 *
 * The verdict -- *"Nothing in C says 'put this result somewhere other than its
 * operand'. ... reachable only if the two values come from statements that do
 * different KINDS of work, and here they do not -- it is one expression"* --
 * is refuted.  The batch-316 AGGREGATE lever reaches it:  declaring the shift
 * result as element [0] of a TWO-WORD aggregate makes local-alloc stop
 * combining the destination with its dying source, so the shift gets a fresh
 * register and the add becomes three-operand.  The aggregate never reaches
 * memory: the generated prologue is `push {lr}` with no `sub sp`.
 *
 * NEW BOUND ON THE AGGREGATE LEVER -- IT IS SIZE-GATED AT TWO WORDS.
 * Measured, five spellings, identical bodies otherwise:
 *
 *     int s;                              (the park's scalar)   2
 *     int sv[1];       sv[0] = k << 4;                          2   INERT
 *     struct { int s; } sv;                                     2   INERT
 *     int sv[2];       sv[0] = k << 4;                          0
 *     struct { int s; int pad; } sv;                            0
 *
 * So the brief's "a struct is byte-identical to the array, so the aggregate
 * KIND is free" holds, and the new clause is that a ONE-WORD aggregate is
 * exactly inert -- it is the same quantity as the scalar.  Two words is what
 * makes local-alloc treat it as an 8-byte pseudo it will not merge.
 *
 * AND THE LOOP-CARRIED VALUE MUST STAY A SCALAR: moving `v` into the aggregate
 * too (`sv[0] = k<<4; sv[1] = sv[0]+1;`) forces a real stack frame -- 24 of 21
 * and +8 bytes.
 *
 * WHAT IT DOES (unchanged from the park, and the loop was already exact):
 * stamps the palette-slot byte `(slot << 4) + 1` into six OAM-ish fields of
 * each of `n` records, whose offsets come from the halfword table at .Lb4100.
 */

extern unsigned short Lb4100[] __asm__(".Lb4100");

void Func_80b06c0(int n, int k, char *base)
{
    int v;
    int sv[2];
    unsigned short *t;
    char *p;

    sv[0] = k << 4;
    v = sv[0] + 1;
    if (n > 0) {
        t = Lb4100;
        do {
            p = base + *t;
            n--;
            t++;
            p[4] = v;
            p[8] = v;
            p[0xc] = v;
            p[0x10] = v;
            p[0x14] = v;
            p[0x18] = v;
        } while (n != 0);
    }
}
