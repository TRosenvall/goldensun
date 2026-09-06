/* OvlFunc_953_2009688
 *   [asm/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_c_a_c.s, lines 7-343 -- the WHOLE
 *   FILE.  It holds EXACTLY ONE function (`grep thumb_func_start` returns one
 *   line) and NO data: zero `.incbin`, zero `.word`, zero `.byte`, no
 *   `.section`.  So there is NO SPLIT and no new .s.
 *
 *   overlays/rom_7d95dc/overlay.ld:36 is
 *     `asm/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_c_a_c.o(.text)`
 *   and it is the ONLY line in ANY .ld naming that FULL PATH.  Two other
 *   overlays carry a file of the SAME BASENAME --
 *   overlays/rom_7a7298/overlay.ld:28 `asm/overlays/rom_7a7298/...` and
 *   overlays/rom_7c097c/overlay.ld:37 `asm/overlays/rom_7c097c/...` -- and
 *   MUST NOT be touched; matching on basename would corrupt two other overlays.
 *   rom_7d95dc/overlay.ld has one other section, `.data` at line 55-57, whose
 *   single entry is ovl_30_c_c_c_c_c.o(.data); there is no .bss and no .rodata
 *   list in this overlay, so no section named for this .o goes unaccounted.
 *   LANDING IS ONE LINE: asm/ -> src/ at overlay.ld:36, plus deleting the .s.]
 *
 * EXACT, measured as a single-function extract (objcmp --func cannot isolate
 * one function inside a multi-function candidate -- this .s holds only one, so
 * the extract is the file):
 *
 *   OK OvlFunc_953_2009688 -- 908 bytes, 347 encodings and 97 relocations identical
 *
 * The verdict holds against the real asm/ path AND against a scratch copy of
 * the same .s, and objcmp prints no `(built with: ...)` line -- adjust=set(),
 * the tree default -O2 -mthumb -mthumb-interwork -fcall-used-r4.  NO FLAG GROUP
 * IS NEEDED, and none could arrive by accident: the only Makefile rules whose
 * targets match this .o are the generic `%.o: %.s` (line 95), `%.o: %.c` (135)
 * and the cross-dir `asm/%.o: src/%.c` (146).  The one rom_7d95dc rule that
 * names a file literally (line 807) names ovl_30_c_c_c_a_a_a_c_a_c_c.o.
 *
 * 334 instructions of straight-line cutscene script with ONE loop -- the same
 * script family as the file-sibling OvlFunc_953_2009298 in
 * ovl_30_c_c_c_a_a_c_a_a.c, replayed on actor 0x11 instead of 0x10, with a
 * 20-iteration shake loop and a __SetFlag(0x8a4) tail bolted on.
 *
 * READ THE PROLOGUE BY CONTENT.  `push {r5, r6, r7, lr}` -- THREE callee-saved
 * registers, all LOW, and ZERO hi-register traffic anywhere in the body.  What
 * they hold is the diagnosis:
 *
 *     r5   0xffff0000, then the loop counter, then 0x80 << 9   THREE roles
 *     r6   __MapActor_GetActor(0x15)                           the actor pointer
 *     r7   that pointer + 0x55                                 a byte cursor
 *
 * r5's THREE ROLES ARE THREE LOCALS (`v`, `i`, `w`).  This is the sibling's
 * recorded "r5's TWO ROLES MUST BE TWO LOCALS" result at one higher count: the
 * ROM sharing a register is the ALLOCATOR, not the source.
 *
 * r7 IS NOT A SOURCE VARIABLE.  `unsigned char *q = p + 0x55` held across some
 * forty calls looks like a named local, and it measures EXACTLY ZERO -- so by
 * "inert scaffolding must not ship" the plain `p[0x55] = 0;` / `p[0x55] = 3;`
 * ships instead and gcc commons the address into r7 by itself.  The ROM holding
 * a value in a callee-saved register across calls is NOT, on its own, evidence
 * for a local.
 *
 * NINETEEN PINS OF EIGHTY-ONE, MINIMAL BY MEASUREMENT.  All 81 pinnable
 * constant-argument sites were pinned first, then stripped greedily with a
 * re-test after every drop and re-grown with an ADD pass, to a fixpoint from
 * BOTH ends; two full rounds change nothing.  A strict pass over the 19
 * survivors drops none -- each costs between 2 and 310:
 *
 *   site  call                                        cost of dropping it
 *   ----  ------------------------------------------  -------------------
 *      0   __Func_80933f8(-1, -1, -1, 0)                310
 *     48   __MapActor_Surprise(0x11, 0x81 << 1)         190
 *     54   __MapActor_Surprise(0xf, 0x81 << 1)          176
 *     53   __MapActor_Surprise(0xe, 0x81 << 1)          165
 *     58   __Func_8092adc(0xe, 0xc0 << 6, 0)            160
 *     87   __Func_8012330(-1, -1, 0xe666)                83
 *     36   __Func_80921c4(0x11, 0xa4, 0xe6 << 2)         14
 *     59   __Func_8092adc(0xf, 0xc0 << 6, 0)             12
 *     55   __MapActor_Surprise(0x11, 0x81 << 1)          10
 *     89   __MapActor_SetSpeed(0x11, 0x19999, 0xcccc)     3
 *     37   __Func_80921c4(0x11, 0xb9, 0xe6 << 2)          3
 *     27   __MapActor_SetSpeed(0x11, 0xcccc, 0x6666)      3
 *     92   __Func_8092adc(0x11, 0xc0 << 6, 0x14)          2
 *     90   __Func_80921c4(0x11, 0xd0, 0xe8 << 2)          2
 *     61   __MapActor_Surprise(0x11, 0x101)               2
 *     52   __Func_8092adc(0x12, 0x80 << 8, 0x14)          2
 *     51   __Func_8092adc(0xf, 0xa0 << 7, 0)              2
 *     50   __Func_8092adc(0xe, 0xd0 << 8, 0)              2
 *     38   __Func_8092adc(0x11, 0xc0 << 8, 0x14)          2
 *
 * THE SURVIVORS ARE NOT "THE REPEATED CONSTANTS".  __MapActor_SetAnim appears
 * nineteen times and needs no pin at any of them, __CutsceneWait fifteen times
 * and needs none.  Meanwhile FOUR of the five __MapActor_Surprise(*, 0x81 << 1)
 * sites need one and the fifth (slot 0x12) does not, and of the two
 * __Func_8012330 calls only the LITERAL one is pinned -- the `(w, w, w)` call
 * ships bare.  That is the recorded "a pin set can need a HOLE where the ROM
 * itself commons" shape, twice in one function.
 *
 * UNIFORM ASCENDING FILL AT ALL NINETEEN.  One statement per argument,
 * ascending q0..q3, whole value per statement.  No survivor wants descending.
 * (With the full 81-pin set, site 60 `OvlFunc_953_2009c5c(0x12, 0xc0 << 6)`
 * DOES: ascending is 2 differing there and descending 0.  That site is not in
 * the minimal set, so the final file has no descending fill anywhere -- but the
 * measurement is worth keeping, because it is the sibling's "direction is
 * load-bearing" result reproduced on a THIRD call.)
 *
 * ------------------------------------------------------------------- NEW ----
 * TWO STORES THAT SHOULD ALIAS AND DO NOT: ADD A FIELD TO THE STRUCT, NOT A
 * PIN.  Sharpens the recorded "Two byte stores that may alias cost a scheduling
 * slot; two struct tags buy it back" from the OPPOSITE DIRECTION, and lands on
 * the step AFTER the one that entry decides on.
 *
 * With everything else exact the residue was ONE transposition, 2 encodings,
 * SIZE and RELOCATIONS both silent:
 *
 *     ROM   mov r3,#0xd0 / lsl r3,#8 / mov r5,#0x80 / strh r3,[r0,#6] / lsl r5,#9
 *     ours  mov r3,#0xd0 / lsl r3,#8 / mov r5,#0x80 / lsl r5,#9 / strh r3,[r0,#6]
 *
 * `-fsched-verbose=8` gives the whole answer, read out of the dump rather than
 * inferred.  The two candidates TIE on priority, so `rank_for_schedule` falls
 * past the priority test; both are class 3 against the last-scheduled insn, so
 * it falls past the class test too; and the DEPENDENT COUNT decides:
 *
 *     NARROW tag   insn 666 [r0+0x6]=r3  prio 96  FIVE dependents
 *                  insn 932 r5=r5<<0x9   prio 96  SEVEN dependents
 *                  -> 932 wins the slot, and the pair comes out transposed
 *
 *     WIDE tag     insn 666 [r0+0x6]=r3  prio 96  SEVEN dependents
 *                  insn 932 r5=r5<<0x9   prio 96  SEVEN dependents
 *                  -> exact tie, so rank_for_schedule falls through to insn
 *                     order and 666 (the lower LUID) takes the slot --
 *                     which is the ROM's order.
 *
 * The two dependents insn 666 gains are 676 and 679 -- the two `int` stores at
 * p+0x18 and p+0x1c.  Under -fstrict-aliasing a `short` store and an `int`
 * store are in different alias sets and carry NO output dependence; declaring
 * BOTH the halfword and the two words as fields of ONE struct tag makes them
 * conflict, and the halfword store inherits exactly the two dependents it needs
 * to stop losing the slot.
 *
 * THE LEVER IS THE STRUCT DEFINITION, NOT THE ACCESS SPELLING.  Measured both
 * ways, twice each (a surprising number is cheaper to re-test than to park):
 *
 *     wide tag + `((struct Actor *)p)->f18 = w;`      0
 *     wide tag + `*(int *)(p + 0x18) = w;`            0
 *     narrow tag + `*(int *)(p + 0x18) = w;`          2
 *
 * So `int f18; int f1c;` must EXIST in `struct Actor` even though nothing
 * forces the stores to go through them.  They are declared AND used here, so
 * that no later reader deletes two "unused" fields and silently un-matches the
 * file.  This is the mirror of the recorded lever: that one SPLITS two accesses
 * into two tags to REMOVE a dependence; this one UNIFIES two accesses into one
 * tag to ADD one.  Standing hazard is the same and inverted: this TU must never
 * fall under an -fno-strict-aliasing rule -- with that flag it is 281 differing
 * and two instructions long.
 *
 * WHAT NEEDED NOTHING.  The nineteen __MapActor_SetAnim calls as bare literals;
 * `__Func_80933f8` reproduced by a PIN4 ascending fill (its bare form CSEs the
 * three -1s into one `neg` and is 2 instructions short); the two iwram blocks
 * as the plain `*(int *)(iwram_3001ebc + (0xe0 << 1))` idiom, with gcc's own
 * `add r3,#0x40` / `sub r3,#0x38` (and `#0x41` / `#0x39`) derivations falling
 * out of it; `0xffff0000` as a plain `int` initialiser; the 20-iteration loop
 * as a plain `for (i = 0; i <= 0x13; i++)` over an `unsigned int` (the ROM's
 * `bls` is what makes it unsigned) with `+= 0x9999` / `-= 0x4ccc` -- gcc
 * canonicalises the subtraction into an add of the pooled 0xffffb334 by itself.
 *
 * PROTOTYPES, 8 OF 25 LOAD-BEARING, AND THEY ARE PER-SITE NOT PER-FILE:
 * dropping __MapActor_SetAnim costs 20, __Func_8092adc 18,
 * __MapActor_Surprise 8, __Func_80921c4 8, __MapActor_SetSpeed 6,
 * __Func_8012350 3 (a VOID, NO-ARGUMENT function -- the declaration lever
 * reaches even there), __Func_8012330 3, __MapActor_SetPos 2.  The other
 * seventeen are inert and stay declared because a complete prototype list is
 * this tree's convention.
 *
 * MEASURED WORSE (against 347 encodings / 908 bytes):
 *
 *   spelling                                              differing
 *   ---------------------------------------------------  ---------
 *   no pins at all                                          301 (-8 insns)
 *   all 81 sites pinned                                       2
 *   narrow struct tag (no f18/f1c)                            2
 *   `*(unsigned short *)(p11 + 6)` for the halfword          107 (+3 insns)
 *   halfword value through a local `int t`                     6
 *   `p[0x55] = 3` moved before the halfword store             11
 *   `p[0x55] = 3` moved after both word stores                 4
 *   `w = 0x80 << 9` moved before the halfword store            2
 *   loop counter typed `int` instead of `unsigned int`         4
 *   `w` typed `short`                                        110 (-2 insns)
 *   dropping any one of the 19 survivors                 2 to 310
 *
 *   INERT (tie at 0, so the plainer form ships):
 *     `unsigned char *q = p + 0x55` instead of `p[0x55]`
 *     `*(int *)(p + 0x18)` instead of the typed field
 *     `w = 0x10000` instead of `0x80 << 9`; `w` typed `unsigned int`
 *     `for (i = 0; i < 0x14; i++)`; `+= -0x4ccc` for `-= 0x4ccc`
 *     `register int w __asm__("r5")`
 *     a separate local for __MapActor_GetActor(0x11)'s result
 *
 * FLAGS: with the final shape, -fno-gcse, -fno-cse-follow-jumps,
 * -fno-rerun-cse-after-loop and -fno-expensive-optimizations are all
 * BYTE-IDENTICAL; -fno-schedule-insns2 is 95 differing and -fno-strict-aliasing
 * is 281, so the post-reload sweep and the alias separation are both live.
 */
struct Actor { unsigned char pad00[6]; unsigned short f6; unsigned char pad08[16]; int f18; int f1c; };

extern unsigned char *iwram_3001ebc;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __SetFlag(int id);
extern void __MapTransitionOut(void);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8091e9c(int n);
extern unsigned char gScript_953__0200af88[];
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_Surprise(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void OvlFunc_953_2009c5c(int a, int b);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_953_2009688(void)
{
    unsigned char *p;
    int v;
    unsigned int i;
    int w;

    __CutsceneStart();
    { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    __PlaySound(0xf7);
    __MapActor_SetAnim(8, 2);
    __MapActor_SetAnim(9, 2);
    __MapActor_SetAnim(0xa, 2);
    __MapActor_SetAnim(0xb, 2);
    __MapActor_SetAnim(0xc, 2);
    __MapActor_SetAnim(0xd, 2);
    __MapActor_SetAnim(0xe, 0);
    __MapActor_SetAnim(0xf, 0);
    __MapActor_SetPos(0x10, 0, 0);
    __MapActor_SetAnim(0x11, 0);
    __MapActor_SetAnim(0x12, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x15), 0);
    v = 0xffff0000;
    *(int *)(__MapActor_GetActor(0x13) + 0x18) = v;
    *(int *)(__MapActor_GetActor(0x14) + 0x18) = v;
    __WaitFrames(1);
    __MapActor_SetPos(0, 0, 0);
    __WaitFrames(1);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x200;
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x20;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    __Func_80925cc(0x11, 1);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x11; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0x11, 0xa4, 0xe2 << 2);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0x11, 9);
    __CutsceneWait(0x28);
    __MapActor_SetAnim(0x11, 0xa);
    __CutsceneWait(0x3c);
    __MapActor_SetAnim(0x11, 1);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x11; q1 = 0xa4; q2 = 0xe6 << 2;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x11; q1 = 0xb9; q2 = 0xe6 << 2;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x11; q1 = 0xc0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_80921c4(0x11, 0xb9, 0xe5 << 2);
    __MapActor_SetAnim(0x11, 0xb);
    __CutsceneWait(0x28);
    __Func_80925cc(0x11, 1);
    __CutsceneWait(0x3c);
    __Func_80925cc(0x11, 3);
    __CutsceneWait(0x28);
    __MapActor_SetBehavior(0x11, gScript_953__0200af88);
    __CutsceneWait(0x50);
    { PIN2; q0 = 0x11; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0xe; q1 = 0xd0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xf; q1 = 0xa0 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x12; q1 = 0x80 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 0xe; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 0xf; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 0x11; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __MapActor_Surprise(0x12, 0x81 << 1);
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0xe; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xf; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    OvlFunc_953_2009c5c(0x12, 0xc0 << 6);
    { PIN2; q0 = 0x11; q1 = 0x101;
      __MapActor_Surprise(q0, q1); }
    p = __MapActor_GetActor(0x15);
    p[0x55] = 0;
    for (i = 0; i <= 0x13; i++) {
        *(int *)(p + 0xc) += 0x9999;
        __WaitFrames(4);
        *(int *)(p + 0xc) -= 0x4ccc;
        __WaitFrames(4);
    }
    __MapActor_SetAnim(0x13, 6);
    __MapActor_SetAnim(0x14, 6);
    __CutsceneWait(0x3c);
    __MapActor_Surprise(0x11, 0x80 << 1);
    __MapActor_SetIdle(0x11);
    __MapActor_SetAnim(0x11, 1);
    ((struct Actor *)__MapActor_GetActor(0x11))->f6 = 0xd0 << 8;
    w = 0x80 << 9;
    p[0x55] = 3;
    ((struct Actor *)p)->f18 = w;
    ((struct Actor *)p)->f1c = w;
    __CutsceneWait(0xa);
    __PlaySound(0x6b);
    __Func_8012330(w, w, w);
    __CutsceneWait(0xa);
    __PlaySound(0x121);
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __Func_8012350();
    { PIN3; q0 = 0x11; q1 = 0x19999; q2 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x11; q1 = 0xd0; q2 = 0xe8 << 2;
      __Func_80921c4(q0, q1, q2); }
    __PlaySound(0x5c);
    { PIN3; q0 = 0x11; q1 = 0xc0 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0x11, 9);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0x11, 0xa);
    __CutsceneWait(0x28);
    __MapActor_SetAnim(0x11, 9);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0x11, 0xa);
    __CutsceneWait(0x50);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x201;
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x10;
    __MapTransitionOut();
    __WaitMapTransition();
    __SetFlag(0x8a4);
    __Func_8091e9c(0x45);
    __CutsceneEnd();
}
