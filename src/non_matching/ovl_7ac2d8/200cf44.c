/* OvlFunc_924_200cf44  [overlays/rom_7ac2d8]
 *
 * Source asm: goldensun/asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c.s
 *
 * BLOCKER CLASS: arg-interleave at an UNGUARDED site. 2 of 28.
 *
 *     rom    mov r0, #0xb   / lsl r1, #0x12
 *     ours   lsl r1, #0x12  / mov r0, #0xb
 *
 * The site is in the function's first basic block and there is no conditional
 * branch anywhere in the 28 instructions, so the dominating-block lever has no
 * boundary to cross. Third confirmed instance of that boundary this batch,
 * alongside 200dca4 and 200bdec.
 *
 * WORTH ACTING ON ELSEWHERE: this is a near-twin of the park
 * src/non_matching/ovl_798dc4/2008d68.c -- same `|= 8` at
 * iwram_3001f30 + 0x71c, same call tail -- which is parked at 2 of 22 on an
 * `orr` register-role swap. THE STRUCT-TYPING LEVER RECOVERS IT: writing
 * `*p = 8 | *p` through an `unsigned char *` gives 4 differing, while a struct
 * with a named field and `p->flags |= 8` gives 2. That park should be
 * re-attacked with the struct form.
 *
 * MEASURED (all 2 unless noted): struct field `|=` 2; `unsigned char *` with
 * `*p = 8 | *p` 4; naming both split builds 2; naming only the first 2; naming
 * only the second 4; naming the slot 2; a shared slot local across both calls
 * 2; __MapActor_SetPos with no prototype 2; --no-rerun-cse 2; --no-sched2 8;
 * --O1 10.
 */
typedef struct S {
    unsigned char pad[0x71c];
    unsigned char flags;
} S;

extern unsigned int iwram_3001f30;
extern void __Func_8096fb0(int a, int b);
extern void __Func_80970f8(int a, int b);
extern void __Func_809728c(void);
extern void __FieldMove(int a);
extern void __Func_8097174(void);

void OvlFunc_924_200cf44(void)
{
    S *p;

    p = (S *)iwram_3001f30;
    __MapActor_SetPos(0xb, 0xd2 << 18, 0x96 << 18);
    __Func_8096fb0(0x5d, 1);
    __Func_80970f8(3, 0xb);
    p->flags |= 8;
    __Func_809728c();
    __FieldMove(1);
    __Func_8097174();
}
