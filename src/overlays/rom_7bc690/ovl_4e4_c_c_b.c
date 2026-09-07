/* OvlFunc_933_20094b0  --  0x020094b0
 *   [asm/overlays/rom_7bc690/ovl_4e4_c_c.s, 2nd of 4]
 *
 * EXACT.  objcmp: "OK OvlFunc_933_20094b0 -- 392 bytes, 158 encodings and 34
 * relocations identical", re-run four times.  Screened against the asm/ path;
 * tryc.makefile_flags("src/overlays/rom_7bc690/ovl_4e4_c_c.c") is the EMPTY
 * set, so the TU falls to the tree default -O2 -fcall-used-r4 and the match
 * depends on no flag.
 *
 * 153 instructions of straight-line cutscene: two save-flag guards that both
 * return early, then one block.  Reads save bits 0x90a and 0x200, sets 0x200.
 *
 * READ THE PROLOGUE BY CONTENT.  `push {r5, r6, r7, lr}` plus `mov r7, r8 /
 * push {r7}` plus `sub sp, #0xc`, and what the registers hold is the whole
 * diagnosis:
 *
 *     r5   iwram_3001ebc, then 0x80000000, then sp   THREE disjoint roles
 *     r7   iwram_3001ebc + 0xcba                     a live pointer
 *     r8   0x96 << 2                                 a constant across 20 calls
 *     r6   the __GetFlag(0x200) result, reused as 0
 *     sp   a three-word vector for __vec3_translate
 *
 * `hiv = 1` -- the ROM holds ONE value in a high register -- and that is
 * exactly the trap the metric sets: unaided gcc has one MORE candidate to shed,
 * and here it sheds it by commoning 0x200 into r5 across the __GetFlag /
 * __SetFlag pair.  That single commoning is 11 of the 20 baseline differences.
 *
 * FOUR PINS, MINIMISED TO A FIXPOINT, AND THE FIRST ONE IS THE EVICTION.
 * 0x200 feeds __GetFlag at the guard and __SetFlag immediately after it.  The
 * recorded "GetFlag(id) guarding a block that ends SetFlag(id)" shape predicts
 * a CSE_CFLAGS group; it is not needed, because a call-clobbered pin on r0 at
 * the FIRST site kills the commoning outright (docs/elevation.md, "the pin is
 * an eviction device").  ONE pin covers both sites; the __SetFlag site takes a
 * plain literal.
 *
 * THE OTHER THREE ARE ORDERING PINS AND ALL THREE ARE PIN2, NOT PIN3.  Every
 * __MapActor_SetSpeed site emits `mov r1 / mov r2 / mov r0 / lsl r1 / lsl r2`;
 * uniform ASCENDING fill reproduces it, and each site needs only r0 and r1
 * named -- naming r2 as well is an exact tie, and naming only r0 is 4 differing
 * at each of the three.  That is the neighbour's width-minimisation finding
 * (src/overlays/rom_7b6668/ovl_314_c_c_a_c_c_a_a.c) reproduced with the
 * boundary visible from BOTH sides: too wide is inert, too narrow costs.
 *
 * THE SIGNED DIVISION MUST BE WRITTEN AS DIVISION.  The ROM spells
 * `cmp #0 / bge / add 0xffff / asr #16` twice, which is gcc's own expansion of
 * `/ 0x10000` on a signed int.  Writing `>> 16` instead is 135 of 158 differing
 * and TWENTY BYTES SHORT -- the recorded "our stream being shorter is a
 * signature" firing on the largest single lever in the function.
 *
 * THREE NAMES ARE LOAD-BEARING AND FOUR ARE NOT, and the split is the recorded
 * one.  `base` (the global read at the top, held across three calls), `hp` (the
 * halfword pointer the ROM keeps in r7) and `k` (0x96 << 2, the ROM's r8) are
 * each required.  A local for 0x80000000, a byte pointer for the two `[0x5a]`
 * sites, a named `int *` for the stack vector, and a local for the GetFlag
 * result reused as the zero are ALL exact ties and none of them ships -- gcc
 * reaches the ROM's shape for those without help.  In particular the recorded
 * "sub sp, #N + mov rX, sp for a stack vector needs a named int *p = v" does
 * NOT apply here: `mov r5, sp` falls out of the bare array.
 *
 * TEARDOWN (each lever removed from the finished file, re-measured; ties are
 * listed because they say the set is minimal but not unique):
 *
 *     `v[0] >> 16` for `v[0] / 0x10000`   135 of 158, 372 bytes, 20 SHORT
 *     `k` removed                         127 differing
 *     `base` removed                       29 differing
 *     the r0 pin at __GetFlag(0x200)       11 differing
 *     `hp` removed                          5 differing
 *     each SetSpeed pin narrowed to PIN1    4 differing
 *     each SetSpeed pin dropped             3 differing
 *     a narrow-local `orr` at the |= 1      2 differing
 *     ---- exact TIES, dropped ----
 *     a local for 0x80000000, a byte pointer for [0x5a], `int *q = v`,
 *     a local for the GetFlag result, PIN3 at each SetSpeed site
 *
 * LANDING NEEDS A SPLIT AND THE .s CARRIES DATA.  ovl_4e4_c_c.s holds FOUR
 * functions and a trailing `.section .data` (Events_TolbiSpring, .L1f48,
 * .L1f70, all `.global`, and .L1f48/.L1f70 are read from OvlFunc_933_2008e2c
 * in another TU) -- deleting the .s would break them.  This function is the
 * 2nd of the four, so `tools/split_s.py asm/overlays/rom_7bc690/ovl_4e4_c_c.s
 * OvlFunc_933_20094b0` gives _a.s (20092fc), _b.s (this one) and _c.s (2009638
 * + 2009874 + the data), and the data stays in _c.s untouched.  The suffixes
 * _a/_b/_c are free under asm/overlays/rom_7bc690/.  Exactly two linker lines
 * name the object, both on the FULL path and both keep their `asm/` prefix
 * VERBATIM (the build rule is `asm/%.o: src/%.c`, so a `src/...o` line matches
 * nothing and is silently ignored):
 *
 *     overlays/rom_7bc690/overlay.ld:39   asm/.../ovl_4e4_c_c.o(.text)
 *     overlays/rom_7bc690/overlay.ld:52   asm/.../ovl_4e4_c_c.o(.data)
 *
 * split_s.py rewrites both into three lines each; the .data lines for _a and
 * _b are empty contributions and cost nothing.  This function references NO
 * `.L` symbol, so no export is needed.
 *
 * -- worked in scratch_elev/b251/lowp; sweep1.sh runs a whole teardown in one
 *    container invocation, m1/ m2/ m3/ hold the three minimisation passes.
 */
extern unsigned char *iwram_3001ebc;

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_WaitMovement(int slot);
extern void __vec3_translate(int a, int b, int *v);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void OvlFunc_933_2009054(void);
extern void OvlFunc_933_2009c78(int a);

void OvlFunc_933_20094b0(void)
{
    unsigned char *base;
    unsigned short *hp;
    int k;
    int v[3];

    base = iwram_3001ebc;
    OvlFunc_933_2009054();
    if (__GetFlag(0x90a) != 0)
        return;
    { PIN1; q0 = 0x80 << 2;
      if (__GetFlag(q0) != 0)
          return; }
    __SetFlag(0x80 << 2);
    OvlFunc_933_2009c78(1);
    hp = (unsigned short *)(base + 0xcba);
    k = 0x96 << 2;
    *hp = k;
    *(int *)(__MapActor_GetActor(0) + 0x24) = 0;
    *(int *)(__MapActor_GetActor(0) + 0x2c) = 0;
    *(int *)(__MapActor_GetActor(0) + 0x38) = 0x80 << 24;
    *(int *)(__MapActor_GetActor(0) + 0x40) = 0x80 << 24;
    __MapActor_SetAnim(0, 1);
    __Func_809280c(0, 8, 0);
    __CutsceneWait(0x28);
    __Func_809259c(0, 2);
    __MapActor_Surprise(0, 0x81 << 1);
    __CutsceneWait(0x28);
    __MapActor_GetActor(0)[0x5a] &= 0xfe;
    v[0] = 0;
    v[1] = 0;
    v[2] = 0;
    __vec3_translate(0xfff00000, *(unsigned short *)(__MapActor_GetActor(0) + 6), v);
    { PIN2; q0 = 0; q1 = 0x80 << 10;
      __MapActor_SetSpeed(q0, q1, 0x80 << 9); }
    __MapActor_SetAnim(0, 2);
    __Func_809228c(0, v[0] / 0x10000, v[2] / 0x10000);
    __MapActor_WaitMovement(0);
    __CutsceneWait(2);
    __MapActor_GetActor(0)[0x5a] |= 1;
    __CutsceneWait(0x1e);
    __PlaySound(0x94);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    { PIN2; q0 = 8; q1 = 0xa0 << 10;
      __MapActor_SetSpeed(q0, q1, 0xa0 << 9); }
    __Func_80921c4(8, 0xa8, 0x68);
    { PIN2; q0 = 8; q1 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, 0x80 << 7); }
    __Func_80921c4(8, 0xa8, 0x5c);
    *hp = k;
    OvlFunc_933_2009c78(0);
}
