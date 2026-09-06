// fakematch
/* OvlFunc_942_2008e40  --  0x02008e40
 *   [asm/overlays/rom_7c6bac/ovl_30_c_c_c_c_c.s, 1st of 1 -- WHOLE FILE, plus
 *    a trailing `.section .data`, so the landing is a CODE/DATA SPLIT]
 *
 * 179 calls of straight-line cutscene script -- no loop, no conditional, no
 * label except the mid-body pool jump -- around ten `__MapActor_GetActor`
 * pokes at the actor's sprite-flag byte and its 0x50 sub-struct.  VERDICT:
 *
 *   OK OvlFunc_942_2008e40 -- 1728 bytes, 670 encodings and 180 relocations identical
 *
 * tryc is WEAK here: the reference keeps its literal pool INSIDE the function
 * (`.pool_aligned` before `.L126c`, ref line 411), so tryc normalises pool
 * loads to `=value` and cannot see pool order.  Everything below is measured
 * with tools/objcmp.py against the ORIGINAL asm/ path.  Both recorded objcmp
 * false-negative shapes were ruled out independently (scratch_elev/b238/
 * f2008e40/rawcmp.py): raw `.text` bytes compare EQUAL at 1728, and the full
 * relocation list -- offset, type and symbol, 179 R_ARM_THM_CALL plus the one
 * R_ARM_ABS32 for `gScript_942__020096e4` -- is equal element by element.
 *
 * THE PUSH LISTS ARE DISJOINT, AND THAT IS THE WHOLE DIAGNOSIS.  The ROM's
 * prologue is `push {r5, r6, lr} / mov r6, r10 / mov r5, r8 / push {r5, r6}`
 * -- FOUR call-saved registers, and every one of them holds a CHEAP constant:
 *
 *   r10 = 0        the zero stored to actor[0x23], actor[0x59], sub[0x26]
 *   r8  = 0xc      the bit set into sub[9] twice and sub[0x15] once
 *   r5  = 0xfe     the AND mask at four sprite-flag pokes ... then 0xc000
 *   r6  = 1        the OR mask at four sprite-flag pokes
 *
 * Plain C spends SEVEN (`push {r5,r6,r7,lr}` + r8/r9/r10/r11 spilled through
 * r5-r7), 678 encodings against 670 with 604 differing -- and the four it
 * commons are r11 = 0x80<<6, r9 = 0x81<<1, r8 = 0x13333, r7 = 0x6666, i.e. the
 * EXPENSIVE script constants the ROM rebuilds at every use.  The two lists do
 * not intersect.  So this function needs BOTH directions of the constant rule
 * at once: pins to destroy the CSEs gcc invents, and named locals to create
 * the four the ROM has.  ("A WIDE PUSH DOES NOT AUTOMATICALLY MEAN NAMED
 * LOCALS -- READ WHAT IT KEEPS" is the recorded form; here reading it that way
 * says "pins AND locals", not "pins or locals".)
 *
 * THIRTY-NINE PINS from 169 candidate call sites.  All 169 pinned also matches
 * byte for byte.  Greedy stripping, re-tested under objcmp after every drop and
 * run to a fixpoint from BOTH ends of the site list, converges on the SAME 39
 * either way -- unusual, and it means no same-value chain here has two
 * interchangeable survivors.  A one-at-a-time confirmation round over the 39
 * finds every one load-bearing (numbering is the ordinal of the `bl` in the
 * reference).
 *
 * THE FILLS ARE UNIFORM ASCENDING, whole value per statement, at all 39 sites,
 * even where the ROM emits the site interleaved (`mov r2 / mov r0 / mov r1 /
 * lsl r2` at __Func_80921c4(0, 0xd8, 0x88<<1), `mov r1 / mov r0 / lsl r1 /
 * mov r2` at __MapActor_Emote(0xd, 0x81<<1, 0x46)).  sched2 reproduces them.
 * Descending EVERYWHERE costs 91; descending at any ONE of 32 of the 39 costs 2
 * to 7; it is neutral at the other seven (42, 54, 61, 67, 90, 110, 157).
 * `__Func_8092c40`, the documented descending exception, is not called here.
 *
 * CONSUME THE POINTER AT THE EIGHT SPRITE-FLAG POKES.  `p = GetActor(0xa);
 * p += 0x5a; *p &= 0xfe;` gives the ROM's destructive `add r0, #0x5a`;
 * `p[0x5a] &= 0xfe` makes gcc preserve the base (`add r1, r0, #0 / add r1,
 * #0x5a`) at every one of the eight, 678 encodings and 492 differing.
 * `p = p + 0x5a` is byte-identical to `p += 0x5a`.  Note the BOUND on the
 * stronger recorded phrasing "a pointer-returning call whose result dies
 * immediately must not be named": dropping the local entirely and writing
 * `__MapActor_GetActor(0xa)[0x5a] &= 0xfe` is 662 encodings and 580 differing
 * here.  What the ROM asks for is that the pointer be CONSUMED, not that it be
 * anonymous, and `p += K; *p op= C;` -- the recorded "consume the pointer, do
 * not index it" spelling -- consumes it while keeping the name.
 *
 * THREE LOCALS THAT ARE ONLY CORRECT TOGETHER.  With the 39 pins, `p += 0x5a`
 * and the named zero in place, the residue is 21 and the three remaining
 * levers are: a SECOND actor pointer `r` for the site-139 result, a SECOND
 * sub-pointer `s` for the last two `[0x50]` re-reads, and
 * `register int one __asm__("r6")` for the OR mask.  Measured as a full 2x2x2:
 *
 *     r    s    one        differing
 *     -    -    -             21
 *     r    -    -            156   (and 672 encodings -- length wrong too)
 *     -    s    -            159
 *     -    -    one           19
 *     r    s    -              2
 *     r    -    one          156
 *     -    s    one          159
 *     r    s    one            0   <- byte-identical
 *
 * NEW, and grepped by concept first ("jointly", "BOTH HALVES", "change ONE
 * thing at a time", "only together", "two results of the same call need two
 * pointer variables"): the recorded failure mode is that bundling changes HIDES
 * a bad one -- "a change that HELPS and a change that HURTS cancel" -- and the
 * recorded remedy is to measure one at a time from a common baseline.  This
 * function is the mirror image: measured one at a time, `r` alone is SEVEN
 * TIMES WORSE than the baseline and lengthens the function, `s` alone is worse
 * still, and `one` alone buys two -- and all three together are exact.  A
 * one-at-a-time sweep here reports "two of the three levers are actively
 * harmful" and is wrong about both.  The mechanism is that `r` and `s` are two
 * halves of ONE statement about live ranges (the ROM keeps the site-139 actor
 * pointer in r1 across three `[0x50]` re-reads, which needs the pointer that is
 * NOT re-read to be a different pseudo from the one that is); supplying either
 * half alone hands local-alloc a register assignment that is inconsistent with
 * the other half's, and it spends a call-saved register to resolve it.  The
 * existing "individually-inert pins are not jointly removable" rule is about
 * subtraction; this is the same non-additivity showing up in ADDITION, and it
 * is worth stating because the discipline it breaks -- "measure one lever at a
 * time" -- is the one the method leans on hardest.
 *
 * THE `orr` POLARITY SPLIT IS THE RECORDED ONE, re-measured not transplanted.
 * `OvlFunc_943_200bc88` (src/overlays/rom_7c7b9c/ovl_30_c_c_b.c) is the same
 * shape -- 0xfe and 1 in the push, four pokes -- and "A COMMUTATIVE OP TIES ITS
 * DESTINATION TO WHICHEVER OPERAND IS WRITTEN FIRST" applies verbatim: three
 * sites emit `orr r3, r6` (r6 still live, the VALUE is the destination, plain
 * `*p |= one`), the fourth emits `orr r6, r3 / strb r6` (r6 dies, the CONSTANT
 * is the destination, so the variable must be the accumulator).  Measured here:
 *
 *     `*p |= 1` everywhere, no local                     2 differing
 *     `int one`, ditto                                   2   (cprop folds it back)
 *     `*p = 1 | *p` / `*p = *p | 1` / a temp at site 142  2   (four spellings, all 2)
 *     `unsigned char m1`, site 142 only                678 encodings, 615 differing
 *     `unsigned char` masks at all eight sites          670 encodings,  67 differing
 *     `register int one __asm__("r6")` + accumulator      0
 *
 * `one |= *p; *p = one;` and the one-statement `*p = one | *p;` are
 * byte-identical to each other; the two-statement form is shipped because that
 * is the spelling `ovl_30_c_c_b.c` already carries.  The AND mask stays a BARE
 * LITERAL -- gcc commons 0xfe into r5 unaided and naming it is what breaks it.
 *
 * THE NAMED ZERO IS REQUIRED AND ITS POSITION IS PART OF IT.  `z = 0;` placed
 * immediately after the first `__MapActor_GetActor(0xb)` -- where the ROM has
 * `mov r1, #0 / mov r10, r1` -- is exact; a bare `0` at the three stores is 668
 * encodings and 624 differing (gcc rematerialises and never spends r10), and
 * the same local assigned at the TOP of the function is 283 differing (the two
 * extra entry instructions shift every pool displacement and move the mid-body
 * pool dump one instruction).  `int`, `unsigned char` and
 * `register int z __asm__("r10")` are all byte-identical, so plain `int` ships.
 *
 * THE 0xc000 HALFWORD STORE NEEDS AN `int`.  `*(unsigned short *)(s + 0x1e) =
 * 0xc000;` pools the literal (the recorded HImode rule: 0 and >= 0x8000), 677
 * encodings and 149 differing.  `int v = 0xc0 << 8;` gives the ROM's `mov r5,
 * #0xc0 / lsl r5, #8`, and the same `v` is then the argument at the two later
 * sites where the ROM reads r5 -- which is what r5's SECOND live range is for.
 * `register int v __asm__("r5")` is byte-identical and therefore not shipped.
 *
 * INERT SCAFFOLDING THAT WAS STRIPPED, each re-measured under objcmp: a named
 * `int` for the 0xc mask (gcc commons it into r8 unaided); `s += 0x26; *s = z;`
 * in place of `s[0x26] = z` (the offset needs an `add` either way and both
 * spellings are exact); a third sub-pointer local; `register` bindings for z
 * and v; and the flat-literal spelling of every `0x80 << 9`-style constant
 * (byte-identical -- the `<<` form is kept because it reads as the ROM's
 * `mov`/`lsl` pair).
 *
 * NO FLAG GROUP.  This TU builds at the tree default -O2 through the generic
 * `asm/%.o: src/%.c` rule; no explicit rule and no wildcard in the Makefile
 * names `rom_7c6bac/ovl_30_c_c_c_c_c` (the three CSE_CFLAGS rules that DO name
 * rom_7c6bac are all explicit, for other stems), and objcmp's own
 * `cflags_for()` on the destination path reports "makefile adjustments: none".
 * `-fno-rerun-cse-after-loop` and `-fno-gcse` are both byte-identical here, so
 * neither is needed and neither must be added; `-fno-schedule-insns2` costs 229
 * (sched2 is what produces the ROM's interleaved fills from the uniform ones).
 * Freshness control: `-fno-omit-frame-pointer` moves the result to 672
 * encodings and 418 differing, so the toolchain is live.
 *
 * LANDING NEEDS A CODE/DATA SPLIT.  The `.s` holds ONE function (one
 * `.thumb_func_start`) and then a `.section .data` of 24 `.incbin` blobs under
 * 24 `.global` labels, 23 of which are referenced from OTHER files (from
 * gScript_930__020096b8 at ten to `.L1dcc` at thirteen), so the data cannot be
 * dropped; the 24th, gScript_942__020096e4, is the one this function itself
 * passes to __MapActor_SetBehavior, and it is already `.global`, so the split
 * needs no new export either way.  All of it is strictly AFTER
 * `.func_end` and the only in-function label, `.L126c`, is a branch target of
 * the function, so tools/split_s.py's code/data path applies directly:
 *
 *     python3 tools/split_s.py asm/overlays/rom_7c6bac/ovl_30_c_c_c_c_c.s \
 *         OvlFunc_942_2008e40
 *
 * writes the function to `..._b.s` and the data to `..._c.s` (`_a` is empty and
 * not written) and rewrites BOTH linker lines.  The two lines in
 * overlays/rom_7c6bac/overlay.ld that name this object, cited by content:
 *
 *     asm/overlays/rom_7c6bac/ovl_30_c_c_c_c_c.o(.text)
 *         -- last entry of the `.text` list before src/overlays/rom_7c6bac/
 *            imports.o(.text), immediately after ovl_30_c_c_c_c_b.o(.text)
 *     asm/overlays/rom_7c6bac/ovl_30_c_c_c_c_c.o(.data)
 *         -- last of the four entries in the `.data` list, immediately after
 *            ovl_30_c_c_c_c_b.o(.data)
 *
 * each becoming a `_b.o(...)` line followed by a `_c.o(...)` line in the same
 * position.  Run the byte-neutral `make compare` WITH THE FUNCTION STILL IN
 * ASSEMBLY before dropping this C in; then the C lands at
 * src/overlays/rom_7c6bac/ovl_30_c_c_c_c_c_b.c and needs NO Makefile rule.
 *
 * Reproduce: scratch_elev/b238/f2008e40/gen.py emits this file from a spec of
 * pinned call-site numbers plus lever tokens; minimise.py runs the greedy sweep
 * from either end and sweep.py the measured tables, each inside ONE container
 * invocation; parse2.py is the abstract interpreter that read the call sites
 * and their argument values out of the reference; diff.py aligns the two
 * disassemblies; rawcmp.py is the independent byte/relocation check.
 */
extern void __SetFlag(int f);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __PlaySound(int id);
extern void __ActorMessage(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int id);
extern void __MapActor_SetBehavior(int slot, unsigned char *script);
extern void __Func_808e118(void);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80922c4(int a, int b, int c);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

extern unsigned char gScript_942__020096e4[];

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_942_2008e40(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *r;
    unsigned char *s;
    register int one __asm__("r6");
    int z;
    int v;

    __SetFlag(0x8ab);
    __CutsceneStart();
    __Func_808e118();
    __MessageID(0x23eb);
    p = __MapActor_GetActor(0xb);
    z = 0;
    p[0x23] = z;
    q = *(unsigned char **)(p + 0x50);
    q[9] |= 0xc;
    q = *(unsigned char **)(p + 0x50);
    q[0x15] |= 0xc;
    __Func_80933f8(0xe8 << 16, -1, 0x98 << 17, 1);
    { PIN3; q0 = 0; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xd8; q2 = 0x88 << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x80 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093530();
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xd; q1 = 0x81 << 1; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xd, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xa; q1 = 0x107; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8092adc(0xa, 0, 0);
    __MapActor_Jump(0xa, 4, 0xd);
    __MapActor_Jump(0xa, 4, 0x1e);
    __ActorMessage(0xa, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xb, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xd, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xd, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xa; q1 = 0x103; q2 = 0x37;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0xa, 0x10, 0);
    __MapActor_Jump(0xa, 7, 0);
    __Func_8092304(0xa, 0x18, 0);
    p = __MapActor_GetActor(0xa);
    p += 0x5a;
    *p &= 0xfe;
    { PIN3; q0 = 0xa; q1 = -0x10; q2 = 0;
      __Func_80922c4(q0, q1, q2); }
    __PlaySound(0x99);
    { PIN3; q0 = 0xd; q1 = 0x26666; q2 = 0x13333;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0xd, 0x10, 0);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0xa, 1);
    p = __MapActor_GetActor(0xa);
    p += 0x5a;
    one = 1;
    *p |= one;
    { PIN2; q0 = 0xd; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __Func_809259c(0xd, 2);
    __PlaySound(0x9b);
    __CutsceneWait(0xa);
    __PlaySound(0x9b);
    __CutsceneWait(0xa);
    __PlaySound(0x9b);
    __CutsceneWait(0xa);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xd; q1 = 0x6666; q2 = 0x3333;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_Jump(0xd, 6, 0);
    __PlaySound(0x9f);
    { PIN3; q0 = 0xd; q1 = -8; q2 = 0;
      __Func_8092304(q0, q1, q2); }
    __CutsceneWait(0x14);
    __ActorMessage(0xa, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xd; q1 = 0x81 << 1; q2 = 0x46;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x10; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x10; q1 = -8; q2 = 0;
      __Func_8092304(q0, q1, q2); }
    { PIN3; q0 = 0x10; q1 = 0xa0 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0x10, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0x10, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xa; q1 = 0xe0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x23);
    __Func_80925cc(0xa, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xa, 0);
    __CutsceneWait(0xa);
    __Func_8092adc(0x10, 0x80 << 6, 0);
    __CutsceneWait(0x37);
    __Func_8092adc(0x10, 0xa0 << 7, 0);
    __CutsceneWait(0x1e);
    __ActorMessage(0x10, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xb; q1 = 0xe0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xb; q1 = 0x81 << 1; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xb, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xd, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xd, 0);
    __CutsceneWait(0xa);
    __Func_8092adc(0xd, 0xa0 << 8, 0);
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0xd; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __ActorMessage(0xd, 0);
    __CutsceneWait(0xa);
    __Func_8092adc(0xa, 0, 0);
    __Func_8092adc(0xb, 0, 0);
    __CutsceneWait(0x14);
    __Func_80925cc(0xa, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xa, 0);
    { PIN3; q0 = 0xa; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0xa, 8, 0);
    { PIN3; q0 = 0x10; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x10; q1 = -8; q2 = 0x10;
      __Func_8092304(q0, q1, q2); }
    { PIN3; q0 = 0x10; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __ActorMessage(0x10, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xa; q1 = 0x81 << 1; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    p = __MapActor_GetActor(0xa);
    p += 0x5a;
    *p &= 0xfe;
    { PIN3; q0 = 0xa; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = -8; q2 = 0;
      __Func_8092304(q0, q1, q2); }
    p = __MapActor_GetActor(0xa);
    p += 0x5a;
    *p |= one;
    __CutsceneWait(0x14);
    __Func_80925cc(0xa, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xa, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x10, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0x10, 0);
    __CutsceneWait(0xa);
    p = __MapActor_GetActor(0xa);
    p += 0x5a;
    *p &= 0xfe;
    { PIN3; q0 = 0xa; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0xa, -0x10, 0);
    p = __MapActor_GetActor(0xa);
    p += 0x5a;
    *p |= one;
    __Func_8092adc(0xa, 0, 0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0xa, 4);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xa; q1 = 0x1cccc; q2 = 0xe666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0xa, 8, 0);
    __MapActor_Jump(0xa, 6, 0);
    __Func_8092304(0xa, 0x18, 0);
    __PlaySound(0x85);
    __MapActor_Jump(0x10, 6, 0);
    __MapActor_SetBehavior(0x10, gScript_942__020096e4);
    p = __MapActor_GetActor(0xa);
    p += 0x5a;
    *p &= 0xfe;
    __MapActor_Jump(0xa, 6, 0);
    { PIN3; q0 = 0xa; q1 = -0xc; q2 = 4;
      __Func_8092304(q0, q1, q2); }
    r = __MapActor_GetActor(0xa);
    r[0x59] = z;
    r[0x23] = 2;
    q = *(unsigned char **)(r + 0x50);
    q[9] |= 0xc;
    s = *(unsigned char **)(r + 0x50);
    s[0x26] = z;
    v = 0xc0 << 8;
    s = *(unsigned char **)(r + 0x50);
    *(unsigned short *)(s + 0x1e) = v;
    { PIN3; q0 = 0xa; q1 = -0xc; q2 = 4;
      __Func_8092304(q0, q1, q2); }
    __Func_8092adc(0xa, 0x80 << 7, 0);
    p = __MapActor_GetActor(0xa);
    p += 0x5a;
    one |= *p;
    *p = one;
    __PlaySound(0x9f);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xb; q1 = 0x81 << 1; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xc0 << 9; q2 = v;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0xb, 0x18, 0);
    __Func_8092adc(0xb, v, 0);
    __CutsceneWait(0xa);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x10, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0x10, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x10; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x10; q1 = -8; q2 = 0;
      __Func_8092304(q0, q1, q2); }
    __CutsceneWait(0x14);
    __ActorMessage(0x10, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xa, 2);
    __CutsceneWait(0x14);
    __CutsceneWait(0xa);
    __Func_8092adc(0x10, 0, 0);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(0xd, 3);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xd, 3);
    __CutsceneWait(0x14);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x10; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x10; q1 = 0x18; q2 = -0x18;
      __Func_8092304(q0, q1, q2); }
    __Func_8092304(0x10, 8, 0);
    __Func_8092adc(0x10, 0xe0 << 8, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xd; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0; q2 = -8;
      __Func_8092304(q0, q1, q2); }
    __CutsceneWait(0xa);
    __CutsceneEnd();
}
