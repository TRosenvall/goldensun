/* OvlFunc_971_200906c  --  0x0200906c
 *
 * Cut out of goldensun/asm/overlays/rom_7fb4a8/ovl_30_c_a_a_a_c.s.
 *
 * UNPARKS src/non_matching/overlays/200906c.c.
 *
 * That park's blocker was "three NEARBY constants chosen by a switch get built
 * by add/sub from one pool load where the ROM pools each separately". Symbol
 * addresses cannot be reached from one another by an immediate, so spelling the
 * three message ids `(int)&_MSG_297f` and friends gives each arm its own pool
 * load -- the ROM's shape.
 *
 * Two other fixes the park did not have: the function returns `int` (the ROM's
 * `pop {r1} / bx r1`), and __Func_8092c40 is declared `int`.
 *
 * The three ids were added to message.sym in their own commit.
 *
 * Drafted by a parallel screening agent; re-screened here before wiring.
 */
typedef struct {
    unsigned char pad00[0x1f4];
    int f1f4;
} GlobalState;

extern GlobalState gState;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __Func_809280c(int a, int b, int c);
extern int __Func_8092c40(int a, int b);
extern int _MSG_297f;
extern int _MSG_2982;
extern int _MSG_2985;

int OvlFunc_971_200906c(int slot)
{
    int base;
    int n;

    n = 0;
    __CutsceneStart();
    switch (slot) {
    case 0xc:
        base = (int)&_MSG_2985;
        break;
    case 0xd:
        base = (int)&_MSG_297f;
        break;
    case 0xe:
    default:
        base = (int)&_MSG_2982;
        break;
    }
    __Func_809280c(slot, gState.f1f4, 0);
    if (__GetFlag(0xc1 << 2)) {
        n = 2 - (__GetFlag(0x305) != 0);
    }
    __MessageID(base + n);
    __Func_8092c40(slot, 0);
    __CutsceneEnd();
}
