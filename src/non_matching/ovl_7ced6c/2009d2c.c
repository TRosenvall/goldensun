/* OvlFunc_946_2009d2c -- NOT MATCHING, a SIZE mismatch: ref 180 bytes against ours 176.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/N9d2c_best.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * objcmp stops at the size check, so no encoding figure exists yet; the
 * body must reach the reference's LENGTH before a distance can be read.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7ced6c/2009d2c.c \
 *     asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_a_c.s --func OvlFunc_946_2009d2c
 *
 * The original prose park is preserved below.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_946_2009774(int a, int b, int c);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_2009d2c(void)
{
    int a;
    int b;
    int c;
    int d;
    int v;
    int s;
    int t;

    a = *(int *)(__MapActor_GetActor(0xa) + 8) >> 20;
    b = *(int *)(__MapActor_GetActor(0xa) + 0x10) >> 20;
    c = *(int *)(__MapActor_GetActor(0xd) + 8) >> 20;
    d = *(int *)(__MapActor_GetActor(0xf) + 8) >> 20;
    if (b == 0x12) {
        if ((unsigned int)(d - 0x1f) <= 2) {
            v = 0x80;
        } else if ((unsigned int)(c - 0x1f) <= 2) {
            v = 0x80;
        } else {
            OvlFunc_946_2009774(0xa, 0, -0x70);
            v = 0x40;
        }
    } else if (b == 0xa) {
        if ((unsigned int)(d - 0x1f) <= 2)
            return;
        if ((unsigned int)(c - 0x1f) <= 2)
            return;
        v = 0x30;
    } else {
        if (b == 7)
            return;
        goto tail;
    }
    OvlFunc_946_2009774(0xa, 0, -v);
tail:
    __WaitFrames(2);
    t = *(int *)(__MapActor_GetActor(0xa) + 0x10) >> 20;
    s = a - 1;
    __Func_8010704(s, b, 3, 1, s, t);
    __Func_8010704(0, 0, 3, 1, s, b);
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_946_2009d2c -- 0x02009d2c,
 * asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_a_c.s
 *
 * 83 lines against the ROM's 87.  Candidate at scratch/N9d2c_best.c.
 *
 * BLOCKER: THE SHARED CALL TAIL, third confirmed instance.  Four paths pick a
 * value and reach ONE call to OvlFunc_946_2009774; the ROM loads the value in
 * each arm and branches to the shared block:
 *
 *      rom   cmp r3, #2 / bhi .L1d66 / mov r2, #0x80 / b .L1d96
 *      ours  mov r2, #0x80 / cmp r3, #2 / bls .L1d96
 *
 * gcc hoists the load ABOVE its own compare and inverts the branch to fall into
 * the call -- exactly the if-conversion recorded on
 * src/non_matching/ovl_common/4cc.c and src/non_matching/ovl_7fb4a8/2008e10.c.
 * Four lines short and the whole chain shifts.
 *
 * NOT RE-SPENT HERE.  Those two files between them screen six spellings of this
 * shape -- separate calls per arm, a shared local for the value, explicit gotos
 * to the join, and the prototype lever on the callee -- and none of them
 * reaches it.  This function is filed as the third data point, not as another
 * six rounds of the same measurements.  When the class breaks, retry all three.
 *
 * The rest is believed right and screens clean up to the first arm: four
 * `>> 20` reads off __MapActor_GetActor into a, b, c, d; the two range tests
 * spelled `(unsigned)(x - 0x1f) <= 2`, which is what produces the ROM's
 * `sub r3, #0x1f / cmp r3, #2 / bhi`; the b == 7 arm returning without the
 * call; and the two six-argument __Func_8010704 tails sharing `s = a - 1` as
 * both the first register argument and the [sp] slot.
 *)

*/
#endif
