// fakematch
/* OvlFunc_932_200af10  --  0x0200af10
 *
 * Was the whole of goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_c.s;
 * split_s.py confirmed one function and no data tail.
 *
 * Eighty-one instructions of straight-line cutscene. Two levers, both of which
 * this tree already knew and both of which were found by reading the residue
 * rather than guessing.
 *
 * A CONSTANT USED TWICE IN TWO DIFFERENT ARGUMENT REGISTERS still gets cached.
 * `0x9999` is __Func_80933d4's first argument and __MapActor_SetSpeed's second,
 * so it is r0 at one site and r1 at the other -- and gcc still hoists it into a
 * callee-saved register, widening `push {lr}` to `push {r5, lr}` and putting 83
 * of 83 out of step. Anchoring the FIRST call's arguments is enough; the second
 * was already pinned. The rematerialisation lever is usually described for a
 * value reused in the SAME register, and this is the reminder that the register
 * does not matter -- only that the value is reused at all.
 *
 * A POOLED CONSTANT THAT FITS AN 8-BIT IMMEDIATE IS A LINKER SYMBOL. The last
 * instruction differing was `ldr r0, =0x56` against our `mov r0, #0x56`. 0x56
 * fits three times over, so cost cannot explain the pool -- and
 * src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_a_c_c_b.c in this same overlay
 * calls the same helper as `__Func_8091f90((int) (&_AREA_51), 0x63)`, with a
 * note pointing at a third file "for why _AREA_51 has to be a symbol rather
 * than the number". `area.sym` has `_AREA_56 = 0x56`, and spelling it that way
 * is exact.
 *
 * WHEN A ROM POOLS A CONSTANT AN IMMEDIATE WOULD HOLD, LOOK FOR A SYMBOL WITH
 * THAT VALUE before treating it as a gcc quirk. The .sym files are the place to
 * look and the siblings in the overlay will usually show the idiom.
 *
 * The gState poke is the one-variable chain: r2 carries the 0x22b offset and
 * then the stored 3, with the base advanced in place -- two pins, one register
 * each, spelled as the ROM has it.
 */
extern unsigned char gState[];
extern int _AREA_56;
extern void OvlFunc_932_200aeec(void);

extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __StartTask(void (*f)(void), int n);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_8091eb0(int a, int b);
extern void __Func_8091f90(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_932_200af10(void)
{
    __CutsceneStart();
    { PIN2; q0 = 0x9999; q1 = 0x1333; __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0xa4; q1 = 1; q2 = 0xae; q3 = 1; q0 <<= 17; q1 = -q1; q2 <<= 15;
      __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q0 = 0; q1 = 0x9999; q2 = 0x4ccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xa4; q2 = 0x74; q1 <<= 1; q0 = 0; __Func_80921c4(q0, q1, q2); }
    __PlaySound(0x94);
    {
        register void (*q0)(void) __asm__("r0");
        register int q1 __asm__("r1");
        q1 = 0xc8; q1 <<= 4; q0 = OvlFunc_932_200aeec;
        __StartTask(q0, q1);
    }
    { PIN3; q0 = 0x80; q1 = 0x80; q2 = 0x80; q0 <<= 9; q1 <<= 9; q2 <<= 9;
      __Func_8012330(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0x1999; q2 = 0xccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0xccc; q0 = 9; q1 = 0x1999; __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(8, 2);
    { PIN3; q1 = 0xa4; q0 = 8; q1 <<= 1; q2 = 0x68;
      __MapActor_TravelTo(q0, q1, q2); }
    { PIN3; q1 = 0xa4; q1 <<= 1; q2 = 0x6c; q0 = 9;
      __MapActor_TravelTo(q0, q1, q2); }
    __CutsceneWait(0x3c);
    { PIN3; q1 = 0x80; q2 = 0; q0 = 0; q1 <<= 1; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 2; q0 = 0; __Func_809259c(q0, q1); }
    __MapActor_WaitMovement(8);
    {
        register unsigned char *g __asm__("r3");
        register int v __asm__("r2");
        g = gState;
        v = 0x22b;
        g += v;
        v = 3;
        *g = v;
    }
    { PIN2; q0 = (int)(&_AREA_56); q1 = 0x63; __Func_8091f90(q0, q1); }
    __Func_8091eb0(0x35, 3);
}
