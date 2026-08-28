/* Cluster OvlFunc_883_20090d8..OvlFunc_883_20090d8 extracted from goldensun/asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c_a.s.
 *
 * Total .text for this TU = 256 bytes (= 0x0100).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c_a_a.o and
 * asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c_a_c.o in
 * goldensun/overlays/rom_780898/overlay.ld.
 *
 * Borrows the camera: copies three words out of actor 0, points the iwram slot
 * at the local copy, walks its third word up over forty frames and back down,
 * then restores the slot.  `buf` is a LOCAL whose address escapes into *p --
 * that is what the ROM's `sub sp, #0xc` and `str r7, [r3]` are doing.
 *
 * q1, q2 AND t1 ARE NAMED IN THE DOMINATING BLOCK -- before the early-return
 * `if`, not beside their calls.  Two argument-setup sites wanted opposite
 * things and both are fixed by naming, in the two directions recorded in
 * docs/elevation.md:
 *
 *   __MapActor_SetSpeed  the ROM puts `mov r0, #0` BEFORE the split build's
 *                        shift; naming the two shifted arguments places it.
 *   __Func_80921c4       the ROM emits the POOLED third argument LAST, after
 *                        r0 and r1; naming it pushes the pool load later.
 *
 * Inline, those two sites are the whole difference -- 5 of 100 lines, and the
 * only ones.  Do not fold the locals back into the argument lists.
 */
extern int *iwram_3001e70;

extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern int *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_801776c(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_800fe9c(void);

void OvlFunc_883_20090d8(void)
{
    int buf[3];
    int *p;
    int save;
    int *a;
    int i;
    int q1, q2, t1;

    q1 = 0x80 << 9;
    q2 = 0x80 << 8;
    t1 = 0x2e5;
    if (__GetFlag(0x808))
        return;
    p = iwram_3001e70;
    __CutsceneStart();
    __MapActor_SetSpeed(0, q1, q2);
    __MapActor_SetAnim(0, 1);
    __CutsceneWait(2);
    __MessageID(0xf4d);
    __Func_8093040(0xf, 0, 2);
    __Func_8093040(0x10, 0, 2);
    a = __MapActor_GetActor(0);
    buf[0] = a[2];
    buf[1] = a[3];
    save = *p;
    buf[2] = a[4];
    *p = (int)buf;
    for (i = 0; i != 0x28; i++) {
        buf[2] += 0x80 << 10;
        __CutsceneWait(1);
        __Func_800fe9c();
    }
    __CutsceneWait(0x3c);
    __Func_801776c(0xf4f, 1);
    __CutsceneWait(6);
    for (i = 0; i != 0x28; i++) {
        buf[2] += 0xfffe0000;
        __CutsceneWait(1);
        __Func_800fe9c();
    }
    *p = save;
    __CutsceneWait(0x3c);
    __Func_80921c4(0, 0x46, t1);
    __CutsceneEnd();
}
