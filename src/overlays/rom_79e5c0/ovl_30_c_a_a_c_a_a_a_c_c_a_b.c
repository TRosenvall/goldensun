// fakematch
/* ovl_30_c_a_a_c_a_a_a_c_c_a.c  --  OvlFunc_911_20083c8 + OvlFunc_911_2008694
 *   [the WHOLE of asm/overlays/rom_79e5c0/ovl_30_c_a_a_c_a_a_a_c_c_a.s -- both
 *    `.thumb_func_start`s, so NO SPLIT would be required and
 *    overlays/rom_79e5c0/overlay.ld:31 stays VERBATIM as
 *    `asm/overlays/rom_79e5c0/ovl_30_c_a_a_c_a_a_a_c_c_a.o(.text)`]
 *
 *   MUST BE BUILT WITH GCSE_CFLAGS (-fno-gcse).  makefile_flags() on both
 *   paths is the EMPTY set today and there is no wildcard hazard
 *   (WILDCARD_HITS empty), so landing this needs ONE new explicit rule
 *     asm/overlays/rom_79e5c0/ovl_30_c_a_a_c_a_a_a_c_c_a.o: \
 *         src/overlays/rom_79e5c0/ovl_30_c_a_a_c_a_a_a_c_c_a.c
 *             $(GCC296_CC) $(GCSE_CFLAGS) -S -o $(@:.o=.s) $<
 *
 *   OK OvlFunc_911_20083c8 -- 716 bytes, 291 encodings and 60 relocations
 *      identical, WITH -fno-gcse.  Measured three times.
 *   PARK OvlFunc_911_2008694 -- 7 of 142 encodings, SIZE and every relocation
 *      offset SILENT.  TWO of the seven are the `_AREA_26`/`_AREA_27` pool
 *      words (see below) and `make compare` resolves them; the real residue is
 *      FIVE, and it is one register transposition at one site.
 *
 *   THE FILE THEREFORE DOES NOT LAND.  asmfacts.py says `2 functions / split
 *   first`, so shipping 20083c8 alone would mean a hand split of the .s and a
 *   new overlay.ld line.  Closing 2008694's five encodings is the cheaper
 *   route and it is the whole remaining job on this file.
 *
 * -------------------------------------------------------------------------
 * 20083c8: A LOOP-INVARIANT CONSTANT THE ROM KEEPS IN r8, AND -fno-gcse IS THE
 * ONLY THING THAT KEEPS IT THERE.
 *
 * The function spawns ten actors in a `for (i = 0; i <= 9; i++)` and the loop
 * body reads 0x10000 twice: `q->f2c = w` and `((i & 3) << 16) + w`.  The ROM
 * holds it in r8 for both -- `add r3, r8` (the hi-register alternative of
 * *thumb_addsi3, operand 2 constraint `*h`) and `mov r3, r8`.  gcc-2.96 at the
 * tree default keeps r8 for the store and REMATERIALISES `mov r2,#0x80 /
 * lsl r2,#9` for the add: two instructions long, 96-99 differing, and NOTHING
 * in the C reaches it.  Measured inert: `w + t` vs `t + w`, a separate `int t`
 * for the sum, `q->f2c = w` written first, `u = w; u += t`, `(int)` casts on
 * either operand, and reading the value back as `q->f2c`.  Writing both uses
 * as literals instead is 274 differing and TWELVE BYTES SHORT, which is the
 * proof that the single named local is right.
 *
 * THE PASS IS gcse, AND THE FLAG SWEEP NAMES IT IN ONE COMPILE.  On the
 * w-before-the-loop spelling: plain 98, -fno-rerun-cse-after-loop 98,
 * -fno-strength-reduce 98, -fno-cse-follow-jumps 98,
 * -fno-expensive-optimizations 32, **-fno-gcse 2**.  NEW, and it is a shape
 * worth adding to the recorded -fno-gcse family: not a pooled constant and not
 * a hoisted load in front of a switch, but a LOOP-INVARIANT CONSTANT THAT THE
 * ROM KEEPS IN A HIGH REGISTER AND GCSE SPLITS IN TWO.  The tell is a
 * candidate that is exactly TWO INSTRUCTIONS LONG with `mov rX, r8` present
 * for one use of a value and `mov/lsl` rebuilt for another.
 *
 * WITH -fno-gcse THE PREHEADER STATEMENT ORDER IS THE LAST TWO ENCODINGS, and
 * all six orders were measured:
 *
 *     i = 0; w = ...; y = ...;     0   <-- ships
 *     w = ...; i = 0; y = ...;     2
 *     w = ...; y = ...; i = 0;     3
 *     i = 0; y = ...; w = ...;     4
 *     y = ...; w = ...; i = 0;     4
 *     y = ...; i = 0; w = ...;     5
 *
 * Without the flag the best spelling is a DIFFERENT one -- `w = 0x80 << 9;`
 * moved INSIDE the `if (q != 0)` block, where loop-invariant motion hoists it
 * to the END of the preheader -- and that reaches 4 differing and no closer.
 * The two spellings are alternatives, not a ladder: the in-loop one is 100
 * differing under -fno-gcse and the pre-loop one is 96 without it.
 *
 * THE REST OF 20083c8 IS THE ORDINARY PIN LADDER.
 *
 *     plain C, no pins                                    236 (exact size)
 *     + a pin block at every multi-argument site            99 (+2 insns)
 *     + `(int)` on the shift so `>> 1` is ASR not LSR       99 (inert here;
 *          it is worth 1 encoding and was folded into the next step)
 *     + w placed so gcse does not split it                   7
 *     + `i = 0;` before `y`                                  6
 *     + the __Func_8092a1c fill written q2, q0, q1            4
 *     + -fno-gcse with w before the loop                      0
 *
 * The __Func_8092a1c fill CONFIRMS the sibling ovl_30_c_a_c_c_c_a_a.c's
 * recorded CALLEE CLASS on a third instance: the pooled script pointer must be
 * nominated BEFORE the two `mov`s or sched2 sinks its `ldr` past them.
 * Ascending is 2 encodings worse here, exactly as it was there.
 *
 * TWO SIX-ARGUMENT SITES NEED THE STACK-ARGUMENT PIN and four do not.  The
 * four inside the CopyMapTiles run reuse r5=5 and r6=4 across calls, which is
 * gcc's own commoning and needs nothing.  The two AFTER the loop have lost
 * those registers to the loop, and there `register int s0 __asm__("r3");
 * register int s1 __asm__("r2");` assigned in that order is exact -- the
 * sibling's recorded `mov rX / str / mov rX / str` tell, on __CopyMapTiles and
 * __Func_8010704.
 *
 * `-fno-schedule-insns2` IMPROVES 20083c8 (236 -> 223) at the plain-C stage,
 * so sched2 owned part of the residue, and the pins took it.
 *
 * -------------------------------------------------------------------------
 * 2008694: THE gState IDIOM, TWO _AREA SYMBOLS, AND ONE UNREACHABLE
 * TRANSPOSITION.
 *
 * `ldr r3, =0x27` and `ldr r3, =0x26` are POOLED CONSTANTS BELOW 256 -- the
 * recorded tell that the source referenced a SYMBOL.  gState+0x1C0 is the area
 * id (see area.sym), so they are `(int)(&_AREA_27)` and `(int)(&_AREA_26)`.
 * Written as plain literals gcc emits `cmp r3, #39` and the pool changes; with
 * the symbols the pool word order is the ROM's exactly.  As recorded, this
 * costs ONE differing encoding per symbol plus an ours-only relocation, and
 * `make compare` is what decides: `_AREA_27 = 0x27` in area.sym, so the linked
 * bytes are identical.  TWO of the seven are these and they are EXPECTED.
 *
 * THE THREE ADDRESS SITES ALL WANT THE NEIGHBOUR'S REBUILD IDIOM but NOT the
 * same spelling of it, and this is new:
 *
 *   site 1 (gState + 0xe0<<1, ldrsh): `r2 = 0xe0; r2 <<= 1;
 *       r3 = (unsigned int)&gState + r2;` -- the ONE-STATEMENT `+ r2` form.
 *       The two-statement `r3 = &gState; r3 += r2;` puts the pool load in r2
 *       and costs 2.
 *   site 2 (iwram_3001ebc + 0xe0<<1): needs a PIN, `register unsigned int b
 *       __asm__("r3"); register unsigned int o1 __asm__("r2");`.  Unpinned it
 *       is 6 differing with the pointer and the offset in each other's
 *       registers, and NINE plain spellings were measured (one statement, two
 *       statements, `unsigned char *` base, `*(int *)(base + off) = off + 0x44`
 *       and the rest): every one is 6 or worse.  With the pin it is EXACT.
 *   site 3 (gState + 0xe1<<1, ldrsh): FIVE DIFFERING AND NOT REACHABLE.
 *
 * THE PARK: SITE 3 IS A REGISTER TRANSPOSITION THE SOURCE CANNOT NAME.
 *
 *     ROM   ldr r3,[pc] / mov r1,#0xe1 / lsl r1,#1 / add r3,r1 /
 *           mov r2,#0   / ldrsh r3,[r3,r2]
 *     ours  ldr r3,[pc] / mov r2,#0xe1 / lsl r2,#1 / add r3,r3,r2 /
 *           mov r1,#0   / ldrsh r3,[r3,r1]
 *
 * The base and the result are the ROM's; the OFFSET and the ldrsh's ZERO INDEX
 * are swapped.  Site 1, four instructions of the same shape, is exact -- there
 * the result is live and takes r2, so the offset must take r1.  At site 3 the
 * result dies at the `cmp`, r2 is free, and gcc's REG_ALLOC_ORDER hands it to
 * the offset because the offset quantity is referenced more often.  The ROM
 * gave r2 to the zero.
 *
 * WHAT WAS MEASURED AGAINST IT, all 7 (i.e. 5 real) unless stated:
 *   - ALL 120 DECLARATION ORDERS of the five locals: every one identical.
 *     Declaration order is INERT on this function, in both directions.
 *   - eleven spellings of the site-3 block: `+ o` in one statement, `+=` in
 *     two, a fresh local for the offset, `*(short *)r3` without the pointer
 *     local, `v = *p` then `v == 1` (8), `p[v]` with `v = 0`, `p[r2]` with
 *     `r2 = 0`, the base and offset locals swapped, `o1 = 0xe1; o2 = o1 << 1;`
 *     as two pseudos.
 *   - a `register ... __asm__("r1")` pin on the offset: THE PIN IS DROPPED.
 *     gcc still emits `mov r2,#225 / lsl r2,#1`.  This is the recorded "a
 *     constant-valued pin is folded by cprop and TRULY INERT", confirmed on a
 *     value that is not a call argument.
 *   - the fakematch escape `register unsigned int o __asm__("r1") = 0x1c2;
 *     __asm__ volatile ("" : : "r" (o));` -- 46 differing and FOUR BYTES
 *     SHORT: the barrier hoists the constant out of the guarded block.  This
 *     is the recorded "THE BARRIER IS ONLY AVAILABLE WHERE THE ROM DOES NOT
 *     USE r8-r11" holding on a SIXTH function (this one's hi count is 5).
 *   - restructuring the guard: `&&` in one `if` (16), `goto` out of it, an
 *     ordering pin on the __GetFlag(0x843) argument, a `do { } while (0)`
 *     wall in front of the block.
 *   - four spellings of the site-2 pin (both registers, one register, either
 *     assignment order): all leave site 3 at exactly 5.
 *   - eight flag combinations on top of -fno-gcse: -fno-rerun-cse-after-loop,
 *     -fno-expensive-optimizations (which breaks 20083c8), -fno-cse-follow-
 *     jumps, -fno-thread-jumps, -fno-peephole2, -fno-cse-skip-blocks,
 *     -fno-delayed-branch: all 7.
 *   - `-fno-schedule-insns2` REGRESSES 2008694 (11 -> 34 on the instruction
 *     stream), so by the recorded sign rule sched2 is already right here and
 *     ALIAS IS THE WRONG AXIS.  It is allocation, and it is the recorded
 *     REG_ALLOC_ORDER class.
 *
 * BLOCKER CLASS: REG_ALLOC_ORDER.  DISCRIMINATOR: at a Thumb `ldrsh rD,[rB,rI]`
 * whose result dies immediately, gcc gives the lower-numbered hard register to
 * the ADDRESS OFFSET and the ROM gives it to the ZERO INDEX.  FLOOR: 5 of 142
 * encodings, SIZE and RELOCATION OFFSETS silent, plus the 2 expected _AREA
 * pool words.
 *
 * Harness: scratch_elev/b256/rich -- decl.h/body1.inc/body2.inc + g.sh (split),
 * f1.c/f2.c (one function each, since objcmp's --func filters only the
 * reference), objcmpf.py (objcmp with extra flags), cmp2.py (encoding count
 * with flags), dis.py (side-by-side objdump), sw*.py (the sweeps above).
 */
struct Sprite {
    unsigned char pad00[9];
    unsigned char b0 : 2,
                  b2 : 2,
                  b4 : 4;
};

struct Actor {
    unsigned char pad00[0x24];
    int f24;
    unsigned char pad28[0x2c - 0x28];
    int f2c;
    unsigned char pad30[0x50 - 0x30];
    struct Sprite *f50;
    unsigned char pad54[0x55 - 0x54];
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    short f64;
};

struct A5a {
    unsigned char pad00[0x5a];
    unsigned char f5a;
};

typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __PlaySound(int id);
extern unsigned int __Random(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, void *s);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Actor_SetScript(struct Actor *a, unsigned char *s);
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092a1c(int a, int b, unsigned char *c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __DeleteFieldActor(int slot);
extern void __LoadFieldActors(unsigned char *p);
extern void OvlFunc_911_200a910(void);
extern void OvlFunc_911_20088ec(void);
extern unsigned char gScript_911__0200ae20[];
extern unsigned char gScript_911__0200add8[];
extern unsigned char gScript_884__0200ae34[];
extern unsigned char L32d8[] __asm__(".L32d8");
extern int _AREA_26;
extern int _AREA_27;

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_911_20083c8(void)
{
    struct Actor *q;
    struct Sprite *s;
    unsigned int i;
    int y;
    int w;
    int v;

    __CutsceneStart();
    { PIN2; q0 = 0x80 << 10; q1 = 0x80 << 7;
      __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0xa8 << 16; q1 = 0; q2 = 0xf6 << 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x14);
    { PIN3; q0 = 8; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN2; q0 = 8; q1 = 2;
      __Func_80925cc(q0, q1); }
    __MessageID(0x1786);
    { PIN2; q0 = 8; q1 = 0;
      __ActorMessage(q0, q1); }
    { PIN2; q0 = 0x6666; q1 = 0xccc;
      __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0xa8 << 16; q1 = 0; q2 = 0xea << 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q0 = 0; q1 = 0x9999; q2 = 0x4ccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xae; q2 = 0x8b << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xe0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 0; q1 = 3;
      __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    { PIN2; q0 = 8; q1 = 3;
      __MapActor_SetAnim(q0, q1); }
    { PIN2; q0 = 8; q1 = 0;
      __ActorMessage(q0, q1); }
    { PIN3; q0 = 8; q1 = 0x90 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 8; q1 = 1;
      __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    ((struct A5a *)__MapActor_GetActor(8))->f5a &= 0xfe;
    { PIN3; q0 = 8; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 2; q2 = 0;
      __MapActor_Jump(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0xe0; q2 = 0xc5;
      __Func_8092158(q0, q1, q2); }
    __PlaySound(0xb0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 8; q1 = 0xea; q2 = 0xc8;
      __Func_8092158(q0, q1, q2); }
    __CutsceneWait(0xa);
    __PlaySound(0xc6);
    __CutsceneWait(0x1e);
    __CopyMapTiles(0x5b, 0, 0x48, 9, 5, 4);
    __CutsceneWait(0xc);
    __CopyMapTiles(0x5b, 4, 0x48, 9, 5, 4);
    __CutsceneWait(9);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CopyMapTiles(0x5b, 8, 0x48, 9, 5, 5);
    __CutsceneWait(6);
    __CopyMapTiles(0x5b, 0xd, 0x48, 9, 5, 6);
    __CutsceneWait(3);
    __PlaySound(0xbc);
    i = 0;
    w = 0x80 << 9;
    y = 0x94 << 16;
    for (; i <= 9; i++) {
        { PIN4; q0 = 0xde; q1 = y; q2 = 0; q3 = 0x81 << 17;
          q = __CreateActor(q0, q1, q2, q3); }
        if (q != 0) {
            q->f55 = 0;
            __Actor_SetSpriteFlags(q, 0);
            s = q->f50;
            s->b2 = 0;
            q->f64 = (__Random() * 40 >> 16) + 0x28;
            v = ((int)((i & 3) << 16) + w) >> 1;
            q->f2c = w;
            q->f24 = v;
            if ((i & 1) != 0)
                q->f24 = -v;
            __Actor_SetAnim(q, 1);
            __Actor_SetScript(q, gScript_911__0200ae20);
        }
        y += 0x80 << 11;
    }
    { register int s0 __asm__("r3"); register int s1 __asm__("r2");
      s0 = 5; s1 = 7;
      __CopyMapTiles(0x5b, 0x13, 0x48, 9, s0, s1); }
    { register int s0 __asm__("r3"); register int s1 __asm__("r2");
      s0 = 8; s1 = 0xb;
      __Func_8010704(0x17, 0xb, 5, 7, s0, s1); }
    { PIN3; q0 = 0; q1 = 0x80 << 11; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 6; q2 = 0;
      __MapActor_Jump(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    { PIN3; q2 = (int)gScript_884__0200ae34; q0 = 8; q1 = 0x80 << 9;
      __Func_8092a1c(q0, q1, (unsigned char *)q2); }
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __SetFlag(0x847);
    __CutsceneEnd();
}
