/* OvlFunc_973_200871c  [ovl_7fc720]
 *
 * Source asm: goldensun/asm/overlays/rom_7fc720/ovl_30_c_a_c_c_c_a.s
 *
 * Blocker: INDEXED STORE WHERE THE ROM COMPUTES THE ADDRESS.
 *
 *     rom    add r2, r1, r3 / mov r3, #0x10 / str r3, [r2]
 *     ours   mov r2, #0x10 / str r2, [r1, r3]
 *
 * THE FIRST STORE IN THE SAME FUNCTION IS CORRECT, and the difference between
 * them is the whole diagnosis. For the first, the offset is MODIFIED between
 * the address computation and the store (`p = base + off; off += 0x44;
 * *p = off;`), so gcc has to materialise the address into a register. For the
 * second the offset is dead after the address is formed, so gcc folds it into
 * a register-offset store instead.
 *
 * The ROM materialises both. It also REUSES the offset register as the stored
 * value -- `sub r3,#0x3c / add r2,r1,r3 / mov r3,#0x10` -- which says the
 * original had one variable doing both jobs, as the first store does.
 *
 * TRIED:
 *   the offset variable reassigned as the value, mirroring store one   10 (worse)
 *   a distinct pointer variable for the second address                  3
 *   the stored value as its own named local                             3
 *   both of those together                                              3
 *   -O1                                                                13 (worse)
 *   -fno-schedule-insns2                                                 7 (worse)
 *   -fno-rerun-cse-after-loop, -fno-force-mem, -fno-strength-reduce      3
 *
 * The first of those SHOULD be the answer and is the worst result: reassigning
 * `off` changes the register allocation of the whole prologue, and base and
 * offset swap registers eight instructions earlier. So the construct that
 * fixes the store breaks the thing that already worked, and no arrangement
 * tried gets both.
 *
 * THREE FUNCTIONS SHARE THIS EXACTLY -- OvlFunc_893_2008054 and
 * OvlFunc_894_2008054 are byte-identical twins in adjacent overlays, and
 * OvlFunc_973_200871c has the same prologue with a different constant. Solving
 * one solves three, which is why it is worth more than three instructions.
 */
extern unsigned int iwram_3001ebc;
extern void *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);

int OvlFunc_973_200871c(void)
{
    unsigned char *base;
    unsigned char *p;
    unsigned int off;
    void *a;
    int v;

    base = (unsigned char *)iwram_3001ebc;
    off = 0xe0;
    off <<= 1;
    p = base + off;
    off += 0x44;
    *(unsigned int *)p = off;
    off -= 0x3c;
    p = base + off;
    *(unsigned int *)p = 0x18;
    v = 0x19999;
    a = __MapActor_GetActor(0xb);
    *(int *)((unsigned char *)a + 0x1c) = v;
    a = __MapActor_GetActor(0xb);
    *(int *)((unsigned char *)a + 0x18) = v;
    __MapActor_SetAnim(0xd, 5);
    __MapActor_SetAnim(0xe, 2);
    return 0;
}
