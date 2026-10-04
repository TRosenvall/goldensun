/* Func_80b110c -- p2, batch 321 bucket C.  THE STRONGEST OF THE THREE.
 *
 * FIGURE 1 of 66 encodings, AND THE 1 IS THE PHANTOM POOL RELOCATION.
 *   All 65 instruction encodings and all 6 call relocations are identical at
 *   identical offsets.  Index 64 is the POOL WORD: ref 00000182, ours 00000000
 *   plus an extra R_ARM_ABS32 against _MSG_182.  Aliased-symbol phantom;
 *   `make compare` is the only authority.
 *
 * PREREQUISITE (owner decision): add `_MSG_182 = 0x182;` to message.sym.
 *   Makefile:57 -- a message.sym edit leaves stage1.o stale.  Use a clean build.
 *
 * Verify with (INSTALLED path -- single function, NO SPLIT NEEDED):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_b0000/rom_b0070_a_a_c_c_a_c_c.c \
 *     asm/rom_b0000/rom_b0070_a_a_c_c_a_c_c.s --func Func_80b110c
 *
 * SPLIT: none.  grep -c thumb_func_start = 1.  datacheck.py: CLEAN.
 * PINS: 0.  Devices: none.  Flags: none.
 *
 * ================ TEST 1: STRUCTURAL IMPOSSIBILITY, AND THE MODE CHECKS OUT ================
 *
 * The ROM has `ldr r0, =0x182 / add r0, r5, r0`, feeding _Func_801e7c0's first
 * argument.  THE MODE IS SImode -- a full 32-bit add whose result is a 32-bit
 * argument, so there is no HImode narrowing for gcc to take.
 *
 * In gcc-2.96 *thumb_movsi_insn the source constraints are, in order,
 *     l, I, J, K, >, l, mi, *lh
 * with CONST_OK_FOR_THUMB_LETTER (arm.h:1095) giving I = <256, J = -255..-1,
 * K = thumb_shiftable_const.  0x182 is 0xc1 << 1, so K MATCHES at alternative 3,
 * and the pool path `mi` is alternative 6.  recog takes the first matching
 * alternative, so AN SImode const_int 0x182 CAN NEVER REACH THE LITERAL POOL.
 * A pool word holding it, reached by an SImode `ldr`, can only be a relocation.
 *
 * THE BATCH-318 MODE CORRECTION WAS APPLIED AND DOES NOT BITE HERE -- and the
 * brief's statement of it is itself wrong; see FINDINGS.md.  *thumb_movhi_insn
 * DOES have an immediate alternative (alt 5, constraint I, `mov %0, %1`).  What
 * makes HImode pool everything is ALTERNATIVE ORDER: its alt-1 source constraint
 * is `mn`, and `n` matches ANY const_int, so a HImode constant is caught at
 * alt 1 and never reaches alt 5.  Probed (probe_mode.c): HImode stores of 0, 5,
 * 0xff and 0x100 ALL pool; QImode stores of 0 and 5 both `mov` (movqi's alt-1
 * source is `m`, with no `n`); SImode stores of 0 and 5 both `mov`.
 * None of that reaches this park, because this reference is SImode.
 *
 * ================ TEST 2: IT COMPLETES THE FUNCTION ================
 *
 * Figure 1, and the 1 is the phantom.  Nothing else differs.  This is NOT the
 * withheld-_FILE_e4 shape (accepted argument, 13 real differences left).
 *
 * CRITERION 2, MEASURED (crossfire, depth 2).  EIGHT device-free spellings --
 * 0x182, 0xc1<<1, 386, 0x180|2, (unsigned short)0x182, via an unsigned int local,
 * via an unsigned short local, and even a compiler-barrier DEVICE -- ALL give
 *   28 differing at 65 instructions against 66, size 148 against 152,
 *   and flagged MEM (the memory profile diverges from ref ldr=4 str=1).
 * Every one emits `mov r3,#193 / lsl r3,#1`.  So the literal is UNREACHABLE
 * rather than merely different -- the same footing as _AREA_3d, whose note
 * records 263 instructions against the ROM's 265.  Note the DEVICE fails too:
 * unlike p1 this is not a scheduling problem, it is a different instruction
 * stream, so no barrier can rescue it.
 *
 * ================ CORROBORATION: 0x182 IS A SHARED BASE, NOT A ONE-OFF ================
 *
 * 0x182 is pool-loaded in EIGHT reference .s files across five ROM regions:
 *   asm/rom_a1000/rom_a4f08_c_a.s:51          asm/rom_a1000/rom_a8604_c_c_a_a_a.s:85
 *   asm/rom_b0000/rom_b0070_a_a_c_c_a_c_c.s:48 (this one)
 *   asm/rom_b0000/rom_b0070_a_a_c_c_c_a_a_a_b.s:253
 *   asm/rom_15000/rom_1aeec_a_a_c_c.s:458     asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a.s:284
 *   asm/rom_15000/rom_17e88_a_a_c.s:517       asm/rom_15000/rom_23178_a_a_a_a_c_a_c_c.s:1704
 *
 * And the ROM's own annotation on Func_80a9aec (the file-mate of p1, which loads
 * the same value) reads "the item name (0x182 + id)".  So this is a message-id
 * BASE added to an item id -- message.sym's shiftable-ids section extending from
 * ids to bases, exactly as the park said.  ONE ENTRY SERVES EIGHT CALL SITES.
 *
 * The park's two control-flow findings (how to place a block before its dominator
 * via an ELSE arm; `bne FAR / b EXIT` needing `if (!c) return; else goto FAR;`)
 * were reproduced -- the body below is at 65 of 65 instructions on them.
 */
extern int _MSG_182;
extern void _Func_8016498(unsigned int);
extern void _Func_801e7c0(unsigned int, unsigned int, unsigned int, unsigned int);
extern void _Func_801ea08(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);

void Func_80b110c(unsigned int arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3)
{
    unsigned int msg;

    if (arg0 == 0)
        return;
    else
        goto start;
one:
    _Func_801e7c0(0xc92, arg0, 0, 8);
    return;
wide:
    msg = 0xc8b;
    _Func_801e7c0(msg, arg0, 0, 8);
    _Func_801ea08(arg2, 5, arg0, 0x20, 8);
    msg -= 3;
    _Func_801e7c0(msg, arg0, 0x48, 8);
    return;
start:
    _Func_8016498(arg0);
    _Func_801e7c0(arg1 + (unsigned int)&_MSG_182, arg0, 0, 0);
    if (arg2 != 0)
        goto wide;
    if (arg3 != 1) {
        if (arg3 != 2)
            goto wide;
        _Func_801e7c0(0xc93, arg0, 0, 8);
    } else
        goto one;
}
