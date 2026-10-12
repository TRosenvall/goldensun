/* OvlFunc_968_2009d48 -- STILL PARKED, but at a REAL DISTANCE for the first time:
 * 40 of 206 stream encodings with the INSTRUCTION COUNTS NOW AGREEING (171/171).
 *
 * Park claimed (batch 338 recovery header): LENGTH gap, ref 171 against ours 172
 * (+1).  RE-DERIVED AT THE HEAD OF BATCH 339 AND CONFIRMED: ref 171 / ours 172,
 * 149 of 206, relocation list symbol- and order-identical with the offsets
 * shifted.  THE LENGTH GAP IS NOW CLOSED and the 40 is a distance, not a phase.
 *
 * THE +1 WAS ONE SPELLING, AND THE PRESERVED PROSE HAD IT EXACTLY BACKWARDS.
 * The prose recorded "MEASURED WORSE: any `*q |= m` spelling other than
 * `*q = *q | m`, +/-2".  Writing the literal at that one site --
 *     *q = *q | 2;        instead of   { unsigned char m = 2; *q = *q | m; ... }
 * -- closes the length gap outright.  The window is the second `*q` update:
 *     ROM    mov r2,r10 / ldrb r3,[r2] / mov r5,#2 / orr r3,r5 / strb r3,[r2]
 *     park   mov r3,r10 / ldrb r2,[r3] / mov r5,#2 / mov r3,r5 / orr r3,r2
 *            / mov r2,r10 / strb r3,[r2]
 * The ROM accumulates INTO the loaded value's register and keeps one low copy
 * of r10; the park's block-scoped `m` makes gcc accumulate into the CONSTANT's
 * register, which costs a `mov r3,r5` and a second `mov r2,r10`.  Note the park
 * already spells the FIRST such update as `*bp = m1 | *bp` and that window
 * matches -- the two sites had been written in opposite orders.
 *
 * BEST BODY (this file): `*q = *q | 2;` plus `*bp = 2 | *bp;` for the 0xa
 * actor's flag.  40 of 206, 171/171 instructions, production flags, no pins.
 * The variant with only the first change is 51 of 206, also 171/171.
 *
 * THE PARK'S "CONFIRMED DISCRIMINATOR" IS REFUTED AT THE NEW LENGTH, and this
 * is the important correction because the prose told the next agent to start
 * from it ("Anyone re-screening should begin from final.c plus the
 * r-definition, at 2 encodings, not from scratch").  Defining `r = &a->f55;`
 * before the `p->f10` test does reproduce `sub sp, #0xc`, as the prose says --
 * but it ADDS instructions.  Measured on the parked body it gives 173 against
 * the reference's 171 (seven placements: after_t / after_tt / after_xt /
 * after_z / after_zt all 138, after_f1 162, after_q 161, after_a 172,
 * after_cs 169, after_p 170, after_e 178); crossed with the length-closed body
 * it gives 173 or 174 at every one of seven placements, 144 at best.  So the
 * spill is reachable and it is NOT on the path -- the prose's "+2 encodings"
 * was +2 INSTRUCTIONS, and on a stream that was already +1 the figure it
 * quoted was measuring misalignment.
 *
 * WHERE THE REMAINING 40 IS.  The register assignment: ROM a=r6 t=r7 p=r11 with
 * `e` spilled to [sp,#8] and `sub sp,#0xc`; ours a=r5 t=r6 p=r7 e=r11 and
 * `sub sp,#8`.  The prose's interference-graph reading is sound as far as it
 * goes -- nine long-lived pseudos conflict with `e`, gcc colours them with six
 * registers because p/r and 0x2d/t each share one, leaving r11 for `e`.  What it
 * does not supply is the PRECONDITION: which allocno is reached first.
 *
 * THE AXIS THAT PAID TWICE THIS BATCH AND HAS NOT BEEN TRIED HERE IS
 * DEFINITION PLACEMENT.  allocno_compare (global.c:598-620) scores
 * floor_log2(n_refs)*n_refs/live_length and find_reg hands out registers in
 * REG_ALLOC_ORDER (arm.h:988-995: 3,2,1,0,12,14,4,5,6,7,8,10,9,11 -- r11 LAST),
 * so the ROM's p=r11 means p is the LOWEST-priority allocno of the group.  `p`
 * has two refs (`p = __MapActor_GetActor(0);` and the single `p->f10` test), so
 * floor_log2(2)*2 = 2 against `r`'s floor_log2(3)*3 = 3 and `a`'s much larger
 * count.  Reordering the declarations cannot move this -- it was measured inert
 * by the prose and that part stands -- but MOVING A DEFINITION changes where its
 * range STARTS, which is the only source handle on live_length.  On
 * OvlFunc_945_200b51c this batch the same park-level claim ("all six orders
 * screened, the lever does not scale to four") was true about ORDER and false
 * about PLACEMENT, and moving two definitions to their first use landed it
 * byte-identical.  START THERE: sweep where `p` is defined (it is currently the
 * third statement and its only use is two thirds of the way down), and where
 * `t`, `x` and `z` are defined, against the body in this file.
 *
 * MEASURED INERT on this body, all 40 of 206: -fno-gcse, -fno-strict-aliasing,
 *   -fno-strength-reduce, -fno-rerun-cse-after-loop, -fno-rerun-loop-opt,
 *   -fno-cse-follow-jumps.
 * MEASURED WORSE on this body: -fno-schedule-insns2 (77), -O1 (170 instructions,
 *   158), -ffixed-r7 (176 instructions, 138), -fno-expensive-optimizations
 *   (172 instructions, 149 -- it undoes the length fix).
 * MEASURED INERT at the parked length: `*q = m | *q`, `*q |= m`,
 *   `*bp = *bp | m`, `*bp = *bp | m1`, and all four crossings of those -- every
 *   one 149 of 206 at 172 instructions.  Only the LITERAL moves it.
 * AUTOSWEPT on the parked body: 8 flag groups (-ffixed-r7 145, -O1 158,
 *   -fno-schedule-insns2 150, rest 149); 40 anchor placements, best anchor30 at
 *   145 -- all of those on a stream one instruction LONGER than the reference,
 *   so they were measuring misalignment.  Not reproduced on the new body.
 *
 * THE "NEITHER SHIPS ALONE" PREMISE IN THE PROSE IS FALSE, as the batch brief
 * says it always is.  The prose closes with "With 2009d48 unclosed the file
 * cannot land whole regardless, so the larger one was left" and on that basis
 * the 248-instruction sibling OvlFunc_968_2009af0 was never attempted.
 * tools/split_s.py cuts before/target/after, so EITHER function cuts out alone
 * and 2009af0's size is not hostage to this park.  tools/piece.py confirms the
 * piece is exactly these two and nothing else.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7f2f14/2009d48.c \
 *     asm/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_a.s --func OvlFunc_968_2009d48
 *
 * THE FIGURE IS EVIDENCE; ANY DIAGNOSIS ABOVE IS A HYPOTHESIS.
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
        *q = m | *q;
        __MapActor_GetActor(0xa)->f59 = t;
        bp = &__MapActor_GetActor(0xa)->f23;
        *bp = 2 | *bp;
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
