/* Cluster OvlFunc_922_20097e4..OvlFunc_922_20097e4 extracted from
 * goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_c_a_c_c_a.s.
 *
 * Total .text for this TU = 0x160 bytes. Never attempted before batch 278.
 * SEVEN PINS IN FOUR BLOCKS -- one fakematch row, and the heaviest scaffolding landed in this
 * run of batches. No flags. Gated on `make compare`, not on objcmp: objcmp reports two extra
 * ABS32 relocations for _AREA_40 / _AREA_43, which are absolute symbols the linker fills, so the
 * object-level compare cannot see through them. Both are already in area.sym (lines 148, 151)
 * and the landed neighbour ovl_30_c_a_c_a_a_b.c uses them identically. All 21 ROM relocations are
 * present and the only encoding differences are those two pool words.
 *
 * THE r5 ZERO PIN IS THE WHOLE FUNCTION -- 11 differing to 2, and it also made the two
 * 2008ed8 interleave pins land. The zero for `L3328[0]=0 / p[0x55]=0 / *(p+0xc)=0` was becoming a
 * GLOBAL allocno because cse1 carried it into case 5's `*(r+0x18)=0`; that stole r6 and pushed
 * the gState pointer to r5. `.17.lreg`: `Register 76 used 5 times across 46 insns; crosses 3
 * calls`. Every naming spelling of the two zeros was inert at 11.
 *
 * THE PIN SET IS THE MEASURED MINIMUM, each number below being that pin's price when dropped
 * from the exact source:
 *
 *     all three 2008ed8 pins (r1,r2,r3)      2
 *     keep only r1 there                     2
 *     keep r1 + r3 there                     2
 *     the first r0 pin                       3
 *     both r0 pins                           3
 *     the r5 zero pin                       10
 *     the r5 pointer pin                     2
 *
 * So nothing is removable. Seven pins is a lot, and it is recorded here rather than argued
 * away: the alternative was to park this at 2 differing.
 *
 * Progress: 42 -> 16 (a named `off` kills the `=gState+450` fold; a named stack pair; separate
 * pointer locals) -> 13 (r0 pins) -> 11 (2008ed8 pins) -> 2 (the r5 zero pin) -> exact.
 * Worse: an r3 pin on the case-5 zero 19; an r2 pin there 18; the pointer at the merge reads 30;
 * pinning the pointer to r6 12; named locals instead of the r0 pins 3.
 * NO FLAG REACHES IT: -fno-cse-skip-blocks, -fno-thread-jumps, -fno-expensive-optimizations and
 * -fno-strength-reduce all 11; -fno-cse-follow-jumps 23; -fno-schedule-insns2 25; -fno-gcse 41.
 *
 * THE SAME-STEM NEIGHBOUR WAS DECISIVE AND IS WORTH NAMING: ovl_30_c_a_c_c_c_c_a_c_c_a_b.c is
 * OvlFunc_922_2009948 -- THE FUNCTION THIS ONE CALLS ON ITS FIRST LINE. It supplied the
 * `extern int _AREA_NN` / `(int)&_AREA_NN` form, the named-stack-argument-pair rule for
 * __Func_8010704 and __CopyMapTiles, and the int-temp range compare
 * (`d = *(unsigned short *)p - 2; if ((unsigned short)d <= 3)` -> `sub / lsl #16 / cmp / bhi`),
 * which would not have come out of the listing. A second neighbour, rom_7b9cb4's
 * ovl_30_a_c_c_a_c_c_a_a_a_a_c_b.c, supplied "name the OFFSET, not the BASE", worth 26 on its own.
 */
extern unsigned char gState[];
extern unsigned char iwram_3001ebc[];
extern unsigned char iwram_3001ee0[];
extern unsigned int L3328[] __asm__(".L3328");
extern int _AREA_40;
extern int _AREA_43;
extern int __GetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __WaitFrames(int n);
extern void __Func_8091200(int a, int b);
extern void __Func_8091220(int a, int b);
extern void __Func_8091254(int a);
extern void __Func_8091494(int a);
extern void __Func_8092950(int a, int b);
extern void __Func_8092b08(int a, int b);
extern void OvlFunc_922_2008ed8(int a, int b, int c, int d);
extern void OvlFunc_922_2009948(void);
extern void OvlFunc_922_2009b1c(void);

void OvlFunc_922_20097e4(void)
{
    unsigned char *gp;
    unsigned char *p;
    unsigned char *q;
    int off;
    int d;
    int y;
    int e1, f1;

    OvlFunc_922_2009948();
    gp = gState;
    off = 0xe1 << 1;
    if (*(short *)(gp + (0xe0 << 1)) == (int)&_AREA_40) {
        if (__GetFlag(0xf13) == 0 && *(short *)(gp + (0xe1 << 1)) == 1)
            OvlFunc_922_2009b1c();
        d = *(unsigned short *)(gState + off) - 2;
        if ((unsigned short)d <= 3) {
            y = 0xe2 << 17;
            { register int q1 __asm__("r1"); register int q2 __asm__("r2"); register int q3 __asm__("r3"); q1 = 0; q2 = y; q3 = 0xdf; OvlFunc_922_2008ed8(0x9c << 16, q1, q2, q3); }
            OvlFunc_922_2008ed8(0xbc << 16, 0, y, 0xdf);
        }
    } else if (*(short *)(gp + (0xe0 << 1)) == (int)&_AREA_43) {
        p = __MapActor_GetActor(8);
        { register int z __asm__("r5"); z = 0; L3328[0] = z; p[0x55] = z; *(int *)(p + 0xc) = z; }
        __Func_8092b08(8, 1);
        __Func_8092950(8, 0xf);
        switch (*(short *)(gp + (0xe1 << 1))) {
        case 1:
        case 2:
            __Func_8091494(0);
            L3328[0] = 1;
            break;
        case 5:
            __Func_8091494(0);
            L3328[0] = 1;
            { register unsigned char *rp __asm__("r5"); rp = *(unsigned char **)iwram_3001ee0; *(int *)(rp + 0x18) = 0; }
            break;
        }
        if (*(short *)(gState + off) <= 6) {
            if (__GetFlag(0x82 << 4)) {
                __CopyMapTiles(0x1e, 0x39, 0x13, 0x39, 1, 1);
                e1 = 8;
                f1 = 7;
                __CopyMapTiles(0x1e, 8, 0xc, 8, e1, f1);
            } else {
                q = *(unsigned char **)iwram_3001ebc;
                *(int *)(q + (0xe0 << 1)) = 0x100;
                { register int q0 __asm__("r0"); q0 = 0x203108; __Func_8091220(q0, 1); }
                { register int q0 __asm__("r0"); q0 = 0x203108; __Func_8091200(q0, 1); }
                __Func_8091254(1);
                __WaitFrames(1);
            }
        }
    }
}
