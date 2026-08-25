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
 *
 * SHARPENED, batch 44. The construct this park calls "the one that should work
 * and is the worst result" -- reusing the offset variable as the stored value --
 * IS CORRECT, and has now been confirmed on a function that matches with it:
 * src/rom_8a000/rom_9ad70_c_c_b.c (Func_809b648) has the same
 * `add r2,r1,r3 / mov r3,#0 / str r3,[r2]` shape and needs exactly that form.
 *
 * So the problem here is not the construct. It is that this function has TWO
 * stores sharing one offset variable, and the reuse that fixes the second one
 * changes the register allocation of the first. Func_809b648 has only one
 * store and no such interaction.
 *
 * That narrows the next attempt: keep the reuse for the second store and find a
 * way to stop it disturbing the first, rather than looking for a different
 * construct.
 *
 * BODY REPLACED, batch 44, and the number went UP on purpose.
 *
 * The previous body was 3 of 30 and one of those three was a WRONG INSTRUCTION
 * FORM -- an indexed `str r2, [r1, r3]` where the ROM computes the address.
 * The body below is 5 of 30 and every one of the five is the SAME instruction
 * with two registers swapped:
 *
 *     rom    ldr r1,[r3] ... add r2, r1, r3 ... str r3,[r2]
 *     ours   ldr r2,[r3] ... add r1, r2, r3 ... str r3,[r1]
 *
 * The base lands in r2 where the ROM has r1; everything else is identical,
 * including both address computations and both stores.
 *
 * FIVE IS A BETTER STARTING POINT THAN THREE. A wrong instruction form is a
 * construct error and says the C is not what Camelot wrote; a register-naming
 * difference says the C is right and the allocator disagreed. This body proves
 * the construct.
 *
 * WHAT DID IT: giving the second offset its own variable, each reused as its
 * own store's value --
 *
 *     off = 0xe0 << 1; p = base + off; off += 0x44;  *p = off;
 *     o2  = off - 0x3c; p = base + o2;  o2 = 0x10;   *p = o2;
 *
 * Reusing ONE variable for both (the obvious reading of the ROM, which reuses
 * r3 throughout) is 10 of 30 -- it disturbs the prologue. Two variables, each
 * reused once, keeps the prologue and fixes both stores.
 *
 * TRIED AND DID NOT MOVE THE REGISTER: declaring the offset before the base,
 * declaring the second offset first, computing the offset before the base
 * (7, worse), distinct pointer variables for the two addresses (10), and
 * re-reading iwram_3001ebc for the second address (12).
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
    unsigned int o2;
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
    o2 = off - 0x3c;
    p = base + o2;
    o2 = 0x10;
    *(unsigned int *)p = o2;
    if (__GetFlag(0x814)) {
        __Func_8091ff0(0x8d);
        __Func_8012330(a, b, c);
        __StartEarthquake();
    }
    return 0;
}
