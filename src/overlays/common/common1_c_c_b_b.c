/* OvlFunc_common1_21c8  --  split out of asm/overlays/common/common1_c_c_b.s;
 * OvlFunc_common1_2060 stays in _a.s and the .data/.data1/.bss in _c.s (.L9,
 * which 2060 loads, was exported for the split). Matched from scratch.
 *
 * Returns `int`: the ROM's `pop {r1}` epilogue names a return value.
 */
struct Actor {
    unsigned char pad00[6];
    unsigned short f6;
    int f8;
    int fc;
    int f10;
};

extern unsigned char gState[];
extern struct Actor *__MapActor_GetActor(int id);
extern void __vec3_translate(int dist, int ang, int *v);
extern int OvlFunc_common1_2018(int *v, struct Actor *a);

int OvlFunc_common1_21c8(void)
{
    struct Actor *a;
    int ang;
    int r;
    int v[3];
    unsigned char *g;

    g = gState;
    a = __MapActor_GetActor(*(int *)(g + 0x1f4));
    ang = (a->f6 + 0x2000) & 0xc000;
    v[0] = (a->f8 & 0xfff00000) + 0x80000;
    v[1] = a->fc;
    v[2] = (a->f10 & 0xfff00000) + 0x80000;
    __vec3_translate(0x100000, ang, v);
    r = OvlFunc_common1_2018(v, a);
    if (!r) {
        v[0] = (a->f8 & 0xfff00000) + 0x80000;
        v[1] = a->fc;
        v[2] = (a->f10 & 0xfff00000) + 0x80000;
        __vec3_translate(0x200000, ang, v);
        r = OvlFunc_common1_2018(v, a);
    }
    return r;
}
