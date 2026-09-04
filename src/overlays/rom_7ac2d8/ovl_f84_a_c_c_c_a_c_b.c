// fakematch
/* OvlFunc_924_2009340  --  0x02009340
 *
 * Cut out of goldensun/asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_a_c.s.
 *
 * Eighty-six instructions, exact on the first screen. One crossed
 * __MapActor_SetSpeed fill takes a single barrier, placed from the listing
 * before the first compile because `tools/crossed.py` flagged it during
 * selection.
 *
 * THE THREE-WAY BRANCH SHARES A CROSS-JUMPED TAIL and that needed no help. The
 * ROM sets r0/r1/r2 three different ways in three arms and then jumps to one
 * common `lsl r2, #2 / bl __MapActor_TravelTo`. Written as three separate
 * calls, each with its own pinned fill and each spelling the shift as
 * `q2 << 2` at the call, gcc cross-jumps the shared tail itself. The temptation
 * is to hoist the call after the if-chain and pass variables; that would not
 * match, because the ROM's arms differ in WHICH register is filled first and a
 * single call site cannot express three orders.
 *
 * The two byte flags on the actor are the two forms already on file: the `&=`
 * site takes the direct byte spelling because the ROM leaves the constant in
 * the destination, and the `|=` site takes the four-statement accumulate form
 * because it does not. Both subscript __MapActor_GetActor's result directly so
 * the address dies in the statement and stays in r0.
 */
extern unsigned char *iwram_3001ebc;
extern void OvlFunc_924_200ba64(void);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __StartTask(void (*f)(void), int n);
extern void __StopTask(void (*f)(void));
extern void __Func_8092158(int a, int b, int c);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_924_2009340(void)
{
    unsigned char *p;
    int t;

    p = iwram_3001ebc;
    __CutsceneStart();
    {
        register void (*q0)(void) __asm__("r0");
        register int q1 __asm__("r1");
        q1 = 0xc8; q1 <<= 4; q0 = OvlFunc_924_200ba64;
        __StartTask(q0, q1);
    }
    {
        PIN3;
        q1 = 0xa0; __asm__ volatile ("" : : "r" (q1));
        q2 = 0xa0; q2 <<= 9; q0 = 0; q1 <<= 10;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    { PIN2; q1 = 1; q0 = 0; __MapActor_SetAnim(q0, q1); }
    __MapActor_GetActor(0)[0x5a] &= 0xfe;
    __PlaySound(0xe4);
    p += 0xb6 << 1;
    t = *(short *)p;
    if (t == 2) {
        PIN3; q2 = 0x9a; q0 = 0; q1 = 0xe8;
        __MapActor_TravelTo(q0, q1, q2 << 2);
    } else if (t == 3) {
        PIN3; q1 = 0xb4; q2 = 0xb6; q0 = 0; q1 <<= 1;
        __MapActor_TravelTo(q0, q1, q2 << 2);
    } else if (t == 4) {
        PIN3; q2 = 0xc6; q0 = 0; q1 = 0xf8;
        __MapActor_TravelTo(q0, q1, q2 << 2);
    } else {
        { PIN3; q1 = 0xae; q2 = 0x94; q0 = 0; q1 <<= 2; q2 <<= 2;
          __Func_8092158(q0, q1, q2); }
        { PIN3; q1 = 0xae; q2 = 0x96; q0 = 0; q1 <<= 2; q2 <<= 2;
          __MapActor_TravelTo(q0, q1, q2); }
        __CutsceneWait(0x1e);
    }
    __MapActor_WaitMovement(0);
    {
        unsigned char *q = __MapActor_GetActor(0);
        int u, k;
        u = q[0x5a]; k = 1; k |= u;
        q[0x5a] = k;
    }
    __StopTask(OvlFunc_924_200ba64);
    __CutsceneEnd();
}
