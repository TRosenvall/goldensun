/*
 * OvlFunc_883_20088c0 (StampPlayerFootprintSolid)
 *   -- asm/overlays/rom_780898/ovl_30_a_a_c_c_c.s
 *
 * ===> FIFTEEN IDENTICAL COPIES across overlays (tools/dupfuncs.py). Solving
 * this elevates fifteen functions. Third-largest duplicate group in the tree,
 * after OvlFunc_883_20080c4 (18) and OvlFunc_883_200834c (17). <===
 *
 * FIRST ATTEMPT. 142 instructions against 142 -- the LENGTH is exact -- with
 * 101 differing and the first difference at line 11. The candidate below is the
 * best of five drafts.
 *
 * WHAT IS ESTABLISHED, and each was worth a lot:
 *
 *   * The model id is a DOUBLE indirection:
 *     `**(short **)(*(unsigned char **)(e + 0x50) + 0x28)`. The obvious single
 *     `*(short *)(obj + 0x28)` emits `mov r1, #0x28 / ldrsh` where the ROM has
 *     `ldr r3, [r3, #0x28] / mov r1, #0 / ldrsh r2, [r3, r1]`. That fix alone
 *     took 132 differing to 101.
 *   * The search table is indexed by a MUTATED OFFSET (`off += 4`), not a
 *     walking pointer and not a subscript. A walking pointer leaves the length
 *     two short; the mutated offset is what makes it exactly 142.
 *   * The loop shape is the one the ROM's annotation describes: index 0 tested
 *     before the loop, the sentinel 7 stored on every pass, and the counter
 *     written only after the increment.
 *
 * WHAT IS LEFT: the first difference is the pool load of the search table being
 * hoisted above the model-id indirection. Two orderings measured -- the table
 * through a pointer local assigned after the model id (101 differing, first
 * difference at 11, the version below) and the model id computed after the
 * offset initialisation (106). Neither reaches it.
 *
 * The rest of the 101 is not yet analysed and is probably mostly downstream of
 * that one hoist. The abs()-and-shift block in the middle and the two
 * six-argument tail calls have not been checked instruction by instruction.
 */
extern unsigned char *iwram_3001e70;
extern unsigned char L61d0[] __asm__(".L61d0");
extern unsigned char L61e8[] __asm__(".L61e8");
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_883_2008244(int a, int b, int c, int d, int e, int f);

int OvlFunc_883_20088c0(int slot)
{
    int pad[2];
    int v[6];
    unsigned char *base;
    unsigned char *e;
    unsigned char *tp;
    int off;
    unsigned int i;
    unsigned int n;
    int m;
    int idx;
    int a, b, w, h;
    int px, pz;
    int sx, sz;

    base = iwram_3001e70;
    e = __MapActor_GetActor(slot);
    m = **(short **)(*(unsigned char **)(e + 0x50) + 0x28);
    i = 0;
    tp = L61d0;
    off = 0;
    if (m == *(int *)(tp + off)) {
        v[0] = i;
    } else {
        for (;;) {
            v[0] = 7;
            i++;
            if (i > 5)
                goto done;
            off += 4;
            m = **(short **)(*(unsigned char **)(e + 0x50) + 0x28);
            if (m == *(int *)(tp + off)) {
                v[0] = i;
                break;
            }
        }
    }
done:
    n = v[0];
    if (n > 6)
        return 0;
    px = *(int *)(e + 8);
    v[2] = px;
    v[3] = *(int *)(e + 0xc);
    pz = *(int *)(e + 0x10);
    v[4] = pz;
    idx = n << 4;
    a = *(int *)(L61e8 + idx + 4);
    if (a < 0)
        a = -a;
    b = *(int *)(L61e8 + idx + 0xc);
    if (b < 0)
        b = -b;
    h = (a + b) >> 4;
    a = *(int *)(L61e8 + idx);
    w = a;
    if (a < 0)
        w = -a;
    b = *(int *)(L61e8 + idx + 8);
    if (b < 0)
        b = -b;
    sx = ((a << 16) + px) >> 20;
    sz = ((*(int *)(L61e8 + idx + 4) << 16) + pz) >> 20;
    w = (w + b) >> 4;
    v[2] = sx;
    v[4] = sz;
    __Func_8010704(sx, sz,
                   w, h,
                   (*(int *)(base + (0x9e << 1)) >> 20) + sx,
                   (*(int *)(base + (0xa0 << 1)) >> 20) + sz);
    OvlFunc_883_2008244(0, v[2], v[4], w, h, 0xff);
    OvlFunc_883_2008244(2, v[2], v[4], w, h, 0xff);
    return 1;
}
