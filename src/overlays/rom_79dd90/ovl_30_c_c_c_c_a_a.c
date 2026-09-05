// fakematch
/* OvlFunc_910_20081e4  --  0x020081e4
 *   [asm/overlays/rom_79dd90/ovl_30_c_c_c_c_a_a.s, the whole file]
 *
 * 233 instructions of straight-line cutscene: a three-deep flag ladder at the
 * top, one guarded actor fetch, two bitfield writes and a four-way join.
 * Built at the tree default -O2 -- no Makefile pattern rule has a prefix that
 * captures rom_79dd90, so `asm/%.o: src/%.c` applies and a scratch-path screen
 * sees the same flags as the real object.
 *
 * EVERY LITERAL HERE IS A LITERAL.  The reference object carries exactly TWO
 * R_ARM_ABS32 relocations, `iwram_3001ebc` at +0x25c and `gScript_910__02008bf4`
 * at +0x274.  The four pooled message ids (0x140d, 0x1413, 0x1414, 0x1416) and
 * the two pooled flag ids (0x201, 0x84a) carry NO relocation, so none of them
 * is a message.sym / const.sym symbol however much the pool tell suggests it.
 *
 * THE PROLOGUE IS `push {lr}` ALONE, so the ROM spends no callee-saved register
 * and every repeated constant is rebuilt at every use.  Plain C is 244 lines
 * against 241 with `push {r5, r6, r7, lr}`: gcse commons the pool loads 0x84a
 * (r7), 0x201 (r5) and 0x107 (r6) and later 0xa4<<1 (r5) into pseudos whose
 * ranges straddle a bl.  It reads as a LENGTH difference first.  Named locals
 * are the wrong cure here -- they give the values somewhere to live instead of
 * taking it away -- so ONLY PINS WORK, and pinning the FIRST use of each
 * commoned value is what kills the commoning: r0-r2 are call-clobbered, so a
 * pinned pseudo cannot span the call gcse wanted to hoist it over.
 *
 * TWENTY-FOUR PINNED SITES, MINIMAL BY MEASUREMENT.  Every one was stripped
 * individually and every one is load-bearing; not one was inert, so there is no
 * joint-removal question to re-verify.  Freeing either flag pin is 243 lines
 * and 233-243 differing (the whole gcse cascade returns); freeing the `&=`
 * site's pair is 243 and 71; the rest are 2-3 differing each.
 *
 * WITHIN A PINNED SITE THE STATEMENTS ARE IN THE ROM'S EMITTED ORDER,
 * INCLUDING THE SHIFT -- `q1 = 0xc0; q0 = 0xb; q1 <<= 6; q2 = 0;` where the ROM
 * puts `mov r0` between the `mov r1` and its `lsl`.
 *
 * THE LAST THREE INSTRUCTIONS NEEDED A SCHEDULING BARRIER, NOT A PIN.
 * __MapActor_SetPos(0xb, 0, 0) is `mov r1 / mov r2 / mov r0` in the ROM and
 * pins alone give the natural r0/r1/r2 -- the two zeros are one quantity to
 * sched2 and it re-lands `mov r0` first.  `do { } while (0)` after the fills
 * ends the scheduling region at zero instruction cost and is exact.  A
 * `__asm__ volatile ("" : : "r" ...)` barrier is also exact; the loop is
 * preferred because it emits nothing.  Six other spellings tie at 2-3
 * differing: swapping q2/q1, `q2 = q1`, dropping q0's pin, redeclaring the
 * pins in ROM order, plain `int` locals, and the bare call.
 *
 * THE TWO BITFIELD WRITES WANT DIFFERENT SPELLINGS, four instructions apart --
 * the batch-85/86 rule that this is a spelling to try and not a rule to apply.
 * `p[0x5a] &= 0xfe` already makes the CONSTANT the `and` destination; plain
 * `p[0x5a] |= 1` makes the VALUE the `orr` destination and the ROM wants the
 * constant, so that one takes the narrow local (`unsigned char one = 1;
 * p[0x5a] = one | p[0x5a];`).  DECLARE THE POINTER FIRST: with `one` declared
 * ahead of the __MapActor_GetActor call its range spans the call and gcc gives
 * it r5, which reinstates `push {r5, lr}` -- 242 lines and 11 differing.
 *
 * BLOCK LAYOUT IS SOURCE ORDER, AND THE `goto` FORM LOSES IT.  Writing the flag
 * ladder as `if (...) goto L24c;` lets jump optimisation invert the second test
 * and pull the 0x1413 block up as the fallthrough, putting three blocks in the
 * wrong order.  Nested `if`/`else` with the then-arm carrying the rest of the
 * ladder expands in the ROM's order and needs no `goto` until the four-way join
 * at the end.
 *
 * LANDING NEEDS NO SPLIT: the .s holds this function alone and
 * overlays/rom_79dd90/overlay.ld:28 already names the single .o.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char gScript_910__02008bf4[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092a1c(int a, int b, unsigned char *s);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern int __Func_8091c7c(int a, int b);
extern void OvlFunc_910_20088e8(void);


#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_910_20081e4(void)
{
    __CutsceneStart();
    { PIN1; q0 = 0x84a;
    if (__GetFlag(q0) != 0) {
        if (__GetFlag(0xc1 << 2) != 0) {
            { PIN1; q0 = 0x201;
            if (__GetFlag(q0) == 0) {
                __MessageID(0x1414);
                __Func_8093040(0xc, 0, 0xa);
                { PIN3; q0 = 0xc; q1 = 0x107; q2 = 0x28;
                  __MapActor_Emote(q0, q1, q2); }
                __Func_8093040(0xc, 0, 0xa);
                __Func_80925cc(0xc, 2);
                __SetFlag(0x201);
            } }
            __MessageID(0x1416);
            __ActorMessage(0xc, 0);
            goto L41c;
        }
        __MessageID(0x1413);
        __ActorMessage(0xc, 0);
        goto L3fa;
    } }
    __MessageID(0x140d);
    { PIN2; q1 = 0; q0 = 0xc;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) != 0)
        goto L408;
    *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    __Func_8093040(0xc, 0, 0xa);
    if (*(int *)(__MapActor_GetActor(0) + 0x10) <= 0x10dffff) {
        { PIN3; q0 = 0xc; q1 = 0xcccc; q2 = 0x6666;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q1 = 0xad; q2 = 0x89; q0 = 0; q1 <<= 1; q2 <<= 1;
          __Func_80921c4(q0, q1, q2); }
        { PIN3; q1 = 0xa4; q2 = 0x8d; q0 = 0; q1 <<= 1; q2 <<= 1;
          __Func_80921c4(q0, q1, q2); }
        { PIN3; q1 = 0xc0; q0 = 0; q1 <<= 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
    }
    { PIN3; q1 = 0x80; q0 = 0xb; q1 <<= 5; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xe0; q0 = 0xc; q1 <<= 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x81; q2 = 0x14; q0 = 0xb; q1 <<= 1;
      __MapActor_Emote(q0, q1, q2); }
    __Func_809259c(0xb, 1);
    __Func_8093040(0xb, 0, 0xa);
    { PIN3; q1 = 0x84; q2 = 0x3c; q0 = 0xc; q1 <<= 1;
      __MapActor_Emote(q0, q1, q2); }
    __Func_809259c(0xc, 1);
    __Func_8093040(0xc, 0, 0x14);
    __MapActor_SetAnim(0xb, 3);
    __MapActor_DoAnim(0xc, 3);
    { PIN3; q1 = 0xc0; q0 = 0xb; q1 <<= 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q2 = 0xa; q0 = 0xc; q1 <<= 7;
      __Func_8092adc(q0, q1, q2); }
    __Func_809259c(0xb, 1);
    __Func_8093040(0xb, 0, 0x14);
    { PIN3; q1 = 0xf0; q0 = 0xb; q1 <<= 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetSpeed(0xc, 0x80 << 9, 0x80 << 8);
    { register int q1 __asm__("r1"); register int q2 __asm__("r2");
      __MapActor_GetActor(0xc)[0x5a] &= 0xfe;
      q1 = 0xad; q1 <<= 1; q2 = 0x107;
      __Func_80921c4(0xc, q1, q2); }
    __CutsceneWait(1);
    { unsigned char *p = __MapActor_GetActor(0xc);
      unsigned char one = 1;
      p[0x5a] = one | p[0x5a]; }
    { PIN3; q1 = 0x9999; q0 = 0xb; q2 = 0x4ccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xa4; q0 = 0xb; q1 <<= 1; q2 = 0x107;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xa4; q0 = 0xb; q1 <<= 1; q2 = 0xfc;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q0 = 0xb; q1 <<= 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    OvlFunc_910_20088e8();
    { PIN3; q1 = 0xa4; q0 = 0xb; q1 <<= 1; q2 = 0xf6;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0; q2 = 0; q0 = 0xb;
      do { } while (0);
      __MapActor_SetPos(q0, q1, q2); }
    __SetFlag(0x84a);
L3fa:
    { PIN2; register unsigned char *q2 __asm__("r2");
      q1 = 0x80; q2 = gScript_910__02008bf4; q0 = 0xc; q1 <<= 9;
      __Func_8092a1c(q0, q1, q2); }
    goto L41c;
L408:
    __ActorMessage(0xc, 0);
    { PIN3; q1 = 0xc0; q0 = 0xc; q1 <<= 6; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
L41c:
    __CutsceneEnd();
}
