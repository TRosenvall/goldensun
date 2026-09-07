/* OvlFunc_906_20084f4  --  src/overlays/rom_79aad8/ovl_314_c_c_c.c
 *   "RunTopple": r0 = slot, the fall itself.
 *
 * EXACT:
 *   OK OvlFunc_906_20084f4 -- 440 bytes, 194 encodings and 22 relocations identical
 *
 * ------------------------------------------------------ LANDING NEEDS A SPLIT
 * [asm/overlays/rom_79aad8/ovl_314_c_c_c.s holds ONE FUNCTION (lines 21-213)
 *  PLUS A TRAILING `.section .data` (lines 215-237) that defines SEVEN blobs,
 *  all `.global`: .L818, .L8d8, gOvl_02008920, .L948, .L978, .L990, .L9f0.
 *  NONE of them is referenced by this function -- they belong to the overlay --
 *  but all seven are consumed by ALREADY-ELEVATED SIBLINGS in this same
 *  overlay, via the `__asm__` label-extern technique:
 *      .L818, .L8d8        src/overlays/rom_79aad8/ovl_314_a.c:32,33
 *      gOvl_02008920       src/overlays/rom_79aad8/ovl_314_c_a_b.c:8
 *      .L948, .L978        src/overlays/rom_79aad8/ovl_314_c_a_c_a.c:14,15
 *      .L990, .L9f0        src/overlays/rom_79aad8/ovl_314_c_a_c_c_b.c:14,15
 *  so deleting the .s outright BREAKS THE LINK on all seven.  They must stay
 *  `.global` and they must stay in a `.s`.
 *
 *  TWO .ld lines name this .o, and they are the only two in any .ld in the tree
 *  that name it on full path:
 *      overlays/rom_79aad8/overlay.ld:31   asm/.../ovl_314_c_c_c.o(.text)
 *      overlays/rom_79aad8/overlay.ld:38   asm/.../ovl_314_c_c_c.o(.data)
 *
 *  This is exactly the shape docs/elevation.md:5971 ("Deleting a single-function
 *  `.s` that also holds `.data`") and :7502 ("Splitting a .s that holds ONE
 *  function plus its data") describe.  split_s.py cannot do it: with a single
 *  function the trailing data is part of that function's block.  The boundary
 *  is clean -- everything through `.func_end`, then `.section .data` -- so the
 *  manual split is TWO files:
 *      ovl_314_c_c_c_b.s   preamble + function   -> becomes this .c
 *      ovl_314_c_c_c_c.s   `.include` + the data section, no functions
 *  and BOTH objects go in BOTH the .text and .data lists, in place, so the
 *  ordering stays obvious and the layout does not move.  The .c references none
 *  of the seven blobs, so the split is genuinely clean -- no `__asm__` label
 *  externs are needed in this file.
 *
 *  THE EXISTING .ld LINES ARE NEVER RE-AIMED AT src/.  The build rule is
 *  `asm/%.o: src/%.c`, so the object is produced under asm/ even for elevated
 *  C; a .ld line naming src/<TU>.o matches nothing and an unmatched entry is
 *  SILENTLY IGNORED.  The two lines are REPLACED BY FOUR lines that still all
 *  say `asm/`.
 *
 *  Then run the byte-neutral `make compare` WITH THE FUNCTION STILL IN
 *  ASSEMBLY, before this .c is added.  That is what distinguishes a mis-placed
 *  split boundary from a bad decompilation while it is still cheap.
 *
 *  NO FLAG GROUP: tryc.makefile_flags() returns set(); the tree default
 *  -O2 -mthumb -mthumb-interwork -fcall-used-r4.  No wildcard rule in the
 *  Makefile reaches this path.]
 *
 * ------------------------------------------------------------------ TWIN ----
 * This file IS src/overlays/rom_798dc4/ovl_314_c_c_c_a.c (OvlFunc_903_2008dd8)
 * with six constants and two callee names substituted, and nothing else:
 *
 *   site                          903                  906
 *   ---------------------------  -------------------  -------------------
 *   a->f6c callback              OvlFunc_903_2008da8  OvlFunc_906_20084c4
 *   the two OvlFunc_*_db8/d4      OvlFunc_903_2008db8  OvlFunc_906_20084d4
 *   .. second argument, both      0x80 << 14           0
 *   first  __MapActor_TravelTo    (0xa0, 0xc0)         (0xbc<<1, 0x90<<1)
 *   second __MapActor_TravelTo    (0x8b, 0xc4)         (0xad<<1, 0x92<<1)
 *
 * Everything else -- struct Actor, struct Cfg, prototypes, statement order,
 * loop bounds, the pin fill -- is character-identical.  Read the 903 file for
 * the template provenance, the aliasing lever, the frame-layout argument and
 * the full worse/inert table; only what DIFFERS is recorded below.
 *
 * ------------------------------------------------------------------- NEW ----
 * THE MINIMISED PIN SET DOES NOT TRANSFER BETWEEN CONSTANT-SUBSTITUTION TWINS,
 * AND THE ALL-CHEAP RULE PREDICTS WHICH SITES WILL MOVE, FROM THE CONSTANTS
 * ALONE, BEFORE ANY COMPILE.
 *
 * Grepped first for "MINIMAL ONLY", "PIN WIDTH", "^## .*[Tt]win", "shifted
 * constant", "cheap constant" and read :6025, :6770, :15958, :17296, :17605,
 * :17939.  The corpus records the pin set as minimal only w.r.t. a FLAG GROUP
 * (batch 243) and records that near-twins' levers can INVERT (:17939) -- but
 * that entry's twins differ STRUCTURALLY, and its advice is the unfalsifiable
 * "re-measure every lever".  It also records "AN ALL-CHEAP CALL SITE NEEDS NO
 * ORDERING PIN" (:17605) as a way to PRUNE a sweep.  What is not recorded is
 * that these two combine into a PREDICTION for the twin play.
 *
 * Per-site drop cost, the same five sites, the two twins side by side:
 *
 *   site                                903    906    constants
 *   ---------------------------------  -----  -----  --------------------------
 *   __MapActor_SetSpeed                    3      3   0xc0<<10, 0xc0<<9  (same)
 *   __Func_8012330(0xa0<<11, ...)          3      3   unchanged
 *   __Func_8012330(-1, -1, 0xe666)       118    119   unchanged
 *   __MapActor_TravelTo #1                 0      2   CHANGED
 *   __MapActor_TravelTo #2                 0      2   CHANGED
 *
 * The three sites whose constants are UNCHANGED cost the same in both files.
 * The two sites whose constants CHANGED are the two that move -- and they move
 * in the direction :17605 predicts.  903's TravelTo arguments are 0xa0/0xc0
 * and 0x8b/0xc4: every one is a bare `mov #imm8`, an all-cheap site,
 * rematerialised free, nothing for gcse to common, pin INERT.  906's are
 * 0xbc<<1/0x90<<1 and 0xad<<1/0x92<<1: every one is a `mov`+`lsl` PAIR, a
 * commoning candidate, pin worth 2 -- which is the identical value the
 * neighbour ovl_cc0_c_c_a_c.c recorded for its own two TravelTo sites, whose
 * constants are also `<<1` pairs.  So the lever transfers by CONSTANT SHAPE,
 * not by call site and not by callee.
 *
 * It bites on WIDTH as well as on count, and there it is not additive:
 *
 *   variant                              903         906
 *   ----------------------------------  ----------  ----------------
 *   PIN2 at any ONE site                EXACT       EXACT
 *   PIN2 at ALL FIVE sites              EXACT       145 (4 SHORT)
 *   drop BOTH TravelTo pins             EXACT       4
 *   drop both + PIN2 at the other 3     EXACT       4
 *
 * PIN2 drops q2, which un-pins the third argument: 0xc0 / 0xc4 on 903 (cheap,
 * free) but 0x90<<1 / 0x92<<1 on 906 (a pair, commonable).  Same mechanism,
 * same direction.  The recorded batch-244 note found width uniformly reducible
 * on its function; here every site reduces ALONE and the five together collapse
 * -- "individually-inert pins are not jointly removable" (:15958) holding for
 * WIDTH, which that entry states only for SITES.
 *
 * PRACTICAL RULE.  When producing a twin by constant substitution, MINIMISE ON
 * THE TWIN WHOSE CONSTANTS ARE EXPENSIVE, or do not minimise at all and ship
 * the uniform fill.  A set minimised on the cheap-constant twin UNDER-COVERS
 * the other one and the failure is silent -- 903's minimal 3-pin set screens
 * exact on 903 and 4 differing on 906.  Both files here ship the uniform
 * five-site PIN3 fill, the only form measured exact on both.
 *
 * FLAGS: no flag group.  -fno-gcse 170, -fno-rerun-cse-after-loop 114,
 * -fno-schedule-insns2 59.  -fno-strict-aliasing, -fno-schedule-insns,
 * -fno-cse-follow-jumps, -fno-expensive-optimizations and -fno-strength-reduce
 * are all BYTE-IDENTICAL.
 */
struct Actor {
    unsigned char pad00[8];
    int x;
    int y;
    int z;
    unsigned char pad14[0xa];
    unsigned short f1e;
    unsigned short f20;
    unsigned char f22;
    unsigned char pad23[5];
    int f28;
    unsigned char pad2c[0xc];
    int f38;
    unsigned char pad3c[0xc];
    int f48;
    unsigned char pad4c[4];
    struct Actor *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char pad56[0x16];
    void (*f6c)(void);
};

struct Cfg {
    int f00;
    int f04;
    int f08;
    int f0c;
    int f10;
    int f14;
    unsigned short f18;
    unsigned short f1a;
    int f1c;
    int f20;
    void (*f24)(void);
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __WaitFrames(int n);
extern int __cos(int a);
extern int __sin(int a);
extern void __PlaySound(int id);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void OvlFunc_906_20084c4(void);
extern void OvlFunc_906_20084d4(struct Actor *p, int n);
extern void OvlFunc_common0_10c(int x, int y, int z, int a, int b, int c,
                                int d, struct Cfg *s);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_906_20084f4(int slot)
{
    struct Actor *a;
    int v[3];
    struct Cfg s;
    unsigned int i;
    int ang;

    a = __MapActor_GetActor(slot);
    a->f55 = 0;
    for (i = 0; i <= 0x11; i++) {
        __WaitFrames(1);
        a->f50->f1e -= 0x100;
        a->x -= __cos(a->f50->f1e) / 2;
        a->f38 = 0x80 << 24;
    }
    a->f6c = OvlFunc_906_20084c4;
    { PIN3; q0 = slot; q1 = 0xc0 << 10; q2 = 0xc0 << 9;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = slot; q1 = 0xbc << 1; q2 = 0x90 << 1;
      __MapActor_TravelTo(q0, q1, q2); }
    a->f48 = 0xcccc;
    a->f55 = 3;
    a->f22 = 0;
    __MapActor_WaitMovement(slot);
    OvlFunc_906_20084d4(a, 0);
    __PlaySound(0xbc);
    { PIN3; q0 = 0xa0 << 11; q1 = 0xa0 << 11; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __PlaySound(0x8d);
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    for (i = 0; i <= 0x10; i++) {
        ang = i << 12;
        v[0] = __cos(ang);
        v[1] = 0;
        v[2] = __sin(ang);
        v[0] = v[0] - v[0] / 4;
        v[2] = v[2] - v[2] / 2;
        OvlFunc_common0_10c(a->x, a->y, a->z, v[0], v[1], v[2], 0, 0);
    }
    a->f28 = 0xa0 << 11;
    { PIN3; q0 = slot; q1 = 0xad << 1; q2 = 0x92 << 1;
      __MapActor_TravelTo(q0, q1, q2); }
    __MapActor_WaitMovement(slot);
    OvlFunc_906_20084d4(a, 0);
    a->f6c = 0;
    a->f50->f1e = 0x80 << 5;
    s.f18 = 0xd6;
    s.f08 = 0x80 << 8;
    s.f0c = 0xcccc;
    s.f10 = 0xc0 << 9;
    s.f14 = 0x13333;
    OvlFunc_common0_10c(a->x, a->y, a->z, 0, 0, 0, 0xe0 << 13, &s);
    __PlaySound(0x9a);
    __MapActor_SetAnim(slot, 3);
    __Func_8012350();
}
