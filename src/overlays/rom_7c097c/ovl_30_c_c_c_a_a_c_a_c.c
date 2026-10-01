/* OvlFunc_936_20095b4  --  LANDS BYTE-IDENTICAL.  Batch 317, brief D.
 *
 * Install as:   src/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c_a_c.c
 * Split:        NONE NEEDED -- the .s holds exactly ONE function and
 *               `tools/datacheck.py` on it prints nothing (no data section).
 * Requires:     a per-file CSE_CFLAGS Makefile rule (-fno-rerun-cse-after-loop)
 * Pins:         0.   fakematch.txt row: NOT needed.
 *
 * THE BODY IS THE PARK'S BODY, UNCHANGED.  Nothing in the C needed to move.
 *
 * FIGURE: 44 bytes, 18 encodings and 4 relocations IDENTICAL, both
 * `objcmp --func` and `objcmp --whole`, under the flag.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c_a_c.c \
 *     asm/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c_a_c.s \
 *     --func OvlFunc_936_20095b4
 * (once the Makefile rule below exists, objcmp picks the flag up by itself and
 *  prints `(built with: -fno-rerun-cse-after-loop)`.  Before then, reproduce
 *  the same figure with OBJCMP_EXTRA=-fno-rerun-cse-after-loop.)
 *
 * (The park named asm/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c_a.s, which no
 * longer exists; the live path is the one above.)
 *
 * MAKEFILE RULE -- the TU holds this one function, so the rule is tight:
 *
 *   # CSE_CFLAGS, batch 317.  OvlFunc_936_20095b4 tests save bit 0x200 and
 *   # then sets it, and the ROM rebuilds `mov r0,#0x80 / lsl r0,#2` at BOTH
 *   # sites.  At -O2 the second CSE pass hoists the id into r6 and keeps it
 *   # live across the __GetFlag call, which pays a wider push/pop and two
 *   # `mov r0,r6` copies -- 14 differing of 18 with the COUNT ALREADY EXACT.
 *   # Byte-exact with the flag at 44 bytes, 18 encodings, 4 relocations.
 *   # -fno-gcse, -fno-cse-follow-jumps, -fno-cse-skip-blocks and
 *   # -fno-expensive-optimizations are all inert at 14; -fno-schedule-insns2
 *   # is 13.  Same shape and the SAME flag id as OvlFunc_936_2009930.
 *   asm/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c_a_c.o: src/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c_a_c.c
 *   	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
 *   	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
 *   	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
 *
 * WHAT THE PARK SAID.  Its MECHANISM SURVIVED exactly: the flag id is built
 * once into r6, pays a wider push and pop, and is copied to r0 twice.  Its
 * figure did not -- the park claims "seven of eighteen" and the measured figure
 * for its own body is 14 of 18.  Nothing in the tree verified the 7.
 *
 * ITS CONCLUSION IS WHAT WAS WRONG.  The park ends by proposing a new third
 * clause for the basic-block lever ("EVERY repeated use must be in a different
 * basic block from the assignment") and stops there.  The cure was already in
 * the tree FOUR TIMES, as per-file CSE_CFLAGS rules whose Makefile comments
 * describe this function's shape verbatim:
 *
 *   OvlFunc_948_20097ac -- "tests save bit 0x220 and then sets it, and the ROM
 *     rebuilds `mov r0,#0x88 / lsl r0,#2` at BOTH sites ... exact with the
 *     flag".  It records the SAME three-spelling negative result the park
 *     records, with the reason the park was missing: "the constant is folded
 *     before CSE ever runs: the rematerialisation is a PASS-LEVEL property,
 *     not a source-level one."
 *   OvlFunc_932_200a804 -- "flag id 0x908 read twice with the first use
 *     dominating the second -- the guard/set shape ... exact here".
 *   OvlFunc_936_2009930 -- the SAME overlay family and the SAME id 0x200,
 *     landed on this rule in batch 302.
 *   OvlFunc_891_200905c -- same shape with a branch between the uses.
 *
 * So the park's "TRIED: two separate locals (7), one local for the second use
 * only (7), the literal at both sites (7)" is the expected result of a
 * source-level search against a pass-level property, and its "the rule needs a
 * third clause" is a true observation about the basic-block lever that is not
 * the blocker here.  docs/elevation.md's "When gcc HOISTS a repeated constant,
 * exactly: dominance" table already predicts the hoist (one dominating use plus
 * one in a branch -> HOISTS, push {r5,lr} -> here push {r5,r6,lr}); the ROM's
 * REBUILD is the thing no source form can ask for.
 */
extern unsigned int iwram_3001ee0;
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void *__MapActor_GetActor(int slot);

void OvlFunc_936_20095b4(void)
{
    unsigned char *p;
    void *a;

    if (!__GetFlag(0x80 << 2)) {
        p = (unsigned char *)iwram_3001ee0;
        a = __MapActor_GetActor(0);
        *(void **)(p + 0x18) = a;
        __SetFlag(0x80 << 2);
    }
}
