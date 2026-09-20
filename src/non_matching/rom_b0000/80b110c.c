/* Func_80b110c -- 0x080b110c, asm/rom_b0000/rom_b0070_a_a_c_c_a_c_c.s (1 function, no
 * data section, so landing needs NO split).
 *
 * ==================== SOLVED IN BATCH 273, HELD ON A BUILD-INPUT DECISION ====================
 *
 * This is NOT a blocker park. Every instruction encoding and all six call relocations
 * match. The one differing word is the POOL ENTRY:
 *
 *   XX ENCODINGS differ in 1 place(s) (ref 66, ours 66)
 *      first at index 64: ref 00000182  ours 00000000
 *   plus our extra ['00000090','R_ARM_ABS32','_MSG_182']
 *
 * which is the documented _MSG_b20 situation (src/rom_a1000/rom_a1050_c_c_c_c_a.c):
 * objcmp cannot return OK for an aliased symbol and `make compare` is the only
 * authority. TO LAND: add `_MSG_182 = 0x182;` to message.sym. Note Makefile:57 --
 * editing message.sym leaves stage1.o stale, so use a clean build.
 *
 * THIS IS A NEW SHAPE FOR THAT SECTION: 0x182 is a message-id BASE added to a parameter
 * (`arg1 + 0x182`), not an id in its own right. message.sym's shiftable-ids section
 * extends from ids to bases.
 *
 * AND THE TELL IS FIRMER THAN THE USUAL ONE. `ldr rX, =<byte << n>` IS ALWAYS A SYMBOL:
 * thumb_shiftable_const plus the K-constraint split in arm.md mean gcc ALWAYS builds a
 * `0xff << n` value as mov/lsl, so a pool load of such a value cannot come from a
 * const_int at all. 0x182 is 0xc1 << 1. That is a structural impossibility rather than
 * "the ROM pooled something small", which is the weaker argument the other .sym requests
 * rest on.
 *
 * TWO CONTROL-FLOW FINDINGS CAME OUT OF THIS FUNCTION and they are the reusable part:
 *
 * 1. HOW TO PLACE A BLOCK BEFORE ITS DOMINATOR. `if (c) goto L;` can never leave L's
 *    block where you wrote it when L has one predecessor: cleanup_cfg/try_merge_blocks
 *    runs at the SIBLING pass (dump .01.sibling, before .02.jump) and merges the
 *    intermediate `[b L]` block with L, physically relocating L's code to the jump site.
 *    The cure is to put the goto in an ELSE ARM -- `if (!c) { ... } else goto L;` --
 *    because jump.c's follow_jumps runs unconditionally at the top of its loop, before
 *    every other rule, and tensions the conditional straight onto L so no
 *    single-successor block ever exists. That one change took this function from 56
 *    differing to 53 with the whole CFG laid out as the ROM has it.
 *
 *    docs/elevation.md's "a two-instruction block reached by goto is DUPLICATED INLINE"
 *    is the same phenomenon; the mechanism and the escape hatch are the addition.
 *
 * 2. `bne FAR / b EXIT` IS NOT REACHABLE from `if (c) goto FAR; return;` -- follow_jumps
 *    retargets the drop-through conditional to the exit and you get `beq EXIT / b FAR`.
 *    Write `if (!c) return; else goto FAR;`.
 *
 * No pins, no flags.
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
