/* Cluster OvlFunc_945_200bf94..OvlFunc_945_200bf94 extracted from goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_c.s.
 *
 * Total .text for this TU = 340 bytes (= 0x0154).
 * Slotted after asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_c_a.o in
 * goldensun/overlays/rom_7cb2c0/overlay.ld.  It was the LAST of eight functions
 * in that .s, so there is no _c part.
 *
 * A ledge-climb cutscene: two identical animate/sound/wait cycles, then a state
 * write and a five-way flag ladder choosing which follow-up runs.
 *
 * EXACT ON THE FIRST SCREEN, with nothing needed.  Two things reproduced on
 * their own that have needed levers elsewhere and are worth noting as the
 * no-lever baseline:
 *
 *   - The stored 0x203 is DERIVED from the offset register.  The ROM builds
 *     0x1c0 for the address and then `add r2, #0x43` to make the value, and
 *     writing the plain literal 0x203 produces exactly that -- the batch-123
 *     derived-constant rule, working without help.
 *   - `__Func_80933f8(0xdc << 17, -1, 0xb0 << 16, 1)` has three constants that
 *     need building and a negation, and the interleaved `mov`/`lsl`/`neg`
 *     ordering came out right inline.  No naming was required.
 */
extern char *iwram_3001ebc;

extern int __GetFlag(int id);
extern void __PlaySound(int id);
extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8091e9c(int n);
extern void OvlFunc_945_200c670(int n);
extern void OvlFunc_945_200c8e8(int a, int b, int c);

void OvlFunc_945_200bf94(void)
{
    char *p;

    __CutsceneStart();
    __MapActor_SetAnim(9, 5);
    OvlFunc_945_200c8e8(0x18, 1, 0);
    __MapActor_SetPos(0, 0, 0);
    OvlFunc_945_200c8e8(0x11, 0, 0);
    OvlFunc_945_200c670(0);
    OvlFunc_945_200c8e8(8, 1, 0x14);
    __Func_80933d4(0x6666, 0xccc);
    __Func_80933f8(0xdc << 17, -1, 0xb0 << 16, 1);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(9, 7);
    __CutsceneWait(0x1e);
    __PlaySound(0xbc);
    __CutsceneWait(0x1e);
    OvlFunc_945_200c670(0x10);
    __CutsceneWait(0x50);
    OvlFunc_945_200c670(0);
    __CutsceneWait(0x3c);
    __MapActor_SetAnim(9, 7);
    __CutsceneWait(0x1e);
    __PlaySound(0xbc);
    __CutsceneWait(0x1e);
    OvlFunc_945_200c670(0x10);
    __CutsceneWait(0x50);
    OvlFunc_945_200c670(0);
    __CutsceneWait(0x5a);
    __PlaySound(0xbc);
    __CutsceneWait(0x1e);
    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x203;
    OvlFunc_945_200c8e8(9, 0, 0);
    if (__GetFlag(0x92b))
        __Func_8091e9c(0x14);
    else if (__GetFlag(0x92a))
        __Func_8091e9c(0x12);
    else if (__GetFlag(0x929))
        __Func_8091e9c(0x11);
    else if (__GetFlag(0x928))
        __Func_8091e9c(0x10);
    else
        __Func_8091e9c(0xd);
}
