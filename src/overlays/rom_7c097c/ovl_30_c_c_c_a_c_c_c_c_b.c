/* OvlFunc_936_2009f14  --  0x02009f14
 *
 * Cut out of goldensun/asm/overlays/rom_7c097c/ovl_30_c_c_c_a_c_c_c_c.s.
 * The step counter it drives, `.L5144`, stays in the sibling piece and is
 * reached with the asm-label extension -- a high-numbered label, so the
 * collision that bit OvlFunc_common1_172c in batch 106 does not apply here.
 *
 * A five-step state machine: each step waits for the previous actor's move to
 * finish, then starts the next one.
 *
 * PARKED IN BATCH 104 AT 12 OF 103, AND THE PARK HAD THE RIGHT IDEA IN THE
 * WRONG PLACE. It established that storing a literal `0` through a `short *`
 * gives `ldr r3, =0x0` -- gcc-2.96 has no immediate alternative for an HImode
 * constant -- so the ROM's `mov r3, #0` means the source's right-hand side is
 * int-typed. Correct. It then wrote `{ int zero = 0; *(short *)(...) = zero; }`
 * inside each case, which is the store's OWN basic block, and gcc coalesced the
 * four into one pseudo whose live range crosses a `bl`. Callee-saved register,
 * `push {r5, lr}`, 12 differing.
 *
 * FOUR SEPARATE `int` LOCALS DECLARED AT THE TOP OF THE FUNCTION match exactly.
 * The rule is the basic-block lever's, applied to the HImode question: the
 * int-typed value must be REMATERIALISED at the store, so its assignment has to
 * dominate the store rather than sit beside it, and each site needs its own
 * local so no pseudo is referenced more than twice.
 *
 * One local shared by all four is 83 differing of 101 -- worse than the park --
 * which is the REG_N_REFS clause biting exactly as the lever's write-up says.
 *
 * Also from the park and still required: the `__MapActor_GetActor` results must
 * NOT go through a named local. Inlined into the store expression, r0 is dead
 * after the `add` and the ROM's form appears; named, gcc keeps the pointer live
 * and copies it, ten instructions over five arms.
 */
extern int L5144 __asm__(".L5144");
extern unsigned char gScript_936__0200bec0[];
extern unsigned char gScript_936__0200bfb0[];
extern char *__MapActor_GetActor(int slot);
extern void __MapActor_SetBehavior(int slot, unsigned char *script);

void OvlFunc_936_2009f14(void)
{
    int z1 = 0;
    int z2 = 0;
    int z3 = 0;
    int z4 = 0;

    switch (L5144) {
    case 0:
        *(short *)(__MapActor_GetActor(0x15) + 0x64) = z1;
        __MapActor_SetBehavior(0x15, gScript_936__0200bec0);
        L5144++;
        break;
    case 1:
        if (*(short *)(__MapActor_GetActor(0x15) + 0x64) == 0)
            return;
        *(short *)(__MapActor_GetActor(0x14) + 0x64) = z2;
        __MapActor_SetBehavior(0x14, gScript_936__0200bfb0);
        L5144++;
        break;
    case 2:
        if (*(short *)(__MapActor_GetActor(0x14) + 0x64) == 0)
            return;
        *(short *)(__MapActor_GetActor(0x14) + 0x64) = z3;
        __MapActor_SetBehavior(0x14, gScript_936__0200bec0);
        L5144++;
        break;
    case 3:
        if (*(short *)(__MapActor_GetActor(0x14) + 0x64) == 0)
            return;
        *(short *)(__MapActor_GetActor(0x15) + 0x64) = z4;
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
