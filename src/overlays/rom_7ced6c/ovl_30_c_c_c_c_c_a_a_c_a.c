/* OvlFunc_946_2009774  --  0x02009774, was
 * asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_a_a_c_a.s (this function alone), so
 * it converts whole. Matched from scratch.
 *
 * `g = gState; *(int *)(g + (0xfa << 1))` gives the ROM's unfolded
 * `mov r2,#0xfa / lsl / add`, and x and z must be computed into locals before
 * the two speed-field stores.
 */
extern unsigned char gState[];
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __PlaySound(int id);
extern void __Actor_WaitMovement(unsigned char *a);

void OvlFunc_946_2009774(int slot, int dx, int dy)
{
    unsigned char *a;
    unsigned char *b;
    unsigned char *g;
    int x, z;

    g = gState;
    a = __MapActor_GetActor(*(int *)(g + (0xfa << 1)));
    b = __MapActor_GetActor(slot);
    __CutsceneStart();
    x = ((*(int *)(a + 8) + (dx << 16)) & 0xfff00000) + 0x80000;
    z = ((*(int *)(a + 0x10) + (dy << 16)) & 0xfff00000) + 0x80000;
    *(int *)(a + 0x30) = 0x10000;
    *(int *)(a + 0x34) = 0x8000;
    __Actor_TravelTo(a, x, *(int *)(a + 0xc), z);
    __Actor_SetAnim(a, 0x1b);
    x = ((*(int *)(b + 8) + (dx << 16)) & 0xfff00000) + 0x80000;
    z = ((*(int *)(b + 0x10) + (dy << 16)) & 0xfff00000) + 0x80000;
    *(int *)(b + 0x30) = 0x10000;
    *(int *)(b + 0x34) = 0x8000;
    __Actor_TravelTo(b, x, *(int *)(b + 0xc), z);
    if (dx < 0 || dy < 0)
        __Actor_SetAnim(b, 4);
    else
        __Actor_SetAnim(b, 3);
    __PlaySound(0xe2);
    __Actor_WaitMovement(a);
    __PlaySound(0x120);
    __CutsceneEnd();
}
