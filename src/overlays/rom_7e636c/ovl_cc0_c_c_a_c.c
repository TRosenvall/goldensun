/* OvlFunc_958_20091f8
 *   [asm/overlays/rom_7e636c/ovl_cc0_c_c_a_c.s, lines 12-182 -- the WHOLE FILE.
 *   It holds EXACTLY ONE FUNCTION (`grep -c thumb_func_start` returns 1) and NO
 *   DATA WHATEVER (`grep -n "\.section\|\.incbin\|\.word\|\.byte\|\.global"`
 *   over the file returns NOTHING).  NO SPLIT, no new .s, no split_s.py run.
 *
 *   ONE .ld LINE NAMES THIS .o AND IT IS THE ONLY LINE IN ANY .ld IN THE TREE
 *   THAT NAMES IT (`grep -rn "ovl_cc0_c_c_a_c\.o" --include=*.ld .` returns
 *   exactly one):
 *     overlays/rom_7e636c/overlay.ld:48  asm/overlays/rom_7e636c/ovl_cc0_c_c_a_c.o(.text)
 *   THAT LINE STAYS VERBATIM.  The build rule is `asm/%.o: src/%.c`, so an
 *   elevated .c still produces asm/overlays/rom_7e636c/ovl_cc0_c_c_a_c.o;
 *   re-aiming the line at src/ would match nothing and drop the function
 *   silently.  The overlay's only other section list is `.data` at line 57,
 *   which does not name this .o, and there is no .bss and no .rodata list, so
 *   nothing goes unaccounted.  LANDING IS: write this .c, delete the .s.
 *
 *   NO FLAG GROUP IS NEEDED: tryc.makefile_flags('src/overlays/rom_7e636c/
 *   ovl_cc0_c_c_a_c.c') returns set(), the tree default
 *   -O2 -mthumb -mthumb-interwork -fcall-used-r4.]
 *
 * EXACT:
 *   OK OvlFunc_958_20091f8 -- 392 bytes, 171 encodings and 22 relocations identical
 *
 * 166 instructions: an actor script -- a 9-iteration sink loop that walks the
 * f50 sub-object's angle down and pushes the actor's x with cos(), a travel
 * leg, a 17-iteration cos/sin ring emitter over OvlFunc_common0_10c, a second
 * travel leg and a fade-out tail.  Its template is
 * src/overlays/rom_7a5214/ovl_17ec_c_c_b.c / src/overlays/rom_7ef4f4/
 * ovl_30_a_a_c_c_a_c.c (neighbour.py, 10/15 shared symbols), which supplied the
 * `struct Cfg`-less ring-loop skeleton verbatim: `unsigned int i; i <= 0x10;
 * ang = i << 12;` an `int v[3]` written v[0]/v[1]/v[2] in that order, the
 * signed-division rounding written out as `v[0] - v[0] / 4`, and the
 * ascending PIN fill at every constant-argument call.
 *
 * ------------------------------------------------------------------- NEW ----
 * THE UNIFIED STRUCT TAG IS THE ALIASING LEVER, AND IT REPLACES THE FLAG.
 * Grepped docs/elevation.md first for "A MISSING RELOAD after a store of a
 * different width is an ALIASING tell", "The aliasing tell is a RECOGNISER",
 * "UNIFY A STRUCT TAG TO *ADD* A DEPENDENCE" and "`volatile` at the use site".
 * The corpus records the recogniser and TWO remedies -- the whole-TU flag
 * `-fno-strict-aliasing`, and a use-site `volatile` -- and it records the
 * unified tag as a lever for a SCHEDULING tie (dependent count in
 * `rank_for_schedule`).  It does NOT record the unified tag as a cure for the
 * MISSING-RELOAD form.  Here it is exactly that, and it is the narrowest of
 * the three.
 *
 * The tell, in the first loop:
 *     rom   strh r3,[r2,#0x1e] / ldr r3,[r6,#0x50] / ldrh r0,[r3,#0x1e]
 *     ours  strh r3,[r2,#0x1e] / ldrh r0,[r2,#0x1e]      <- pointer reload gone
 * A halfword store cannot alias a POINTER read under -fstrict-aliasing, so
 * `a->f50` was commoned across it.  Measured, all three remedies restore the
 * reload and all three land the same instruction count:
 *
 *   struct Sub *f50, separate tag                 145 differing, 1 short
 *   ... + -fno-strict-aliasing                    (restores the reload)
 *   ... + `struct Sub *volatile f50;`             (restores the reload)
 *   struct Actor *f50, f1e declared IN struct Actor    0   <- what ships
 *
 * MECHANISM: with `f50` pointing at the SAME TAG that carries `f1e`, the store
 * and the pointer load are both components of one record type and gcc-2.96
 * stops proving them independent.  It costs no flag, no `volatile`, and no
 * TU-wide decision -- and on the shipped source `-fno-strict-aliasing` is then
 * BYTE-IDENTICAL, which is the cleanest possible statement that the dependence
 * is now in the types rather than in the flags.  Prefer this whenever the
 * re-read and the stored field can be spelled as fields of one tag; fall back
 * to the flag only when they genuinely cannot.
 *
 * ------------------------------------------------------ CONFIRMATION --------
 * THE PIN IS AN EVICTION DEVICE (seventh time).  Before the pins, `hi` was
 * right (r8/r9/r10/r11 all used, push mask exact) but the TENANTS were wrong:
 * gcc commoned `0x80 << 9` between `__MapActor_SetSpeed` and
 * `__Func_8012330`, parked it in r5, and pushed the `&a->f55` pointer into r7
 * -- which is the register the ROM wants for `&v[0]`.  128 differing, 3
 * instructions SHORT.  Pinning the constants back to r0-r2 took it to 7 with
 * the r5/r7 allocation correcting itself, unaided.  A wrong low-register
 * tenancy with a correct high-register set is the same commoning tell the
 * entry describes for the push mask.
 *
 * EVICTION PINS AS A SET (second confirmation).  The five sites were added
 * together, then stripped greedily with a re-test after each drop, to a
 * fixpoint.  NO SITE DROPS:
 *
 *   site  call                                          cost of dropping it
 *   ----  --------------------------------------------  -------------------
 *      2  __Func_8012330(0xa0<<11, 0xa0<<11, 0x80<<9)          97 (1 short)
 *      3  __Func_8012330(-1, -1, 0xe666)                       91 (1 short)
 *      0  __MapActor_SetSpeed(slot, 0x80<<10, 0x80<<9)         33
 *      1  __MapActor_TravelTo(slot, 0xec<<1, 0x90<<1)           2
 *      4  __MapActor_TravelTo(slot, 0xdc<<1, 0x9a<<1)           2
 *
 * The two TravelTo sites are worth 2 each and are the LAST two instructions of
 * the residue -- with the other three pinned the function sat at 7 differing,
 * three transpositions, and the TravelTo pair took it to 3.  ADD PASS beyond
 * these five: nothing left to pin; every remaining call site is all-cheap or
 * takes register arguments only.
 *
 * PIN WIDTH IS PER SITE.  PIN2 is enough at both __Func_8012330 sites (tie at
 * 0, so the uniform PIN3 ships) and is NOT enough at __MapActor_SetSpeed (34,
 * i.e. one worse than dropping the pin outright -- "A PARTIAL PIN CAN BE WORSE
 * THAN NO PIN AT ALL", holding here because the unpinned argument is the
 * CONSTANT that was being commoned).
 *
 * ------------------------------------------------------------------- NEW ----
 * A ZERO REUSED AS A LOOP-CARRIED REGISTER IS NOT NECESSARILY A LOCAL.  The
 * last residue was three instructions -- a permutation of the second loop's
 * preheader:
 *     rom   mov r4,#0 / add r7,sp,#0x10 / mov r8,r4 / mov r10,r7 / mov r9,r4
 *     ours  mov r4,#0 / add r7,sp,#0x10 / mov r9,r4 / mov r8,r4 / mov r10,r7
 * r9 holds 0 from before the loop through to the tail's `a->f6c = 0`, and it
 * feeds FOUR sites (`v[1]`, two stack arguments, and the tail store).  The
 * template's own idiom for that is a named `int z;`, and this tree's recorded
 * levers ("A stack argument equal to a register argument is ONE named local",
 * "no `z` local, literal 0 at both stores" as an INERT tie) both point at
 * naming it.  Here naming it is the ONLY thing standing between 3 and 0:
 *
 *   no `z` local, literal 0 at all four sites                0   <- ships
 *   `z = 0;` immediately before the loop, z at all four       3
 *   `z` for v[1] and the two stack args, 0 in the tail        3
 *   `z` for v[1] only, literals for the stack args           17
 *   `z = 0;` hoisted before the two __Func_8012330 calls     100
 *   `z = 0;` hoisted to the top of the function              164
 *
 * The register is created by gcc's own commoning of four literal zeroes, and
 * naming it makes the assignment a STATEMENT that has to be placed -- and gcc
 * places it first in the preheader, where the ROM places it last.  So: when a
 * shared zero's only defect is WHERE its `mov` lands in a preheader, delete
 * the local rather than moving it.  A named local can only be scheduled where
 * its statement is; a commoned literal is free to land anywhere.
 *
 * WHAT NEEDED NOTHING.  `unsigned int i` for both loops (the ROM's `bls`;
 * `int` costs 159 differing and TWO extra instructions); the two loops as
 * plain `for`s with the increment written in the header; `a->f22 = 0` as a
 * literal; the two-instruction shifted constants as `0x80 << 24` etc.; the
 * high registers r8/r9/r10/r11 chosen by gcc with no help at all.
 *
 * MEASURED WORSE (against 392 bytes / 171 encodings):
 *
 *   spelling                                              differing
 *   ---------------------------------------------------  ---------
 *   `int v0, v1, v2;` instead of `int v[3]`                  167 (12 short)
 *   `z = 0;` at the top of the function                      164
 *   loop counters typed `int`                                159
 *   separate `struct Sub` tag for the f50 sub-object          145 (1 short)
 *   `z = 0;` before the two __Func_8012330 calls              100
 *   drop the __Func_8012330(0xa0<<11,...) pin                  97 (1 short)
 *   drop the __Func_8012330(-1,-1,0xe666) pin                  91 (1 short)
 *   PIN2 instead of PIN3 at __MapActor_SetSpeed                34
 *   drop the __MapActor_SetSpeed pin                           33
 *   `z` for v[1] only                                          17
 *   drop the __MapActor_TravelTo prototype                       4
 *   drop the __Func_8012330 prototype                            3
 *   drop the __MapActor_SetSpeed prototype                       3
 *   a named `int z;` for the shared zero                         3
 *   drop either __MapActor_TravelTo pin                          2
 *   drop the __Func_8012350 prototype                            2
 *   drop the __MapActor_SetAnim prototype                        2
 *
 *   INERT (tie at 0, so the plainer or more uniform form ships):
 *     `for (i = 0; i < 9; i++)` / `i < 0x11`
 *     `v[0] -= v[0] / 4;` and `v[2] -= v[2] / 2;`
 *     `ang` inlined into both __cos/__sin calls
 *     a named `p = a->f50;` for the halfword update
 *     plain literals (0x50000, 0x1d8, 0x200000, ...) for every shifted constant
 *     PIN2 at either __Func_8012330 site
 *     dropping any of __WaitFrames, __PlaySound, __CutsceneWait, __cos, __sin,
 *       __MapActor_GetActor, __MapActor_WaitMovement, OvlFunc_958_20091d8 or
 *       OvlFunc_common0_10c
 *
 * FLAGS: no flag group.  -fno-schedule-insns2 is 55 differing and
 * -fno-rerun-cse-after-loop is 96 (3 LONG); -fno-gcse is 148 (4 short).
 * -fno-strict-aliasing, -fno-schedule-insns, -fno-cse-follow-jumps and
 * -fno-expensive-optimizations are all BYTE-IDENTICAL -- see the NEW entry
 * above for why the aliasing flag is silent here.
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
    unsigned char pad23[0x15];
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
extern void __CutsceneWait(int n);
extern void OvlFunc_958_20091c8(void);
extern void OvlFunc_958_20091d8(struct Actor *p, int n);
extern void OvlFunc_common0_10c(int x, int y, int z, int a, int b, int c, int d, int e);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_958_20091f8(int slot)
{
    struct Actor *a;
    int v[3];
    unsigned int i;
    int ang;

    a = __MapActor_GetActor(slot);
    a->f55 = 0;
    for (i = 0; i <= 8; i++) {
        __WaitFrames(1);
        a->f50->f1e -= 0x100;
        a->x -= __cos(a->f50->f1e) / 2;
        a->f38 = 0x80 << 24;
    }
    a->f6c = OvlFunc_958_20091c8;
    __PlaySound(0x88);
    { PIN3; q0 = slot; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = slot; q1 = 0xec << 1; q2 = 0x90 << 1;
      __MapActor_TravelTo(q0, q1, q2); }
    a->f48 = 0xcccc;
    a->f55 = 3;
    a->f22 = 0;
    __MapActor_WaitMovement(slot);
    OvlFunc_958_20091d8(a, 0x80 << 14);
    { PIN3; q0 = 0xa0 << 11; q1 = 0xa0 << 11; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
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
    { PIN3; q0 = slot; q1 = 0xdc << 1; q2 = 0x9a << 1;
      __MapActor_TravelTo(q0, q1, q2); }
    __MapActor_WaitMovement(slot);
    OvlFunc_958_20091d8(a, 0x80 << 14);
    a->f6c = 0;
    a->f50->f1e = 0x80 << 5;
    __PlaySound(0x9a);
    __MapActor_SetAnim(slot, 3);
    __Func_8012350();
    __CutsceneWait(0xa);
    __MapActor_SetAnim(slot, 2);
}
