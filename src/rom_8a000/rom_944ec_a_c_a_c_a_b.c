/* GetMarsDjinni  --  0x08095dd0  (read from goldensun.elf with
 * `arm-none-eabi-nm goldensun.elf | grep GetMarsDjinni`, NOT from the .s stem).
 *
 * The second of the two functions in goldensun/asm/rom_8a000/rom_944ec_a_c_a_c_a.s
 * (the other is Func_8095c08 @ 0x08095c08, still asm).  179 ROM instructions,
 * 186 encodings, 460 bytes of .text including a 6-word pool at the end.
 *
 * TWIN: GetJupiterDjinni (0x08095a44), already elevated in
 * src/rom_8a000/rom_944ec_a_c_a_a_c_a_b.c -- the given template, and a true
 * near-twin: same opening actor fetch + null return, same v[3] position copy
 * into Func_80974d8, same 24-iteration Func_809ba90/809ba7c/809ba70 sprite
 * loop, same gState+0x1f4 tail.  GetVenusDjinni (0x08096140) and
 * GetMercuryDjinni (0x080965a8) exist in the ELF and are still unelevated;
 * neither is in this .s.
 *
 * LEVERS THAT MATTERED, each re-measured here rather than transplanted:
 *
 *  1. gState offset built at RUNTIME, twice.  The template's documented lever
 *     ("the gState offset needed a local pointer so it is built at runtime
 *     rather than folded into ldr =gState+500") is necessary here too, but the
 *     template's `gp = gState; g = gp + (0xfa << 1);` is NOT sufficient:
 *       - the non-destructive form emits `add rD, rB, rN`; the ROM has the
 *         destructive `add rB, rN`, which wants `g = gState; g += 0x1f4;`.
 *       - ONE variable for both sites makes ONE pseudo spanning the whole body,
 *         so it lands in a callee-saved register and shifts every other
 *         allocation by one.  Two locals (g0 at the top, g after the loop) give
 *         the top one the ROM's short-lived r3.  67 differing lines -> 23.
 *
 *  2. The three loop counters are `i = 0x17; ... i--; if (i >= 0) goto`, as in
 *     the template.
 *
 *  3. Loops 2 and 3 hoist their stored constant.  `q = base; val = N;
 *     q += 0x98; i = 0x17;` -- the split-pointer interleave -- puts `mov r1,#N`
 *     between `mov r2,r10` and `add r2,#0x98` exactly as the ROM has it, and
 *     stops gcc from CSEing `base + 0x98` across the two loops into a register
 *     that would otherwise clobber the iwram base.  67 -> 38.
 *
 *  4. ONE MATERIALISATION IS NOT ONE VARIABLE, and the reverse.  The tail
 *     writes e2 (a HIGH register) through three low-register copies in the ROM
 *     and only two in the naive spelling.  The fix is not a spelling of the
 *     stores at all -- every reordering and every naming of the 0 and of
 *     0x80<<9 compiled to identical code (48 permutations, all inert).  What
 *     moves it is WHICH pseudo carries the zero: reusing `g`, the pseudo the
 *     allocator has already put in r5, reproduces the ROM's `mov r5,#0 /
 *     mov r3,r8 / str r5,[r3,#0x6c]` and forces the third copy.  185 lines
 *     (one SHORT) -> 186.
 *
 *  5. THE ALIAS-SET LEVER, and it is the whole remaining residue.  The ROM has
 *
 *          mov r2, r5 / str r3,[r5,#0x6c] / add r2,#0x64 / mov r3,#0 / strh
 *
 *     and every plain spelling gives `mov r2,r5 / add r2,#0x64 / str`.  At -O2
 *     gcc-2.96 turns on -fstrict-aliasing; the `void *` store at +0x6c and the
 *     `unsigned short` store at +0x64 are in different alias sets, the
 *     post-reload scheduler sees no memory dependence between them, and the
 *     `add` (whose chain reaches the strh) outranks the dependence-free `str`.
 *     Restoring the dependence puts them on equal priority and program order
 *     decides.  This is the shape docs/elevation.md records under
 *     "gcse hashes the MEMORY ALIAS SET" as reachable only by `volatile` on
 *     both sides or by the -fno-strict-aliasing rule.  A THIRD escape works and
 *     is used here: reach ONE of the two through a UNION member.  See the NEW
 *     FINDING note below.
 *
 * MEASURED WORSE / INERT (all against this same reference):
 *   - template spelling `gp = gState; g = gp + (0xfa<<1);`            67 differ
 *   - one `g` local for both gState sites                             67 differ
 *   - `q = base + 0x98;` (no split interleave), both loops            67 differ
 *   - `*(short *)(e + 0x64) = 0;` with a literal 0: gcc emits
 *     `ldrh r3, .Lpool` for the HImode constant, which adds a pool
 *     word, splits the literal pool mid-function and costs a `b`
 *     over it -- 188 encodings against 186, two gState pool words
 *   - 48 permutations of the tail (0 and 0x80<<9 named / not named,
 *     separate pointer locals, reordered): ALL byte-identical
 *   - `ep = e + 0x64` before / after the str, `ep += 0x64` before /
 *     after, `*(unsigned short *)(e + 0x64)` direct: 2 or 5 differ,
 *     never 0 -- the scheduler, not the spelling
 *   - `volatile` on ONE side only (either side): 2 differ
 *   - `union { unsigned short h; unsigned char b[2]; }` on the strh
 *     side: 2 differ -- the union must CONTAIN the other side's type
 *   - `struct { void *fp; unsigned short h; }` members instead of a
 *     union: 5 differ -- struct members keep the FIELD's alias set
 *
 * NEW FINDING -- A UNION MEMBER ACCESS IS A THIRD SOURCE-LEVEL WAY TO ADD A
 * MEMORY DEPENDENCE, and unlike the recorded `char`-lvalue trick it works for
 * any mode.  docs/elevation.md's batch-238 bound says "The only source-level
 * escape is `volatile` on BOTH sides of the pair"; the batch-187 entry adds the
 * `char` lvalue (alias set 0), which cannot help here because a `char` lvalue
 * cannot produce a `strh`.  Measured, four controls:
 *     union {void *fp; unsigned short h;} on the HALFWORD store,
 *         plain `*(void **)` on the other                          -> exact
 *     the same union tag on the POINTER store, plain halfword      -> exact
 *     two DIFFERENT union tags, one on each side                   -> exact
 *     union {int i; unsigned short h;} on the halfword store,
 *         other side still `*(void **)`                            -> 2 differ
 *     ...the same union, other side respelled `*(int *)`           -> exact
 *     struct {void *fp; unsigned short h;} members, both sides     -> 5 differ
 * So the rule is not "unions alias everything".  A COMPONENT_REF of a UNION
 * carries the UNION's alias set, and gcc-2.96's record_component_aliases makes
 * every member type's set a SUBSET of it; alias_sets_conflict_p then reports a
 * conflict against any access whose type is a member of that union -- and only
 * those.  A COMPONENT_REF of a STRUCT carries the FIELD's set instead, which is
 * why the struct spelling measures inert.  That is the same subset machinery
 * the "distinct alias set per struct tag" entry uses for SEPARATION, read in
 * the other direction.
 *
 * FLAG-GROUP ALTERNATIVE -- SAID LOUDLY.  This function ALSO matches, with the
 * union removed and both stores written plainly, under the existing
 * ALIAS_CFLAGS group (`-fno-strict-aliasing`) already in the Makefile for
 * rom_ad274_a.c, ovl_30_c_c_a_c_c_c_c_c_c_c_c_b.c, ovl_314_a_a_a_c.c and
 * ovl_314_c_a_c_c.c.  Verified: `tryc --cflags -fno-strict-aliasing` on the
 * plain spelling is exact.  The union spelling is shipped instead because it
 * needs NO Makefile rule; if a reviewer prefers the flag (the Makefile's own
 * ovl_314_c_a_c_c note argues the flag is "the honest description"), delete the
 * union and add a per-object rule.  Nothing else in this TU depends on strict
 * aliasing either way -- there is only one function in it.
 */
union ActorSlot { void *fp; unsigned short h; };
extern unsigned char gState[];
extern unsigned char *iwram_3001f30;
extern unsigned char *MapActor_GetActor(int slot);
extern void Func_80958a8(void);
extern void _Func_80b0840(int a);
extern void WaitFrames(int n);
extern void Func_8092adc(int a, int b, int c);
extern void _PlaySound(int id);
extern void Func_80925cc(int a, int b);
extern void Func_8095bac(void);
extern void Func_8095bd8(void);
extern void _Actor_SetAnim(unsigned char *a, int n);
extern void Func_80974d8(int *v);
extern void Func_809ba90(unsigned char *p, int a, int b, int c);
extern void Func_8095c08(void);
extern void Func_809ba7c(unsigned char *p, void (*f)(void));
extern void Func_809ba70(unsigned char *p, int n);
extern void _Sprite_SetColorswap(int a, int b);
extern unsigned int Random(void);
extern unsigned int __udivsi3(unsigned int a, unsigned int b);
extern void Func_8095b8c(void);
extern void _Func_80b0894(void);
extern void Func_80958e4(void);

void GetMarsDjinni(int slot)
{
    int v[3];
    int *bp;
    unsigned char *e;
    unsigned char *e2;
    unsigned char *base;
    unsigned char *p;
    unsigned char *q;
    unsigned char *g;
    unsigned char *g0;
    unsigned char *ep;
    unsigned short zero;
    int val;
    int i;

    e = MapActor_GetActor(slot);
    g0 = gState;
    g0 += (0xfa << 1);
    e2 = MapActor_GetActor(*(int *)g0);
    if (e == 0)
        return;
    Func_80958a8();
    base = iwram_3001f30;
    _Func_80b0840(0x201090);
    WaitFrames(0x1e);
    Func_8092adc(slot, 0x80 << 7, 0);
    WaitFrames(0x14);
    _PlaySound(0xad);
    Func_80925cc(slot, 1);
    _PlaySound(0xae);
    Func_80925cc(slot, 1);
    _PlaySound(0xaf);
    Func_80925cc(slot, 1);
    WaitFrames(0x14);
    _PlaySound(0x8c);
    ep = e;
    *(void **)(e + 0x6c) = Func_8095bac;
    ep += 0x64;
    zero = 0;
    ((union ActorSlot *)ep)->h = zero;
    WaitFrames(0x50);
    *(void **)(e + 0x6c) = Func_8095bd8;
    _Actor_SetAnim(e, 3);
    bp = v;
    bp[0] = *(int *)(e + 8);
    bp[1] = *(int *)(e + 0xc);
    bp[2] = *(int *)(e + 0x10);
    Func_80974d8(bp);
    p = base + 0x58;
    i = 0x17;
loop1:
    Func_809ba90(p, 0x8e << 1, bp[0], bp[2]);
    Func_809ba7c(p, Func_8095c08);
    Func_809ba70(p, 7);
    _Sprite_SetColorswap(*(int *)p, 0xa);
    *(int *)(p + 0x2c) = __udivsi3(Random(), 3) + (0x80 << 9);
    *(int *)(p + 0x28) = *(int *)(p + 0x2c);
    i--;
    WaitFrames(1);
    p += 0x48;
    if (i >= 0)
        goto loop1;
    WaitFrames(0x3c);
    g = gState;
    g += (0xfa << 1);
    Func_8092adc(*(int *)g, 0x80 << 7, 0);
    WaitFrames(0x14);
    _Actor_SetAnim(MapActor_GetActor(*(int *)g), 0x1c);
    WaitFrames(0x14);
    q = base;
    val = 2;
    q += 0x98;
    i = 0x17;
loop2:
    if (*(signed char *)(q + 5) != 0)
        *q = val;
    i--;
    q += 0x48;
    if (i >= 0)
        goto loop2;
    WaitFrames(0x3c);
    *(void **)(e2 + 0x6c) = Func_8095b8c;
    WaitFrames(0x64);
    q = base;
    val = 5;
    q += 0x98;
    i = 0x17;
loop3:
    if (*(signed char *)(q + 5) != 0)
        *q = val;
    i--;
    q += 0x48;
    if (i >= 0)
        goto loop3;
    WaitFrames(0xa);
    g = 0;
    *(void **)(e2 + 0x6c) = g;
    *(int *)(e2 + 0x18) = 0x80 << 9;
    *(int *)(e2 + 0x1c) = 0x80 << 9;
    WaitFrames(0x1e);
    _Func_80b0894();
    Func_80958e4();
}
