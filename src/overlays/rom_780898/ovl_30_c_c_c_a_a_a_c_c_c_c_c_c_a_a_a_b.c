// fakematch
/* OvlFunc_883_200acb0  --  0x0200acb0
 *   [asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_a_a.s, 3rd of 3]
 *
 * 228 instructions of straight-line cutscene with two guarded actor fetches and
 * one `if` in the middle.  Built at the tree default -O2: no Makefile pattern
 * rule matches rom_780898/ovl_30_c_c_c_a_a_a%, so `asm/%.o: src/%.c` applies and
 * a scratch-path screen sees the same flags as the real object (objcmp prints
 * no `(built with: ...)` line).
 *
 * THE MESSAGE ID IS A LIVE VARIABLE, AND gcse's CONSTANT PROPAGATION KILLS IT.
 * The ROM's prologue is `push {r5, lr}` and r5 holds ONE value for the whole
 * body: `ldr r5, =0x1c45 / mov r0, r5` at the first __MessageID and
 * `add r0, r5, #6` at the second, 118 instructions later.  A three-operand add
 * off a held pool constant is a LIVE PSEUDO, so the source is
 * `m = 0x1c45; ... __MessageID(m + 6);` and not two literals -- cse.c only
 * relates CONST values through get_related_value, which needs a SYMBOL_REF, so
 * two independent literals can never produce the add.
 *
 * But written as a plain `int m` this does not survive at -O2: gcse's cprop
 * pass rewrites the second use to the constant and cse folds `m + 6` to
 * 0x1c4b, giving `ldr r0, =0x1c45` / `ldr r0, =0x1c4b` and no r5 at all
 * (228 of 237 encodings wrong).  Isolated on a four-line probe: the fold fires
 * only when the function has more than one basic block -- gcse.c bails at
 * `n_basic_blocks <= 1` -- which is why the same two statements with no
 * intervening `if` keep the register.  `-fno-gcse` restores the ROM's exact
 * sequence; -fno-rerun-cse-after-loop and -fno-cse-follow-jumps are inert
 * (control: -fno-omit-frame-pointer widens the push, so the sweep is live).
 *
 * PINNING THE VARIABLE TO r5 IS THE CURE, AND IT NEEDS NO BUILD CHANGE.
 * `register int m __asm__("r5")` matches at the tree default -O2: cprop will
 * not substitute a constant into a hard register, so the pseudo stays live and
 * the ROM's `mov r0, r5` / `add r0, r5, #6` appear.  Adding the object to
 * GCSE_CFLAGS with a plain `int m` matches too and is the alternative if the
 * pin is ever judged too strong, but it changes flags for the whole TU.
 *
 * 0x1c45 IS A LITERAL, NOT A SYMBOL, AND ONLY objcmp CAN SAY SO.  Spelled
 * `(int)&_MSG_1c45` the function is 231 lines and ONE instruction differing by
 * tryc -- a better score than most rejected spellings -- but objcmp shows the
 * candidate carrying an R_ARM_ABS32 the reference does not have.  The reference
 * object's only relocation in this function's 0x16d4..0x1938 range is
 * R_ARM_ABS32 iwram_3001ebc at 0x192c; 0x1c45, 0x1c4b, 0x1001, 0x4008, 0x105,
 * 0x303, 0xcccc, 0x6666 and 0x2460000 are all bare literals, and none of them
 * has an entry in message.sym, area.sym or const.sym.  This is the exact
 * judgement the sibling ovl_30_..._a_a_b.c had to make in the other direction
 * for _MSG_1c60.
 *
 * TWENTY-TWO PINS, MINIMAL BY MEASUREMENT.  With r5 taken by the message id the
 * ROM keeps NOTHING else, so every repeated constant is rebuilt at every use:
 * 0x4008 at eight sites, 0x1001 at four, 0x105, 0xcccc/0x6666, 0xd0<<8, 0xa0<<7
 * and 0x98<<2 at two each.  Unpinned, cse_main commons them into pseudos that
 * straddle `bl` and the function comes out 648 bytes against 612 with r6/r7
 * in the push.  Twenty-five sites were stripped one at a time under objcmp:
 * three were inert -- __Func_8092adc(8, 0xc0<<6, 0), __Func_8093040(0x1001, 0,
 * 0x28) at the second-to-last 0x1001 site and __Func_8093040(0x4008, 0, 0xa) --
 * and all three come out together.  A second round over the surviving 22 found
 * every one load-bearing, and the whole sweep was repeated after the r5 pin
 * replaced -fno-gcse, because a pin set is only valid in the shape it was
 * measured in.  Note "N pins is a size, not a set": the three that fall are the
 * LAST use of their value, never the first.
 *
 * WITHIN A PINNED SITE, WRITE THE STATEMENTS IN THE ROM'S EMITTED ORDER,
 * INCLUDING THE SHIFT -- the same rule the sibling needed fourteen times.
 *
 * THE do/while(0) IS LOAD-BEARING, 8 ENCODINGS.  Without it sched hoists
 * `ldr r5, =0x1c45` seven slots up, to immediately after `bl __CutsceneStart`;
 * the loop note `jump.c` leaves behind ends the region and pins it to the ROM's
 * slot.  `while (0) ;` is byte-identical; `if (0) ;` is INERT, so this is the
 * loop and not a label.  Ending the region costs the PRECEDING call its
 * scheduler, which is why __Func_809280c needs a pin of its own (2 encodings).
 *
 * The `iwram_3001ebc` offset build is the same non-interleaved shape as the
 * sibling: pointer first, then 0xec<<1, which is what plain C emits.
 *
 * LANDING NEEDS A SPLIT.  The .s holds three functions and this is the last;
 * overlays/rom_780898/overlay.ld:63 names the single .o.
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
extern int __Func_8091c7c(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092950(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_883_200acb0(void)
{
    unsigned char *p;
    register int m __asm__("r5");

    __CutsceneStart();
    __Func_8092950(0, 0);
    { PIN3; q2 = 0x14; q1 = 0; q0 = 8;
      __Func_809280c(q0, q1, q2); }
    do { } while (0);
    m = 0x1c45;
    __MessageID(m);
    __Func_809259c(8, 2);
    __Func_8093040(8, 0, 0x14);
    __Func_80933d4(0x80 << 9, 0x80 << 6);
    __Func_80933f8(0xc7 << 17, -1, 0x246 << 16, 1);
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xd2; q2 = 0x98; q0 = 0; q1 <<= 1; q2 <<= 2;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 0; q1 <<= 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(8, 0xc0 << 6, 0);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(1, *(int *)(p + 8), *(int *)(p + 0x10));
    { PIN3; q1 = 0xc9; q2 = 0x98; q0 = 1; q1 <<= 1; q2 <<= 2;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xd0; q2 = 0x14; q0 = 1; q1 <<= 8;
      __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 0x1001; q1 = 0;
      __ActorMessage(q0, q1); }
    { PIN3; q1 = 0xa0; q2 = 0x14; q0 = 8; q1 <<= 7;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(8, 3);
    { PIN2; q0 = 0x4008; q1 = 0;
      __ActorMessage(q0, q1); }
    __MapActor_SetAnim(0, 3);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x14);
    __Func_80925cc(8, 2);
    { PIN2; q1 = 0; q0 = 0x4008;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 1) {
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __Func_809259c(8, 1);
    }
    { PIN3; q0 = 0x4008; q1 = 0; q2 = 0x28;
      __Func_8093040(q0, q1, q2); }
    { PIN3; q1 = 0x105; q2 = 0x3c; q0 = 8;
      __MapActor_Emote(q0, q1, q2); }
    __MessageID(m + 6);
    { PIN3; q2 = 0x14; q0 = 0x4008; q1 = 0;
      __Func_8093040(q0, q1, q2); }
    __Func_80925cc(1, 1);
    __CutsceneWait(0x28);
    { PIN3; q2 = 0x28; q0 = 0x1001; q1 = 0;
      __Func_8093040(q0, q1, q2); }
    __Func_80925cc(8, 1);
    { PIN3; q1 = 0xd0; q2 = 0x14; q0 = 8; q1 <<= 8;
      __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 0x4008; q1 = 0;
      __ActorMessage(q0, q1); }
    __MapActor_DoAnim(1, 3);
    { PIN3; q0 = 0x1001; q1 = 0; q2 = 0x78;
      __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 0x4008; q1 = 0; q2 = 0x14;
      __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x105; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x1001, 0, 0x28);
    __MapActor_DoAnim(8, 4);
    { PIN3; q2 = 0x14; q0 = 0x4008; q1 = 0;
      __Func_8093040(q0, q1, q2); }
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x28);
    { PIN3; q1 = 0xa0; q0 = 8; q1 <<= 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x4008, 0, 0xa);
    __MapActor_SetAnim(0, 3);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(8, 3);
    __MapActor_SetAnim(1, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __SetFlag(0x303);
    __CutsceneEnd();
}
