// fakematch
/* OvlFunc_944_20084b0  --  0x020084b0
 * [asm/overlays/rom_7ca63c/ovl_30_c_c_a_c_c_a_c.s, second of three functions]
 *
 * 66 instructions: transition in, walk actor 8 through a short scripted route,
 * say one line, transition out. Byte-exact: 180 bytes, 70 encodings and 18
 * relocations identical.
 *
 * THIS FUNCTION WAS INVISIBLE UNTIL BATCH 222. tools/filtered.py hard-rejected
 * it as the duplicate-constant CSE class, on the strength of a park that called
 * itself the canonical specimen of that wall and closed its search after
 * thirteen flags. That park is now byte-exact -- all thirteen measurements
 * assumed the lever had to be a FLAG, and none tried a hard register. The
 * reject is a warning column now, and this is the first candidate recovered by
 * that change to be landed.
 *
 * It is also a clean confirmation of the prologue rule. The ROM's prologue is
 * `push {lr}` ALONE: no callee-saved register is spent anywhere, so every
 * repeated constant is rebuilt at every use, and only pins reach it. `0xd0<<8`
 * appears at two of the four __Func_8092adc sites and `0xa4` at both
 * __Func_80921c4 sites; written plainly the function is 67 lines against 66
 * with 28 differing.
 *
 * SEVEN PINS, MINIMISED AS A SET. Nine sites reached exact; stripping each
 * individually found two inert (__MapActor_Jump and __Func_809259c), and --
 * because individually-inert pins are not always jointly removable -- both were
 * then removed TOGETHER and re-measured, which is still byte-exact. The other
 * seven cost 2 to 3 differing lines each.
 *
 * THE LAST THREE LINES WERE A POOL LOAD IN AN ARGUMENT. __Func_80921c4's third
 * argument is the pooled 0x141, and a call whose arguments include a pool load
 * issues that load FIRST unless the fill is pinned; the ROM wants
 * `mov r0 / mov r1 / ldr r2`. Pinning that one site in the ROM's order closed
 * it. Note 0x141 is a LITERAL, not a symbol -- the reference object's
 * relocations are the eighteen calls and nothing else.
 *
 * 0x202 is derived from the offset register: the ROM builds 0x1c0 in r2 for
 * the address and then `add r2, #0x42` for the stored value. Writing the store
 * plainly gets that for free.
 *
 * No wildcard captures this object -- the single Makefile rule naming
 * rom_7ca63c is an explicit rule for a different one -- so the tree default
 * -O2 applies.
 */
extern char *iwram_3001ebc;
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MessageID(int id);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8091e9c(int a);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_944_20084b0(void)
{
    *(int *)(iwram_3001ebc + 0x1c0) = 0x202;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 8; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0xa4; q2 = 0x141; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xd0; q0 = 8; q1 <<= 8; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xb0; q0 = 8; q1 <<= 8; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xd0; q0 = 8; q1 <<= 8; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q0 = 8; q1 <<= 6; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN3; q2 = 0xa7; q0 = 8; q1 = 0xa4; q2 <<= 1; __Func_80921c4(q0, q1, q2); }
    __MapActor_Jump(8, 4, 0x28);
    __Func_809259c(8, 2);
    __MessageID(0x1e3a);
    __Func_8093040(8, 0, 0x14);
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0xa);
}
