/* MEASURED FIGURE, backfilled in batch 324 (this park carried none).
 *
 *   73 differing encodings of 91.  SIZE DIFFERS (ref 91, ours 93 -- two long).
 *
 * First diff at index 7 (ref 4b29, ours 4b2a) -- a one-register difference that
 * early, combined with two extra instructions, suggests the residue is a single
 * allocation decision plus its consequences rather than 73 independent
 * problems.  Decompose before costing it.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_9000/80113e4.c asm/rom_9000/rom_108e4_c.s --func Func_80113e4
 *
 * The figure is EVIDENCE.  Everything below it is a HYPOTHESIS, and across
 * pass two a park's diagnosis has been wrong roughly 40 times in 42.
 */

/* Func_80113e4 -- NON-MATCHING.  objcmp: SIZE ref 188 bytes / ours 192, ENCODINGS
 * differ in 73 place(s) (ref 91, ours 93) -- positional, so it overstates the gap; a
 * difflib alignment of the tryc --full listing puts it at 39 lines of 92 differing.
 * (A same-length but structurally worse spelling -- plain for loops, unsigned char *m,
 * x/z as scalars with p[0]/p[2] -- reports 61 of 91 differing; see
 * scratchpad K/Func_80113e4.c from batch 286.)
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_9000/80113e4.c asm/rom_9000/rom_108e4_c.s --func Func_80113e4
 *
 * The .s holds ten functions (Func_80108e4 .. Func_80114a0); this one needs a split.
 *
 * WHAT IS RIGHT here, each measured:
 *  - `x = *p++; z = p[1];` gives the ROM's `ldmia r3!, {r2}` / `ldr r3, [r3, #4]`.
 *  - `(x + (int)0xff000000) >> 25` -- the int cast is what gives `asr`; the plain
 *    unsigned constant makes the sum unsigned and emits `lsr`.
 *  - A struct with `u16 tbl[]` at +0x138 gives the ROM's `idx*2 + 0x138` then
 *    `ldrh rD, [m, rIdx]`; `*(u16 *)(m + idx*2 + 0x138)` adds m first.
 *  - Writing `z + j` INLINE (both as the call argument and inside the mask) makes
 *    loop.c strength-reduce it: .08.loop shows "giv at 227 combined with giv at 228 ...
 *    reduced to (reg 72)".  That is the ROM's r6 cursor (`ldr r6,[sp,#4]` in the
 *    preheader, `add r6,#1` at the bottom) plus the separate qz copy.  With a named
 *    `qz = z + j` or an explicit `zz++` cursor loop.c says "giv ... not worth while,
 *    0 vs 24" and there is no cursor.  x and z then spill to the stack as in the ROM.
 *
 * THE BLOCKER: the constant 0xf.  The ROM rematerialises `mov r2,#0xf` in BOTH the
 * middle loop (row) and the inner loop (qx & 0xf).  Every loop-form spelling here gets
 * the middle one hoisted: .08.loop reports "Insn 79: regno 51 (life 2), move-insn
 * savings 2  moved to ..." -- savings 2 because combine_movables matches it with the
 * inner loop's identical (set (reg) (const_int 15)), which on its own is "savings 1 not
 * desirable".  The hoisted 0xf takes a callee-saved register (fp/r9), which pushes the
 * qz copy into r4 with a caller-save spill around the call (the extra str/ldr r4 = the
 * 4 extra bytes and the 0x14 frame) and shifts every high register.
 *   ROM: i r5, cursor r6, row r7, qz r8, layer r9, j r10, bias r11; x,z,m on stack.
 *
 * INERT or worse (all measured, difflib lines-differing of 92): goto-built loops (all
 * three as goto kills the 0xf hoist but also the giv, so no cursor/copy, 176 bytes;
 * mixed goto/for combinations score 38-45, none with the cursor); do/while at any
 * level (40); `% 16` on unsigned for either mask; `(qx & 0xf) + row` operand swap;
 * qx as unsigned short; named k/t temporaries; `int v[2]` for x/z (gets registerised as
 * a DImode pseudo -- much worse); `(qz << 28) >> 24` stops the hoist but is not the ROM's
 * code.  Untried: whatever raises the middle loop's insn_count or lowers the matched
 * savings so move_movables' threshold test fails for the middle-loop 0xf.
 */
struct M {
    int *pos;
    unsigned char pad[0x134];
    unsigned short tbl[1];
};
extern struct M *iwram_3001e70;
extern int Func_80108e4(int layer, int qx, int qz, int tileset, int force);

void Func_80113e4(void)
{
    struct M *m;
    int *p;
    int x, z;
    unsigned int layer, j, i;
    int bias;
    int qz, qx, row, zz, t, k;

    m = iwram_3001e70;
    x = 0;
    z = 0;
    p = m->pos;
    if (p != 0) {
        x = *p++;
        z = p[1];
    }
    x = (x + (int)0xff000000) >> 25;
    z = (z + (int)0xfec00000) >> 25;
    for (layer = 0, bias = 0; layer <= 1; layer++, bias += 0x140) {
        for (j = 0; j <= 1; j++) {
            for (i = 0; i <= 1; i++) {
                Func_80108e4(layer, x + i, z + j, m->tbl[(((z + j) & 0xf) << 4) + ((x + i) & 0xf)] + bias, 1);
            }
        }
    }
}
