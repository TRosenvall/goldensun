/* OvlFunc_962_2008240  --  0x02008240
 *   [asm/overlays/rom_7ec19c/ovl_30_c_c_a.s, 1st of 1 -- NO SPLIT NEEDED]
 *
 * 764 instructions of straight-line cutscene script guarded by one save-bit
 * test, with a single if/else near the end.  Built at the tree default -O2: no
 * Makefile pattern rule matches rom_7ec19c/ovl_30_c_c_a, so `asm/%.o: src/%.c`
 * applies and a scratch-path screen sees the same flags as the real object
 * (objcmp prints no `(built with: ...)` line from either path).
 *
 * A PIN FUNCTION, AND THE PROLOGUE SAYS SO.  `push {lr}` alone: the ROM spends
 * no callee-saved register anywhere, so every repeated constant is rebuilt at
 * every use.  Plain C is 770 lines against the ROM's 770 -- the LENGTH MATCHES
 * -- and still 710 of 770 differ, because cse_main commons the seven repeated
 * multi-instruction constants into pseudos that straddle `bl` and gcc reaches
 * into r8-r11, spilling them through `push {r5, r6, r7}` at entry.  The equal
 * length is a coincidence (the widened prologue costs exactly what the removed
 * rematerialisations save), so the documented "length difference first" tell for
 * this class does NOT fire here; the r8-r11 traffic in the diff is what does.
 *
 * THE SEVEN HELD VALUES, read off plain C's `mov rN, r8..r11` copies:
 * 0xc0 << 8 (7 sites + one as __Func_809233c's r3), 0x81 << 1 (7),
 * 0x80 << 1 (6), 0x80 << 6 (6), 0x13333 and 0x9999 (3 __MapActor_SetSpeed
 * sites), 0xb8 << 1 (2).  Every 8-bit `mov` constant in the function is left
 * alone by CSE and needs nothing.
 *
 * THE UNIFORM ASCENDING FILL IS ENOUGH AT 34 OF 35 SITES.  One statement per
 * argument, ascending q0..q3, whole value per statement, at every site using one
 * of those seven values: 710 differing -> 8.  sched2 reproduces all of the ROM's
 * transposed emitted orders (`mov r1 / mov r0 / lsl r1 / mov r2`,
 * `mov r1 / mov r2 / lsl r1 / mov r0`, `mov r1 / lsl r1 / mov r2 / mov r0`) from
 * that one spelling -- the shift lands itself.
 *
 * THE RESIDUE WAS HOMOGENEOUS AND IT WAS THE UNPINNED SITES, NOT THE FILLS.
 * The remaining 8 lines were three sites the pattern had missed: two
 * __Func_8092304 calls whose `neg` was hoisted ahead of the other two movs, and
 * __MapActor_Emote(0x13, 0x107, 0x28) with its pool load one slot early.  All
 * three are unique-constant sites, so they are not CSE victims -- they are
 * argument ORDERING, which is the second job a pin does.  Pinning them uniform
 * ascending is exact; transcribing the ROM's order into the fill
 * (`q2 = 0x10; q0 = 0x13; q1 = 0; q2 = -q2;`) is byte-identical, so the
 * transcription was dropped and the uniform form kept.
 *
 * __Func_8092c40 WANTS THE DESCENDING FILL, the fifth function to show it.
 * `q1 = 0; q0 = 0x13;` matches; `q0 = 0x13; q1 = 0;` costs 2 encodings and is
 * byte-identical to leaving the site unpinned, which is the sharpest form of the
 * tell yet -- the ascending fill is not a weaker pin, it is no pin at all here.
 *
 * THIRTY-FIVE PINS, NOT ONE REMOVABLE.  Each of the 35 was stripped
 * individually under objcmp and every one fails (2 to 433 encodings, or a size
 * change of 4-8 bytes).  An earlier sweep, taken in the transcribed-fill shape,
 * found the LAST __Func_8092adc(0x13, 0xc0 << 8, 0) and the LAST
 * __Func_8092adc(0x13, 0x80 << 6, 0) inert, greedy passes from both ends of the
 * list agreed on those two, and both stay dropped here -- consistent with "the
 * ones that fall are the last use of their value, never the first".  The sweep
 * was then repeated in the uniform shape, where the surviving 35 are all
 * load-bearing.
 *
 * ALL SIX POOLED VALUES ARE BARE LITERALS.  0x98a, 0x25eb, 0x13333, 0x9999,
 * 0x107 and 0x101 need no symbol: none is a shifted byte gcc could build with
 * mov+lsl, so each pools unaided, and objcmp reports 253 relocations identical
 * -- the function's only non-`bl` relocation is R_ARM_ABS32 iwram_3001ebc.
 * Nothing belongs in const.sym or message.sym for this function.
 *
 * The `iwram_3001ebc` increment appears in BOTH arms of the if/else, in the
 * ROM's two different positions relative to __ActorMessage; written that way
 * plain C emits the pointer-then-0xec<<1 build with no help.
 *
 * The far `beq / b` pair at the top is gcc's own long-branch expansion of
 * `if (__GetFlag(0x98a) == 0)`; the guarded body is 1500 bytes, past Thumb's
 * conditional range.
 */
extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern void __PlaySound(int id);
extern void __PlayMapMusic(void);
extern int __Func_8091c7c(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_809233c(int a, int b, int c, int d);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_962_2008240(void)
{
    unsigned char *p;

    __SetFlag(0x9b << 4);
    if (__GetFlag(0x98a) == 0) {
        __PlaySound(0x1e);
        __CutsceneStart();
        __Func_80933f8(0xb8 << 17, -1, 0xd0 << 15, 1);
        { PIN3; q0 = 0; q1 = 0xb8 << 1; q2 = 0xa0;
          __Func_80921c4(q0, q1, q2); }
        { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN4; q0 = 0x13; q1 = 0; q2 = -0x10; q3 = 0xc0 << 8;
          __Func_809233c(q0, q1, q2, q3); }
        __MapActor_WaitMovement(0x13);
        __Func_8093530();
        __MessageID(0x25eb);
        __CutsceneWait(0xa);
        __Func_80925cc(0x14, 2);
        __CutsceneWait(0x14);
        __ActorMessage(0x14, 0);
        __CutsceneWait(0xa);
        __Func_80925cc(0x13, 2);
        __CutsceneWait(0x14);
        { PIN3; q0 = 0x13; q1 = 0x13333; q2 = 0x9999;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 0x13; q1 = 0; q2 = -0x10;
          __Func_8092304(q0, q1, q2); }
        __Func_8092adc(0x13, 0, 0);
        __CutsceneWait(0x1e);
        __Func_8092adc(0x13, 0xe0 << 8, 0);
        __CutsceneWait(0x1e);
        __Func_8092adc(0x13, 0, 0);
        __CutsceneWait(0x1e);
        { PIN3; q0 = 0x13; q1 = 0x80 << 1; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        __ActorMessage(0x13, 0);
        __CutsceneWait(0xa);
        __Func_80925cc(0x15, 2);
        __CutsceneWait(0x14);
        __ActorMessage(0x15, 0);
        __CutsceneWait(0xa);
        { PIN3; q0 = 0x13; q1 = 0x13333; q2 = 0x9999;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 0x13; q1 = 0; q2 = -0x18;
          __Func_8092304(q0, q1, q2); }
        __Func_8092304(0x13, 0x30, 0);
        { PIN3; q0 = 0x13; q1 = 0xc0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x1e);
        __ActorMessage(0x13, 0);
        __CutsceneWait(0x14);
        { PIN3; q0 = 0x13; q1 = 0x80 << 1; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        __ActorMessage(0x13, 0);
        __CutsceneWait(0x14);
        { PIN3; q0 = 0x14; q1 = 0x81 << 1; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        __ActorMessage(0x14, 0);
        __CutsceneWait(0xa);
        { PIN3; q0 = 0x13; q1 = 0x80 << 6; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x14);
        __ActorMessage(0x13, 0);
        __CutsceneWait(0xa);
        __Func_80925cc(0x15, 2);
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0x15, 4);
        __CutsceneWait(0x14);
        __ActorMessage(0x15, 0);
        __CutsceneWait(0xa);
        { PIN3; q0 = 0x13; q1 = 0xc0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x1e);
        { PIN3; q0 = 0x13; q1 = 0x107; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        __ActorMessage(0x13, 0);
        __CutsceneWait(0xa);
        __Func_809259c(0x14, 2);
        __Func_80925cc(0x15, 2);
        __CutsceneWait(0x14);
        { PIN3; q0 = 0; q1 = 0x13333; q2 = 0x9999;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 0; q1 = 0xb8 << 1; q2 = 0x68;
          __Func_80921c4(q0, q1, q2); }
        __Func_8092304(0, 0x10, 0);
        __Func_8092adc(0, 0, 0);
        __CutsceneWait(0x14);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0x13, 4);
        __CutsceneWait(0x14);
        __ActorMessage(0x13, 0);
        __CutsceneWait(0x14);
        { PIN3; q0 = 0x13; q1 = 0x81 << 1; q2 = 0x32;
          __MapActor_Emote(q0, q1, q2); }
        __ActorMessage(0x13, 0);
        __CutsceneWait(0xa);
        __Func_80925cc(0x14, 2);
        __CutsceneWait(0x14);
        __ActorMessage(0x14, 0);
        __CutsceneWait(0xa);
        __Func_809280c(0, 0x14, 0);
        { PIN3; q0 = 0x13; q1 = 0x80 << 6; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x14);
        __ActorMessage(0x13, 0);
        __CutsceneWait(0xa);
        __Func_80925cc(0x15, 2);
        __CutsceneWait(0x14);
        __ActorMessage(0x15, 0);
        __CutsceneWait(0xa);
        __Func_809280c(0, 0x15, 0);
        { PIN3; q0 = 0x13; q1 = 0xc0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x28);
        __MapActor_DoAnim(0x13, 3);
        __CutsceneWait(0x1e);
        __ActorMessage(0x13, 0);
        __CutsceneWait(0xa);
        __Func_80925cc(0x15, 2);
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0x15, 4);
        __CutsceneWait(0x14);
        __ActorMessage(0x15, 0);
        __CutsceneWait(0xa);
        __Func_8092848(0x13, 0, 0x1e);
        __ActorMessage(0x13, 0);
        __CutsceneWait(0xa);
        __Func_80925cc(0x14, 2);
        __CutsceneWait(0x14);
        __ActorMessage(0x14, 0);
        __CutsceneWait(0xa);
        __Func_809280c(0, 0x14, 0);
        { PIN3; q0 = 0x13; q1 = 0x80 << 6; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x28);
        __CutsceneWait(0xa);
        __Func_80925cc(0x15, 2);
        __CutsceneWait(0x14);
        __ActorMessage(0x15, 0);
        __CutsceneWait(0xa);
        __Func_809280c(0, 0x15, 0);
        { PIN3; q0 = 0x13; q1 = 0xc0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x28);
        __MapActor_SetAnim(0, 3);
        __MapActor_DoAnim(0x13, 3);
        __CutsceneWait(0x1e);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0x14, 4);
        __CutsceneWait(0x14);
        __ActorMessage(0x14, 0);
        __CutsceneWait(0xa);
        { PIN3; q0 = 0; q1 = 0x80 << 1; q2 = 0;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 0x13; q1 = 0x80 << 1; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        __CutsceneWait(0xa);
        __Func_809280c(0, 0x14, 0);
        { PIN3; q0 = 0x13; q1 = 0x80 << 6; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x28);
        __CutsceneWait(0xa);
        __Func_80925cc(0x15, 2);
        __CutsceneWait(0x14);
        __ActorMessage(0x15, 0);
        __CutsceneWait(0xa);
        { PIN3; q0 = 0; q1 = 0x80 << 1; q2 = 0x32;
          __MapActor_Emote(q0, q1, q2); }
        __Func_809280c(0, 0x15, 0);
        __CutsceneWait(0x1e);
        __Func_80925cc(0x15, 2);
        __CutsceneWait(0x1e);
        __CutsceneWait(0xa);
        { PIN3; q0 = 0x13; q1 = 0xc0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x1e);
        __ActorMessage(0x13, 0);
        __CutsceneWait(0xa);
        __Func_80925cc(0x15, 2);
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0x15, 3);
        __CutsceneWait(0x1e);
        __CutsceneWait(0xa);
        { PIN3; q0 = 0x13; q1 = 0x81 << 1; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        __ActorMessage(0x13, 0);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0x14, 4);
        __CutsceneWait(0x14);
        __ActorMessage(0x14, 0);
        __CutsceneWait(0xa);
        __MapActor_Emote(0x13, 0x101, 0x32);
        __CutsceneWait(0xa);
        __Func_809280c(0, 0x14, 0);
        { PIN3; q0 = 0x13; q1 = 0x80 << 6; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x14);
        __CutsceneWait(0xa);
        __Func_80925cc(0x14, 2);
        __CutsceneWait(0x14);
        __ActorMessage(0x14, 0);
        __CutsceneWait(0xa);
        { PIN3; q0 = 0x13; q1 = 0x80 << 1; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        __ActorMessage(0x13, 0);
        __CutsceneWait(0xa);
        __Func_80925cc(0x15, 2);
        __CutsceneWait(0x14);
        __ActorMessage(0x15, 0);
        __Func_80925cc(0x15, 2);
        __CutsceneWait(0x14);
        __CutsceneWait(0xa);
        __Func_8092848(0x13, 0, 0);
        __CutsceneWait(0x1e);
        __ActorMessage(0x13, 0);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0x14, 4);
        __CutsceneWait(0x14);
        __ActorMessage(0x14, 0);
        __CutsceneWait(0xa);
        { PIN3; q0 = 0; q1 = 0x81 << 1; q2 = 0;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 0x13; q1 = 0x81 << 1; q2 = 0x50;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 0x15; q1 = 0x81 << 1; q2 = 0x32;
          __MapActor_Emote(q0, q1, q2); }
        __ActorMessage(0x15, 0);
        __CutsceneWait(0xa);
        __Func_80925cc(0x15, 2);
        __CutsceneWait(0x14);
        __ActorMessage(0x15, 0);
        __Func_80925cc(0x15, 3);
        __CutsceneWait(0x14);
        __CutsceneWait(0xa);
        __Func_8092adc(0x13, 0xc0 << 8, 0);
        __CutsceneWait(0x1e);
        __Func_80925cc(0x13, 2);
        __CutsceneWait(0xa);
        __ActorMessage(0x13, 0);
        __CutsceneWait(0xa);
        __Func_80925cc(0x14, 2);
        __CutsceneWait(0x14);
        __ActorMessage(0x14, 0);
        __CutsceneWait(0xa);
        __Func_809280c(0, 0x14, 0);
        __Func_8092adc(0x13, 0x80 << 6, 0);
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(0x13, 3);
        __CutsceneWait(0x1e);
        __ActorMessage(0x13, 0);
        __CutsceneWait(0xa);
        __Func_809259c(0x14, 2);
        __Func_80925cc(0x15, 2);
        __CutsceneWait(0x1e);
        __CutsceneWait(0xa);
        __Func_8092848(0x13, 0, 0x14);
        __Func_8092304(0x13, -0xc, 0);
        __CutsceneWait(0x14);
        { PIN2; q1 = 0; q0 = 0x13;
          __Func_8092c40(q0, q1); }
        if (__Func_8091c7c(0, 0) == 0) {
            __CutsceneWait(0x14);
            __ActorMessage(0x13, 0);
            *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        } else {
            __CutsceneWait(0xa);
            *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
            __ActorMessage(0x13, 0);
        }
        __CutsceneWait(0xa);
        __Func_80925cc(0x13, 2);
        __CutsceneWait(0x14);
        __ActorMessage(0x13, 0);
        __CutsceneWait(0xa);
        { PIN3; q0 = 0x13; q1 = 0x81 << 1; q2 = 0x32;
          __MapActor_Emote(q0, q1, q2); }
        __ActorMessage(0x13, 0);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0, 3);
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0x13, 3);
        __CutsceneWait(0x1e);
        __PlaySound(0x1e);
        { PIN3; q0 = 0x13; q1 = 0x13333; q2 = 0x9999;
          __MapActor_SetSpeed(q0, q1, q2); }
        __MapActor_SetAnim(0x13, 2);
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_TravelTo(0x13, *(short *)(p + 0xa), *(short *)(p + 0x12));
        __MapActor_WaitMovement(0x13);
        __MapActor_SetPos(0x13, 0, 0);
        __CutsceneWait(0xa);
        __PlayMapMusic();
        __CutsceneEnd();
    }
}
