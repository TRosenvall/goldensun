/* OvlFunc_879_2008054  --  0x02008054, was asm/overlays/rom_779188/ovl_30_c_c_a.s
 * (this function alone), so it converts whole. Matched from scratch.
 *
 * - `return 0;` IN EACH ARM stops jump2 merging the arms' identical
 *   `mov r1,#2 / bl __SetDestMap` tails: each arm's `mov r0,#0` is first merged
 *   against the block falling into the return label, which makes a NEW label, and
 *   jump.c's pairwise jump_chain search only covers labels older than the pass.
 *   Sign: the ROM's shared return label sits on its `mov r0,#0`. 84 -> 3.
 * - The last 3 were pool words: _AREA_01, _AREA_04 and _CONST_b (const.sym, added
 *   this batch -- the ROM pools __Func_8002f3c's 0xb).
 */
struct Actor {
    unsigned char pad00[0x55];
    unsigned char f55;
};

extern int _AREA_00;
extern int _AREA_01;
extern int _AREA_04;
extern int _CONST_b;
extern unsigned char gState[];
extern int gKeyPress;
extern int gKeyHeld;
extern struct Actor *__MapActor_GetActor(int id);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __CutsceneWait(int n);
extern void __SetDestMap(int map, int entrance);
extern void __StartGS1Credits(int a);
extern void __Func_8003b70(int a);
extern void __Func_8003ce0(void);
extern void __Func_8002f3c(int a);
extern void __NintendoLogo(int a);
extern void __CamelotLogo(int a);
extern int __Func_801f77c(void);
extern int __StartTitleScreen(int a);
extern void __Func_8077f70(void);
extern void OvlFunc_879_20082e8(int a);

int OvlFunc_879_2008054(void)
{
    unsigned char *g;
    short *area;
    int i;

    g = gState;
    area = (short *)(g + 0x1c2);
    if (*area == 10) {
        __MapActor_GetActor(*(int *)(g + 0x1f4))->f55 = 0;
        __PlaySound(0x4b);
        OvlFunc_879_20082e8(0);
        __WaitFrames(0x78);
        i = 0;
        if (gKeyPress == 0) {
            do {
                __WaitFrames(1);
                if (++i > 0xe0f)
                    break;
            } while (gKeyPress == 0);
        }
        __SetDestMap((int)&_AREA_00, 2);
        return 0;
    } else if (*area == 9) {
        __PlaySound(0x43);
        __StartGS1Credits(0);
        __PlaySound(0x11);
        __Func_8003b70(0x3c);
        __Func_8003ce0();
        __CutsceneWait(0xf0);
        __PlaySound(0x13);
        __SetDestMap((int)&_AREA_01, 2);
        return 0;
    } else {
        __Func_8002f3c((int)&_CONST_b);
        if (*area == 2) {
        top:
            __PlaySound(0x13);
            __NintendoLogo(0);
            __CamelotLogo(0);
            if (__Func_801f77c() > 0) {
                __PlaySound(0x46);
                if (__StartTitleScreen(1) == 0) {
                    __PlaySound(0x11);
                    __Func_8003b70(0x1e);
                    __Func_8003ce0();
                    i = 0;
                    if (gKeyHeld == 0) {
                        do {
                            __WaitFrames(1);
                            if (++i > 0x77)
                                break;
                        } while (gKeyHeld == 0);
                    }
                    goto top;
                }
            }
            __SetDestMap((int)&_AREA_01, 1);
        } else {
            __PlaySound(0x40);
            __StartTitleScreen(0);
            __Func_8077f70();
            __SetDestMap((int)&_AREA_04, 0x10);
            __PlaySound(0x11);
        }
        __PlaySound(0x11);
        __Func_8003b70(0x1e);
        __Func_8003ce0();
        __CutsceneWait(0x3c);
        __PlaySound(0x13);
    }
    return 0;
}
