/* Field_Force  --  0x08098cd8, was asm/rom_8a000/rom_97b54_a_c_c_a_a.s (this
 * function alone; tools/datacheck.py reports NO data section), so it converts
 * whole with NO split and no extra exports.
 *
 * NON-MATCHING, 352 of 371 encodings differ.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/8098cd8.c \
 *     asm/rom_8a000/rom_97b54_a_c_c_a_a.s --func Field_Force
 *   -> XX SIZE  ref 832 bytes, ours 796
 *      XX ENCODINGS differ in 352 place(s) (ref 371, ours 354)
 *      XX RELOCATIONS differ
 *
 * READ THE RELOCATION SEQUENCE, not just the line.  All 41 entries are present
 * on both sides and every one of the 38 R_ARM_THM_CALL entries is in the SAME
 * ORDER, so the control-flow skeleton and the call order are right.  The only
 * sequence difference is that the ROM's two R_ARM_ABS32 pool words
 * (iwram_3001f30, Func_8098b10) sit at 0xd0/0xd4 -- inside the function, in the
 * `.pool_aligned` the ROM flushes after the `p == 0` early return -- where ours
 * are at the end.  That is a CONSEQUENCE of ours being 36 bytes shorter (no pool
 * flush is forced), not an independent residue.
 *
 * Shims: NONE (tools/shimcount.py reports 0).
 *
 * WHAT IS RIGHT.  The relocation SEQUENCE matches call for call, all 41 entries,
 * which fixes the whole control-flow skeleton.  Landed in this draft:
 *   - the frame: `sub sp, #0x2c`, with `arr` at sp+0x1c, `v` at sp+0x10, `cell`
 *     at sp+0xc and three reload spills at sp+0/4/8.  gcc-2.96 lays locals out
 *     DOWNWARD in declaration order, so the FIRST-declared local gets the
 *     HIGHEST address; `arr` must therefore be declared first and must hold
 *     FOUR pointers (0x1c..0x2c) even though only three are used -- three
 *     pointers give `sub sp, #0x28` and shift every offset.
 *   - loop 1 (12 iterations, stride 0x48 from m+0x58) is EXACT once the
 *     `i--` is written AFTER `WaitFrames(2)`, not before: sched2 will not move
 *     the decrement across the call on its own.  Direct transfer from
 *     src/rom_8a000/rom_944ec_a_c_a_a_c_a_b.c (GetJupiterDjinni), which is the
 *     same Func_809ba90/ba7c/ba70 + _Sprite_SetColorswap idiom.
 *   - the `while (*(int *)(e + 0x18) < (0x80 << 9))` rotation, the `arr[i] = q`
 *     descending strength-reduced store, and the `*pp++` ascending read loop.
 *
 * BLOCKER, by pass: global.c register allocation, one PAIRWISE SWAP.
 *
 *   ROM   r5=q  r6=e   r7=arr-walker  r8=i  r9=m  r10=&v  r11=prev/snd
 *   ours  r5=q  r6=&v  r7=e           r8=i  r9=m  r10=arr-walker  r11=prev
 *
 * Seven values are live across calls and exactly seven registers are available
 * (r5,r6,r7 + r8..r11; r4 is call-used under -fcall-used-r4), so all seven are
 * allocated and only the ORDER differs.  REG_ALLOC_ORDER is {...,4,5,6,7,8,10,
 * 9,11}, so the register a value receives is a direct readout of its
 * allocno_compare priority, floor_log2(n_refs) * n_refs / live_length:
 *
 *   ROM   q > e > walker > i > &v > m > prev     (&v is FIFTH, so it gets r10)
 *   ours  q > &v > e > i > walker > m > prev     (&v is SECOND, so it gets r6)
 *
 * `&v` losing to the walker is worth 17 instructions: with `&v` in r10 the ROM
 * pays a `mov <low>, r10` at every access block (about eleven of them) and gets
 * `add r7, sp, #0x24` / `sub r7, #4` for free in loop 2, where ours pays five
 * instructions to drive the walker through r10.
 *
 * MEASURED and REJECTED (each one compile, count of ref 371):
 *   354  this draft
 *   354  `v` referenced as an array everywhere, no `bp` variable at all
 *   354  `bp` dropped, `arr[i]` and `pp = arr` used directly
 *   354  an explicit descending walker `*w = q; w--;` instead of `ap[i] = q`
 *   354  that walker pinned `register unsigned char **w __asm__("r7")` -> 348,
 *        i.e. the pin took r7 but did NOT push `&v` out of r6
 *   354  `q` pinned to r5; `bp` declared last; four separate loop counters
 *   356  `e` pinned to r6 (it was already in a low register)
 *   356  QImode zero locals for the two `[0x55] = 0` stores
 *   374  `register int *bp __asm__("r10")` -- the pin makes `bp` a HARD register,
 *        which grows the frame to 0x34 and makes reload compute `r2 = r10 + 4`
 *        per store instead of copying r10 into one low base.  A pin is the wrong
 *        instrument here: the ROM's `mov <low>, r10` copies are RELOAD output for
 *        an allocno that global.c put in r10, not source-level copies.
 *   354  source-level low copies of `bp` hoisted into loops 1 and 3 (which is
 *        what the ROM's `mov r6, r10` / `mov r7, r10` LOOK like) -- gcse's
 *        cprop_insn coalesces them straight back, the Field_Ply mechanism.
 *
 * WHAT WOULD MOVE IT.  Anything that lowers `&v`'s priority below the loop-2
 * walker's without introducing a coalescible copy.  This is the same class as
 * the 37 parks in HANDOFF.md's REG_ALLOC_ORDER entry and would be settled by
 * that entry's testable next step.
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001f30;
extern unsigned char L9f0b4[] __asm__(".L9f0b4");
extern unsigned char L9f12c[] __asm__(".L9f12c");
extern void Func_8097384(void);
extern void _PlaySound(int id);
extern void Func_80974d8(int *v);
extern void Func_809ba90(unsigned char *p, int a, int b, int c);
extern void Func_809ba7c(unsigned char *p, void (*f)(void));
extern void Func_809ba70(unsigned char *p, int n);
extern void _Sprite_SetColorswap(int a, int b);
extern void WaitFrames(int n);
extern void Func_8098b10(void);
extern void vec3_translate(int d, int a, int *v);
extern unsigned char *CreateParticleActor(int id, int x, int y, int z);
extern void Func_809748c(void);
extern void _Actor_SetAnim(unsigned char *a, int n);
extern void _Actor_SetColorswap(unsigned char *a, int n);
extern unsigned char *Func_8096c48(void *a, unsigned char *b);
extern void _Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void _Actor_SetScript(unsigned char *a, unsigned char *s);
extern int _TestCollision(unsigned char *a, int *v);
extern int _Func_800d924(unsigned char *a, int *v);
extern int Func_808e4b4(int a, int b, int *v);
extern void Func_8096b28(int a, int b, int c);
extern void Func_8003f3c(int n);

void Field_Force(void)
{
    unsigned char *arr[4];
    int v[3];
    int cell;
    int ev;
    int *bp;
    unsigned char **ap;
    unsigned char **pp;
    unsigned char *m;
    unsigned char *t;
    unsigned char *e;
    unsigned char *p;
    unsigned char *q;
    unsigned char *prev;
    unsigned char *sv;
    int snd;
    int i;
    int h;

    m = iwram_3001f30;
    sv = *(unsigned char **)(m + 0x14);
    Func_8097384();
    _PlaySound(0x82);
    bp = v;
    p = m + 0x58;
    i = 0xb;
loop1:
    t = *(unsigned char **)(m + 0x10);
    bp[0] = *(int *)(t + 8);
    bp[1] = *(int *)(t + 0xc) + (0x80 << 13);
    bp[2] = *(int *)(t + 0x10);
    Func_80974d8(bp);
    Func_809ba90(p, 0x8e << 1, bp[0], bp[2]);
    Func_809ba7c(p, Func_8098b10);
    Func_809ba70(p, 7);
    _Sprite_SetColorswap(*(int *)p, 9);
    *(int *)(p + 0x2c) = 0xb333;
    *(int *)(p + 0x28) = 0xb333;
    WaitFrames(2);
    i--;
    p += 0x48;
    if (i >= 0)
        goto loop1;
    t = *(unsigned char **)(m + 0x10);
    bp[0] = *(int *)(t + 8);
    bp[1] = *(int *)(t + 0xc) + (0x80 << 13);
    bp[2] = *(int *)(t + 0x10);
    vec3_translate(0x80 << 12, *(int *)m, bp);
    e = CreateParticleActor(0xd7, bp[0], bp[1], bp[2]);
    if (e == 0) {
        Func_809748c();
        return;
    }
    *(int *)(e + 0x1c) = 0x80 << 7;
    *(int *)(e + 0x18) = 0x80 << 7;
    *(short *)(e + 6) = *(int *)m;
    *(int *)(e + 0x30) = 0x80 << 11;
    *(int *)(e + 0x34) = 0x80 << 11;
    e[0x55] = 0;
    _Actor_SetAnim(e, 5);
    _Actor_SetColorswap(e, 3);
    while (*(int *)(e + 0x18) < (0x80 << 9)) {
        h = *(int *)(e + 0x18) + (0xa0 << 3);
        *(int *)(e + 0x1c) = h;
        *(int *)(e + 0x18) = h;
        WaitFrames(1);
    }
    WaitFrames(3);
    ap = arr;
    prev = 0;
    for (i = 2; i >= 0; i--) {
        q = CreateParticleActor(0xd7, *(int *)(e + 8), *(int *)(e + 0xc),
                                *(int *)(e + 0x10));
        ap[i] = q;
        if (q != 0) {
            *(int *)(q + 0x1c) = 0xf0 << 8;
            *(int *)(q + 0x18) = 0xf0 << 8;
            *(short *)(q + 6) = *(int *)m;
            *(int *)(q + 0x30) = 0x80 << 11;
            *(int *)(q + 0x34) = 0x80 << 11;
            q[0x55] = 0;
            _Actor_SetAnim(q, 5);
            _Actor_SetColorswap(q, 2);
            prev = Func_8096c48(*(void **)(q + 0x50), prev);
        }
    }
    snd = prev[0x1c];
    if (*(signed char *)(m + 0x20) != 0) {
        t = *(unsigned char **)(m + 0x10);
        bp[0] = *(int *)(t + 8);
        bp[1] = *(int *)(t + 0xc) + (0x80 << 13);
        bp[2] = *(int *)(t + 0x10);
        vec3_translate(0xe0 << 14, *(int *)m, bp);
    } else {
        bp[0] = *(int *)(m + 4);
        bp[1] = *(int *)(m + 8) + (0x80 << 13);
        bp[2] = *(int *)(m + 0xc);
    }
    _Actor_TravelTo(e, bp[0], bp[1], bp[2]);
    _Actor_SetScript(e, L9f12c);
    pp = ap;
    i = 2;
loop3:
    q = *pp++;
    if (q != 0) {
        WaitFrames(3);
        _Actor_TravelTo(q, bp[0], bp[1], bp[2]);
        _Actor_SetScript(q, L9f0b4);
    }
    i--;
    if (i >= 0)
        goto loop3;
    i = 0;
    while (*(int *)e != 0) {
        WaitFrames(1);
        i++;
        if (i > 0x3b)
            break;
    }
    if (sv != 0 && *(signed char *)(m + 0x35) == 0) {
        if (*(signed char *)(m + 0x34) != 0)
            *(int *)(sv + 0x28) = 0x80 << 12;
        bp[0] = *(int *)(sv + 8);
        bp[1] = *(int *)(sv + 0xc);
        bp[2] = *(int *)(sv + 0x10);
        vec3_translate(0x80 << 13, *(int *)m, bp);
        if (_TestCollision(sv, bp) == 0 && _Func_800d924(sv, bp) == 0) {
            *(int *)(sv + 0x34) = 0x80 << 9;
            *(int *)(sv + 0x30) = 0x80 << 9;
            _Actor_TravelTo(sv, bp[0], bp[1], bp[2]);
        }
    }
    ev = Func_808e4b4(0x50000005, 4, &cell);
    if (ev != 0)
        Func_8096b28(ev, *(int *)(gState + (0xfa << 1)), cell);
    WaitFrames(0xa);
    Func_809748c();
    WaitFrames(0x14);
    if (snd != 0x60)
        Func_8003f3c(snd);
}
