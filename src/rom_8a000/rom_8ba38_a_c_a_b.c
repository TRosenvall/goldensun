/* InitPlayerPos  --  0x0808cf78, split out of asm/rom_8a000/rom_8ba38_a_c_a.s;
 * FieldMain and Func_808ce74 (parked) stay in _a.s, Debug_PaletteEditor in _c.s.
 * Matched from scratch; the overlay entry is reached through __start_overlay[3].
 */
struct Ent {
    short id;
    short flag;
    short x;
    short y;
    short z;
    unsigned short dir;
    short pad0c;
    short f0e;
    short f10;
    short f12;
    short f14;
    short pad16;
};

struct Map {
    unsigned char pad[0xec];
    int f0ec;
    int f0f0;
    int f0f4;
    int f0f8;
};

extern struct Map *iwram_3001e70;
extern unsigned char gState[];
extern unsigned int __start_overlay[];
extern int _GetFlag(int id);

void InitPlayerPos(void)
{
    struct Map *m;
    int id;
    struct Ent *e;
    int found;
    unsigned char *g;

    m = iwram_3001e70;
    g = gState;
    id = *(short *)(g + (0xe1 << 1));
    e = ((struct Ent *(*)(void))__start_overlay[3])();
    found = 0;
    for (;; e++) {
        if (e->id == -1)
            break;
        if (e->id == id && (e->flag == -1 || _GetFlag(e->flag))) {
            found = 1;
            break;
        }
    }
    if (!found)
        e = ((struct Ent *(*)(void))__start_overlay[3])();
    if (!_GetFlag(0x109)) {
        unsigned char *g2 = gState;
        *(int *)(g2 + (0xee << 1)) = e->x << 16;
        *(int *)(g2 + (0xf0 << 1)) = e->y << 16;
        *(int *)(g2 + (0xf2 << 1)) = e->z << 16;
        *(int *)(g2 + (0xf4 << 1)) = e->dir;
        *(unsigned short *)(g2 + (0xf6 << 1)) = 0;
    }
    if (e->f0e != -1)
        m->f0ec = e->f0e << 16;
    if (e->f10 != -1)
        m->f0f0 = e->f10 << 16;
    if (e->f12 != -1)
        m->f0f4 = e->f12 << 16;
    if (e->f14 != -1)
        m->f0f8 = e->f14 << 16;
    if (m->f0ec + 0xf00000 > m->f0f4)
        m->f0ec = m->f0f4 - 0xf00000;
    if (m->f0f0 + 0xa00000 > m->f0f8)
        m->f0f0 = m->f0f8 - 0xa00000;
}
