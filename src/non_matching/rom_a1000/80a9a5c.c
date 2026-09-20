/* Func_80a9a5c -- 0x080a9a5c, asm/rom_a1000/rom_a8604_c_c_a_a_a.s (2 functions,
 * no data section).
 *
 * ==================== SOLVED IN BATCH 272, HELD ON A BUILD-INPUT DECISION ====================
 *
 * This is NOT a blocker park. The C below is exact at the instruction level and the
 * only thing standing between it and a landing is one line in message.sym, which is
 * a build input and so a decision rather than a measurement.
 *
 *   objcmp:  XX ENCODINGS differ in 1 place(s) (ref 59, ours 59)
 *            first at index 58: ref 00000b24  ours 00000000
 *
 * Index 58 is the POOL WORD. All 58 instruction encodings and all 12 relocations are
 * at identical offsets; ours holds an R_ARM_ABS32 against `_MSG_b24` where the ROM
 * holds the literal 0xb24. That is the same situation
 * src/rom_a1000/rom_a1050_c_c_c_c_a.c records for _MSG_b20 -- objcmp cannot return OK
 * for an aliased symbol, and `make compare` is the only authority (see
 * docs/elevation.md, "objcmp cannot verify an aliased symbol").
 *
 * TO LAND IT:
 *   1. add `_MSG_b24 = 0xb24;` to message.sym
 *   2. split the .s two ways (this function is one of two)
 *   3. NOTE Makefile:57 -- editing message.sym leaves stage1.o stale and the failure
 *      mode is an undefined reference from an overlay. A clean build avoids it.
 *
 * WHY A SYMBOL IS REQUIRED, and this is the measured part. With the plain literal
 * 0xb24 the C is 58/58 lines and NINE differing encodings, and the sole fault is
 * sched2 hoisting `ldr r5, =0xb24` into the load-latency slot after
 * `ldr r3, =iwram_3001f2c`, ahead of the three leading calls. -fsched-verbose=5 shows
 * the constant as insn 29, priority 11, dep 0, cost 2, competing with `ldr r3, [r3]`
 * (also 11) and winning the tie. Its priority is structurally pinned at
 * priority(mov r0, r5) + 2 = 11, above the calls' 9, and the scheduler fills the
 * post-call gap anyway -- so no priority or tie-break spelling reaches the ROM's
 * placement.
 *
 * That is docs/elevation.md's recorded tell firing on its own: "a pool load of a
 * SYMBOL is not hoisted, where a pool load of an int constant is." Note the OTHER
 * tell says nothing here -- 0xb24 is not a shiftable byte, so "gcc never pools a
 * constant it can build with a mov" does not apply. This is the hoist tell alone,
 * which is why the case is worth stating carefully before anyone adds the entry.
 *
 * MEASURED (58 lines throughout): plain literal 9 differing; chained leading calls
 * plus the literal 6; four separate literals 53 lines/57; a
 * `register int msg __asm__("r5")` pin 57 lines/36; -fno-schedule-insns,
 * -fno-sched-interblock, -fno-sched-spec and -fsched-spec-load all identical to
 * default; THE SYMBOL 1 (the pool word alone). Chaining the leading calls is NOT
 * needed once the symbol is used, so the prototypes stay honest.
 *
 * A FALLBACK THAT NEEDS NO .sym EDIT: `msg = (int)&_MSG_b20 + 4;` also reaches 1
 * differing, emitting pool word 0x00000004 plus ABS32 _MSG_b20, which links to 0xb24.
 * It works and it is a lie about the source -- recorded so nobody adopts it by
 * accident, and REJECTED here for that reason.
 *
 * The file-mate heuristic paid twice: src/rom_a1000/rom_a8604_a_c_b.c gave
 * `extern unsigned int iwram_3001f2c;`, the ignored _GetUnit result and the
 * `0xe4 << 1` spelling verbatim; rom_a1814_c_c_c_a_b.c gave the `(int)&_MSG_*` base
 * idiom.
 *
 * The verified candidate is scratch_elev/b272/C/a9a5c_v6.c; the fallback is a9a5c_v7.c.
 */
