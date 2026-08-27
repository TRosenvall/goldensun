/* OvlFunc_945_2009804  --  0x02009804
 *
 * Cut out of goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_a.s.
 *
 * The shared body of the "follow me" prompt: three parameters -- the actor
 * slot, the message id and the flag to set -- so one routine serves several
 * villagers. Accepting walks the actor to the player and sets two flags;
 * declining bumps a counter instead. Matched on the first screen.
 *
 * The three parameters are held in r6, r5 and r7 across the whole function,
 * which is what a three-argument prototype gives without any lever.
 *
 * Two callees are declared `int` and the rest `void`, read off whether the ROM
 * emits `mov r0` first or last: __Func_8092c40 and __MapActor_TravelTo are
 * `int`, __MapActor_SetAnim and __MapActor_SetPos next to them are `void`.
 */
extern char *iwram_3001ebc;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern char *__MapActor_GetActor(int slot);
extern int __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __MapActor_SetAnim(int slot, int n);
extern int __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void OvlFunc_945_200c86c(int slot);

void OvlFunc_945_2009804(int slot, int msg, int flag)
{
    char *a;
    unsigned short *q;

    __CutsceneStart();
    __MessageID(msg);
    __Func_8092c40(slot, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        OvlFunc_945_200c86c(slot);
        __MapActor_SetAnim(slot, 2);
        a = __MapActor_GetActor(0);
        if (a != 0)
            __MapActor_TravelTo(slot, *(short *)(a + 0xa), *(short *)(a + 0x12));
        __MapActor_WaitMovement(slot);
        __MapActor_SetPos(slot, 0, 0);
        __SetFlag(0xc0 << 2);
        __SetFlag(flag);
    } else {
        q = (unsigned short *)(iwram_3001ebc + (0xec << 1));
        *q += 1;
        OvlFunc_945_200c86c(slot);
    }
    __CutsceneEnd();
}
