/* OvlFunc_968_20086a0 -- overlay 968 (rom_7f2f14), ovl_30_a_a_c_a_a.
 *
 * EXACT.  Re-run seven times:
 *
 *   OK OvlFunc_968_20086a0 -- 180 bytes, 84 encodings and 5 relocations identical
 *
 * Same family and same template as its batch-mate OvlFunc_968_20085e4
 * (src/overlays/rom_7f2f14/ovl_30_a_a_a_c_c_c_b.c): a particle emitter that
 * fills a 0x28-byte config struct on the frame and makes one or two
 * eight-argument calls to OvlFunc_968_2008118.  First draft was 5 of 84.
 *
 * LANDING: WHOLE, no split, no linker edit.
 *   asmfacts.py: WHOLE  convert directly
 *   One function, no data directives, no cross-object symbol.
 *     overlays/rom_7f2f14/overlay.ld:31
 *         asm/overlays/rom_7f2f14/ovl_30_a_a_c_a_a.o(.text)   <- VERBATIM
 *   tryc.makefile_flags() = set(); no wildcard reaches this stem.
 *   overlays/rom_7ed0a0 has a file of the same basename -- match the FULL PATH.
 *
 * ------------------------------------------------------------------- NEW ----
 * `x = base->f + t;` AND `x = base->f; x += t;` ARE DIFFERENT CODE, AND WHICH
 * ONE IS RIGHT IS DECIDED BY THE ADD'S OPERAND COUNT IN THE ROM.
 *
 * Both of this function's coordinate operands read a struct field and add a
 * shifted term.  The ROM loads STRAIGHT INTO THE ARGUMENT REGISTER and adds in
 * place; gcc's one-expression form loads into a scratch and adds three-operand:
 *
 *     rom    ldr r0,[r5,#8]  / lsls r3,#16 / adds r0,r0,r3     two-operand
 *     ours   ldr r2,[r5,#8]  / lsls r3,#16 / adds r0,r2,r3     three-operand
 *
 * This is the recorded add/shift-form rule ("the shift is destructive, so it is
 * a compound assignment") read on an ADD rather than a shift, and it costs the
 * SAME instruction count -- only the register names move, which is why it
 * survives every count-based screen.  `x = src->f8; x += m << 16;` is exact at
 * both sites.
 *
 * THE COMPOUND FORM HAS A COST AND IT IS NOT FREE: THE LOAD MUST NOT CROSS A
 * CALL.  At the second site the term comes from `__Random()`, so writing
 * `x = src->f8;` FIRST puts the load above the call and forces a spill -- the
 * candidate grows by an instruction and every relocation offset moves.  The
 * fix is to name the random term in its own local first:
 *
 *     d = (int)(__Random() * 9 >> 16) - 4;   <- the call happens here
 *     x = src->f8;                           <- load AFTER it
 *     x += d << 16;
 *
 * The same shape at the first site (`m = 8 - (int)(iwram_3001e40 & 0xf);`) also
 * puts the `subs` ahead of the `ldr` in source order, which is the ROM's
 * schedule.  Writing the load first and the mask second is 5 differing with the
 * REGISTERS right and the ORDER wrong -- the two halves of this lever are
 * separable and you need both.
 *
 * MEASURED WORSE / INERT (ref 84 encodings / 180 bytes, relocations silent
 * except where noted):
 *
 *   spelling                                                   differing
 *   --------------------------------------------------------  ---------
 *   THIS FILE (m/d named, load then +=)                          0  EXACT
 *   `m = (...) << 16; x = src->f8; x += m;` at both sites         0  (tie)
 *   one-expression `x = src->f8 + (...)` at both sites            5
 *   compound at site 1 only                                       5
 *   compound at site 2 only                                       longer, all
 *                                                                 relocations moved
 *   `d = rand; x = src->f8; x += (d - 4) << 16;` (subtract late)  2
 *   `extern volatile unsigned int iwram_3001e40;`                 0  (INERT)
 *
 * THE `volatile` LEVER IS INERT HERE AND THAT IS WORTH RECORDING.  The template
 * OvlFunc_968_200c968 NEEDS `volatile` on iwram_3001e40 to stop gcc CSEing its
 * two reads.  This function also reads it twice and needs nothing, because a
 * call to OvlFunc_968_2008118 falls BETWEEN the two reads and gcc cannot common
 * a global load across a call.  So the recorded volatile lever is not "the ROM
 * reads it twice" -- it is "the ROM reads it twice with NO CALL IN BETWEEN".
 * The plain declaration ships.
 *
 * WHAT NEEDED NOTHING.  `-((int)(__Random() * 8 >> 16) * 0x3333)` reproduces
 * the ROM's shift-and-add multiply chain (u*3, *17, *257) and its trailing
 * `negs`; r8 doing double duty (0, then 0xa0<<12) and r10 holding the 0xf mask
 * are gcc's own allocation with no pin; `z` named once and passed to two
 * argument slots.
 *
 * -- worked in scratch_elev/b253/f968/86a0
 */
struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x28 - 0x10];
};

struct A { unsigned char pad00[8]; int f8; int fc; int f10; };

extern unsigned int iwram_3001e40;

extern unsigned int __Random(void);
extern void OvlFunc_968_2008118(int a, int b, int c, int d,
                                int e, int f, int g, struct P *p);

int OvlFunc_968_20086a0(struct A *src)
{
    struct P t;
    int z;
    int x;
    int n;
    int v;
    int m;
    int d;

    t.f8 = 0xcccc;
    t.fc = 0xcccc;
    z = 0;
    t.f0 = z;
    n = -((int)(__Random() * 8 >> 16) * 0x3333);
    m = 8 - (int)(iwram_3001e40 & 0xf);
    x = src->f8;
    x += m << 16;
    OvlFunc_968_2008118(x, src->fc + (0xd0 << 13), src->f10, 0, n, z,
                        0xa0 << 12, &t);
    v = iwram_3001e40 & 0xf;
    if (v == 0) {
        t.f8 = 0x80 << 8;
        t.fc = 0x80 << 8;
        d = (int)(__Random() * 9 >> 16) - 4;
        x = src->f8;
        x += d << 16;
        OvlFunc_968_2008118(x, src->fc, src->f10, 0, v, v, 0xa0 << 12, &t);
    }
    return 0;
}
