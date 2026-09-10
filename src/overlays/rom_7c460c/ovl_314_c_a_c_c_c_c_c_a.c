// fakematch
/* ovl_314_c_a_c_c_c_c_c_a.c  --  OvlFunc_939_200931c + OvlFunc_939_20095bc
 *   [the WHOLE of asm/overlays/rom_7c460c/ovl_314_c_a_c_c_c_c_c_a.s -- both
 *    `.thumb_func_start`s, so NO SPLIT is required and
 *    overlays/rom_7c460c/overlay.ld:59 stays VERBATIM.  Its file-mate
 *    src/overlays/rom_7c460c/ovl_314_c_a_c_c_c_c_c_b.c (line 60) is untouched.]
 *
 *   OK WHOLE OBJECT -- 844 bytes, 325 encodings and 94 relocations identical
 *   (per function, objcmp --func: 200931c 672 bytes / 262 encodings / 71
 *    relocs; 20095bc 172 bytes / 63 encodings / 23 relocs.  Each measured 3x.)
 *
 * Two straight-line cutscene scripts, 257 + 62 instructions.  FAKEMATCH:
 * matched with the register-pin idiom, so both names go in fakematch.txt.
 *
 * SAME TWO LEVERS AS THE SIBLING ovl_314_c_c_a_a_c_c_c.c -- 18 ASCENDING pins,
 * part fill-order and part eviction -- plus one that is new here.
 *
 * A PUSH WE HAVE AND THE ROM LACKS IS THE COMMONING TELL, AND IT FIRED IN THE
 * SMALL FUNCTION.  Bare, OvlFunc_939_20095bc opens `push {r5, lr}` against the
 * ROM's `push {lr}`: 0x120 (`0x90 << 1`) is passed to __Func_809218c twice, gcc
 * commons it into r5 and pays a callee-saved save for it.  Ascending PIN3 at
 * both sites rebuilds it and the mask collapses to the ROM's.  That is the
 * whole of that function -- 19 differing to 0 with two pins.
 *
 * BOTH `|=` BYTE SITES NEED THE AGGREGATE MEMBER, AND THEY ARE WORTH DIFFERENT
 * AMOUNTS.  The batch-240 discriminator ("AGGREGATE MEMBER REFERENCE versus
 * dereferenced pointer" on `->fNN |= k`) holds twice here, isolated on the
 * otherwise-finished file with everything else held fixed:
 *
 *   both f23 and f55 through `struct Actor *` .......... 0 differing
 *   f23 struct, f55 as `p[0x55] |= 2` .................. 2 differing
 *   f55 struct, f23 as `p[0x23] |= 1` .................. 5 differing
 *   neither -- both through `unsigned char *` .......... 7 differing
 *
 * The f23 pair is the recorded shape: the ROM has `orr r3, r5` at the first
 * site and `orr r5, r3` at the second, and only the struct member gets the
 * second one's tie fixed up.  f55 is a SINGLE site and still needs it -- there
 * the ROM puts the MASK in the destination (`ldrb r2 / mov r3, #2 / orr r3, r2`)
 * and the pointer spelling swaps the two registers.  So the member lever is not
 * only about the second member of a commoned-mask pair; a lone `|=` can want it
 * too.  THE STRUCT IS FOR f23 AND f55 ONLY: +0xc, +0x28 and +0x2c stay as
 * `*(int *)(p + N)` casts off the `unsigned char *` return, per the sibling's
 * "THE STRUCT IS FOR f5a ONLY" warning.
 *
 * WHAT THE PIN SET IS WORTH: all 18 pins removed is 270 differing and the
 * object is 4 bytes LONG (327 encodings against 325) -- three shifted-constant
 * pairs and the two pooled speed words all get commoned into r5/r6.
 *
 * MECHANISM CONTROL: the finished file compiled -fno-schedule-insns2 is 90
 * differing, same reading as the sibling.
 *
 * THE TWO POLL LOOPS FALL OUT OF `while`, NOT `do/while`.  `__CutsceneWait(1);`
 * followed by `while (actor->fc != 0) __CutsceneWait(1);` reproduces the ROM's
 * `bl / b .Ltest / .Lbody: bl / .Ltest: ldr / cmp / bne` verbatim, including
 * the pre-loop call.  No barrier and no pin anywhere near them.
 *
 * THE TASK-FUNCTION ADDRESS NEEDS NO LEVER.  The ROM loads
 * OvlFunc_939_20092a4 once into r6 and copies it (`mov r0, r6`) at the first
 * __StartTask/__StopTask pair, then RE-LOADS it with `ldr r0, =...` at the
 * second pair on the far side of a loop.  Four plain references to the symbol
 * reproduce that split exactly; this is the recorded "a symbol address the ROM
 * loads once and copies" failure mode NOT firing, because the loop between the
 * pairs is what ends the first live range.
 *
 * PIN MINIMISATION ran to a fixpoint FROM BOTH ENDS and the two directions
 * produce BYTE-IDENTICAL sources: 18 of 23 required.  The five dropped are the
 * second of the two 0x120 __Func_809218c pins, __MapActor_TravelTo, the second
 * __MapActor_Surprise, and the two constant-only __Func_809218c(8/9, N, 0xc8)
 * sites -- i.e. exactly the sites whose constants need no shift and no pool.
 *
 * Harness: scratch_elev/b256/zero/ -- see the sibling's header.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __SetCameraTarget(int a, int b);
extern void __MapTransitionIn(void);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern int __StartTask(void (*fn)(void), int n);
extern void __StopTask(void (*fn)(void));
extern void OvlFunc_939_20092a4(void);
extern void __FieldMove(int a);
extern void __Func_8096fb0(int a, int b);
extern void __Func_80970f8(int a, int b);
extern void __Func_809728c(void);
extern void __Func_8097174(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

struct Actor {
    unsigned char pad00[0x23];
    unsigned char f23;
    unsigned char pad24[0x31];
    unsigned char f55;
};

void OvlFunc_939_200931c(void)
{
    int w;

    __CutsceneStart();
    { PIN3; q0 = 0x0; q1 = 0xa0 << 16; q2 = 0x80 << 16; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0x98 << 16; q2 = 0xe0 << 15; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0xa8 << 16; q2 = 0xe0 << 15; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0x80 << 7; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x11; q1 = 0xc0 << 6; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x12; q1 = 0xa0 << 7; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    __SetCameraTarget(0x0, 0x0);
    __MapTransitionIn();
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0x0; q1 = 0x1cccc; q2 = 0xe666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0x1cccc; q2 = 0xe666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0x1cccc; q2 = 0xe666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0x98; q2 = 0x90 << 1; __Func_809218c(q0, q1, q2); }
    __Func_809218c(0x9, 0xa8, 0x90 << 1);
    __MapActor_SetAnim(0x0, 0x4);
    __MapActor_TravelTo(0x0, 0xa0, 0x94 << 1);
    __StartTask(OvlFunc_939_20092a4, 0xc8 << 4);
    __CutsceneWait(0x1);
    __PlaySound(0x79);
    __CutsceneWait(0x14);
    __Func_8092b08(0x8, 0x3);
    __Func_8092b08(0x9, 0x3);
    __PlaySound(0x79);
    __CutsceneWait(0x1e);
    ((struct Actor *)__MapActor_GetActor(0x8))->f23 |= 1;
    ((struct Actor *)__MapActor_GetActor(0x9))->f23 |= 1;
    __MapActor_SetAnim(0x0, 0x4);
    __PlaySound(0x79);
    __MapActor_WaitMovement(0x0);
    __MapActor_SetAnim(0x8, 0x1);
    __MapActor_SetAnim(0x9, 0x1);
    __StopTask(OvlFunc_939_20092a4);
    ((struct Actor *)__MapActor_GetActor(0x0))->f55 |= 2;
    w = 0xc0 << 11;
    *(int *)(__MapActor_GetActor(0x0) + 0x28) = w;
    *(int *)(__MapActor_GetActor(0x0) + 0x2c) = w;
    __CutsceneWait(0x1);
    while (*(int *)(__MapActor_GetActor(0x0) + 0xc) != 0)
        __CutsceneWait(0x1);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 8; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0x0, 0x13);
    __PlaySound(0x7f);
    { PIN2; q0 = 0x0; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    __StartTask(OvlFunc_939_20092a4, 0xc8 << 4);
    __CutsceneWait(0x2);
    *(int *)(__MapActor_GetActor(0x0) + 0x28) = 0xc0 << 10;
    __CutsceneWait(0x1);
    while (*(int *)(__MapActor_GetActor(0x0) + 0xc) != 0)
        __CutsceneWait(0x1);
    __MapActor_Surprise(0x0, 0x81 << 1);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0x0, 0x1);
    __StopTask(OvlFunc_939_20092a4);
    __CutsceneWait(0x32);
    __MessageID(0x2410);
    __ActorMessage(0x8, 0x0);
    ((struct Actor *)__MapActor_GetActor(0x8))->f23 |= 1;
    ((struct Actor *)__MapActor_GetActor(0x9))->f23 |= 1;
    { PIN3; q0 = 0x8; q1 = 0x80 << 9; q2 = 0x80 << 8; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0x80 << 9; q2 = 0x80 << 8; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_809218c(0x8, 0x90, 0xc8);
    __Func_809218c(0x9, 0xb0, 0xc8);
    __MapActor_WaitMovement(0x8);
    __MapActor_WaitMovement(0x9);
    __MapActor_SetAnim(0x8, 0x1);
    __MapActor_SetAnim(0x9, 0x1);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0x8; q1 = 0xc0 << 6; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0xa0 << 7; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    __CutsceneEnd();
}

void OvlFunc_939_20095bc(void)
{
    __CutsceneStart();
    __MapTransitionIn();
    __Func_809218c(0x0, 0x98, 0xa8);
    __MapActor_WaitMovement(0x0);
    __CutsceneWait(0x14);
    __Func_8096fb0(0x92, 0x1);
    __Func_80970f8(0x0, 0x0);
    __Func_809728c();
    __FieldMove(0x1);
    __Func_8097174();
    __Func_809218c(0x0, 0x90, 0xb8);
    __MapActor_WaitMovement(0x0);
    __Func_809218c(0x0, 0x58, 0xb8);
    __MapActor_WaitMovement(0x0);
    __Func_809218c(0x0, 0x58, 0xc8);
    __MapActor_WaitMovement(0x0);
    __Func_809218c(0x0, 0x48, 0xc8);
    __MapActor_WaitMovement(0x0);
    { PIN3; q0 = 0x0; q1 = 0x48; q2 = 0x90 << 1; __Func_809218c(q0, q1, q2); }
    __MapActor_WaitMovement(0x0);
    { PIN3; q0 = 0x0; q1 = 0x58; q2 = 0x90 << 1; __Func_809218c(q0, q1, q2); }
    __MapActor_WaitMovement(0x0);
    __CutsceneEnd();
}
