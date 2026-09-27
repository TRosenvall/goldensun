/* Func_808f32c  --  0x0808f32c, split out of asm/rom_8a000/rom_8d9a4_c_c_a_a.s;
 * Func_808f498, Task_ScreenWindowTransition and Func_808fe38 stay in _c.s.
 * Matched from scratch.
 *
 * A LOOP-ENTRY `b` TO THE BOTTOM TEST CAN BE TWO COPIES OF THE TEST MERGED BACK:
 * gcc duplicated the test above the loop and jump2 merged it into a `b` once both
 * copies landed in the same registers -- which happened after the inner loop
 * became `while (e->f0 != -1)` with `e->f0 & 0xf` read in the body (146 -> 1).
 * Before blaming a blocked copy, check whether the copies differ only in a reload
 * register. The player pointer and the created actor are ONE variable
 * (two call-crossing pointers in one register), giving `mov r5,r0`.
 */
struct Ent {
    unsigned char pad00[8];
    int f08;
    int f0c;
    int f10;
    unsigned char pad14[0x6c - 0x14];
    void *f6c;
};

struct Map {
    unsigned char pad00[0x10];
    unsigned char *f10;
};

struct Rec {
    int f0;
    short f4;
    short f6;
    int f8;
};

extern struct Map *iwram_3001e70;
extern unsigned char gState[];
extern unsigned int __start_overlay[];
extern unsigned char L9e8a0[] __asm__(".L9e8a0");

extern struct Ent *GetFieldActor(int id);
extern int Func_808ed4c(int id);
extern int _GetFlag(int id);
extern struct Ent *_CreateActor(int id, int x, int y, int z);
extern void _Actor_SetScript(struct Ent *a, unsigned char *s);
extern void _Actor_SetSpriteFlags(struct Ent *a, int f);
extern void Func_808f28c(void);

void Func_808f32c(void)
{
    unsigned char *list;
    struct Ent *a;
    struct Rec *e;
    int px, pz;
    int id;
    int x, z;
    int v;
    unsigned int t;
    unsigned char *g;

    list = iwram_3001e70->f10;
    g = gState;
    a = GetFieldActor(*(int *)(g + 0x1f4));
    px = a->f08 >> 20;
    pz = a->f10 >> 20;
    if (list == 0)
        return;
    while (x = *list++, z = *list++, x != 0xff || z != 0xff) {
        id = *list++;
        if (Func_808ed4c(id) != 0)
            continue;
        if ((unsigned)(id - 0x64) > 0x8b)
            continue;
        if (px - x >= 0) {
            if (px - x > 8)
                continue;
        } else {
            if (x - px > 8)
                continue;
        }
        if (pz - z >= 0) {
            if (pz - z > 5)
                continue;
        } else {
            if (z - pz > 5)
                continue;
        }
        e = ((struct Rec *(*)(void))__start_overlay[9])();
        while (e->f0 != -1) {
            if (e->f4 == id && (e->f0 & 0xf) == 3) {
                t = e->f8 & 0xfff00000;
                switch (t) {
                case 0:
                case 0x100000:
                case 0x200000:
                case 0x300000:
                case 0x500000:
                    if (e->f6 != -1 && !_GetFlag(e->f6)) {
                        a = _CreateActor(0x16, (x << 20) + 0x80000, 0, (z << 20) + 0x80000);
                        if (a != 0) {
                            _Actor_SetScript(a, L9e8a0);
                            _Actor_SetSpriteFlags(a, 0);
                            a->f6c = Func_808f28c;
                        }
                    }
                    break;
                }
            }
            e++;
        }
    }
}
