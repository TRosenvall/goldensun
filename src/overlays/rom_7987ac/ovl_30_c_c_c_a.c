/* OvlFunc_902_20084e4  --  0x020084e4
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * keeps its name (`asm/%.o: src/%.c`) and the linker script is unchanged.
 *
 * Arrival fixups for one map, by entrance: entrance 5 repaints a tile block and
 * drops slot 8 to ground level; entrances 7 and 0xb spawn a watcher object and
 * register its task.
 *
 * THREE LEVERS, all of them already in docs/elevation.md, and the function
 * needed all three.
 *
 * 1. THE STACK-ARG PAIR. `__CopyMapTiles` takes two arguments on the stack and
 *    the ROM materialises both into registers before storing either. Named as
 *    `s0` and `s1` immediately before the call, in the order the ROM stores
 *    them.
 *
 * 2. THE THREE `__MapActor_GetActor` RESULTS MUST NOT GO THROUGH A NAMED LOCAL.
 *    `a = GetActor(8); a[0x55] = 0;` keeps the result live and copies it
 *    (`mov r2, r0 / add r2, #0x55`); inlined into the store expression, r0 is
 *    dead after the add and the ROM's form appears.
 *
 * 3. THE ZERO IS NAMED, AND ITS PLACEMENT IS THE LEVER. The ROM holds one zero
 *    in r5 across all three calls. With bare literals gcc builds it twice --
 *    `mov r3, #0` for the byte store and a separate `mov r5, #0` for the two
 *    word stores. `int zero = 0;` written immediately before the first store
 *    matches; the SAME declaration hoisted to the top of the function is 14
 *    differing of 61, because the value then has to survive `__CopyMapTiles`
 *    too and the allocator makes a different choice. Adjacency again.
 *
 * The stored 0x209 and the 0x1c2 read from gState are BOTH derived by gcc from
 * the 0x1c0 already in a register (`add r2, #0x49` then `sub r2, #0x47`). They
 * are not written that way -- see the add/sub-chain note in docs/elevation.md.
 */
extern char *iwram_3001ebc;
extern unsigned char gState[];
extern char *__MapActor_GetActor(int slot);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __StartTask(void *fn, int prio);
extern void OvlFunc_902_2008570(int a, int x, int y, int z);
extern void OvlFunc_902_2008030(void);

int OvlFunc_902_20084e4(void)
{
    char *p;
    unsigned char *g;
    int s0;
    int s1;
    int zero;
    int e;

    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x209;
    g = gState;
    e = *(short *)(g + (0xe1 << 1));
    if (e == 5) {
        s0 = 4;
        s1 = 3;
        __CopyMapTiles(0, 0x78, 8, 0x43, s0, s1);
        zero = 0;
        *(__MapActor_GetActor(8) + 0x55) = zero;
        *(int *)(__MapActor_GetActor(8) + 0xc) = zero;
        *(int *)(__MapActor_GetActor(8) + 0x14) = zero;
    } else if (e == 7 || e == 0xb) {
        OvlFunc_902_2008570(0xe7, 0x8e << 18, 0x80 << 13, 0xa8 << 18);
        __StartTask(OvlFunc_902_2008030, 0xc8 << 4);
    }
    return 0;
}
