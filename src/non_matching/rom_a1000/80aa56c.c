/* Func_80aa56c (0x080aa56c) -- 186 instructions.  Returns 1.
 * NON-MATCHING: 193 encodings of 203 differ (objcmp).
 * ours 208 encodings / 524 bytes against ref 203 / 508, so 193 is NOT a true
 * distance.  tryc.py --align says 95 instructions in disagreeing regions of
 * 192, and that is the figure that ranks variants.
 *
 * Reference: asm/rom_a1000/rom_aa538_c_c_a_a.s -- TWO functions (this one and
 * the state machine Func_80aa768 at 0x080aa768), so landing needs a split.
 * datacheck clean, NO cross-function label reference in either direction:
 * .Laa6c0 is a pool word inside this function and the `.word .Laa7e8 ...` run
 * is Func_80aa768's own jump table inside its own body.  No export needed.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80aa56c.c \
 *     asm/rom_a1000/rom_aa538_c_c_a_a.s --func Func_80aa56c
 *
 * STRUCTURE: the same screen-driver skeleton as the LANDED Func_80a7380
 * (src/rom_a1000/rom_a7380_a_b.c) and as this batch's Func_80a24d0, plus a
 * four-way _GetFlag chain writing 1 / 0xe / 0x1b / 0x1c into
 * *(int *)(buf + 0x212c).  Read from the .s:
 *  - r7 = p (galloc_iwram(0x37, 0xa70)), r8 = buf (Func_8004970(0x2130)),
 *    r9 = the saved gState[0x20c] byte, r10 = the constant 1, r6 = buf+0x212c,
 *    r4 = p+0x10c, r5 = zero and then the Func_8001af8 pointer.
 *  - the epilogue pops only r8/r9/r10, so r11 must stay unused -- it does.
 *  - the box handle is RELOADED from *(int *)(p + 0x10c) for Func_80ad508
 *    rather than reused from the _CreateUIBox result.
 *  - iwram_3001e68 is loaded from its SYMBOL twice, with no long-lived base
 *    register -- the opposite of Func_80a24d0 in the same batch, so the two
 *    siblings genuinely differ in whether the base is named.
 *  - r10 holding 1 across the prologue and being re-read for the `= 1` arm of
 *    the flag chain is the landed sibling's `one = 1` idiom, and it is why the
 *    arm is written `v = one`, not `v = 1`.
 *
 * LEVER FOUND HERE, worth 6 of the 101: the flag chain's BRANCH POLARITY is
 * readable off the .s and the obvious spelling is backwards.  The ROM branches
 * AWAY on `!= 0` at each level (`bne .Laa5fe`, `bne .Laa5fa`, `bne .Laa628`),
 * and gcc emits "branch-if-NOT-condition to the else arm", so every test in
 * the source is spelled `== 0` with the 1/0xe pair as the THEN arm:
 *     if (_GetFlag(0x16f) == 0) { if (_GetFlag(0x171) == 0) v = one; else v = 0xe; }
 *     else                      { if (_GetFlag(0x171) == 0) v = 0x1b; else v = 0x1c; }
 * Writing it the other way round (`!= 0` first, 0x1c/0x1b as the then arm)
 * measures 101 and lays the four arms out in the reverse order.
 *
 * DROP LADDER (--align of 192; ours/ref lengths in brackets):
 *   first candidate, `!= 0` chain ................... 101   (191/192)
 *   + the chain respelled with `== 0` tests ......... 100   (191/192)
 *   + a block-local zero for the two BYTE stores at
 *     p+0x1c / p+0x1d only  <-- this file ...........  95   (191/192)
 *   a single named `zero` for every zero in the
 *     function ......................................  92   (188/192)  REJECTED
 *   same, byte stores left as literals ..............  100  (187/192)
 * The 92 is REJECTED on the batch-292 length rule: it is four instructions
 * SHORT of the ROM, because one held zero register replaces the ROM's two
 * separate `mov r5,#0` re-materialisations.  95 at the right length beats 92 at
 * the wrong one.
 *
 * The block-local zero is the documented "pooled halfword zero is REG_EQUIV"
 * lever aimed at the right site: the ROM feeds its two `strb` from a POOLED
 * zero (`ldr r2, .Laa6c0  @ 0`) while every other zero in the function is a
 * `mov rN,#0`, so the byte stores want their own zero and nothing else does.
 *
 * BLOCKER -- the same REG_EQUIV/allocno contest as Func_80a24d0, one register
 * over.  The ROM saves THREE high registers (push {r5,r6,r7} for r8/r9/r10) and
 * spends r10 on the constant 1; we save only TWO (`push {r6,r7}`) and spend the
 * high register we do have on a zero, re-materialising the 1 inline.  Until
 * local-alloc stops halving a named constant's priority (local-alloc.c:869,
 * `REG_LIVE_LENGTH (regno) *= 2` for a REG_EQUIV constant) the ROM's
 * three-callee-saved-high-register prologue is not reachable from source here.
 *
 * SMALLER RESIDUE, downstream: we derive p+0x178 from p+0x10c by move2add
 * (`add r3,#0x6c`) where the ROM materialises `0xbc << 1` independently, and
 * our 0xff comes from the POOL where the ROM uses `mov r3,#0xff` -- the same
 * pool/immediate coin-flip as the sibling, in the opposite direction.
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001e68[];
extern unsigned char *galloc_iwram(int tag, int size);
extern unsigned char *Func_8004970(int size);
extern void _Func_80170f8(int a, int b, int c, int d);
extern void WaitFrames(int n);
extern void Func_80a1090(int a);
extern int _GetFlag(int flag);
extern void Func_80a1070(void);
extern void _Func_801e3c8(int a);
extern void _Func_80219c8(int addr);
extern int _Func_80796c4(unsigned char *buf);
extern void Func_80ae88c(void);
extern void Func_80a3354(int a, int b, int c, int d);
extern void Func_80aa544(int a);
extern void Func_80a2144(int a);
extern int _CreateUIBox(int a, int b, int c, int d, int e);
extern void Func_80ad508(int box, int b);
extern void Func_80aa768(void);
extern void Func_80ad658(void);
extern void Func_80ae8dc(void);
extern void Func_80a34c0(void);
extern void _Func_801e318(void);
extern void Func_8001af8(void *dst, void *src, int len);
extern void Func_80a1050(void);
extern void _ClearUIRegion(int a, int b, int c, int d);
extern void free(void *p);
extern void gfree(int tag);
extern void _Func_8091858(void);

int Func_80aa56c(void)
{
    void (*copy)(void *, void *, int);
    unsigned char *p;
    unsigned char *buf;
    int *q;
    int saved;
    int one;
    int z8;
    int v;

    p = galloc_iwram(0x37, 0xa7 << 4);
    saved = gState[0x83 << 2];
    gState[0x83 << 2] = 2;
    one = 1;
    *(short *)(iwram_3001e68[0] + 4) = one;
    _Func_80170f8(0, 0, 0x1e, 0x14);
    WaitFrames(1);
    Func_80a1090(0);
    buf = Func_8004970(0x2130);
    *(unsigned char **)(p + (0xc2 << 1)) = buf;
    q = (int *)(buf + 0x212c);
    *(int *)(buf + 0x2128) = 0;
    *q = 0;
    if (_GetFlag(0xb7 << 1) != 0) {
        if (_GetFlag(0x16f) == 0) {
            if (_GetFlag(0x171) == 0)
                v = one;
            else
                v = 0xe;
        } else {
            if (_GetFlag(0x171) == 0)
                v = 0x1b;
            else
                v = 0x1c;
        }
        *q = v;
    }
    Func_80a1070();
    _Func_801e3c8(1);
    _Func_80219c8(0x6002500);
    *(unsigned char *)(p + 0x219) = _Func_80796c4(p + (0x82 << 2));
    Func_80ae88c();
    Func_80a3354(0, 3, 0, 7);
    Func_80aa544(0);
    Func_80a2144(0xe);
    *(int *)(p + (0x86 << 1)) = _CreateUIBox(0xd, 0, 0x11, 5, 2);
    *(short *)(p + (0xbc << 1)) = 0xff;
    z8 = 0;
    p[0x1c] = z8;
    p[0x1d] = z8;
    *(short *)(p + (0xba << 1)) = 0;
    *(short *)(p + (0xba << 1) + 2) = 0;
    Func_80ad508(*(int *)(p + (0x86 << 1)), 0);
    Func_80aa768();
    Func_80ad658();
    Func_80ae8dc();
    WaitFrames(1);
    Func_80a34c0();
    _Func_80170f8(0, 0, 0x1e, 0x14);
    *(short *)(iwram_3001e68[0] + 4) = 0;
    _Func_801e318();
    _Func_801e3c8(0);
    copy = Func_8001af8;
    copy((void *)0x6004000, buf + 0xa8, 0x80 << 6);
    copy((void *)0x5000080, buf + 0x20a8, 0x80);
    WaitFrames(1);
    Func_80a1050();
    _ClearUIRegion(0, 0, 0x1e, 0x14);
    free(*(void **)(p + (0xc2 << 1)));
    gfree(0x37);
    _Func_8091858();
    gState[0x83 << 2] = saved;
    return 1;
}
