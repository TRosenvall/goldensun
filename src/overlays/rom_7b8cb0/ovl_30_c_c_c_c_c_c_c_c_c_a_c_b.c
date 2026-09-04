// fakematch
/* OvlFunc_931_20086a4  --  0x020086a4
 *
 * Cut out of goldensun/asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_c.s.
 *
 * Clears a slot on actor 0, plays a chime, hands a stage value to a transition
 * and waits for it.
 *
 * PARKED AT 2 OF 28, AND THE PARK CALLED ITSELF A PREDICTION. Its own words:
 * "the first function PARKED BY PREDICTION rather than by discovery -- the
 * mechanism was settled earlier the same round and it said in advance that
 * this one cannot match." The residue was one instruction:
 *
 *     rom   mov r2, #0x10 / mov r1, #0x2 / neg r2, r2 / mov r0, #0
 *     ours  mov r2, #0x10 / neg r2, r2  / mov r1, #0x2 / mov r0, #0
 *
 * The ROM splits the two-instruction build of -0x10 around another argument.
 * Pinning the three argument registers and assigning them in the ROM's order
 * matches on the first screen:
 *
 *     q2 = 0x10;  q1 = 2;  q2 = -q2;  q0 = 0;
 *
 * THE PREDICTION WAS SOUND AND THE PARK WAS STILL WRONG, which is the point
 * worth keeping. It reasoned from the basic-block lever: rebuilding a value at
 * its use needs REG_BASIC_BLOCK < 0, that needs the pseudo to span more than
 * one block, and this function has no branch at all -- so it concluded
 * "unreachable in plain C". Every step of that is correct about the lever it
 * names. It is wrong as a conclusion because it was written before the pin
 * existed, and a pin does not go through basic blocks at all: it names the
 * hard register, so the placement is decided at assignment rather than by
 * liveness.
 *
 * A prediction from a mechanism is only as wide as the mechanism. This park
 * predicted correctly that the BASIC-BLOCK lever could not reach the site, and
 * then stated that as a property of the function. The same correction has now
 * been made to the dominating-branch rule (batch 193) and to the "same value"
 * rule (batch 195); this is the third time a rule about one lever has been
 * recorded as a fact about C.
 *
 * The park's other measurement stands and is worth keeping: naming the
 * constant at the top of the function is WORSE, 6 of 28, because gcc keeps it
 * live exactly as update_equiv_regs says it will.
 */

extern unsigned int iwram_3001ebc;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);
extern void __Func_8091e9c(int a);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);

void OvlFunc_931_20086a4(void)
{
    unsigned char *base;
    unsigned char *q;

    base = (unsigned char *)iwram_3001ebc;
    __CutsceneStart();
    q = (unsigned char *)__MapActor_GetActor(0) + 0x55;
    *q = 0;
    __PlaySound(0x7b);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q2 = 0x10;
        q1 = 2;
        q2 = -q2;
        q0 = 0;
        __Func_8092208(q0, q1, q2);
    }
    base += 0xb6 << 1;
    __Func_8091e9c(*(short *)base);
    __MapTransitionOut();
    __WaitMapTransition();
    __CutsceneEnd();
}
