/* OvlFunc_936_20098a4  --  0x020098a4
 *
 * Cut out of goldensun/asm/overlays/rom_7c097c/ovl_30_c_c_c_a_c_c_a_c.s.
 *
 * Arrival fixup for one map: if the bridge flag is set, repaint three tile
 * blocks so the bridge is drawn extended, then a separate area check.
 *
 * EACH STACK-ARGUMENT SITE NEEDS ITS OWN LOCAL. This refines the
 * stack-arg-pair lever in docs/elevation.md, which says to name both values
 * adjacent to the call. That is necessary and not sufficient when there are
 * SEVERAL such calls: reusing one local for the three `[sp, #4]` values puts it
 * in r2 where the ROM has r3, at every site --
 *
 *     rom    mov r3, #0x3 / str r3, [sp, #4]
 *     ours   mov r2, #0x3 / str r2, [sp, #4]
 *
 * -- four differing of 58. A distinct local per site is exact. That is the
 * rebuilt-vs-carried rule reaching the stack-argument case: these three values
 * are rebuilt at each site, so they get one local each.
 *
 * The value 2 IS shared, because the ROM shares it -- `mov r5, #2` once, then
 * `str r5, [sp]` at both __CopyMapTiles calls. Carried, so one local, named
 * before the first use. Both rules in one function, on adjacent arguments.
 *
 * The stored 0x204 is derived by gcc from the 0x1c0 already in a register
 * (`add r2, #0x44`); it is not written that way.
 */
extern char *iwram_3001ebc;
extern unsigned char gState[];
extern int __GetFlag(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);
extern void __WaitFrames(int n);
extern void __Func_8091ff0(int n);

void OvlFunc_936_20098a4(void)
{
    char *p;
    unsigned char *g;
    int two;
    int s0;
    int s1;
    int s2;
    int s3;

    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x204;
    if (__GetFlag(0x915)) {
        s1 = 3;
        two = 2;
        __CopyMapTiles(0x3a, 5, 0x3a, 8, two, s1);
        s0 = 8;
        s2 = 0xa;
        __Func_8010704(8, 0xb, 2, 1, s0, s2);
        s3 = 1;
        __CopyMapTiles(8, 0xc, 8, 0xb, two, s3);
        __Func_800fe9c();
        __WaitFrames(1);
    }
    g = gState;
    if (*(short *)(g + (0xe1 << 1)) <= 3)
        __Func_8091ff0(0xaa);
}
