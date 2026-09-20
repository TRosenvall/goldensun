/* Cluster Func_80b1e80..Func_80b1e80 extracted from goldensun/asm/rom_b0000/rom_b0070_a_a_c_c_c_c.s.
 *
 * Total .text for this TU = 204 bytes (= 0xcc).
 * Preserves the original ROM layout when slotted between
 * asm/rom_b0000/rom_b0070_a_a_c_c_c_c_b.o and asm/rom_b0000/rom_b0070_a_a_c_c_c_c_c_c.o in
 * goldensun/stage1.ld.
 *
 * Never attempted before batch 274. No pins, no flags, no symbols.
 *
 * A SINGLE EXIT AND A `ret` LOCAL IS THE LEVER, and it is a frame-size lever rather than
 * a register one. With an early `return ret` the function came to 86 of 87: gcc kept the
 * value in r5 and never built a frame at all. Wrapping the body in
 * `if (... && cnt > 1) { ... } return ret;` spills `ret` to `[sp]` exactly as the ROM
 * does -- `sub sp, #4 / str r2, [sp] / str r0, [sp] / ldr r0, [sp]` -- and lands it.
 *
 * So the ROM's `sub sp, #4` on a function with no arrays and no address-taken locals is
 * evidence about CONTROL FLOW: a single-exit shape with a result variable live across
 * more paths than an early return would create. Read the frame size before the registers.
 *
 * Its file-mate Func_80b153c in this same batch needed the same restructure for a
 * different reason -- there it was reload's find_equiv_reg reusing a still-live r0 -- so
 * the lever covers both a frame-size and a pressure symptom.
 */
typedef struct { unsigned char pad00[0xd8]; unsigned short items[1]; } Unit;
typedef struct { short price; unsigned char pad02; unsigned char f03; } ItemInfo;
struct P { unsigned char pad00[5]; unsigned char f05; };
typedef struct {
    unsigned char pad000[0x380];
    struct P *f380;
    unsigned char pad384[4];
    short f388;
    short f38a;
    unsigned char pad38c[0x3a8 - 0x38c];
    unsigned char f3a8;
} State;

extern unsigned char iwram_3001f2c[];

extern Unit *_GetUnit(int unit);
extern ItemInfo *_GetItemInfo(int item);
extern int Func_80b19cc(int item);
extern int _GetInventoryItem(int unit, int slot);
extern void Func_80b04dc(int msg);
extern void Func_80b0a6c(int a, int b, int c);
extern int Func_80b1614(int a, int b, int c);
extern void WaitFrames(int n);
extern void _Func_80a17c4(unsigned int a);

int Func_80b1e80(int unit, int slot)
{
    State *st;
    Unit *u;
    ItemInfo *info;
    int ret;
    int n;
    int cnt;
    int sx;
    int sy;

    st = *(State **)iwram_3001f2c;
    u = _GetUnit(unit);
    info = _GetItemInfo(u->items[slot]);
    ret = 1;
    n = Func_80b19cc(u->items[slot]);
    cnt = _GetInventoryItem(unit, slot);
    if ((info->f03 & 0x10) != 0 && cnt > 1) {
        Func_80b04dc(0xcad);
        sx = st->f388;
        sy = st->f38a;
        st->f380->f05 = 4;
        st->f3a8 = 0xc;
        Func_80b0a6c(0, 0x80, 0x30);
        ret = Func_80b1614(0, cnt, n);
        WaitFrames(1);
        _Func_80a17c4((unsigned int)st->f380);
        Func_80b0a6c(0, sx, sy);
    }
    return ret;
}
