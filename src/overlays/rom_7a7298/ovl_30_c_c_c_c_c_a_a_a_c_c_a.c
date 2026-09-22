/* WHOLE-FILE CONVERSION of asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_a_a_a_c_c_a.s
 * -- both of its functions, no split and no linker change.
 *
 *   OvlFunc_921_2008f90  548 insns  1468 bytes  570 encodings / 151 relocations
 *   OvlFunc_921_2008b70  405 insns  1056 bytes  427 encodings /  87 relocations
 *
 * 2008b70 needs _AREA_32, WHICH THIS COMMIT ADDS to area.sym, and with it the whole
 * file is exact.  The in-function control is the strongest kind that table can have:
 * the function opens `ldr r3, =0x32 / cmp r2, r3` and THREE INSTRUCTIONS LATER does
 * `ldr r3, =0x33 / cmp r2, r3`, and _AREA_33 has been in the table since batch 68.
 *
 * ================================================================
 * SPLIT THE PINNED mov/lsl PAIR IN THE SOURCE -- the lever of the batch
 * ================================================================
 *
 *     { PIN3; q1 = 0x80; q0 = 3; q1 <<= 7; q2 = 0; f(q0, q1, q2); }
 *
 * This defeats the repeated-constant CSE *and* gives sched2 the ROM's interleave,
 * because the source order now puts INSN_LUID(mov r0) BETWEEN the mov and the lsl.
 *
 * BATCH 280 CURED THE SAME ADJACENCY BY *UN*-PINNING q1 -- and that lets the CSE
 * straight back in when the constant repeats.  Splitting the pair in the source gets
 * both effects at once.  14 sites in 2008f90 (40 -> 18 -> 0), 5 in 2008b70.  NO FLAG
 * REACHES IT: -fno-rerun-cse-after-loop, -fno-gcse, -fno-expensive-optimizations and
 * -fno-cse-follow-jumps are all inert at 87.
 *
 * A NARROWING COMPARISON CAN BE WORTH 49 OF 55, AND EVERY SYMPTOM IT DROPS LOOKS
 * LIKE AN ALLOCATION BUG.  `(short)dir == -1`, `(short)dir == (short)-1` and a
 * separate `short dh` all plateau at 55; `if (dir << 16 == (int)0xffff0000)` goes to
 * 6 AND TAKES ELEVEN r2/r3 PERMUTATIONS WITH IT.  Another instance of batch 280's
 * rule that register permutations at this size are symptoms.
 *
 * A LOOP-INVARIANT CONSTANT ADDEND IS NEVER A `movable`.  .08.loop shows it as
 * (plus (reg) (const_int 2048)) -- an immediate operand -- so scan_loop builds
 * nothing, move_movables' threshold test is never reached, and reload rematerialises
 * it per iteration.  A BLOCK-SCOPED `register int d7 __asm__("r7")` forces the ROM's
 * hoist (101 -> 90 over three loops).  A FUNCTION-SCOPED pin on the same register is
 * much worse (144), because the ROM reuses r7 for four roles.
 *
 * `register short`, NOT `register int`, IS WHAT POOLS A HALFWORD ADDEND -- the other
 * side of batch 280's *thumb_movhi_insn lever (90 -> 74).
 *
 * A PIN BLOCK MUST WRAP THE SITE, NOT THE REGION.  Enclosing a function body in
 * { PIN1; ... } keeps r0 live across the call and forces `mov r5, r0 / cmp r5` where
 * the ROM has `cmp r0, #0`.
 *
 * .18.greg NAMED THE r5/r6 INVERSION IN ONE STEP: halfword constants written BEFORE
 * their GetActor call are live across it, so local-alloc hands them r5 before global
 * alloc runs, and the gState base can then never have it.
 *
 * No per-file Makefile flag override applies to this stem.
 */
/* Whole-file conversion of asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_a_a_a_c_c_a.s
 * (2 functions, 405 + 548 instructions, 1056 + 1468 bytes).  NO SPLIT AND NO
 * LINKER CHANGE: both functions of the file land, so the .o keeps its name and
 * its slot.
 *
 *   OvlFunc_921_2008b70  OK -- 1056 bytes, 427 encodings, 87 relocations
 *   OvlFunc_921_2008f90  OK -- 1468 bytes, 570 encodings, 151 relocations
 *
 * both measured with tools/objcmp.py against the reference with its two pooled
 * area ids spelled as symbols.
 *
 * SYMBOL TELL -- ONE NEW .sym LINE IS NEEDED AND IS *NOT* ADDED HERE:
 *
 *     _AREA_32 = 0x32;          (area.sym)
 *
 * 2008b70 opens `ldr r3, =0x32 / cmp r2, r3` against the area halfword at
 * gState+0x1C0, immediately followed by `ldr r3, =0x33 / cmp r2, r3` -- and
 * _AREA_33 has been in area.sym since batch 68.  That is area.sym's own
 * criterion with its own in-function control sitting three instructions away:
 * the SAME comparison, against the NEXT id, already provisioned.  The
 * (per _MSG_1299's note); replace it with the area.sym line when the entry is
 * approved.  With the shim the function is byte-exact, so this symbol COMPLETES
 * its function -- the condition this tree adds a symbol on.
 *
 * THE LEVER OF THE FILE -- SPLIT THE PINNED mov/lsl PAIR IN THE SOURCE.  This
 * is new, it is general, and it took OvlFunc_921_2008f90 from 40 normalised
 * differences to 18 and then to 0.  A repeated shifted constant
 * (0x80 << 7, 0x80 << 8, 0x80 << 11 ...) is CSE'd into the one callee-saved
 * register the ROM needs for its script pointers; the cure is an argument pin,
 * but a pinned `q1 = 0x80 << 7` emits `mov r1,#0x80 / lsl r1,#7` ADJACENT while
 * the ROM slots `mov r0,#N` BETWEEN them.  Batch 280 cured that by UN-pinning
 * q1, which here lets the CSE straight back in.  Writing the pin as two
 * statements
 *
 *     { PIN3; q1 = 0x80; q0 = 3; q1 <<= 7; q2 = 0; __Func_8092adc(q0,q1,q2); }
 *
 * defeats the CSE *and* gives sched2 the ROM's order, because the source order
 * now puts INSN_LUID(mov r0) below INSN_LUID(mov r1) and above the lsl -- which
 * is exactly how the sched2 tie is broken.  Fourteen sites in 2008f90, five in
 * 2008b70.  NO FLAG REACHES THIS: -fno-rerun-cse-after-loop, -fno-gcse,
 * -fno-expensive-optimizations and -fno-cse-follow-jumps are all INERT here
 * (87 differing, unchanged, on the 2008f90 ladder).
 *
 * THE HALFWORD-STORE LEVER RUNS BOTH WAYS, and both directions are used in
 * this file.  `*(short *)(p + 0x64) = 0;` pools the zero (batch 280's
 * *thumb_movhi_insn alt 1); the ROM wants `mov r3,#0`, so the value is pinned
 * to r3 -- `{ register int q3 __asm__("r3"); q3 = 0; *(short *)(a+0x64) = q3; }`
 * -- and an `int` local is NOT enough: a shared `int z = 0` earns a
 * callee-saved register and grows the push (2008b70 needed one pseudo per
 * store site, h1..h5, for the same reason).
 *
 * A VALUE LIVE ACROSS ITS OWN GetActor CALL IS WHAT PUSHES EVERYTHING ELSE
 * AROUND.  In 2008b70 the halfword constants were written before their
 * __MapActor_GetActor call, so local-alloc had to give them r5 BEFORE global
 * alloc ran; the gState base then could not have r5 and the whole r5/r6 pair
 * inverted.  Reading `.18.greg` (`;; N conflicts: ... 5 ...` against a pseudo
 * lreg had already placed) named the cause in one step.  Computing each
 * constant AFTER the call fixed the class; the remaining inversion took two
 * `register` pins on the roles the ROM gives r5 and r6.
 *
 * ONE LOCAL SERVING SEVERAL ROLES, twice.  2008f90's r5 is three different
 * script pointers and two masks in the ROM; one `unsigned char *s` with the
 * masks cast through it is what keeps the push at `{r5, lr}`.  And the last
 * two differences of the function were an `orr`/`strb` pair whose destination
 * is the MASK's register because that is its last use -- reachable only by
 * accumulating into the mask (`m5 |= *a; *a = m5;`) with the mask pinned to r5.
 *
 * TWO SMALLER ONES worth keeping:
 *   - The gState halfword read is spelled with the established
 *     base/off/base+=off/off=0 idiom (src/overlays/rom_7b4558/ovl_30_a_c_c_c.c).
 *     Anything shorter folds `gState+448` into one pool word and the ROM
 *     builds the offset in a register.
 *   - Cross-jumping is gcc's, not ours: 2008b70's two __CopyMapTiles tails
 *     share a block in the ROM only because the shared `3` lives in the
 *     SELECTOR's register.  Assigning `sel = 3` in the sel==4 arm and passing
 *     `sel, sel` makes both tails identical and gcc merges them.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char iwram_3001ebc[];
extern unsigned char gScript_921__0200a4f4[];
extern int _AREA_32;
extern int _AREA_33;
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __WaitFrames(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int n);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __StartTask(void (*fn)(void), int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);
extern void __Func_8092950(int a, int b);
extern void __Func_8092b08(int a, int b);
extern void OvlFunc_921_2008f90(void);
extern void OvlFunc_921_2009794(void);
extern void OvlFunc_921_20098c4(void);
extern void OvlFunc_921_2009960(void);
extern void OvlFunc_921_20099bc(void);
extern void OvlFunc_921_20099e8(void);
extern unsigned char gScript_921__0200a5ec[];
extern unsigned char gScript_921__0200a670[];
extern unsigned char gScript_921__0200a6e0[];
extern unsigned char gScript_921__0200a74c[];
extern unsigned char gScript_921__0200a760[];
extern unsigned char L31c0[] __asm__(".L31c0");
extern unsigned char L31d6[] __asm__(".L31d6");
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapTransitionIn(void);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_DoAnim(int slot, int n);
extern void __MapActor_Surprise(int slot, int n);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_WaitScript(int slot);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __Func_8010560(unsigned char *p, int a, int b);
extern void __Func_8091200(int a, int b);
extern void __Func_8091220(int a, int b);
extern void __Func_8091254(int a);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092a1c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8093530(void);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void OvlFunc_921_20096c8(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")
int OvlFunc_921_2008b70(void)
{
    unsigned char *a;
    unsigned char *b1;
    unsigned char *b2;
    unsigned char *b3;
    unsigned char *b4;
    unsigned char *b5;
    register unsigned int base __asm__("r6");
    unsigned int off;
    unsigned int p;
    int area;
    register int sel __asm__("r5");
    int v1;
    int v2;
    int h1;
    int h2;
    int h3;
    int h4;
    int h5;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    p = base + off;
    off = 0;
    area = *(short *)((char *)p + off);
    if (area == (int)(&_AREA_32)) {
        a = __MapActor_GetActor(0);
        *(int *)(*(unsigned char **)iwram_3001ebc + (0xe0 << 1)) = 0x80 << 1;
        __MapActor_SetAnim(0xa, 9);
        { PIN1; q0 = 0x109; if (__GetFlag(q0)) {
            __ClearFlag(0x80 << 2);
            __ClearFlag(0x201);
        } }
        { register int q2 __asm__("r2");
          q2 = 0;
          *(short *)(a + 0x64) = q2;
          *(short *)(a + 0x66) = q2; }
        { register int q1 __asm__("r1"); q1 = 0xc8 << 4;
          __StartTask(OvlFunc_921_2009794, q1); }
        { register int q1 __asm__("r1"); q1 = 0xc8 << 4;
          __StartTask(OvlFunc_921_20098c4, q1); }
        __Func_8092b08(0xb, 1);
        if (__GetFlag(0x203))
            OvlFunc_921_2009960();
        { PIN1; q0 = 0x109; if (__GetFlag(q0) == 0) {
            off = 0xe1;
            off <<= 1;
            p = base + off;
            off = 0;
            if (*(short *)((char *)p + off) == 9)
                OvlFunc_921_20099bc();
        } }
    } else if (area == (int)(&_AREA_33)) {
        *(int *)(*(unsigned char **)iwram_3001ebc + (0xe0 << 1)) = 0x209;
        p = base + (0xe1 << 1);
        off = 0;
        sel = *(short *)((char *)p + off);
        if (sel == 1) {
            __Func_8092950(0x15, 0xf);
            { register unsigned char *a0 __asm__("r0");
              a0 = __MapActor_GetActor(0x15) + 0x59;
              { register int m3 __asm__("r3"); m3 = 8; m3 |= *a0; *a0 = m3; } }
            __Func_8092b08(0x15, 1);
            if (__GetFlag(0x881)) {
                v1 = 0xa; v2 = 8; __Func_8010704(0xa, 7, 1, 1, v1, v2);
                __CopyMapTiles(3, 0x7d, 9, 0x45, 3, 3);
                __Func_800fe9c();
                __WaitFrames(1);
                __MapActor_SetBehavior(8, (unsigned char *)2);
                __MapActor_SetPos(0xa, 0, 0);
            } else if (__GetFlag(0x82c) && __GetFlag(0x82a)) {
                __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
                __MapActor_SetPos(9, 0xae << 16, 0xa4 << 16);
                __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
                __MapActor_SetAnim(9, 5);
                { PIN3; q1 = 0xa8; q2 = 0x98; q0 = 8; q1 <<= 16; q2 <<= 16; __MapActor_SetPos(q0, q1, q2); }
                b1 = __MapActor_GetActor(8); h1 = 0xc0 << 6; *(short *)(b1 + 6) = h1;
                if (__GetFlag(0x82b) == 0)
                    OvlFunc_921_2008f90();
            } else {
                v1 = 0xa; v2 = 8; __Func_8010704(0xa, 7, 1, 1, v1, v2);
                __CopyMapTiles(3, 0x7d, 9, 0x45, 3, 3);
                __Func_800fe9c();
                __WaitFrames(1);
                if (__GetFlag(0x82c)) {
                    __MapActor_SetPos(8, 0x95 << 16, 0xe8 << 15);
                    b4 = __MapActor_GetActor(8);
                    h4 = 0;
                    *(short *)(b4 + 6) = h4;
                    *(short *)(__MapActor_GetActor(9) + 0x66) = h4;
                    __MapActor_SetBehavior(9, gScript_921__0200a4f4);
                } else {
                    __MapActor_SetBehavior(8, (unsigned char *)2);
                }
            }
        } else if (sel == 2) {
            if (__GetFlag(0x881) == 0) {
                b5 = __MapActor_GetActor(0xb); h5 = 1; *(short *)(b5 + 0x66) = h5;
                __MapActor_SetBehavior(0xb, gScript_921__0200a4f4);
            }
        } else if (sel == 4) {
            if (__GetFlag(0x881)) {
                { PIN3; q1 = 0xb6; q2 = 0x2420000; q0 = 0xc; q1 <<= 17; __MapActor_SetPos(q0, q1, q2); }
                __Func_8092b08(0xc, 2);
                base = 4;
                { register unsigned char *a0 __asm__("r0");
                  a0 = __MapActor_GetActor(0xc) + 0x59;
                  *a0 = *a0 | base; }
                sel = 3;
                __CopyMapTiles(6, 0x7d, 0x16, 0x58, sel, sel);
                { PIN3; q1 = 0xf6; q2 = 0x2420000; q0 = 0xd; q1 <<= 17; __MapActor_SetPos(q0, q1, q2); }
                __Func_8092b08(0xd, 2);
                { register unsigned char *a0 __asm__("r0");
                  a0 = __MapActor_GetActor(0xd) + 0x59;
                  base |= *a0; *a0 = base; }
                __CopyMapTiles(9, 0x7d, 0x1c, 0x58, sel, sel);
            } else {
                *(int *)(__MapActor_GetActor(0xc) + 0x18) = 0xffff0000;
                __Actor_SetSpriteFlags(__MapActor_GetActor(0xc), 0);
                __MapActor_SetAnim(0xc, 5);
                __Actor_SetSpriteFlags(__MapActor_GetActor(0xd), 0);
                __MapActor_SetAnim(0xd, 5);
            }
        } else if (sel == 3) {
            if (__GetFlag(0x881)) {
                { PIN2; q1 = 0xe6; q0 = 0xf; q1 <<= 17; __MapActor_SetPos(q0, q1, 0x81 << 17); }
                __Func_8092b08(0xf, 2);
                { register unsigned char *a0 __asm__("r0");
                  a0 = __MapActor_GetActor(0xf) + 0x59;
                  { register int m3 __asm__("r3"); m3 = 4; m3 |= *a0; *a0 = m3; } }
                __MapActor_SetPos(0xe, 0xcc << 17, 0x84 << 17);
                b3 = __MapActor_GetActor(0xe); h3 = 0x80 << 5; *(short *)(b3 + 6) = h3;
                __CopyMapTiles(0xc, 0x7d, 0x1a, 0x46, sel, sel);
            } else {
                { PIN2; q1 = 0xe6; q0 = 0xe; q1 <<= 17; __MapActor_SetPos(q0, q1, 0x81 << 17); }
                __Func_8092b08(0xe, 2);
                { register unsigned char *a0 __asm__("r0");
                  a0 = __MapActor_GetActor(0xe) + 0x59;
                  { register int m3 __asm__("r3"); m3 = 4; m3 |= *a0; *a0 = m3; } }
                *(int *)(__MapActor_GetActor(0xf) + 0x18) = 0xffff0000;
                __Actor_SetSpriteFlags(__MapActor_GetActor(0xf), 0);
                __MapActor_SetAnim(0xf, 5);
            }
        } else if (sel == 7) {
            if (__GetFlag(0x881)) {
                b2 = __MapActor_GetActor(0x14); h2 = 0xc0 << 6; *(short *)(b2 + 6) = h2;
                if (__GetFlag(0x82e) == 0) {
                    { PIN3; q2 = 0xa1; q0 = 0x14; q1 = 0x28a0000; q2 <<= 16; __MapActor_SetPos(q0, q1, q2); }
                    OvlFunc_921_20099e8();
                } else {
                    { PIN3; q1 = 0xa1; q2 = 0xa6; q0 = 0x14; q1 <<= 18; q2 <<= 16; __MapActor_SetPos(q0, q1, q2); }
                }
            }
        }
    }
    return 0;
}

void OvlFunc_921_2008f90(void)
{
    unsigned char *p;
    unsigned char *a;
    unsigned char *s;

    __CutsceneStart();
    { PIN3; q1 = 0xb6; q2 = 0x96; q0 = 3; q1 <<= 16; q2 <<= 16; __MapActor_SetPos(q0, q1, q2); }
    { PIN4; q0 = 0x8d << 16; q1 = -1; q2 = 0xdd << 16; q3 = 0; __Func_80933f8(q0, q1, q2, q3); }
    __WaitFrames(1);
    __Func_80933d4(0x4ccc, 0x999);
    { PIN4; q0 = 0x8c << 16; q1 = -1; q2 = 0xa4 << 16; q3 = 1; __Func_80933f8(q0, q1, q2, q3); }
    p = *(unsigned char **)iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x80 << 1;
    *(int *)(p + (0xe4 << 1)) = 0x28;
    __MapTransitionIn();
    { PIN3; q0 = 0; q1 = 0x6666; q2 = 0x3333; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x6666; q2 = 0x3333; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x6666; q2 = 0x3333; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0, 0x8e, 0xdd);
    __Func_8092adc(0, 0xd0 << 8, 0);
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_SetPos(1, *(int *)(a + 8), *(int *)(a + 0x10));
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_SetPos(2, *(int *)(a + 8), *(int *)(a + 0x10));
    __Func_809218c(1, 0x96, 0xea);
    __Func_80921c4(2, 0x86, 0xea);
    __MapActor_SetAnim(1, 1);
    s = gScript_921__0200a74c;
    { PIN3; q0 = 0; q2 = (int)s; q1 = 0x10003; __Func_8092a1c(q0, q1, q2); }
    { PIN3; q2 = (int)s; q0 = 1; q1 = 0x10003; __Func_8092a1c(q0, q1, q2); }
    { PIN3; q2 = (int)s; q0 = 2; q1 = 0x10003; __Func_8092a1c(q0, q1, q2); }
    __Func_8093530();
    s = gScript_921__0200a5ec;
    __MapActor_SetBehavior(9, s);
    __CutsceneWait(0x28);
    { PIN2; q1 = 0x81 << 1; q0 = 3; __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x28);
    __Func_80925cc(3, 1);
    __MessageID(0x155c);
    __Func_8093040(3, 0, 0x14);
    __MapActor_SetBehavior(9, s);
    __Func_8093040(9, 0, 0x14);
    { PIN3; q1 = 0x80; q0 = 3; q1 <<= 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(8, 0, 0xa);
    __MapActor_DoAnim(8, 4);
    __Func_8093040(8, 0, 0x28);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0xa);
    { PIN3; q1 = 0x80; q0 = 3; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 8; __Func_8092adc(q0, 0xc0 << 6, 0x14); }
    __Func_8093040(3, 0, 0xa);
    __MapActor_SetBehavior(9, s);
    OvlFunc_921_20096c8();
    { PIN3; q0 = 3; q1 = 0x101; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(3, 0, 0x28);
    __Func_8093040(9, 0, 0x14);
    { PIN3; q2 = 0x3c; q0 = 8; q1 = 0x105; __MapActor_Emote(q0, q1, q2); }
    __MapActor_SetAnim(9, 7);
    __Func_8010560(L31c0, 0xa, 0x45);
    __CutsceneWait(0xa);
    __Func_80925cc(3, 2);
    __MapActor_DoAnim(3, 4);
    __Func_8093040(3, 0, 0x14);
    __Func_80925cc(9, 1);
    __CutsceneWait(0x28);
    __MapActor_SetAnim(9, 8);
    __Func_8010560(L31d6, 0xa, 0x45);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x14);
    __Func_8092adc(8, 0, 0x14);
    __MapActor_DoAnim(8, 3);
    __Func_8093040(8, 0, 0xa);
    { PIN3; q0 = 3; q1 = 0x101; q2 = 0x1e; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0xa; q0 = 3; q1 <<= 8; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(3, 4);
    __Func_8093040(3, 0, 0xa);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x28);
    { PIN3; q1 = 0x80 << 9; q2 = 0x80 << 8; q0 = 3; __MapActor_SetSpeed(q0, q1, q2); }
{ register unsigned char *a0 __asm__("r0"); register int q3 __asm__("r3");
      a0 = __MapActor_GetActor(3); q3 = 0; *(short *)(a0 + 0x64) = q3; }
    __MapActor_SetBehavior(3, gScript_921__0200a670);
    while (*(short *)(__MapActor_GetActor(3) + 0x64) == 0)
        __WaitFrames(1);
    { PIN4; q0 = 0x8c << 16; q1 = -1; q2 = 0xc6 << 16; q3 = 1; __Func_80933f8(q0, q1, q2, q3); }
    __MapActor_WaitScript(3);
    { PIN3; q0 = 3; q1 = 0x101; q2 = 0x50; __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(3, 0, 0x28);
    __Func_80925cc(3, 1);
    __CutsceneWait(0xa);
    __ActorMessage(3, 0);
    __PlaySound(0x83);
    { PIN2; q0 = 0x80 << 9; q1 = 0; __Func_8091220(q0, q1); }
    __Func_8091200(0x207e9f, 0);
    __Func_8091254(0xa);
    __WaitFrames(1);
    __PlaySound(0xdc);
    __WaitFrames(0x28);
    { PIN2; q0 = 0x80 << 9; q1 = 0; __Func_8091200(q0, q1); }
    __Func_8091254(0x3c);
    __WaitFrames(0x3c);
    { PIN2; q1 = 0x81 << 1; q0 = 3; __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x14);
    __Func_8092adc(3, 0, 0xa);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 3; q1 <<= 10; q2 <<= 9; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(3, 0xca, 0xc6);
    __CutsceneWait(0x28);
    __Func_80925cc(3, 2);
    __ActorMessage(3, 0);
    __MapActor_DoAnim(3, 4);
    __Func_8093040(3, 0, 0x14);
    { PIN2; q1 = 0x81 << 1; q0 = 3; __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x28);
    __Func_8093040(3, 0, 0x28);
    { PIN3; q1 = 0x80; q2 = 0x28; q0 = 3; q1 <<= 1; __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(3, 0);
    __MapActor_SetIdle(0);
    __MapActor_SetIdle(1);
    __MapActor_SetIdle(2);
    __MapActor_SetSpeed(3, 0xc0 << 10, 0xc0 << 9);
{ register unsigned char *a0 __asm__("r0"); register int q3 __asm__("r3");
      a0 = __MapActor_GetActor(3); q3 = 0; *(short *)(a0 + 0x64) = q3; }
    __MapActor_SetBehavior(3, gScript_921__0200a6e0);
    while (*(short *)(__MapActor_GetActor(3) + 0x64) == 0)
        __WaitFrames(1);
    { PIN3; q1 = 0x80; q0 = 0; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 1; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 2; q1 <<= 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 11; q2 <<= 10; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 1; q1 <<= 11; q2 <<= 10; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80 << 11; q2 = 0x80 << 10; q0 = 2; __MapActor_SetSpeed(q0, q1, q2); }
    __PlaySound(0x98);
    s = (unsigned char *)0xfe;
    a = __MapActor_GetActor(0) + 0x5a;
    *a = *a & (int)s;
    a = __MapActor_GetActor(1) + 0x5a;
    *a = *a & (int)s;
    a = __MapActor_GetActor(2) + 0x5a;
    *a = *a & (int)s;
    __MapActor_TravelTo(0, 0x84, 0xce);
    __MapActor_TravelTo(1, 0x88, 0xdd);
    __MapActor_TravelTo(2, 0x7a, 0xee);
    __MapActor_WaitScript(3);
    __CutsceneWait(0x50);
    { register int m5 __asm__("r5");
      m5 = 1;
      a = __MapActor_GetActor(0) + 0x5a;
      *a = *a | m5;
      a = __MapActor_GetActor(1) + 0x5a;
      *a = *a | m5;
      a = __MapActor_GetActor(2) + 0x5a;
      m5 |= *a;
      *a = m5; }
    { PIN3; q1 = 0xcccc; q0 = 0; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0x6666; q0 = 2; q1 = 0xcccc; __MapActor_SetSpeed(q0, q1, q2); }
    s = gScript_921__0200a760;
    __MapActor_SetBehavior(1, s);
    __MapActor_RunScript(2, s);
    __CutsceneWait(0x14);
    p = *(unsigned char **)iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x209;
    *(int *)(p + (0xe4 << 1)) = 0x18;
    __SetFlag(0x82b);
    __CutsceneEnd();
}
