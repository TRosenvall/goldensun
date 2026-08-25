/* OvlFunc_957_200b610  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7e3e08/ovl_30_c_c_c_c.s
 * Best screen: 3 instructions in disagreeing regions, of 25 (streams same length).
 *
 * BLOCKER CLASS: register allocation -- and a strange one.
 *
 * All three remaining differences are the SAME register.  The pointer read
 * from `actor + 0x50` lives in r4 in the ROM and in r0 for us:
 *
 *      rom   ldr r4, [r5, #0x50]   ours  ldr r0, [r5, #0x50]
 *      rom   ldrb r1, [r4, #0x9]   ours  ldrb r1, [r0, #0x9]
 *      rom   strb r3, [r4, #0x9]   ours  strb r3, [r0, #0x9]
 *
 * Note what that means: the ROM's prologue is `push {r5, lr}`.  It uses r4
 * WITHOUT SAVING IT.  r4 is callee-saved in this ABI, so the ROM function
 * clobbers its caller's register.  gcc will not produce that from any source
 * spelling -- if it allocates r4 it also pushes it, and if it does not want to
 * push it, it uses a caller-saved register, which is exactly what we get.
 *
 * WHAT WAS TRIED
 *
 *  1. The declaration lever, moving the pointer's declaration to the head of
 *     the local list.  No change, 3 of 25.
 *  2. Loading it BEFORE the call to __MapActor_GetActor, so that it is live
 *     across a call and gcc is forced to give it a callee-saved register.
 *     WORSE, 7 of 25 -- gcc does then use a callee-saved register, but it also
 *     pushes it and moves the load above the call, so three differences become
 *     seven.
 *  3. `--no-sched2`.  Byte-identical output; this is not the scheduler.
 *
 * TWO SPELLINGS THAT DID FIX SIX OF THE ORIGINAL EIGHT, kept below:
 *
 *  - `m = 0xc; m &= u;` rather than `m = 0xc & u;`.  The ROM's `and r2, r3`
 *    has the CONSTANT as the destination.  Written as a single expression, gcc
 *    makes the loaded byte the destination, and -- because 0xc then stays live
 *    as a constant -- it derives the second mask as `sub r3, #0x19`
 *    (0xc - 0x19 = -13) instead of the ROM's `mov r3, #0xd / neg r3, r3`.
 *    Making the constant the destination kills it after the AND, and the
 *    derivation disappears with it.
 *  - It also fixed the instruction ORDER: gcc had been hoisting both `ldrb`s
 *    together, and with 0xc dead it emits them where the ROM does.
 *
 *  Compare src/non_matching/rom_c0/8006384.c, where the same lever does
 *  nothing because both AND operands are registers rather than a literal.
 */
extern unsigned char *__MapActor_GetActor(int slot);

void OvlFunc_957_200b610(unsigned char *actor)
{
    unsigned char *a;
    unsigned char *p;
    unsigned char *q;
    unsigned char *s;
    unsigned char *d;
    int u;
    int v;
    int m;
    int n;

    a = actor;
    if (a == 0)
        return;
    p = a + 0x23;
    *p = 0;
    q = __MapActor_GetActor(0);
    s = *(unsigned char **)(q + 0x50);
    d = *(unsigned char **)(a + 0x50);
    u = s[9];
    m = 0xc;
    m &= u;
    v = d[9];
    n = 0xd;
    n = -n;
    n = n & v;
    n = n | m;
    d[9] = n;
}
