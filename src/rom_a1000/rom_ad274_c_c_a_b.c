/* Func_80ad69c  --  ResetPartyAnimations  --  0x080ad69c
 *
 * MATCHES.  0 of 25 encodings; 56 bytes, 25 encodings and 2 relocations
 * identical, and `--whole` green on the split part.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_a1000/rom_ad274_c_c_a_b.c asm/rom_a1000/rom_ad274_c_c_a_b.s --func Func_80ad69c
 *
 * SPLIT SHAPE.  tools/datacheck.py asm/rom_a1000/rom_ad274_c_c_a.s is silent
 * (exit 0).  tools/split_s.py asm/rom_a1000/rom_ad274_c_c_a.s Func_80ad69c
 * --dry-run:
 *     would write asm/rom_a1000/rom_ad274_c_c_a_b.s  (1 function,   31 lines) <- this
 *     would write asm/rom_a1000/rom_ad274_c_c_a_c.s  (2 functions, 1871 lines)
 *     would REMOVE asm/rom_a1000/rom_ad274_c_c_a.s, would rewrite stage1.ld
 * Install path: src/rom_a1000/rom_ad274_c_c_a_b.c.  EXPORTS: none.  PINS: 0.
 * DEVICES: none.  FLAG GROUPS: none.
 *
 * ================= HOW IT CLOSED, FROM 17 =================
 * The park read 17 of 25 and diagnosed it correctly -- "gcc CSEs the SUM where
 * the ROM CSEs the CONSTANT" -- but its three enumerated shapes were not
 * exhaustive, and the 17 was mostly MISALIGNMENT: the reference holds 23
 * instructions plus 2 pool words and the park's body held 22 plus a 2-byte pad,
 * so ref idx 12..22 equalled ours idx 11..21 EXACTLY.  The whole 17 was ONE
 * MISSING INSTRUCTION -- the ROM's second `adds r7, r2, r1` -- plus its shift
 * and the register renaming around it.
 *
 * LEVER 1 -- THE OFFSET MUST BE A LITERAL, NOT A NAMED LOCAL.  All three of the
 * park's shapes name the offset (`unsigned int off = 0x219`).  That makes the
 * address `(plus reg reg)`, which IS a legal Thumb `ldrb rD,[rA,rB]`, so expand
 * creates NO add insn and the one pointer the source names is the only `plus` in
 * the function -- which is exactly the single expression gcse then folded.  A
 * LITERAL makes it `(plus reg (const_int 537))`, and 537 is far outside `ldrb`'s
 * imm5, so legitimize_address forces the sum into a register AT EACH SITE.
 * `base[0x219]` in both the entry test and the loop condition: 17 -> 2, size
 * exact, relocations exact.  No flag; the park's `-fno-gcse` row is unnecessary.
 *
 * LEVER 2 -- THE WALK MUST BE AN INDEX, NOT A PRE-HEADER POINTER.  The last 2
 * were the order of the two pre-header adds:
 *     ROM   movs r3,#0x8a / lsls r3,#1 / adds r7,r2,r1 / adds r5,r2,r3
 *     ours  movs r3,#0x8a / lsls r3,#1 / adds r5,r2,r3 / adds r7,r2,r1
 * `move_movables` hoists a loop invariant with `emit_insn_before (..., loop_start)`
 * (loop.c:2055), i.e. at the END of the pre-header, so it lands AFTER anything
 * the SOURCE put there.  `strength_reduce` is called at loop.c:1135, AFTER
 * `move_movables` at loop.c:1110, and its giv initialiser goes in with
 * `emit_iv_add_mult (bl->initial_value, ..., loop_start)` (loop.c:4778) -- also
 * before loop_start, therefore AFTER the hoisted invariant.  So spelling the walk
 * as an INDEX and letting strength reduction build the `ldmia r5!, {r0}` puts the
 * two adds in the ROM's order.  A named pointer assigned in the pre-header cannot.
 *
 * MEASURED, for the record: `((int *)(state+0x114))[i]` with if+do/while 0;
 * `*(int *)(state + 0x114 + i*4)` 0; the `for` form below 0.  With a named
 * pre-header `q`: 2 (literal offsets), 13 (`for` loop), 13 (`q` before the `if`),
 * 25 at 60 bytes (`q` before the `if` plus `q[i]`).  With a named `p2 = base +
 * 0x219`: 17 either side of `q` -- the park's own figure, reproduced.
 * -fno-schedule-insns2 is WORSE (9): sched2 does not decide the two-add order.
 */
extern unsigned char *iwram_3001f2c;
extern void _Sprite_SetAnim(int handle, int anim);

void Func_80ad69c(void)
{
    unsigned char *state;
    int i;

    state = iwram_3001f2c;
    for (i = 0; i < state[0x219]; i++)
        _Sprite_SetAnim(((int *)(state + 0x114))[i], 1);
}
