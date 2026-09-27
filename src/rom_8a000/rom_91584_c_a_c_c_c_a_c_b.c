/* Func_8091890  --  0x08091890, split out of asm/rom_8a000/rom_91584_c_a_c_c_c_a_c.s;
 * Func_80919d8 and Func_8091a58 stay in _c.s. Matched from scratch.
 *
 * The four bar-fraction blocks are a DO-WHILE MACRO: its loop notes are the
 * barrier that keeps both copy `strh`s ahead of the sign-extend; as a static
 * inline it is 3 off. Repeated inline-looking blocks whose ROM order looks
 * barriered may have been do-while macros.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern void _Func_8079664(void);
extern void Func_8091858(void);
extern void *_GetUnit(int id);
extern int _GetPartySize(void);

struct Unit {
    unsigned char pad0[0x14];
    short hpFrac;      /* 0x14 */
    short ppFrac;      /* 0x16 */
    unsigned char pad18[0x34 - 0x18];
    short maxHp;       /* 0x34 */
    short maxPp;       /* 0x36 */
    short hp;          /* 0x38 */
    short pp;          /* 0x3a */
    unsigned char pad3c[0x131 - 0x3c];
    unsigned char f131;
};

static inline int Clamp4000(int v)
{
    if (v > 0x4000)
        return 0x4000;
    if (v < 0)
        return 0;
    return v;
}

#define UPDATE_FRAC(frac, cur, max) do {              \
    (frac) = Clamp4000(((cur) << 14) / (max));         \
    if ((frac) == 0 && (cur) != 0)                     \
        (frac) = 1;                                    \
} while (0)

void Func_8091890(int id)
{
    struct Unit *u;
    int alive;
    int i;
    int n;
    unsigned char *gs;
    unsigned char *gs2;

    _Func_8079664();
    Func_8091858();
    u = _GetUnit(id);
    u->hp = u->maxHp;
    u->pp = u->maxPp;
    UPDATE_FRAC(u->hpFrac, u->hp, u->maxHp);
    UPDATE_FRAC(u->ppFrac, u->pp, u->maxPp);
    u->f131 = 0;
    alive = 0;
    n = _GetPartySize();
    for (i = 0; i < n; i++) {
        gs = (unsigned char *)&gState;
        u = _GetUnit(gs[i + (0xfc << 1)]);
        if (u->hp != 0)
            alive++;
    }
    if (alive == 0) {
        gs2 = (unsigned char *)&gState;
        gs2 += 0xfa << 1;
        u = _GetUnit(*(int *)gs2);
        u->hp = 1;
        UPDATE_FRAC(u->hpFrac, u->hp, u->maxHp);
        UPDATE_FRAC(u->ppFrac, u->pp, u->maxPp);
    }
}
