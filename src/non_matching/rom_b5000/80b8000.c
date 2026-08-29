/* Func_80b8000 -- NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_b5000/rom_b7410_c_c_a_a.s
 * Best screen: 43 instructions against the ROM's 44, 16 differing.
 *
 * BLOCKER CLASS: two pointer chains and a shared zero competing for r2/r3.
 *
 * The function writes bytes at +0x5a and +0x58 and words at +0x28 and +0x44,
 * the last two both zero. The ROM interleaves them:
 *
 *      mov r3, r5 / mov r2, #0 / add r3, #0x5a
 *      str r2, [r5, #0x28] / str r2, [r5, #0x44] / strb r2, [r3]
 *      mov r2, r5 / add r2, #0x58 / mov r3, #1 / strb r3, [r2]
 *
 * -- the ZERO lives in r2 and the +0x5a pointer in r3, and r2 is then reused
 * for the +0x58 pointer, which is why the ROM recomputes `mov r2, r5` rather
 * than deriving 0x58 from 0x5a.
 *
 * BATCH 97'S TWO-POINTER LEVER HELPS BUT DOES NOT CLOSE IT. Written with the
 * two byte writes as struct fields, gcc derives the second address from the
 * first (`add r2, #0x5a / strb / sub r2, #0x2 / strb`), 33 differing of 42.
 * Computing BOTH pointers into locals before the first store stops the
 * derivation and gets the length to 43 against 44, 16 differing. That is the
 * lever from src/non_matching/ovl_7aa430/2009bc8.c and it behaves the same way
 * here.
 *
 * WHAT WAS TRIED AGAINST THE REST:
 *   - naming the shared zero as an `int` local, with both pointers up front
 *     (identical at 16)
 *   - naming the zero and computing the +0x58 pointer AFTER the +0x5a store,
 *     which is the ROM's own order (WORSE: back to the derived form, 33)
 *
 * What is missing is one instruction, and the ROM's account of it is that the
 * zero and the second pointer SHARE r2. We give them separate registers, so we
 * never need the recompute. Nothing at the statement level asks gcc to run out
 * of registers.
 *
 * Everything else screens clean: the four `mov`+`lsl` constants, the pooled
 * 0xab85, the `_Actor_TravelTo` argument order, and the signed `/ 8` before
 * atan2 (which is why the ROM has the `cmp r0, #0 / bge / add r0, #7` rounding
 * -- that is gcc's division, not something the source spells).
 */
struct A {
    unsigned char pad00[6];
    unsigned short f6;
    unsigned char pad08[0x28 - 8];
    int f28;
    unsigned char pad2c[4];
    int f30;
    int f34;
    unsigned char pad38[0x44 - 0x38];
    int f44;
    unsigned char pad48_[0];
    int f48;
    unsigned char pad4c[0x58 - 0x4c];
    unsigned char f58;
    unsigned char pad59[1];
    unsigned char f5a;
};

struct C {
    struct A *f0;
    unsigned char pad04[8];
    int fc;
    int f10;
};

extern struct C *GetBattleActor(void);
extern void _Actor_Stop(struct A *a);
extern void _Actor_TravelTo(struct A *a, int x, int y, int z);
extern int atan2(int y, int x);

void Func_80b8000(void)
{
    struct C *c;
    struct A *a;
    unsigned char *p5a;
    unsigned char *p58;

    c = GetBattleActor();
    a = c->f0;
    a->f34 = 0x80 << 10;
    a->f30 = 0x80 << 12;
    a->f48 = 0xab85;
    p5a = &a->f5a;
    p58 = &a->f58;
    a->f28 = 0;
    a->f44 = 0;
    *p5a = 0;
    *p58 = 1;
    _Actor_Stop(a);
    _Actor_TravelTo(a, c->fc, 0, c->f10);
    a->f6 = atan2(c->f10 / 8, c->fc) + (0x80 << 8);
}
