/* OvlFunc_968_2009d48 -- NOT MATCHING, a LENGTH gap: ref 171 instructions against ours 172 (+1).
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch_elev/b248/f2009d48/final.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * THE LENGTH-GAP CRITERION APPLIES, and it is this tree's most productive
 * targeting instrument -- four batches run on it closed 8 of 15, 7 of 25,
 * 8 of 25 and 10 of 32, and LOCATED the missing-or-extra instruction on
 * essentially every target.  Our side is 1 instruction(s) LONGER, so the
 * search is what lets gcc REUSE a value -- see docs/elevation.md, 'OUR STREAM BEING
 * SHORTER IS A SIGNATURE'.
 * No encoding figure exists until the lengths agree.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7f2f14/2009d48.c \
 *     asm/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_a.s --func OvlFunc_968_2009d48
 *
 * The original prose park is preserved below.
 */
/* OvlFunc_968_2009d48 -- PARKED at 149 differing encodings.
 * ref: asm/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_a.s (2nd of 2 functions)
 *
 *   XX ENCODINGS differ in 149 place(s) (ref 206, ours 206)
 *      first at index 7: ref b083  ours 2200
 *   SIZE exact (480 bytes).  INSTRUCTION COUNT exact.  The relocation LIST is
 *   the ROM's exactly -- same 31 entries, same symbols, same order; only the
 *   offsets shift, so every call site and both pool symbols are right.
 *
 * ONE BLOCKER, and it is visible in the first differing encoding: the ROM has
 * `sub sp, #0xc` and spills the OvlFunc_968_2008098 result to [sp, #8]; gcc
 * keeps it in r11 and emits `sub sp, #8`.  Every other difference in the 149 is
 * the resulting rename (ROM a=r6 t=r7 p=r11, ours a=r5 t=r6 p=r7 e=r11).
 *
 * The interference graph explains it and gives the discriminator.  Nine
 * long-lived pseudos (a, 0x2d, q=&a->f23, p, t, x, z, r=&a->f55, the constant 2)
 * all conflict with `e`.  gcc colours them with SIX registers because p and r do
 * not overlap (they share r7) and 0x2d and t do not overlap (they share r6),
 * leaving r11 free for e.  The ROM's build did NOT share p with r, needed seven,
 * and spilled e.  Defining `r = &a->f55;` anywhere BEFORE the `p->f10` test
 * makes p and r conflict and reproduces `sub sp, #0xc` exactly -- confirmed on
 * four placements (w1/c1/c2/c3 in this directory).  It costs +2 encodings,
 * because gcc then puts r in r11 and needs `mov r2,#0x55 / add r2,r5 / mov r11,r2`
 * where the ROM has `mov r5,r6 / add r5,#0x55`.  So the spill is reachable and
 * the residue is which of p/r lands in the high register.
 *
 * MEASURED INERT: two-locals-before-the-store at the else-arm Func_8010704;
 * declaration order; `void *e[1]` (gcc scalarises it into r11); taking &e;
 * `int e`; `struct Actor *e`; hoisting x and z above the if/else; a named
 * variable for the shared 0x2d; an explicit copy of p before its dereference.
 * MEASURED WORSE: `*q |= m` spellings other than `*q = *q | m` (+/-2 encodings).
 */
struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x22 - 0x14];
    unsigned char f22;
    unsigned char f23;
    unsigned char pad24[0x44 - 0x24];
    int f44;
    unsigned char pad48[0x48 - 0x48];
    int f48;
    unsigned char pad4c[0x55 - 0x4c];
    unsigned char f55;
    unsigned char pad56[0x59 - 0x56];
    unsigned char f59;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetBehavior(int slot, void *script);
extern void __MapActor_WaitScript(int slot);
extern void __Actor_SetSpriteFlags(struct Actor *a, int n);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __PlaySound(int id);
extern void __DeleteActor(void *e);
extern void __Func_8010560(unsigned char *s, int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8092b08(int a, int b);
extern void *OvlFunc_968_2008098(int x, int y, int z, int kind);
extern void OvlFunc_968_200894c(struct Actor *a);
extern unsigned char gScript_968__0200d3c4[];
extern unsigned char L5ce8[] __asm__(".L5ce8");

void OvlFunc_968_2009d48(void)
{
    struct Actor *a;
    struct Actor *p;
    unsigned char *q;
    unsigned char *r;
    unsigned char *bp;
    void *e;
    int x;
    int z;
    int t;

    e = 0;
    a = __MapActor_GetActor(9);
    p = __MapActor_GetActor(0);
    __CutsceneStart();
    __Func_8010704(0x6d, 0x2b, 7, 5, 0x2d, 0x2b);
    q = &a->f23;
    if ((*q & 2) != 0)
        __Func_8010704(0x2d, 0x2d, 1, 1, 0x2e, 0x2d);
    else
        __Func_8010704(0x30, 0x2a, 1, 1, a->f8 >> 20, a->f10 >> 20);
    x = a->f8 >> 20;
    if (x != 0x2e)
        goto done;
    z = a->f10 >> 20;
    if (z != 0x2d)
        goto done;
    t = __GetFlag(0x301);
    if (t != 0)
        goto done;
    if ((p->f10 >> 20) <= 0x2d) {
        e = OvlFunc_968_2008098(0xba << 18, 0, 0xb0 << 18, 0x14);
        __Func_8092b08(0, 3);
    }
    __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
    a->f22 = t;
    r = &a->f55;
    *r = 3;
    a->f48 = 0x1999;
    a->f44 = t;
    __Func_8010704(0x2b, 0x2d, 1, 1, x, z);
    OvlFunc_968_200894c(a);
    __PlaySound(0xbc);
    *r = t;
    a->fc = 0xfff00000;
    __Func_8092b08(9, 3);
    *q = 2;
    __Func_8010704(0x2d, 0x2d, 1, 1, x, z);
    bp = &__MapActor_GetActor(0)->f23;
    { unsigned char m1 = 1; *bp = m1 | *bp; }
    __DeleteActor(e);
    a->f59 = t;
    {
        unsigned char m = 2;
        *q = *q | m;
        __MapActor_GetActor(0xa)->f59 = t;
        bp = &__MapActor_GetActor(0xa)->f23;
        *bp = m | *bp;
    }
    __MapActor_SetPos(0xa, 0xca << 18, 0xb6 << 18);
    __MapActor_SetBehavior(0xa, gScript_968__0200d3c4);
    __MapActor_WaitScript(0xa);
    __CutsceneWait(0x1e);
    __PlaySound(0x9e);
    __Func_8010560(L5ce8, 0x6e, 0x29);
    __Func_8010704(0x2e, 0x29, 1, 1, x, 0x2a);
    __SetFlag(0x301);
done:
    __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_968_2009d48 -- 0x02009d48,
 * asm/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_a.s
 *
 * 149 differing encodings out of 206 -- but read the shape before the number.
 * SIZE IS EXACT (480 bytes), the instruction count is exact, and the RELOCATION
 * LIST IS THE ROM'S EXACTLY: same 31 entries, same symbols, same order, with
 * only the offsets shifted. Every call site and both pool symbols
 * (gScript_968__0200d3c4, .L5ce8) are already correct.
 *      first at index 7: ref b083  ours 2200
 * Candidate at scratch_elev/b248/f2009d48/final.c.
 *
 * ONE BLOCKER, AND IT IS THE FIRST DIFFERING ENCODING. The ROM emits
 * `sub sp, #0xc` and SPILLS the OvlFunc_968_2008098 result to [sp, #8]; gcc
 * keeps it in r11 and emits `sub sp, #8`. All 149 are the resulting rename --
 * ROM a=r6 t=r7 p=r11 against ours a=r5 t=r6 p=r7 e=r11.
 *
 * BLOCKER CLASS: INTERFERENCE-GRAPH COLOURING. Nine long-lived pseudos (a, the
 * shared 0x2d, q=&a->f23, p, t, x, z, r=&a->f55, and the constant 2) all
 * conflict with e. gcc colours them with SIX registers because p/r do not
 * overlap (they share r7) and 0x2d/t do not overlap (they share r6), which
 * leaves r11 free for e. The ROM's build did not share p with r, needed seven,
 * and spilled e.
 *
 * THE DISCRIMINATOR IS CONFIRMED, AND THE SPILL IS REACHABLE. Defining
 * `r = &a->f55;` anywhere before the `p->f10` test makes p and r conflict and
 * reproduces `sub sp, #0xc` EXACTLY -- verified at four different placements.
 * It costs +2 encodings, because gcc then puts r in r11
 * (`mov r2,#0x55 / add r2,r5 / mov r11,r2`) where the ROM has
 * `mov r5,r6 / add r5,#0x55`.
 *
 * So this is NOT "allocation looks different". The spill is producible on
 * demand; the residue is narrowed to WHICH OF p/r TAKES THE HIGH REGISTER, and
 * that is where the next round should start. Anyone re-screening should begin
 * from final.c plus the r-definition, at 2 encodings, not from scratch.
 *
 * MEASURED INERT: two locals before the store at the else-arm __Func_8010704;
 * declaration order; `void *e[1]` (gcc scalarises it into r11); `&e`; `int e`;
 * `struct Actor *e`; hoisting x/z above the if/else; a named variable for the
 * shared 0x2d; an explicit copy of p before its dereference.
 * MEASURED WORSE: any `*q |= m` spelling other than `*q = *q | m`, +/-2.
 *
 * LANDING SHAPE IF CLOSED: one src/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_a.c
 * holding BOTH functions in the .s (it has two and no data sections).
 * overlay.ld:71 already names the object and MUST STAY NAMING asm/.
 * makefile_flags() is empty, so default -O2.
 *
 * THE SIBLING WAS SIZED AND DELIBERATELY NOT ATTEMPTED. OvlFunc_968_2009af0 is
 * 248 instructions -- a 480-iteration particle loop with three __Random
 * multiply chains, four distinct high registers live through the loop, a
 * mid-loop __CopyMapTiles fork and a Func_8012330 fade. With 2009d48 unclosed
 * the file cannot land whole regardless, so the larger one was left.
 *)

*/
#endif
