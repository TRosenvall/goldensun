/* OvlFunc_935_2008704  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_c_a.s
 * Best screen: 6 instructions in disagreeing regions, of 24 (rom 24, ours 18).
 *
 * BLOCKER CLASS: register allocation -- a DEAD callee-saved register.
 *
 * Every instruction of the body matches exactly.  The entire difference is
 * that the ROM reserves r8, sets it to zero, and never reads it again:
 *
 *      mov r7, r8 / push {r7}        <- prologue, 2 instructions
 *      mov r3, #0x0 ... mov r8, r3   <- 2 instructions
 *      pop {r3} / mov r8, r3         <- epilogue, 2 instructions
 *
 * Six instructions, all of them bookkeeping for a value with no consumer.  The
 * loop itself -- countdown in r5, index in r6 walking up from 0x10, the OR
 * constant 2 hoisted into r7, and the read-modify-write on actor byte 0x23 --
 * is reproduced exactly by the source below.
 *
 * WHAT WAS TRIED
 *
 *  1. The do/while below, with the countdown and the index as separate locals.
 *     6 of 24, and the six are precisely the r8 traffic.
 *  2. `for (i = 0; i < 6; i++)` calling `__MapActor_GetActor(0x10 + i)`, on the
 *     theory that r8 is an induction variable that strength reduction left
 *     behind and the 0x10-based r6 is its derived form.  WORSE, 18 of 24 --
 *     gcc reshapes the whole loop and r8 still never appears.
 *
 * A dead register cannot be requested from the source directly; it has to fall
 * out of pressure the original code had and ours does not.  Nothing here
 * creates that pressure, so the remedy is probably a variable that the real
 * source uses and this reconstruction has folded away.
 */
extern unsigned char *__MapActor_GetActor(int slot);

void OvlFunc_935_2008704(void)
{
    unsigned char *p;
    int i;
    int c;
    int m;
    int t;

    i = 0x10;
    m = 2;
    c = 5;
    do {
        p = __MapActor_GetActor(i);
        p += 0x23;
        t = *p;
        c--;
        *p = t | m;
        i++;
    } while (c >= 0);
}
