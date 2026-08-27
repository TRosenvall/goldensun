/* OvlFunc_943_2008a48 -- NOT MATCHING
 * OvlFunc_943_2008af0 -- the same routine for a second villager, so this park
 *                        covers two.
 *
 * Source asm: goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_a.s
 * Best screen: 2 differing of 57, streams the same length, both functions.
 *
 * BLOCKER CLASS: a pool load issued before an immediate that the ROM issues
 * after it.
 *
 *     rom    mov r2, #0 / mov r0, #0x15 / ldr r1, =0x103 / bl __MapActor_Emote
 *     ours   mov r2, #0 / ldr r1, =0x103 / mov r0, #0x15 / bl __MapActor_Emote
 *
 * Two instructions, transposed. gcc issues the pool load as early as it can and
 * the ROM does not.
 *
 * THE RETURN-TYPE LEVER DOES NOT APPLY. Batch 99 established that an
 * argument-move rotation is usually decided by whether the callee is declared
 * `void` or `int` (see src/rom_a1000/rom_a47b4_a_b.c). Declaring
 * __MapActor_Emote `int` changes nothing here, and neither does deleting its
 * declaration entirely. So a rotation involving a POOL LOAD has a different
 * cause from one involving two register moves -- worth knowing before reaching
 * for the lever on sight.
 *
 * ALSO TRIED, all identical at 2: naming 0x103 as an `int` local assigned
 * inside the else-block; -fno-rerun-cse-after-loop; -fno-schedule-insns.
 * -fno-schedule-insns2 is WORSE (8 of 57).
 *
 * Everything else matches, including the `__Random() * 0x5a >> 16` scaling
 * (second operand becomes the multiply's destination, batch 96's rule) and the
 * three-way flag ladder.
 */
struct A { unsigned char pad00[0x64]; unsigned short f64; };

extern unsigned char gScript_943__0200c4d8[];
extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int arg);
extern unsigned int __Random(void);
extern void __Func_80925cc(int a, int b);
extern void __Func_809259c(int a, int b);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);

void OvlFunc_943_2008a48(void)
{
    struct A *a;

    __CutsceneStart();
    if (__GetFlag(0x925)) {
        __MessageID(0x1e08);
        __ActorMessage(0x15, 0);
    } else if (__GetFlag(0x922)) {
        __Func_80925cc(0x15, 2);
        __MessageID(0x1d6f);
        __ActorMessage(0x15, 0);
        a = __MapActor_GetActor(0x15);
        a->f64 = (__Random() * 0x5a >> 16) + 0x3c;
        __MapActor_SetBehavior(0x15, gScript_943__0200c4d8);
    } else {
        __MapActor_Emote(0x15, 0x103, 0);
        __Func_809259c(0x15, 3);
        __MessageID(0x1d36);
        __ActorMessage(0x15, 0);
    }
    __CutsceneEnd();
}

void OvlFunc_943_2008af0(void)
{
    struct A *a;

    __CutsceneStart();
    if (__GetFlag(0x925)) {
        __MessageID(0x1e09);
        __ActorMessage(0x18, 0);
    } else if (__GetFlag(0x922)) {
        __Func_80925cc(0x18, 2);
        __MessageID(0x1d70);
        __ActorMessage(0x18, 0);
        a = __MapActor_GetActor(0x18);
        a->f64 = (__Random() * 0x5a >> 16) + 0x3c;
        __MapActor_SetBehavior(0x18, gScript_943__0200c4d8);
    } else {
        __MapActor_Emote(0x18, 0x103, 0);
        __Func_809259c(0x18, 3);
        __MessageID(0x1d37);
        __ActorMessage(0x18, 0);
    }
    __CutsceneEnd();
}
