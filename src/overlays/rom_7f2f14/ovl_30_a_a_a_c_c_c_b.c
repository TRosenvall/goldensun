/* OvlFunc_968_20085e4 -- overlay 968 (rom_7f2f14), ovl_30_a_a_a_c_c_c_b.
 *
 * EXACT on the SECOND draft.  Re-run seven times:
 *
 *   ~~ relocation _divsi3_RAM / __divsi3 is ONE symbol (same address in the
 *      linked ELF)
 *   OK OvlFunc_968_20085e4 -- 172 bytes, 78 encodings and 7 relocations identical
 *
 * (The `~~` line is objcmp's own overlay-alias note, not a defect: every
 * overlay .ld carries `__divsi3 = _divsi3_RAM;`.)
 *
 * TEMPLATE, and it was worth the whole function: OvlFunc_968_200c968 in
 * src/overlays/rom_7f2f14/ovl_30_c_c_c_c_a.c -- same overlay, same
 * `int f(struct A *src)` shape, same `iwram_3001e40 & N` guard, same three
 * `src->fN + ((rand - k) << 16)` operands, same eight-argument tail call to
 * OvlFunc_968_2008118 with a 0x28-byte config struct at sp+0x10.  Transcribing
 * it with this function's constants was 78 of 78 encodings right on the FIRST
 * screen; only the divide was wrong.
 *
 * LANDING: WHOLE, no split, no linker edit.
 *   asmfacts.py: WHOLE  convert directly
 *   One function, no data directives, no cross-object symbol.
 *     overlays/rom_7f2f14/overlay.ld:29
 *         asm/overlays/rom_7f2f14/ovl_30_a_a_a_c_c_c_b.o(.text)  <- VERBATIM
 *   tryc.makefile_flags() = set(); no wildcard reaches this stem.
 *   SIX OTHER OVERLAYS HAVE A FILE OF THIS BASENAME (rom_799abc, rom_7a04ac,
 *   rom_7d30e0, rom_7e3e08, rom_7ed0a0, rom_7ef4f4) -- the .ld edit is a
 *   no-op here, but any future grep for this object MUST match the full path.
 *
 * ------------------------------------------------------------------- NEW ----
 * A DIVIDE CAN BE THE *ONLY* THING WRONG, AND IT SHOWS UP AS A RELOCATION WITH
 * SIZE AND ENCODINGS BOTH SILENT.
 *
 * The first draft reported NO size difference, NO encoding difference, and one
 * relocation differing:
 *
 *   ref  ... ['00000074', 'R_ARM_THM_CALL', '_divsi3_RAM'] ...
 *   ours ... ['00000074', 'R_ARM_THM_CALL', '__udivsi3']   ...
 *
 * `__Random()` is declared `unsigned int` (it must be -- every `rand * k >> 16`
 * in this family relies on the unsigned shift), so the whole dividend
 * expression is unsigned and gcc picks the UNSIGNED helper.  The two helpers
 * have identical call sequences, so the divide is invisible to tryc.py and to
 * every byte-level check except the relocation list.  One `(int)` cast on the
 * dividend -- exactly the spelling the twin
 * src/overlays/rom_7f2f14/ovl_30_a_c_c_a_c_c.c already uses -- is the fix.
 *
 * This sharpens the recorded "the relocation line partitions the residue":
 * a relocation differing with encodings AND size silent is neither the CSE case
 * nor the ordering case.  It is a SIGNEDNESS case, it is one cast wide, and it
 * can only appear on a call to a compiler helper.  Worth checking first
 * whenever `_divsi3_RAM` / `__udivsi3` / `__aeabi_*` is in the reference.
 *
 * MEASURED WORSE / INERT (ref 78 encodings / 172 bytes):
 *
 *   spelling                                               result
 *   ----------------------------------------------------  -------------------
 *   THIS FILE                                              0   EXACT
 *   unsigned dividend (no `(int)` cast)                    __udivsi3, otherwise
 *                                                          byte-identical
 *
 * WHAT NEEDED NOTHING.  `iwram_3001e40` declared PLAIN `int`: this function
 * reads it ONCE, so the `volatile` that the template OvlFunc_968_200c968 needs
 * for its two reads is not wanted here -- the recorded volatile lever is about
 * defeating CSE between two loads and there is only one.  The three high
 * registers in the prologue (r8 for &t, r10 for the parameter) are gcc's own
 * choice with no pin.  The struct stores in ROM order (f0, f8, fc, f4) and the
 * `3 - (rand*2 >> 16)` / `(rand*9 >> 16) - 4` / `0x20 - (rand*32 >> 16)` forms
 * all fell out first time.
 *
 * -- worked in scratch_elev/b253/f968/85e4
 */
struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x28 - 0x10];
};

struct A { unsigned char pad00[8]; int f8; int fc; int f10; };

extern int iwram_3001e40;

extern unsigned int __Random(void);
extern void OvlFunc_968_2008118(int a, int b, int c, int d,
                                int e, int f, int g, struct P *p);

int OvlFunc_968_20085e4(struct A *src)
{
    struct P t;
    int v;
    int x;
    int y;
    int n;

    v = iwram_3001e40 & 7;
    if (v == 0) {
        t.f0 = 3 - (__Random() * 2 >> 16);
        t.f8 = 0x6666;
        t.fc = 0x6666;
        t.f4 = 0xe;
        x = src->f8 + (((__Random() * 9 >> 16) - 4) << 16);
        y = src->fc + ((0x20 - (__Random() * 32 >> 16)) << 16);
        n = (int)(((__Random() * 5 >> 16) << 16) + (0xa0 << 11)) / 0xa;
        OvlFunc_968_2008118(x, y, src->f10, 0, n, v, 0xb0 << 12, &t);
    }
    return 0;
}
