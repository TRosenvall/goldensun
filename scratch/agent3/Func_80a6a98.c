/* Func_80a6a98 (DrawItemDetail) -- PARKED at 8 differing of 89.
 * ref: asm/rom_a1000/rom_a5534_c_c_c_a_c.s   first diff at position 29.
 *
 * Line count is right (89 vs 89) and 81 of 89 are exact.  The eight are two
 * independent register-naming residues:
 *
 *  (1) six instructions around the halfword read.  The ROM has
 *        ldrh r2,[r7,r3] / mov r3,r2 / cmp r3,#0 / ... / ldr r0,=0x1ff /
 *        ldr r3,=0x53a / and r0,r2 / add r0,r3
 *      and we get the same eight instructions with h in r0 and the COPY in r3,
 *      so the compare reads the original and the AND reads the copy -- the
 *      reverse of the ROM.  Semantically identical, purely which pseudo the
 *      allocator put in r0.
 *  (2) `mov r2,#0x1 / mov r10,r2` vs our `mov r3,#0x1 / mov r10,r3`: the
 *      scratch register for building the loop-invariant 1.
 *
 * WHAT WAS LOAD-BEARING (keep all four if this is retried):
 *   - `off` named as a local: `off = d->f18*2 + (0xe4<<1); *(u16*)(p+off)`
 *     gives the ROM's register-offset `ldrh r2,[r7,r3]`.  Folded into the
 *     subscript gcc emits `add r3,r7 / ldrh r3,[r3]`.
 *   - `h` declared `unsigned short` with `t = h;` as an int copy: that is what
 *     produces the ROM's redundant-looking `mov r3, r2`.  With `h` an int the
 *     copy is coalesced and the function is 88 lines against 89.
 *   - `t = 0x1ff; t &= h;` REUSING t (constant-as-destination).  A third local
 *     for the mask collapses the copy again: 60 differing.
 *   - `i = 0;` as its own statement with `for (; i <= 4; i++)`, and `one`
 *     assigned between it and `k = 1;`.  That exact order is what puts i in r6
 *     and k in r5; `for (i = 0; ...)` with `one; k;` first gives 16.
 *
 * MEASURED NEGATIVES: -fno-rerun-cse-after-loop (8, unchanged),
 * -fno-schedule-insns (8), -fno-gcse (20); `0x53a + t` (8); `t = t & h` (8);
 * four different declaration orders of the seven locals (8 every time --
 * declaration order is inert on this function).
 */
struct Desc {
    unsigned char pad_00[8];
    int f8;                 /* 0x08 */
    unsigned char pad_0c[4];
    int f10;                /* 0x10 */
    unsigned char pad_14[4];
    int f18;                /* 0x18 */
};

extern unsigned char *iwram_3001f2c;
extern int  _GetFlag(int id);
extern void _ClearFlag(int id);
extern void _Func_8016498(int a);
extern void WaitFrames(int n);
extern void _Func_801e7c0(int a, int b, int c, int d);
extern void Func_80a2268(int a, int b, int c, int d, int e, int f);

int Func_80a6a98(int a0, int a1, struct Desc *d)
{
    unsigned char *p;
    int i;
    int k;
    int one;
    int off;
    unsigned short h;
    int t;

    p = iwram_3001f2c;
    d->f18 = d->f8 * 5 + d->f10;
    if (_GetFlag(0x151) == 0) {
        _Func_8016498(*(int *)(p + 0x2c));
        WaitFrames(1);
        off = d->f18 * 2 + (0xe4 << 1);
        h = *(unsigned short *)(p + off);
        t = h;
        if (t != 0) {
            t = 0x1ff;
            t &= h;
            _Func_801e7c0(t + 0x53a, *(int *)(p + 0x2c), 0, 0);
        }
    } else {
        _ClearFlag(0x2ff);
    }
    i = 0;
    one = 1;
    k = 1;
    for (; i <= 4; i++) {
        if (i == d->f10)
            Func_80a2268(*(int *)(p + 0x20), 0, k, 0xf, one, 0xe);
        else
            Func_80a2268(*(int *)(p + 0x20), 0, k, 0xf, one, 0xf);
        k += 2;
    }
    WaitFrames(1);
    return 1;
}
