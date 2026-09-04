/* Func_801b5c0 -- 0x0801b5c0
 *
 * One step of a menu countdown: if the pending count is non-zero, run the
 * pre-step hook, hold a frame at state 0x21, then either consume one unit (when
 * the sub-counter is exactly 1 and the count is still live) or just tick the
 * sub-counter down, and finish by restoring state 1 and running the two
 * post-step hooks a frame apart.
 *
 * THE WHOLE RESIDUE WAS THE NARROW-STORE TABLE, for the fourth batch running.
 * Written as casts -- `*(short *)(s + 0x3a2) = 0x21;` and the three like it --
 * every stored literal goes to the POOL (`ldr r3, =0x21`, `ldr r3, =0x8`) where
 * the ROM builds it (`mov r3, #0x21`). 47 differing. Routing the four narrow
 * stores through a TYPED STRUCT gets the immediate in a scratch register and
 * closes the function exactly, at no register cost.
 *
 * The two COUNTERS stay raw pointers rather than struct fields, because the ROM
 * holds `s + 0x39c` and `s + 0x39e` in callee-saved registers across the calls
 * and reuses them; a field access would recompute the address each time. So the
 * function wants BOTH spellings -- struct fields for the one-shot stores,
 * pointer locals for the values that live across calls -- which is the same
 * "name what must survive something" test applied to addresses.
 *
 * The offset forms fall out on their own: 0x39c is `0xe7 << 2` and gcc builds
 * it with `mov`/`lsl`, while 0x39e is not shiftable and pools, exactly as the
 * ROM has them. The decrements are written `*p = *p - 1` and gcc emits the
 * halfword form `ldr r1, =0xffff / add`.
 *
 * VERIFIED WITH tools/objcmp.py -- 164 bytes, 72 encodings and 7 relocations
 * identical -- and that check was not optional here: the reference holds SEVEN
 * functions, so tryc.py SKIPS its size check entirely and its "OK" covers only
 * the instruction stream.
 */
struct S {
    unsigned char pad00[8];
    short f08;
    short f0a;
    unsigned char pad0c[0x32];
    short f3e;
    unsigned char pad40[0x308];
    unsigned char *f348;
    unsigned char pad34c[0x50];
    unsigned short f39c;
    unsigned short f39e;
    unsigned char pad3a0[2];
    short f3a2;
};

extern void Func_801b9a8(unsigned char *s, int n);
extern void Func_801ba68(unsigned char *s, int n);
extern void Func_801b9ec(unsigned char *s, int n);
extern void Func_801b010(int a, int b);
extern void WaitFrames(int n);

void Func_801b5c0(unsigned char *s)
{
    struct S *e;
    unsigned short *p;
    unsigned short *q;
    unsigned short *r;
    unsigned char *t;
    int v;

    e = (struct S *)s;
    p = (unsigned short *)(s + (0xe7 << 2));
    if (*(int *)p == 0)
        return;
    q = (unsigned short *)(s + 0x39e);
    Func_801b9a8(s, *q);
    e->f3a2 = 0x21;
    WaitFrames(1);
    v = *q;
    if (v == 1 && *p != 0) {
        e->f08 = 8;
        *p = *p - 1;
        Func_801ba68(s, 0);
        if (*p == 0)
            e->f0a = 0;
        e->f3e = v;
    } else {
        r = (unsigned short *)(s + 0x39e);
        *r = *r - 1;
    }
    e->f3a2 = 1;
    Func_801b9ec(s, *(unsigned short *)(s + 0x39e));
    WaitFrames(1);
    t = *(unsigned char **)(s + (0xd2 << 2));
    Func_801b010(*(unsigned short *)(t + 0xa), 0);
    WaitFrames(1);
}
