/* OvlFunc_943_200985c  [overlays/rom_7c7b9c]
 *
 * Source asm: goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_c.s
 *
 * BLOCKER CLASS: pool-loads-first argument ordering (3 sites), plus one
 * `orr` destination register. 62 lines against 62, NINE differing, and they
 * are only these two shapes:
 *
 *   THREE SITES, all the same:
 *     rom    mov r0, #imm / lsl r1, #0x10 / ldr r2, =X
 *     ours   lsl r1, #0x10 / ldr r2, =X   / mov r0, #imm
 *   The ROM fills the FIRST argument register before finishing the expensive
 *   ones; gcc finishes the expensive ones and sets r0 last. One
 *   __MapActor_SetPos site and both __MapActor_SetSpeed sites.
 *
 *   ONE SITE:
 *     rom    orr r5, r3 / strb r5, [r0]
 *     ours   orr r3, r5 / strb r3, [r0]
 *   r5 holds the shared constant 0x80, r3 the loaded byte. The ROM makes the
 *   CONSTANT's register the destination; gcc makes the loaded byte's register
 *   the destination. Note the FIRST of the two identical `|= 0x80` sites
 *   matches -- only the second differs, so this is not a property of the
 *   expression, it is allocation state at the second site.
 *
 * THE PER-CALL-SITE PROTOTYPE LEVER DOES NOT REACH THIS, and that is worth
 * recording because the sibling file in this very overlay is where the lever
 * is documented. src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_b.c
 * records that for __MapActor_SetPos "five of the seven calls want `mov r0`
 * LAST and two want it FIRST", solved by leaving the callee UNDECLARED for the
 * five (implicit int, r0 last) and routing the other two through `SetPosD`, an
 * __asm__ alias with a real prototype (r0 first).
 *
 * That reads as a two-way switch, and here NEITHER position moves anything:
 *
 *   MEASURED, every one of them 9 differing:
 *     real prototypes for both callees (the natural spelling)      9
 *     __MapActor_SetSpeed left undeclared (implicit int)           9
 *     __MapActor_SetSpeed via an __asm__ alias with a prototype    9
 *     the second `|= 0x80` written `*p = 0x80 | *p`                9
 *
 * The last is a no-op: gcc normalises the operand order back, so the `orr`
 * destination is not reachable by swapping the source operands. The first
 * three say the prototype lever is narrower than the sibling's note implies --
 * it moved r0 on THAT function's SetPos calls and moves nothing on this
 * function's, so whatever selects between the two orderings is not the
 * declaration alone. Do not spend a round re-running these four.
 */
extern unsigned char L5160[] __asm__(".L5160");
extern unsigned char gScript_943__0200c58c[];
extern unsigned char gScript_943__0200c628[];

extern void __CutsceneStart(void);
extern int __CutsceneEnd(void);
extern void __LoadFieldActors(unsigned char *p);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int who, int x, int z);
extern unsigned char *__MapActor_GetActor(unsigned int slot);
extern void __MapActor_SetSpeed(int who, int a, int b);
extern void __MapActor_SetBehavior(int who, unsigned char *s);
extern int __GetFlag(int id);
extern void OvlFunc_943_200c218(void);

void OvlFunc_943_200985c(void)
{
    unsigned char *p;

    __CutsceneStart();
    __LoadFieldActors(L5160);
    __WaitFrames(1);
    __MapActor_SetPos(0x14, 0, 0);
    __MapActor_SetPos(0x17, 0xee << 16, 0x2720000);
    __MapActor_SetPos(0x16, 0xcc << 16, 0x2090000);
    *(int *)(__MapActor_GetActor(0x16) + 0xc) = 0x80 << 13;
    p = __MapActor_GetActor(0x16) + 0x59;
    *p |= 0x80;
    __MapActor_SetSpeed(0x16, 0x9999, 0x4ccc);
    __MapActor_SetBehavior(0x16, gScript_943__0200c58c);
    p = __MapActor_GetActor(0x15) + 0x59;
    *p = 0x80 | *p;
    __MapActor_SetSpeed(0x15, 0xcccc, 0x6666);
    __MapActor_SetBehavior(0x15, gScript_943__0200c628);
    if (__GetFlag(0x109) != 0)
        OvlFunc_943_200c218();
    __CutsceneEnd();
}
