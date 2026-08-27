/* OvlFunc_965_200a738
 *
 * Cut out of goldensun/asm//overlays/rom_7ef4f4/ovl_30_c_a_c_a_b.s.
 *
 * Picks which door routine runs from the players facing quadrant and a flag.
 *
 * Screened by a parallel agent; re-verified here before wiring.
 */
struct Actor {
    unsigned char pad00[6];
    unsigned short facing;
};

extern unsigned char gState[];
extern struct Actor *__MapActor_GetActor(int slot);
extern int OvlFunc_965_200a660(void);
extern int __Func_8093fa0(void);
extern int __Func_8093e28(void);
extern void OvlFunc_965_200a6fc(void);

void OvlFunc_965_200a738(void)
{
    struct Actor *a;
    unsigned char *g;
    int t;
    int q;
    int v;

    a = __MapActor_GetActor(0);
    t = OvlFunc_965_200a660();
    q = (a->facing + 0x2000) & 0xc000;
    v = -1;
    g = gState;
    if (g[0x1f2] == 1 || t == 0) {
        if (q == 0xc000)
            v = __Func_8093fa0();
        if (q == 0x4000)
            v = __Func_8093e28();
    }
    if (v != 0) {
        g = gState;
        if (g[0x1f2] != 1)
            OvlFunc_965_200a6fc();
    }
}
