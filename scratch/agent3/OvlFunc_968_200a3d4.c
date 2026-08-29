/* OvlFunc_968_200a3d4 -- MATCHES on the default flags (and unchanged under
 * -fno-rerun-cse-after-loop).  ref: asm/overlays/rom_7f2f14/ovl_30_c_c_a_a_a.s
 * tryc.py: OK (75 lines).  Matched first try.
 *
 * Worth keeping: `unsigned int i` (the ROM's loop test is `bls`, not `ble`),
 * and `c = 0;` before the loop -- the ROM's `mov r7, #0` is a real source
 * initialiser, since c is only assigned on a path the loop can skip entirely
 * and is dereferenced after it.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __PlaySound(int id);
extern void OvlFunc_968_2008b08(int a);
extern void __CutsceneWait(int n);

void OvlFunc_968_200a3d4(int arg)
{
    unsigned char *b;
    unsigned char *c;
    unsigned char *p;
    int best;
    unsigned int i;
    int slot;
    int y;

    best = 0xffb00000;
    c = 0;
    for (i = 0; i <= 5; i++) {
        slot = i + 8;
        if (slot == arg)
            continue;
        b = __MapActor_GetActor(slot);
        c = __MapActor_GetActor(arg);
        if (*(int *)(b + 8) >> 20 != *(int *)(c + 8) >> 20)
            continue;
        if (*(int *)(b + 0x10) >> 20 != *(int *)(c + 0x10) >> 20)
            continue;
        y = *(int *)(b + 0xc) + (0x80 << 13);
        if (best <= y) {
            p = c + 0x64;
            *(short *)p = slot;
            best = y;
        }
    }
    __MapActor_SetSpeed(arg, 0x80 << 11, 0x80 << 10);
    __Actor_TravelTo(c, *(int *)(c + 8), best, *(int *)(c + 0x10));
    __MapActor_WaitMovement(arg);
    __PlaySound(0xbc);
    OvlFunc_968_2008b08(arg);
    __CutsceneWait(0x1e);
}
