/* OvlFunc_907_2008584  --  0x02008584
 *   [asm/overlays/rom_79b154/ovl_30_c_a_c_a.s, 1st of 1]
 *
 * A 296-instruction cutscene behind TWO save-flag guards (0x845 and 0x848),
 * with a two-armed __MessageID branch chosen by comparing two actors' field 8,
 * and a long shared tail.  Message base 0x1775; sets save bit 0x849.
 *
 * LANDING NEEDS NOTHING.  The .s holds exactly ONE function, has NO .data and
 * NO .rodata section, and exactly one linker line anywhere in the tree names
 * its .o -- overlays/rom_79b154/overlay.ld:37,
 * `asm/overlays/rom_79b154/ovl_30_c_a_c_a.o(.text)`.  The tree's solved C files
 * still build to their `asm/**.o` path through the default `asm/%.o: src/%.c`
 * rule, so that line is already correct and needs no remap.  NO split, NO
 * linker-script change.
 *
 * NO FLAG GROUP IS INVOLVED, and that was measured rather than assumed.  Nothing
 * in the Makefile mentions rom_79b154 at all -- no per-file rule and no wildcard
 * -- so this TU falls to the tree default at -O2, and objcmp against the
 * ORIGINAL asm path prints no `(built with: ...)` line.  At the fixpoint
 * -fno-rerun-cse-after-loop and -fno-gcse are BOTH INERT (0 differing either
 * way) and -fno-schedule-insns2 is far worse (80 differing), so sched2 is
 * WANTED and no flag may be added.
 *
 * THE WALK IS THE WHOLE FUNCTION.  144 of the 149 differences in the first
 * screen came from one spelling.  The ROM reaches actor+0x5a four times with a
 * DESTRUCTIVE `add r0, #0x5a` on the pointer GetActor just returned in r0;
 * `a[0x5a]` makes gcc materialise the sum in a fresh pseudo (`adds r2, r0, #0 /
 * adds r2, #90`), one extra instruction at each of the four sites and a whole
 * extra callee-saved register.  `a += 0x5a; *a ...` -- the walk form the
 * elevation notes record for `symbol + K` -- is exact.  Measured at the
 * fixpoint: without it, 144 differing and `push {r5, r6, r7, lr}`.
 *
 * TWO POINTERS, NOT ONE, AND THE REASON IS A CALL.  `p` (the actor from
 * GetActor(0)) is read only once, at `ldr r3, [r5, #8]`, but TWO __GetFlag calls
 * stand between the fetch and the read, so p must be callee-saved.  The four
 * later GetActor(0xe) results are read immediately and want r0.  Written as ONE
 * variable they are one pseudo, gcc gives the whole thing r7, and every later
 * site pays a copy: 148 differing with the wide push.  This is the
 * "disjoint live ranges should be two variables" rule, with the discriminator
 * being which side of a call the use falls on.
 *
 * `m __asm__("r6")` IS REQUIRED AND `n __asm__("r5")` IS HARMFUL -- the same
 * lever, opposite directions, on two masks three instructions apart.  The ROM
 * holds 0xfe in r5 and 1 in r6 across overlapping ranges.  n reaches r5 unaided;
 * PINNING it costs 142 (and makes the output SHORTER than the ROM at 776 bytes,
 * the "something that should be live is missing" tell -- the pin lets gcc kill
 * r5 at the first `and` and rematerialise later).  m does NOT reach r6 unaided:
 * unnamed it is rematerialised into r2 at the first site (49 differing), and as
 * `unsigned char m` it is worse still (113).  Note m ALONE, before the walk was
 * found, measured WORSE than no pin at all (146 against 141); it only pays once
 * the walk is in.  Do not read a lever's sign before the rest of the function
 * is right.
 *
 * THE `and` PAIR IS INSENSITIVE AND THE `orr` PAIR IS NOT.  All of `*a &= n`,
 * `*a = n & *a` and `*a = *a & n` are byte-identical at BOTH `and` sites -- the
 * recorded "the statement form is not the lever" for `and`, confirmed here with
 * `int n` -- so both are written the plain way.  The `orr` pair is the opposite
 * and each site wants a DIFFERENT spelling: the first must be `*a |= m` (mask
 * live afterwards, so gcc is forced to put the loaded byte in the destination,
 * `orr r3, r6`), and the second must be the accumulate `m |= *a; *a = m;`
 * (`orr r6, r3`, mask in the destination).  Swapping either costs exactly 2.
 *
 * TWENTY PINS, AND THE SET IS A TRUE FIXPOINT.  All 34 ordering candidates were
 * pinned, then dropped one at a time until no further drop held -- run
 * SEPARATELY forward and backward, and BOTH directions converged on the SAME 20.
 * Fourteen came out.  Three of the survivors (the __Func_8092a1c trio) cost
 * 1296 when dropped because they also lose the shared `mov r2, r5`; the rest
 * cost 2 or 17.  Sites whose ROM setup is already plain ascending were pruned
 * before the sweep and none was needed back.
 *
 * A PIN SET THAT NEEDS A HOLE, and it is the one site whose fill looked most
 * obviously wrong.  __Func_80933f8's ROM group is
 * `mov r1,#1 / mov r2,#0xad / lsl r2,#16 / mov r3,#1 / ldr r0 / neg r1`, and
 * transcribing it verbatim leaves 2 differing with the two `mov`s transposed.
 * All 185 valid orderings of that fill were compiled and 8 reproduce the ROM --
 * but at the fixpoint the site is best left UNPINNED entirely, and the plain
 * `__Func_80933f8(0x107 << 16, -1, 0xad << 16, 1)` is exact.  Dropping a pin was
 * the fix; adding the right one was merely A fix.
 *
 * NEW FINDING -- THE CONSTANT-CSE HOIST IS DEFEATED BY A TWO-ARMED `if/else`,
 * AT DEFAULT FLAGS, WITH NO FLAG NEEDED.  This function uses the pool constant
 * 0x4ccc at two call sites, __Func_80933d4 near the top and __MapActor_SetSpeed
 * far below, with the first use DOMINATING the second.  Both the
 * "Pool-constant CSE: the complete rule" table (`a branch | default | hoisted`)
 * and the dominance probe (`one use dominating + one in a branch -> HOISTS`)
 * predict gcc hoists it into a callee-saved register and pays a push.  It does
 * not: the ROM reloads it and so do we, byte for byte, at -O2 with no flag.
 * Probed in isolation under this tree's exact flags, six one-file variants:
 *
 *     between the two dominating uses          result
 *     nothing (straight line)                  HOISTS, push {r5, lr}
 *     a ONE-ARMED `if`                         HOISTS, push {r5, lr}
 *     an `if/else` with an EMPTY else          HOISTS  (gcc collapses it)
 *     a REAL two-armed `if/else` diamond       REBUILDS, push {lr}
 *     TWO such diamonds                        REBUILDS, push {lr}
 *     a diamond placed BEFORE both uses        HOISTS, push {r5, lr}
 *
 * So the recorded table's `a branch` row was measured with a ONE-ARMED if, and
 * one-armed versus two-armed is the discriminator -- not the presence of a
 * label, not distance, not the number of intervening calls.  This does NOT
 * unblock the genuinely straight-line script band, which still has no diamond to
 * offer.  What it does is reclassify every candidate that repeats a pool
 * constant across a real if/else: those are reachable at default flags, and
 * tools/blocked_cse.py and tools/script_candidates.py currently reject them.
 * This function is one such, and it matched on the default rule with no flag.
 */

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __ActorMessage(int slot, int a);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_801776c(int a, int b);
extern void __Func_808f1c0(int a, int b);
extern void __Func_8091a58(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092a1c(int a, int b, unsigned char *c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern unsigned char ActorCmd_ARRAY_907__020091c0[];
extern unsigned char *iwram_3001ebc;

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_907_2008584(void)
{
    unsigned char *p;
    unsigned char *a;
    int n;
    register int m __asm__("r6");
    unsigned char *s;

    p = __MapActor_GetActor(0);
    if (__GetFlag(0x845) != 0 && __GetFlag(0x848) != 0) {
        __CutsceneStart();
        __Func_80933d4(0x26666, 0x4ccc);
        __Func_80933f8(0x107 << 16, -1, 0xad << 16, 1);
        __Func_8093530();
        if (*(int *)(__MapActor_GetActor(0xc) + 8) > *(int *)(p + 8)) {
            { PIN3; q1 = 0xa0; q0 = 0xd; q1 <<= 7; q2 = 0x14;
              __Func_8092adc(q0, q1, q2); }
            __MapActor_Emote(0xd, 0x80 << 1, 0x14);
            __MessageID(0x1775);
            __Func_8093040(0xd, 0, 0xa);
            { PIN3; q1 = 0x80; q0 = 0xc; q1 <<= 1; q2 = 0;
              __MapActor_Emote(q0, q1, q2); }
        } else {
            { PIN3; q1 = 0xc0; q0 = 0xc; q1 <<= 6; q2 = 0x14;
              __Func_8092adc(q0, q1, q2); }
            __MapActor_Emote(0xc, 0x80 << 1, 0x14);
            __MessageID(0x1775);
            __Func_8093040(0xc, 0, 0xa);
            { PIN3; q1 = 0x80; q0 = 0xd; q1 <<= 1; q2 = 0;
              __MapActor_Emote(q0, q1, q2); }
        }
        { PIN3; q1 = 0x80; q0 = 0xe; q1 <<= 1; q2 = 0;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q1 = 0xc0; q0 = 0xe; q1 <<= 6; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0xa0; q0 = 0xc; q1 <<= 7; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0xc0; q0 = 0xd; q1 <<= 6; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0x86; q0 = 0; q1 <<= 1; q2 = 0xb8;
          __Func_80921c4(q0, q1, q2); }
        { PIN3; q1 = 0xc0; q2 = 0x28; q0 = 0; q1 <<= 8;
          __Func_8092adc(q0, q1, q2); }
        __Func_80925cc(0xd, 2);
        __Func_8093040(0xd, 0, 0xa);
        __Func_8092adc(0xd, 0, 0);
        { PIN3; q1 = 0xc0; q0 = 0xe; q1 <<= 6; q2 = 0x14;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0x80; q2 = 0x14; q0 = 0xc; q1 <<= 8;
          __Func_8092adc(q0, q1, q2); }
        __MapActor_DoAnim(0xc, 3);
        __MapActor_Surprise(0xe, 0x81 << 1);
        __CutsceneWait(0x28);
        { PIN3; q1 = 0xc0; q0 = 0xe; q1 <<= 6; q2 = 0xa;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0xa0; q0 = 0xc; q1 <<= 7; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0xc0; q2 = 0xa; q0 = 0xd; q1 <<= 6;
          __Func_8092adc(q0, q1, q2); }
        __Func_80925cc(0xe, 1);
        __Func_8093040(0xe, 0, 0xa);
        __MapActor_SetAnim(0xc, 3);
        __MapActor_DoAnim(0xd, 3);
        __CutsceneWait(0x14);
        __ActorMessage(0xe, 0);
        __MapActor_SetSpeed(0xe, 0x9999, 0x4ccc);
        a = __MapActor_GetActor(0xe);
        a += 0x5a;
        n = 0xfe;
        *a &= n;
        __Func_80921c4(0xe, 0x85 << 1, 0xac);
        __CutsceneWait(1);
        a = __MapActor_GetActor(0xe);
        a += 0x5a;
        m = 1;
        *a |= m;
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0xe, 3);
        __Func_8093040(0xe, 0, 0xa);
        __Func_801776c(0x177a, 1);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __Func_808f1c0(0xc2, 3);
        { PIN2; q1 = 0; q0 = 0xc2;
          __Func_8091a58(q0, q1); }
        __MapActor_DoAnim(0xe, 3);
        __MapActor_SetAnim(0, 1);
        { PIN3; q1 = 0xc0; q0 = 0; q1 <<= 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __MapActor_SetSpeed(0xe, 0x80 << 9, 0x80 << 8);
        a = __MapActor_GetActor(0xe);
        a += 0x5a;
        *a &= n;
        __Func_80921c4(0xe, 0x83 << 1, 0x9c);
        __CutsceneWait(1);
        a = __MapActor_GetActor(0xe);
        a += 0x5a;
        m |= *a;
        *a = m;
        __CutsceneWait(0x14);
        __Func_80925cc(0xc, 2);
        __Func_8093040(0xc, 0, 0xa);
        __MapActor_SetAnim(0xc, 3);
        __MapActor_SetAnim(0xd, 3);
        __MapActor_DoAnim(0xe, 3);
        s = ActorCmd_ARRAY_907__020091c0;
        { PIN2; q1 = 0x80; q0 = 0xc; q1 <<= 9;
          __Func_8092a1c(q0, q1, s); }
        { PIN2; q1 = 0x80; q0 = 0xd; q1 <<= 9;
          __Func_8092a1c(q0, q1, s); }
        { PIN2; q1 = 0x80; q0 = 0xe; q1 <<= 9;
          __Func_8092a1c(q0, q1, s); }
        __SetFlag(0x849);
        __CutsceneEnd();
    }
}
