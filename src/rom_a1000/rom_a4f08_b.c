/* Func_80a4f08 -- 0x080a4f08, first of the four functions that were in
 * asm/rom_a1000/rom_a4f08.s.  Landed as the `_b` part; Func_80a51d0, Func_80a524c
 * and Func_80a5388 stay in assembly as `_c`, together with the .rodata.
 *
 * TEXT/DATA SPLIT, one export.  The file's whole data section is a single unnamed
 * label at the very end -- `.Laf08c: .incrom 0xaf08c, 0xaf20c`, 0x180 bytes -- read
 * by exactly one instruction, `ldr r0, =.Laf08c`, inside this function.  So the
 * `_c` part carries the data and needed `.global .Laf08c` added; nothing else
 * crosses the boundary in either direction, and the other three functions need no
 * export at all.  The export was gated on its own before the split, and the split
 * on its own before this file, so all three changes stay separable.
 *
 * REQUIRES `_MSG_182`, ALREADY ADMITTED in message.sym.  objcmp reports one
 * differing encoding and one extra relocation, and they are the same fact: the pool
 * word holds `R_ARM_ABS32 _MSG_182` here and the resolved 0x00000182 in the
 * reference.  objcmp compares pre-link objects and cannot see through a relocation;
 * the compare against baserom is the proof.  The evidence is the usual pooling tell
 * -- 0x182 is thumb_shiftable_const (0xc1 << 1), so as a bare literal gcc builds it
 * in two instructions and never emits the ROM's `ldr r3,=0x182`.
 *
 * THE PRIORITY FORMULA READ AS AN INSTRUCTION, NOT AS A WALL.  `win` and `i` have
 * FOURTEEN real refs each, so allocno_compare's floor_log2(n_refs) * n_refs is 42
 * for both and only live_length separates them.  Writing `i = a;` ABOVE the `state`
 * load makes `i`'s live range a strict superset of `win`'s, so `win` ranks first and
 * takes r7: 68 -> 19 in disagreeing regions, and the entire residue went with it.
 * Two earlier batches used this formula to prove parks unreachable by arithmetic;
 * this is the first time it was used to decide WHAT TO MOVE.
 *
 * Also load-bearing: the `__modsi3` divisor is a variable, not a constant -- a
 * literal `% n` expands inline as five instructions, and a `mov rN,#K` feeding the
 * libcall is evidence of a variable that cprop folded; `moved = 1;` before
 * `give = 0;`; and `obj->tile += 4` as a 10-bit bitfield.
 *
 * SHIM, NAMED: `DMA3_SET` from include/dma.h, the tree's register-pinned
 * `stmia r3!,{r0,r1,r2} / sub r3,#0xc`.  It is the only asm in this file.
 * `extern const unsigned char Laf08c[] __asm__(".Laf08c");` is a name binding for a
 * label that is not a valid C identifier, not a register pin.
 */
#include "dma.h"

struct Obj {
    unsigned char pad_00[0x18];
    unsigned short tile : 10;
    unsigned short rest : 6;
};

extern unsigned char *iwram_3001f2c;
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyRepeat;
extern const unsigned char Laf08c[] __asm__(".Laf08c");
extern int _MSG_182;
extern void *galloc_ewram(int tag, int size);
extern void gfree(int tag);
extern void Func_80a4eb8(void);
extern void _Func_8016498(int win);
extern void _Func_80164ac(int win);
extern int Func_80a3d9c(int id, int item);
extern int AllocSpriteSlot(void);
extern void UploadSpriteGFX(int slot, int size, void *src);
extern struct Obj *_Func_801eadc(int slot, unsigned int a, int win, int y, int b);
extern void Func_80a1ac0(int x, int y);
extern void Func_80a1a40(int x, int y);
extern void _Func_801e7c0(int msg, int win, int x, int y);
extern void _Func_80b06c0(int v, int x, void *buf);
extern void _Func_801ea08(int v, int digits, int win, int x, int y);
extern void _Func_801e8b0(void *unit, int win, int x, int y);
extern void *_GetUnit(int id);
extern void _PlaySound(int id);
extern void WaitFrames(int n);
extern int _GetFlag(int flag);

int Func_80a4f08(int a, int n, int mode)
{
    unsigned char *state;
    void *gfx;
    int win;
    struct Obj *obj;
    int i;
    int slot;
    int moved;
    int give;
    int recv;

    i = a;
    state = iwram_3001f2c;
    gfx = galloc_ewram(0xe, 0x400);
    moved = 1;
    give = 0;
    win = *(int *)(state + 0x10c);
    Func_80a4eb8();
    _Func_8016498(win);
    if (mode == 0)
        give = Func_80a3d9c(state[0x21b],
                            *(unsigned short *)(state + 0x178) & 0x1ff);
    recv = Func_80a3d9c(state[0x21a],
                        *(unsigned short *)(state + 0x178) & 0x1ff);
    slot = AllocSpriteSlot();
    if (slot == 0x60)
        goto out;
    UploadSpriteGFX(slot, 0x100, 0);
    _Func_801eadc(slot, 0x40004000, win, 0x30, 0x20);
    obj = _Func_801eadc(slot, 0x40004000, win, 0x50, 0x20);
    obj->tile += 4;
    Func_80a1ac0(0x80, 0x28);
    for (;;) {
        if (_GetFlag(0x150))
            break;
        if (moved != 0) {
            moved = 0;
            i = (i + n) % n;
            _Func_8016498(win);
            _Func_801e7c0(0xade, win, 0x20, 0);
            DMA3_SET(Laf08c, gfx, 0x84000040);
            _Func_80b06c0(0x1e, 0xe, gfx);
            _Func_80b06c0(n + a, 0, gfx);
            _Func_80b06c0(a + i + 1, 0xa, gfx);
            _Func_80b06c0(a, 2, gfx);
            UploadSpriteGFX(slot, 0x100, gfx);
            _Func_801ea08(i + 1, 2, win, 0x20, 0x20);
            _Func_801e7c0((*(unsigned short *)(state + 0x178) & 0x1ff) + (int)&_MSG_182,
                          win, 0x10, 8);
            _Func_801ea08(recv - i - 1, 2, win, 0x10, 0x18);
            if (mode == 0)
                _Func_801ea08(give + i + 1, 2, win, 0x50, 0x18);
            _Func_801e8b0(_GetUnit(state[0x21a]), win, 0x10, 0x10);
            if (mode == 0)
                _Func_801e8b0(_GetUnit(state[0x21b]), win, 0x50, 0x10);
        }
        if (gKeyPress & 1) {
            _PlaySound(0x70);
            goto out;
        }
        if (gKeyPress & 2) {
            i = -1;
            _PlaySound(0x71);
            goto out;
        }
        Func_80a1a40(0x80, 0x28);
        if (gKeyRepeat & 0x20) {
            i--;
            moved = 1;
            _PlaySound(0x6f);
        }
        if (gKeyRepeat & 0x10) {
            i++;
            moved = 1;
            _PlaySound(0x6f);
        }
        WaitFrames(1);
    }
out:
    _Func_8016498(win);
    _Func_80164ac(win);
    gfree(0xe);
    *(char *)(*(int *)(state + 0x21c) + 5) = 0xd;
    if (_GetFlag(0x150))
        i = -1;
    return i;
}
