/* OvlFunc_943_2008a48  --  0x02008a48
 * OvlFunc_943_2008af0  --  0x02008af0
 *
 * The .s held only these two and no data, so the whole file converts and the
 * .o only changes name in goldensun/overlays/rom_7c7b9c/overlay.ld.
 *
 * The same villager routine for two villagers: a three-way flag ladder that
 * picks one of three greetings, with a randomised idle delay on the middle
 * branch.
 *
 * PARKED SINCE BATCH 96 ON A POOL LOAD ISSUED TOO EARLY:
 *
 *     rom    mov r2, #0 / mov r0, #0x15 / ldr r1, =0x103 / bl __MapActor_Emote
 *     ours   mov r2, #0 / ldr r1, =0x103 / mov r0, #0x15 / bl __MapActor_Emote
 *
 * THE BASIC-BLOCK LEVER CLOSES IT, and the park's own record of what failed is
 * exactly why: it tried "naming 0x103 as an `int` local assigned inside the
 * else-block". That is the SAME block as the use, which is the case the lever's
 * write-up says keeps the value in a register. Assigned at the top of the
 * function -- a block that dominates the else and is not it -- gcc
 * rematerialises the pool load at the call and it lands in the ROM's position.
 *
 * That is worth recording against docs/elevation.md's "pool loads come first"
 * section, which lists this shape as reachable only by register pinning with
 * inline asm. The lever reaches it whenever there is a boundary to use.
 *
 * The `__Random() * 0x5a >> 16` scaling is unchanged from the park (the second
 * operand becomes the multiply's destination, batch 96's rule).
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
    int id;

    id = 0x103;
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
        __MapActor_Emote(0x15, id, 0);
        __Func_809259c(0x15, 3);
        __MessageID(0x1d36);
        __ActorMessage(0x15, 0);
    }
    __CutsceneEnd();
}

void OvlFunc_943_2008af0(void)
{
    struct A *a;
    int id;

    id = 0x103;
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
        __MapActor_Emote(0x18, id, 0);
        __Func_809259c(0x18, 3);
        __MessageID(0x1d37);
        __ActorMessage(0x18, 0);
    }
    __CutsceneEnd();
}
