/* OvlFunc_924_2009db4  --  0x02009db4  --  MATCHING under RERUNLOOP_CFLAGS.
 *
 * ZERO: 636 bytes, 291 encodings and 23 relocations IDENTICAL, --whole, with
 * `-fno-rerun-loop-opt` (RERUNLOOP_CFLAGS).  Found by tools/autosweep.py's flag
 * axis in batch 342 brief D; the park's own line "NO FLAG GROUP HELPS" was a
 * BOUND STATED FROM SIX FLAGS AND THIS IS THE SEVENTH -- it never tried
 * -fno-rerun-loop-opt or -O1.  Plain -O2 is 21 of 291 (ours 291), first
 * differing index 113.  The body below is the park's body, UNCHANGED.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     -e OBJCMP_EXTRA=-fno-rerun-loop-opt goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7ac2d8/ovl_1db4_a_a.c \
 *     asm/overlays/rom_7ac2d8/ovl_1db4_a_a.s --whole
 *
 * Shape: ONE function in the .s (tools/piece.py), datacheck.py exit 0 with no
 * EXPORTS line, so this is a plain WHOLE-FILE conversion -- no split, no
 * export, no linker-script change.  ZERO pins (tools/shimcount.py), no shim,
 * no device, not a fakematch.
 *
 * ===== THE MECHANISM, read from the dumps rather than recalled =====
 *
 * The park's blocker was diagnosed CORRECTLY and bounded WRONGLY.  It says: a
 * reload takes r3 to build the constant 0x90000 for the seventh stack argument,
 * `allocate_reload_reg` then resumes its round robin at `last_spill_reg + 1`,
 * and the eighth argument's reload of the spilled `&p` therefore takes r2 where
 * the ROM has r3 -- "and no source spelling reaches that".  The second half is
 * right about SOURCE and wrong as a bound, because the pass that creates the
 * situation is the SECOND RUN OF loop_optimize, which `-frerun-loop-opt` turns
 * on at -O2 and no source spelling can reach (docs/elevation.md's own table:
 * a loop hoist is "not reachable by a barrier at all", loop.c's
 * `emit_insn_before (..., loop_start)`).
 *
 * MEASURED, -da dumps for both builds (scratch_elev/b342/D/d_dump.sh):
 *   * The ONLY RTL difference at `.08.loop` is that the second loop pass
 *     COMMONS the three per-arm `(set (reg:SI N) (const_int 589824 [0x90000]))`
 *     insns -- 249/reg 87, 334/reg 117, 414/reg 145 -- into ONE pseudo (reg 87)
 *     at insn 664, emitted immediately before the outer loop's
 *     `NOTE_INSN_LOOP_BEG` (note 152).  Base: 196 insns / max pseudo 166.
 *     With the flag: 199 insns / max pseudo 169, and 249/334/414 all survive.
 *   * `.18.greg` shows the consequence directly.  Base conflict rows read
 *     `;; 161 conflicts: 32 33 49 59 71 87 161 2 13`; the flag build's read
 *     `;; 161 conflicts: 32 33 49 59 71 161 2 13`.  The sets differ by EXACTLY
 *     PSEUDO 87 -- the hoisted invariant, now live across the whole two-level
 *     loop and conflicting with every allocno in it.  Reload counts are equal
 *     (61 "Spilling for insn" lines each), so this is not a pressure story: it
 *     is one extra conflicting allocno rotating the round robin.
 *   * In the emitted code the hoist is rematerialised (both builds still do
 *     `mov r3,#144 / lsl r3,#12` inside the loop), so the hoist is invisible in
 *     the .s and only its reload decision survives -- which is why reading the
 *     park's disassembly could never have named this pass.
 *   * The resulting window IS the park's quoted ROM shape:
 *       flag: str r3,[sp,#8] / ldr r3,[sp,#16] / add r5,r5,fp / mov r0,#198 /
 *             str r3,[sp,#12]
 *       base: ldr r2,[sp,#16] / str r3,[sp,#4] / ... / str r2,[sp,#12]
 *
 * EVERYTHING THE PARK ESTABLISHED ABOUT THE BODY STILL HOLDS AND IS WHY THE
 * FLAG ALONE CLOSES IT -- the body is at 21 before the flag, not at 200:
 *   (1) the two positions are EXPLICIT ACCUMULATORS (`up`, `down`), not an
 *       expression in `i`; the literal form is 241 differing because gcc
 *       manufactures only one giv of the pair;
 *   (2) the tail order `down -= ...; up += ...; i++;` is 21 and the ROM's own
 *       EMISSION order (down, i, up) is 29 -- read emission order as a hint;
 *   (3) the `* 0x3333` shift-add chain MUST be inline in the argument list with
 *       its `- 0xcccc` bias inline too, so `expand_mult` has no target pseudo
 *       and the accumulator IS the result (a named local is 109-200).
 * The park's full inert/worse tables are preserved in reports/batch-342.md.
 *
 * FLAG-GROUP NOTE for the Makefile rule: RERUNLOOP_CFLAGS already exists
 * (Makefile:973) and already carries one member, asm/rom_15000/rom_1fe2c_c_a_b.o.
 * The rule must be written against `asm/overlays/rom_7ac2d8/ovl_1db4_a_a.o`
 * EXACTLY -- an explicit rule, no `%`, and no doubled slash
 * (docs/elevation.md, "A Makefile target with a doubled slash is a different
 * target": that mistake made every overlay compare clean while the SHA1 failed).
 */
struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x28 - 0x10];
};

extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __PlaySound(int id);
extern unsigned int __Random(void);
extern void __CutsceneWait(int n);
extern void OvlFunc_common0_10c(int x, int y, int z, int a,
                                int b, int c, int d, struct P *p);

void OvlFunc_924_2009db4(int mode)
{
    struct P p;
    unsigned int j, i;
    int up, down;

    __PlaySound(0xd3);
    if (mode == 0) {
        __CopyMapTiles(0x6f, 0x39, 0x71, 0x2a, 1, 1);
        __CopyMapTiles(0x6f, 0x3b, 0x71, 0x2b, 1, 1);
    } else if (mode == 1) {
        __CopyMapTiles(0x71, 0x3a, 0x70, 0x2e, 1, 1);
        __CopyMapTiles(0x73, 0x3a, 0x71, 0x2e, 1, 1);
    } else {
        __CopyMapTiles(0x73, 0x39, 0x74, 0x2c, 1, 1);
        __CopyMapTiles(0x71, 0x39, 0x73, 0x2c, 1, 1);
    }
    p.f4 = 7;
    p.f8 = 0x80 << 8;
    p.fc = 0x80 << 8;
    j = 0;
    do {
        i = 0;
        down = 0xcb * 0x40000 - (j << 20);
        up = (j << 20) + 0xb0 * 0x40000;
        do {
            if (i & 1) {
                if (mode == 0) {
                    OvlFunc_common0_10c(0xc6 * 0x40000, 0, up,
                                        ((__Random() << 3) >> 16) * 0x3333 - 0xcccc, 0,
                                        ((__Random() << 3) >> 16) * 0x3333 - 0xcccc, 0x90 << 12, &p);
                } else if (mode == 1) {
                    OvlFunc_common0_10c(up + 0xc0 * 0x8000, 0, 0x2ea0000,
                                        ((__Random() << 3) >> 16) * 0x3333 - 0xcccc, 0,
                                        ((__Random() << 3) >> 16) * 0x3333 - 0xcccc, 0x90 << 12, &p);
                } else {
                    OvlFunc_common0_10c(down, 0, 0x2ca0000,
                                        ((__Random() << 3) >> 16) * 0x3333 - 0xcccc, 0,
                                        ((__Random() << 3) >> 16) * 0x3333 - 0xcccc, 0x90 << 12, &p);
                }
                __CutsceneWait(1);
            }
            down -= 0x10000;
            up += 0x10000;
            i++;
        } while (i <= 7);
        if (mode == 0) {
            __CopyMapTiles(0x6f, 0x3a, 0x71, j + 0x2b, 1, 1);
            __CopyMapTiles(0x6f, 0x3b, 0x71, j + 0x2c, 1, 1);
        } else if (mode == 1) {
            __CopyMapTiles(0x72, 0x3a, j + 0x71, 0x2e, 1, 1);
            __CopyMapTiles(0x73, 0x3a, j + 0x72, 0x2e, 1, 1);
        } else {
            __CopyMapTiles(0x72, 0x39, 0x73 - j, 0x2c, 1, 1);
            __CopyMapTiles(0x71, 0x39, 0x72 - j, 0x2c, 1, 1);
        }
        j++;
    } while (j <= 1);
}
