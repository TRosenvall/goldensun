/* OvlFunc_936_2009f14 -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7c097c/ovl_30_c_c_c_a_c_c_c_c.s
 * Best screen: 104 instructions against the ROM's 103, 12 differing.
 *
 * BLOCKER CLASS: register class on the stored zero.
 *
 *     rom    mov r3, #0x0 / add r0, #0x64 / strh r3, [r0]
 *     ours   mov r5, #0x0 / add r0, #0x64 / strh r5, [r0]
 *
 * plus the push {r5, lr} / pop {r5} that r5 costs. Everything else in the
 * function is identical.
 *
 * WHAT WAS LEARNED HERE IS WORTH MORE THAN THE PARK. Storing a literal 0
 * through a short * does NOT give `mov r3, #0` -- gcc-2.96 has no immediate
 * alternative for an HImode constant and emits `ldr r3, =0x0`, a four-byte
 * pool load of zero. Measured on every spelling: short, unsigned short, a
 * short * local, indexing a short * return. All pool.
 *
 * The ROM's mov is an SImode zero truncated by the strh, so the source's
 * right-hand side is INT-typed. Writing
 * `{ int zero = 0; *(short *)(...) = zero; }` per case produces exactly the
 * ROM's instruction sequence -- 106 differing down to 12 -- and the only
 * residue left is that gcc coalesces the four zeros into one pseudo whose live
 * range crosses the bl, so it lands in a callee-saved register. The ROM
 * materialises its zero AFTER the call.
 *
 * Also solved on the way, and independent of the above: the four
 * __MapActor_GetActor results must NOT go through a named local. With
 * `p = GetActor(n); *(short *)(p + 0x64) = 0;` gcc keeps p live and copies
 * (mov r2, r0 / add r2, #0x64), ten instructions long. Inlined into the store
 * expression, r0 is dead after the add and the ROM's form appears.
 *
 * The bls/b at the dispatch is a CONSEQUENCE, not a cause: a Thumb conditional
 * branch reaches +-254 bytes, and at 105 instructions plus a five-word table
 * the epilogue is just out of range, so gcc inverts. It disappears on its own
 * once the body is the right length.
 *
 * The spelling below is the plain one; the int-zero variant that reaches 12 is
 * scratch/c9f14_F.c in the batch-105 working set.
 */
extern int L5144 __asm__(".L5144");
extern unsigned char gScript_936__0200bec0[];
extern unsigned char gScript_936__0200bfb0[];
extern char *__MapActor_GetActor(int slot);
extern void __MapActor_SetBehavior(int slot, unsigned char *script);

void OvlFunc_936_2009f14(void)
{
    switch (L5144) {
    case 0:
        *(short *)(__MapActor_GetActor(0x15) + 0x64) = 0;
        __MapActor_SetBehavior(0x15, gScript_936__0200bec0);
        L5144++;
        break;
    case 1:
        if (*(short *)(__MapActor_GetActor(0x15) + 0x64) == 0)
            return;
        *(short *)(__MapActor_GetActor(0x14) + 0x64) = 0;
        __MapActor_SetBehavior(0x14, gScript_936__0200bfb0);
        L5144++;
        break;
    case 2:
        if (*(short *)(__MapActor_GetActor(0x14) + 0x64) == 0)
            return;
        *(short *)(__MapActor_GetActor(0x14) + 0x64) = 0;
        __MapActor_SetBehavior(0x14, gScript_936__0200bec0);
        L5144++;
        break;
    case 3:
        if (*(short *)(__MapActor_GetActor(0x14) + 0x64) == 0)
            return;
        *(short *)(__MapActor_GetActor(0x15) + 0x64) = 0;
        __MapActor_SetBehavior(0x15, gScript_936__0200bfb0);
        L5144++;
        break;
    case 4:
        if (*(short *)(__MapActor_GetActor(0x15) + 0x64) == 0)
            return;
        L5144 = 0;
        break;
    }
}
