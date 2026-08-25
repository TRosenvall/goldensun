/* OvlFunc_893_2008054  [ovl_78dd40]
 *
 * Source asm: goldensun/asm/overlays/rom_78dd40/ovl_30_c_c.s
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
extern int __GetFlag(int id);
extern void __Func_8091ff0(int a);
extern void __Func_8012330(int a, int b, int c);
extern void __StartEarthquake(void);

int OvlFunc_893_2008054(void)
{
    unsigned char *base;
    unsigned char *p;
    unsigned int off;
    int a;
    int b;
    int c;

    a = 0x80 << 9;
    b = 0x80 << 9;
    c = 0x80 << 9;
    base = (unsigned char *)iwram_3001ebc;
    off = 0xe0;
    off <<= 1;
    p = base + off;
    off += 0x44;
    *(unsigned int *)p = off;
    off -= 0x3c;
    p = base + off;
    *(unsigned int *)p = 0x10;
    if (__GetFlag(0x814)) {
        __Func_8091ff0(0x8d);
        __Func_8012330(a, b, c);
        __StartEarthquake();
    }
    return 0;
}
