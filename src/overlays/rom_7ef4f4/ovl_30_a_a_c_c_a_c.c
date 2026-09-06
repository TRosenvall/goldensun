/* OvlFunc_965_2008d4c and OvlFunc_965_2008eac
 *   [asm/overlays/rom_7ef4f4/ovl_30_a_a_c_c_a_c.s, lines 6-214 -- the WHOLE
 *   FILE.  It holds EXACTLY TWO FUNCTIONS (`grep -c thumb_func_start` returns
 *   2) and NO DATA WHATEVER: `grep -n "\.section\|\.incbin\|\.word\|\.byte\|
 *   \.global"` over the file returns NOTHING.  BOTH ARE SOLVED HERE, so there
 *   is NO SPLIT, no new .s, and no split_s.py run.
 *
 *   ONE .ld LINE NAMES THIS .o, ON THE FULL PATH, AND IT IS THE ONLY LINE IN
 *   ANY .ld IN THE TREE THAT NAMES `ovl_30_a_a_c_c_a_c.o` AT ALL
 *   (`grep -rn "ovl_30_a_a_c_c_a_c\.o" --include=*.ld .` returns exactly one):
 *     overlays/rom_7ef4f4/overlay.ld:34  asm/overlays/rom_7ef4f4/ovl_30_a_a_c_c_a_c.o(.text)
 *   The overlay's only other section list is `.data` at lines 67-70, whose two
 *   entries are ovl_30_c_c_b.o(.data) and ovl_30_c_c_c.o(.data); there is no
 *   .bss and no .rodata list.  So NO section named for this .o goes
 *   unaccounted, and there is no `.data` line to re-aim.
 *   LANDING IS ONE LINE: asm/ -> src/ at overlay.ld:34, plus deleting the .s.
 *
 *   THE TWO FUNCTIONS OF THIS BATCH DO **NOT** SHARE A .s.  The twin
 *   OvlFunc_918_20098b8 lives in asm/overlays/rom_7a5214/ovl_17ec_c_c.s, in a
 *   DIFFERENT OVERLAY, and that file DOES carry a real `.section .data` and
 *   DOES need a split -- see src/overlays/rom_7a5214/ovl_17ec_c_c_b.c, which
 *   carries this batch's full write-up.]
 *
 * EXACT.  Measured as SINGLE-FUNCTION EXTRACTS against the ORIGINAL asm/ path
 * -- objcmp's --func cannot isolate one function inside a multi-function
 * CANDIDATE, so each was screened from a file holding only itself:
 *
 *   OK OvlFunc_965_2008d4c -- 352 bytes, 143 encodings and 29 relocations identical
 *   OK OvlFunc_965_2008eac -- 172 bytes, 64 encodings and 21 relocations identical
 *
 * and the COMBINED file (this one) was then compared whole-object against the
 * entire .s: ref 524 bytes / 207 encodings / 50 relocations, ours 524 / 207 /
 * 50, encodings and relocations identical.  352 + 172 = 524 exactly, so
 * nothing was gained or lost at the function boundary.
 *
 * objcmp prints no `(built with: ...)` line for either -- adjust=set(), the
 * tree default -O2 -mthumb -mthumb-interwork -fcall-used-r4.  NO FLAG GROUP IS
 * NEEDED and none can arrive by accident: no Makefile rule, literal or
 * `%`-patterned, names asm/overlays/rom_7ef4f4/ovl_30_a_a_c_c_a_c.o.  The
 * rom_7ef4f4 `%` rules in that file all sit under ovl_30_a_c_c_c_c_c*, which
 * this stem does not match.  Flag sensitivity, both functions:
 * -fno-strict-aliasing, -fno-cse-follow-jumps, -fno-rerun-cse-after-loop and
 * -fno-expensive-optimizations are BYTE-IDENTICAL; -fno-schedule-insns2 is 45
 * on 2008d4c and 13 on 2008eac; -fno-gcse is 137 on 2008d4c and silent on
 * 2008eac.
 *
 * OvlFunc_965_2008d4c IS THE TWIN OF OvlFunc_918_20098b8, one overlay away.
 * The same 135-instruction script -- cutscene opener, 17-iteration cos/sin
 * ring loop, fade-out tail -- with ONE extra `__CutsceneWait(0x1e)` between
 * `__WaitMapTransition()` and `__PlaySound(0xcc)`, and two different callees
 * (OvlFunc_965_2008cf0 for the config's f24 slot, OvlFunc_965_2008ae8 for the
 * emitter; the 918 copy calls the cross-overlay OvlFunc_common0_10c).  It was
 * exact on the first screen from the 918 file with three lines changed.
 * `tools/solved_twins.py` reported ZERO twins for both, correctly -- neither
 * was solved.  A TWIN MISS IS NOT A FAMILY MISS: the family was found by
 * grepping the corpus for `bl __cos` and for the literal `0x1090001`.
 *
 * ------------------------------------------------------ CORRECTION ----------
 * OvlFunc_965_2008eac RETIRES A PARK, AND THE PARK'S BLOCKER WAS FALSE.
 * src/non_matching/overlays/2008eac.c parks this exact function on
 * "constant_reuse": the ROM builds -1 three separate times for
 * __Func_80933f8(-1,-1,-1,0), gcc builds it once and copies, two instructions
 * shorter, and the park concludes "there is no source spelling that separates
 * them".  docs/elevation.md's "The same call is spelled both ways in the same
 * ROM" cites this same function by name for the same claim.
 *
 * The PIN4 ascending fill separates them, and it post-dates both.  The
 * function is exact with nothing else added:
 *
 *     { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0;
 *       __Func_80933f8(q0, q1, q2, q3); }        0
 *     bare __Func_80933f8(-1, -1, -1, 0)        53 differing, 4 bytes short
 *     PIN1 (q0 only)                          [918: 126, i.e. no better than none]
 *
 * DELETE src/non_matching/overlays/2008eac.c.  The elevation entry's general
 * claim -- that a reuse verdict belongs to the translation unit and must not be
 * carried from one function to another -- still stands; only its example is
 * wrong, and the right reading is that the park was written before the lever
 * existed.  ("RE-ATTACK PARKS WITH LEVERS FOUND AFTER THEY WERE WRITTEN.")
 *
 * The park's own list of what already reproduced was accurate and is worth the
 * credit: the stores through repeated __MapActor_GetActor(0) results, the
 * two-instruction constants 0x82<<16 and 0x80<<7, the pooled 0xfffb0000, and
 * OvlFunc_965_2008cd0 taking the actor pointer -- all of that is unchanged
 * here.  Only the first call needed the pin.
 *
 * THE `neg` ORDER DIFFERS BETWEEN THE TWO FUNCTIONS IN THIS FILE AND BOTH COME
 * FROM THE SAME ASCENDING FILL:
 *     2008d4c   mov r0,r1,r2 #1 / neg r1 / neg r2 / mov r3,#0 / neg r0
 *     2008eac   mov r0,r1,r2 #1 / neg r2 / neg r1 / mov r3,#0 / neg r0
 * Writing 2008eac's descending to "match" costs 2.  This is "DO NOT TRANSCRIBE
 * THE ROM'S SHIFT ORDER" holding for `mov`/`neg`, read out of one file.
 *
 * THE WIDE `struct Actor` IS REQUIRED BY 2008eac AND INERT FOR 2008d4c.
 * 2008eac stores the pooled 0xfffb0000 at offset 0x28, so the tag carries
 * `int f28` where the 918 copy's does not.  Measured both ways: 2008d4c is
 * byte-identical with and without the field, and 2008eac is byte-identical
 * whether the store goes through the field or through a cast.  This is the
 * template's "UNIFY A STRUCT TAG TO *ADD* A DEPENDENCE" lever measuring ZERO
 * on transplant -- there is no halfword store here and no alias-set tie to
 * break, which -fno-strict-aliasing confirms by being byte-identical for both
 * functions.  SUFFICIENT, NOT NECESSARY, and here not even that.
 *
 * PINS.  2008d4c carries the 918 file's four, unchanged and all load-bearing;
 * see that file for the site table and the minimality work.  2008eac needs
 * exactly ONE (__Func_80933f8, 53) and the ADD pass finds nothing: pinning
 * __Func_8092950(0, 0xf) ascending OR descending is inert, which is the
 * recorded "AN ALL-CHEAP CALL SITE NEEDS NO ORDERING PIN".
 *
 * MEASURED WORSE on OvlFunc_965_2008eac (against 172 bytes / 64 encodings):
 *
 *   spelling                                              differing
 *   ---------------------------------------------------  ---------
 *   bare __Func_80933f8(-1, -1, -1, 0)                        53 (4 bytes short)
 *   cache `p = __MapActor_GetActor(0)` for all six sites      44 (20 bytes short)
 *   `f28 = 0xfffb0000` moved before `f55 = 3`                  7
 *   descending PIN4 fill at __Func_80933f8                     2
 *
 *   INERT (tie at 0, so the plainer form ships):
 *     no `z` local, literal 0 at both stores
 *     the f28 store through a cast instead of a named field
 *     `OvlFunc_965_2008cd0(int)` instead of `(struct Actor *)`
 *     a local for the `slot` parameter
 *     a pin at __Func_8092950, either direction
 *     dropping any of the four prototypes tested here
 *       (__Actor_SetSpriteFlags, __Func_8092950, __Func_8091e9c,
 *        OvlFunc_965_2008cd0)
 */
struct Actor {
    unsigned char pad00[8];
    int x;
    int y;
    int z;
    unsigned char pad14[0x14];
    int f28;
    unsigned char pad2c[0x18];
    int f44;
    int f48;
    unsigned char pad4c[9];
    unsigned char f55;
};

struct Cfg {
    int f00;
    int f04;
    int f08;
    int f0c;
    unsigned char pad10[0x14];
    void (*f24)(void);
};

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __PlaySound(int id);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_800fe9c(void);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_Surprise(int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern int __cos(int a);
extern int __sin(int a);
extern void __Actor_SetSpriteFlags(struct Actor *p, int f);
extern void __Func_8092950(int a, int b);
extern void __Func_8091e9c(int n);
extern void OvlFunc_965_2008cd0(struct Actor *p);
extern void OvlFunc_965_2008cf0(void);
extern void OvlFunc_965_2008ae8(int x, int y, int z, int a, int b, int c, int d, struct Cfg *s);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_965_2008d4c(void)
{
    struct Cfg s;
    int v[3];
    struct Actor *a;
    unsigned int i;
    int ang;
    int z;

    __CutsceneStart();
    { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    __WaitFrames(1);
    __MapActor_GetActor(0)->y = 0x82 << 16;
    __MapActor_GetActor(0)->f48 = 0x80 << 8;
    z = 0;
    __MapActor_GetActor(0)->f44 = z;
    __MapActor_GetActor(0)->f55 = z;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x1e);
    __PlaySound(0xcc);
    __MapActor_GetActor(0)->f55 = 3;
    __CutsceneWait(0x18);
    a = __MapActor_GetActor(0);
    s.f04 = 7;
    s.f24 = OvlFunc_965_2008cf0;
    s.f08 = 0xcccc;
    s.f0c = 0xcccc;
    for (i = 0; i <= 0x10; i++) {
        ang = i << 12;
        v[0] = __cos(ang);
        v[1] = 0;
        v[2] = __sin(ang);
        v[0] = v[0] + v[0] / 2;
        OvlFunc_965_2008ae8(a->x, a->y, a->z, v[0], v[1], v[2], 0x1090001, &s);
    }
    __PlaySound(0xbc);
    { PIN2; q0 = 0; q1 = 0x101;
      __MapActor_Surprise(q0, q1); }
    __MapActor_SetAnim(0, 0x16);
    { PIN3; q0 = 0xa0 << 11; q1 = 0xa0 << 11; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __Func_8012350();
    __MapActor_Surprise(0, 0x80 << 1);
    __MapActor_GetActor(0)->f48 = 0x80 << 9;
    __MapActor_GetActor(0)->f44 = 0x80 << 7;
    __CutsceneEnd();
}

void OvlFunc_965_2008eac(int slot)
{
    int z;

    __CutsceneStart();
    { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    __WaitFrames(1);
    __MapActor_GetActor(0)->y = 0x82 << 16;
    __MapActor_GetActor(0)->f48 = 0x80 << 7;
    z = 0;
    __MapActor_GetActor(0)->f44 = z;
    __MapActor_GetActor(0)->f55 = z;
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0xa);
    __PlaySound(0xcc);
    __MapActor_GetActor(0)->f55 = 3;
    __MapActor_GetActor(0)->f28 = 0xfffb0000;
    OvlFunc_965_2008cd0(__MapActor_GetActor(0));
    __Func_8092950(0, 0xf);
    __Func_8091e9c(slot);
    __CutsceneEnd();
}
