// fakematch
/* ovl_30_c_c_c_a_c_c_c_a_c_c_c_c.c  --  OvlFunc_882_2008d5c
 *
 *   OK OvlFunc_882_2008d5c -- 360 bytes, 143 encodings and 27 relocations
 *   identical.  Measured three times with tools/objcmp.py.
 *
 * LANDING SHAPE -- WHOLE, NO SPLIT.  asm/overlays/rom_77dd1c/
 * ovl_30_c_c_c_a_c_c_c_a_c_c_c_c.s carries exactly ONE .thumb_func_start and
 * `python3 tools/asmfacts.py` on it reports "WHOLE  convert directly".  The
 * linker line stays VERBATIM, with its asm/ prefix:
 *
 *     overlays/rom_77dd1c/overlay.ld:40
 *         asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_a_c_c_c_c.o(.text)
 *
 * NO MAKEFILE FLAG RULE.  No explicit or wildcard rule in the Makefile names
 * this object, so `asm/%.o: src/%.c` fires at the tree default -O2.  The .s
 * carries no .section/.data/.bss/.word/.byte and no cross-file `.L` symbol.
 *
 * *** STRICT ALIASING IS LOAD-BEARING.  THIS FILE MUST NEVER FALL UNDER AN
 * ALIAS_CFLAGS RULE. ***  -fno-strict-aliasing on the shipping source is 4
 * differing -- exactly the residue this match was parked on.  The two struct
 * tags below ARE the match; the flag destroys them.
 *
 * WHAT THE PARK GOT WRONG.  It was parked at 4 differing with the class read as
 * "ORDERING, two adjacent-instruction reorders -- ALIAS IS THE WRONG AXIS, and
 * the diagnostic says so", on the -fno-schedule-insns2 sign rule (4 -> 32, a
 * regression).  The count was right and the axis was wrong: alias was the ONLY
 * thing that moved either reorder.  This is the THIRD counterexample to the
 * sign rule and it fits the recorded amendment exactly -- the rule is
 * diagnostic when the residue is a REGION, and here sched2 got 139 of 143
 * encodings right and exactly two adjacent ties wrong, so turning it off
 * measured the 139 it was already getting right.  Read the ready list instead.
 *
 * WHAT THE .23.sched2 DUMP SAID, with -fsched-verbose=5.  Both defects are one
 * mechanism.  Reached through `unsigned char *`, the actor byte at +0x23 and
 * the sub-object byte at +0x09 are both ALIAS SET 0, so every store to one
 * carries a memory dependence to every load of the other.
 *
 *   defect 1.  After `strb r3,[r7]` (insn 123, the +0x23 store) the ready list
 *   at t=181 is `152 150 130 488`.  `movs r3,#13` (488) and `ldrb r2,[r6,#9]`
 *   (130) TIE on priority at 27, so priority decides nothing.  They are then
 *   separated by the CLASS test, before the dependent count is ever reached:
 *   488 is ANTI-dependent on 123 (it overwrites r3) = class 2, while 130 is
 *   MEMORY-dependent on 123 at cost 2 = class 1.  Higher class wins, so the
 *   constant takes the slot -- even though 130 has FIVE dependents to 488's
 *   two and would have won the dependent-count step outright.
 *
 *   defect 2.  The +0x09 store (216) and the +0x23 load (223) are joined by the
 *   same set-0 memory dependence, which puts 223 in 216's dependent list and so
 *   lifts 216's priority to 11 against 223's 10.  The ROM issues the load
 *   first.
 *
 * THE CURE, AND IT TAKES BOTH SIDES.  gcc-2.96 gives a distinct alias set per
 * struct tag, so reaching the two bytes through TWO DIFFERENT tags deletes the
 * dependence.  Defect 1's 130 then has no dep on 123 at all -> class 3, and it
 * beats the constant's class 2; defect 2's 216 loses its dependent and the two
 * insns swap.  4 -> 2 in one edit.  Four controls, everything else held fixed:
 *
 *   spelling                                              differing
 *   ---------------------------------------------------   ---------
 *   both bytes `unsigned char` (the park)                      4
 *   only the +0x23 byte given a tag                            4
 *   only the +0x09 byte given a tag                            4
 *   ONE shared tag carrying both bytes                         4
 *   two DISTINCT tags                                          2
 *
 * That is the recorded "THE PAIR MUST SIT IN TWO DISTINCT NAMED ALIAS SETS"
 * exactly, and the shared-tag control is the sharpest form of it yet: one tag
 * with a byte at +0x00 and a byte at +0x09 is worth NOTHING, because a struct
 * COMPONENT_REF takes the FIELD's set and both fields are `unsigned char`.
 * Only the TAG IDENTITY separates them.  Adding an `int` member to
 * struct ActorByte is INERT (still exact), which says the same thing: it is the
 * tag, not the member set.
 *
 * THE LAST TWO ENCODINGS WERE A REGISTER SWAP, NOT AN ORDER.  With the tags in,
 * `q->f09 |= 0xc;` emits `ldrb r2,[r6,#9] / movs r3,#12` against the ROM's
 * `ldrb r3,[r6,#9] / movs r2,#12` -- same `orrs r3,r2` after, so the two insns
 * are the only difference.  READING THE BYTE INTO AN EXPLICIT TEMP FIRST
 * reverses the operands and is exact.  ELEVEN spellings of the OR measure 2
 * differing -- `|=`, `a|K`, `K|a`, a named `unsigned char` or `int` constant in
 * either position, a decimal constant, and a temp holding the RESULT -- while
 * all FOUR spellings that name a temp holding the LOADED BYTE are 0, with
 * `unsigned char` and `int` interchangeable for that temp.
 *
 *   `q->f09 |= 0xc;`                                       2
 *   `{ unsigned char t = q->f09; q->f09 = t | 0xc; }`      EXACT
 *
 * Note the sister site one line below wants the OPPOSITE: the +0x23 byte needs
 * the CONSTANT named (`unsigned char bit = 1;`), and both `ACTOR_BYTE(f) |= 1;`
 * and `ACTOR_BYTE(f) = 1 | ACTOR_BYTE(f);` are 3 differing.  Two byte
 * read-modify-writes, adjacent, opposite answers -- name whichever operand the
 * ROM puts in the SECOND register.
 *
 * THE MASK MUST STAY A NAMED `int`, which the park already settled: written
 * inline, `(q->f09 & ~0xc) | 4` folds the mask to a QImode `movs r3,#243` and
 * wrecks the whole function -- 96 of 143 encodings differ and the relocations
 * move, though with the struct tag in place the byte SIZE happens to stay 360.
 * (The park recorded the same spelling as twelve bytes short; that was measured
 * through `unsigned char`, so the two numbers are not comparable.)
 * `int msk = -13;` is EXACT, so it is the NAMING that matters, not the `~`.
 *
 * PINS.  Twenty-one pinnable call sites; drop-to-fixpoint keeps SEVEN, and
 * width narrowing then takes four PIN3 down to PIN2 and one to PIN1.  The
 * survivors are the two leading __GetFlag guards, the four three-argument calls
 * in the cutscene body whose arguments the ROM materialises out of order, and
 * the tail __Func_80921c4.
 *
 * MEASURED WORSE / INERT (against 143 encodings / 360 bytes):
 *
 *   spelling / flag                                        differing
 *   ----------------------------------------------------   ---------
 *   -fno-schedule-insns2                                        32
 *   -fno-strict-aliasing                                         4
 *   -fno-gcse                                                    0  (INERT)
 *   -fno-strength-reduce                                         0  (INERT)
 *   no struct tag on either byte                                 4
 *   struct tag on the +0x23 byte only                            4
 *   struct tag on the +0x09 byte only                            4
 *   ONE shared tag carrying both bytes                           4
 *   an extra `int` member in struct ActorByte                    0  (INERT)
 *   inline mask `(q->f09 & ~0xc) | 4`                           96  + relocs
 *   `int msk = -13;` instead of `~0xc`                           0  (INERT)
 *   `q->f09 |= 0xc;`                                             2
 *   `{ int t = q->f09; ... }` instead of `unsigned char t`       0  (INERT)
 *   `ACTOR_BYTE(f) |= 1;`                                        3
 *   `ACTOR_BYTE(f) = 1 | ACTOR_BYTE(f);`                         3
 *   the 0x80<<11 __Func_8012330 pin narrowed PIN2 -> PIN1        4
 *   the 0x1d90000 / 0xac<<1 / -1 pins narrowed PIN2 -> PIN1      2  each
 *   NO PINS AT ALL                                             146  (+12 bytes,
 *                                                                   149 insns)
 *
 * Also measured and NOT reproduced: the park's claim that fifteen mask
 * spellings, four byte-field spellings and four barrier placements were all
 * worse.  Those were all measured with BOTH bytes in alias set 0, where the
 * dependence graph forbade the move outright -- none of them could have worked.
 *
 * Harness: scratch_elev/b265/OvlFunc_882_2008d5c -- disf.py + d.sh
 * (side-by-side objdump diff, takes extra cc flags), dump.sh (-da
 * -fsched-verbose=5 dumps), minimise.py (pin drop-to-fixpoint), sweep.py +
 * spec_*.py (spelling sweeps), cmp.sh (one-line objcmp).
 * FAKEMATCH: register-pin idiom, so the name goes in fakematch.txt.
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void __Func_809202c(void);
extern void __Func_80921c4(int slot, int x, int y);
extern void OvlFunc_882_2008ec4(void);
extern void OvlFunc_882_2009a64(int a, int b);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")

/* TWO DISTINCT TAGS, DELIBERATELY -- see the header.  Reaching both bytes
 * through `unsigned char` puts them in alias set 0 and costs two scheduling
 * slots; one shared tag is worth nothing.  Do not merge these.  */
struct ActorByte { unsigned char f00; };
struct SubByte   { unsigned char pad00[9]; unsigned char f09; };

#define ACTOR_BYTE(x)  (((struct ActorByte *)(x))->f00)

void OvlFunc_882_2008d5c(void)
{
    unsigned char *p;
    struct SubByte *q;
    unsigned char *f;
    int m;

    if (({ PIN1; q0 = 0xc4 << 2; __GetFlag(q0); }) != 0)
        return;
    __CutsceneStart();
    if (({ PIN1; q0 = 0x83 << 4; __GetFlag(q0); }) == 0) {
        p = __MapActor_GetActor(0xb);
        q = *(struct SubByte **)(p + 0x50);
        { PIN2; q0 = 0x80 << 11; q1 = 0x80 << 11;
          __Func_8012330(q0, q1, 0x80 << 9); }
        __PlaySound(0x8d);
        f = p;
        __WaitFrames(0x28);
        f += 0x23;
        __PlaySound(0x91);
        ACTOR_BYTE(f) = 0xfe & ACTOR_BYTE(f);
        { int msk = ~0xc; q->f09 = (q->f09 & msk) | 4; }
        { PIN2; q0 = 0xb; q1 = 0x1d90000;
          __MapActor_SetPos(q0, q1, 0xe9 << 18); }
        *(int *)(p + 0x30) = 0xc0 << 9;
        *(int *)(p + 0x34) = 0xc0 << 9;
        *(int *)(p + 0xc) += 0xf0 << 16;
        *(int *)(p + 0x3c) = *(int *)(p + 0xc);
        *(int *)(p + 0x44) = 0x6666;
        { PIN2; q0 = 0xb; q1 = 0xac << 1;
          __Func_80921c4(q0, q1, 0xe9 << 2); }
        { unsigned char t = q->f09; q->f09 = t | 0xc; }
        { unsigned char bit = 1; ACTOR_BYTE(f) = bit | ACTOR_BYTE(f); }
        __CutsceneWait(0x28);
        __PlaySound(0x121);
        { PIN2; q0 = -1; q1 = -1;
          __Func_8012330(q0, q1, 0xe666); }
        __Func_8012350();
        __Func_809202c();
        __SetFlag(0x83 << 4);
    }
    OvlFunc_882_2008ec4();
    __SetFlag(0xc4 << 2);
    if (__GetFlag(0x837) != 0 && __GetFlag(0x841) == 0 && __GetFlag(0xc3 << 2) == 0) {
        if (*(int *)(__MapActor_GetActor(0) + 0xc) > 0x80 << 16) {
            m = 0x396;
            OvlFunc_882_2009a64(0xa3 << 1, m);
            { PIN1; q0 = 0;
              __Func_80921c4(q0, 0x123, m); }
        } else {
            OvlFunc_882_2009a64(0x14f, 0x3bd);
        }
        __SetFlag(0xc3 << 2);
    }
    __CutsceneEnd();
}
