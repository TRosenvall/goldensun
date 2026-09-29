// fakematch
/* OvlFunc_952_200a014  --  0x0200a014
 *   [asm/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c_c_c_c.s, 1st of 2]
 *
 * 2,805 instructions -- the largest function converted in this tree. A
 * straight-line cutscene script of 869 calls behind eight save/choice tests:
 * 11 conditional branches, 21 labels, no `sub sp` frame, no reference to
 * r8-r11, no `.call_via`. Byte-exact: 7468 bytes, 2837 encodings and 875
 * relocations identical (tools/objcmp.py).
 *
 * tryc's OK is WEAK on this family and its DIRTY is worse: the reference keeps
 * its literal pool INSIDE the function, so tryc normalises pool loads, and the
 * final `if` puts gcc's pool-skip label immediately before the if's own join
 * label -- two label definitions at one address, which shifts every later
 * positional line. This file screens at "16 differ, first diff at 2811" while
 * being byte-identical; all 16 are the label cascade documented in
 * docs/elevation.md under "A DIRTY screen that opens on a LABEL is a false
 * negative". objcmp is the only verdict here. `--align` is unreliable for the
 * same pool reason.
 *
 * THE ONE CALLEE-SAVED REGISTER IS `&iwram_3001ebc`, NOT A COMMONED CONSTANT.
 * The sibling template ovl_30_c_a_a_c_c_c_c_c_a_b.c has `push {lr}` and keeps
 * NO value anywhere; this one has `push {r5, lr}`, and the expectation that
 * exactly one commoned constant survives in r5 is WRONG. r5 holds the ADDRESS
 * of the `unsigned char *iwram_3001ebc` pointer, hoisted by gcse across the
 * .L2a3a block, which reads `[r5]` three times over two calls. Sixteen other
 * sites increment the same halfword and each rebuilds the address, because
 * each is alone in its basic block. Nothing else in 2,805 instructions is
 * held: every repeated constant is rebuilt at every use, which is why the
 * pins below are the lever.
 *
 * 145 PIN SITES, 432 REGISTER PINS (tools/shimcount.py), AT A GREEDY FIXPOINT.
 * Start: pin every site with an argument outside 0..255 -- 174 sites, which is
 * already byte-exact ("N pins is a size, not a set"). Greedy removal, one site
 * at a time last-to-first, re-screened after every drop, finds 29 inert:
 * __MessageID(0x22c4), __ActorMessage(-1, 0), two __Func_8092304 sites with a
 * negated argument, two __MapActor_Emote sites and 23 __Func_8092adc sites
 * whose second argument is a shifted byte. A second full round over the
 * surviving 145 drops NOTHING, so every pin left is individually
 * load-bearing.
 *
 * THE FILLS ARE UNIFORM EXCEPT FOR SEVEN DESCENDING SITES, AND SIX OF THEM ARE
 * `__Func_8092c40`. docs/elevation.md records that callee by name; here it
 * wants `q1 = 0; q0 = N;` at SIX of its EIGHT sites -- every site whose call
 * is followed by the `__Func_8091c7c` test -- while the two that end a block
 * take no pin at all. `__Func_8091a58(0xa4, 0)` is the seventh. Everything
 * else is ascending q0..q3, one statement per argument, whole value per
 * statement, even where the ROM emits that site's instructions in another
 * order; sched2 reproduces the ROM's variations from the one spelling.
 *
 * THE `|= 1` SITE NEEDS THE NARROW LOCAL; THE `&= 0xfe` SITE MUST NOT HAVE A
 * LOCAL AT ALL. Both are `__MapActor_GetActor(0x14)[0x5a]`, four instructions
 * apart, and they pull opposite ways -- exactly the pair recorded in
 * src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_a_a_b.c. The ROM
 * puts the CONSTANT in the destination at both (`mov r3, #0xfe / and r3, r2`
 * and `mov r3, #1 / orr r3, r2`); the inline expression gets `and` right and
 * `orr` wrong, and `unsigned char one = 1; p[0x5a] = one | p[0x5a];` with `p`
 * named only at this one site closes it. Naming `p` at BOTH sites costs 2,529
 * differing -- the second local turns `add r0, #0x5a` into `mov r1, r0 /
 * add r1, #0x5a` and the function grows.
 *
 * THE LAST TWO ENCODINGS NEEDED A VOLATILE-ASM BARRIER, AND THE REASON IS
 * `rank_for_schedule`'s DEPENDENT COUNT. In the .L2a3a block the ROM emits
 * `add r2, #1 / mov r1, #0 / strh r2, [r3] / mov r0, #1 / bl __Func_8092c40`;
 * every pin arrangement, both fill orders, both pin widths, every statement
 * order and seven upstream pins give `mov r1 / mov r0 / strh` instead.
 * -fsched-verbose=8 says why: the three insns TIE on priority at 35, tie on
 * class, and the tie then falls through to "prefer the insn which has more
 * later insns that depend on it" -- each `mov` has three dependents (the two
 * calls plus the output dependence on the later same-register mov for the
 * `__Func_8091c7c` argument) and the `strh` has only two, so no source
 * spelling can lift it. `__asm__ volatile ("")` between the store and
 * `q0 = 1` splits the scheduling region and lands it. The barrier is available
 * because the ROM uses NO high register in this function (docs/elevation.md:
 * "THE BARRIER IS ONLY AVAILABLE WHERE THE ROM DOES NOT USE r8-r11"); it emits
 * a bare `.code 16` and no bytes.
 *
 * CONTROL FLOW, READ OFF THE BRANCH POLARITY. `if (__GetFlag(0x951))` -- the
 * far `beq` is expanded to `bne .L2140 / b .L2320`, so the THEN block is the
 * one that follows. All seven `__Func_8091c7c` tests are the other polarity,
 * a plain `bne` to the else block, i.e. `if (... == 0)`. The three tail tests
 * are `if (p != 0)` on a `__MapActor_GetActor(0)` result read as
 * `short *`: `b[5]` and `b[9]` give the ROM's `mov r2, #0xa / ldrsh r1,
 * [r0, r2]` because thumb has no immediate-offset `ldrsh`. Three `b` to the
 * label after a mid-body pool are pool skips, not control flow.
 *
 * NO SYMBOL SPELLINGS AND NO STACK. objcmp reports 875 relocations identical
 * against 869 `bl` plus the six `iwram_3001ebc` pool words, so every other
 * pooled constant is a bare literal (0x22c4, 0x951, 0xcccc, 0x6666, 0x13333,
 * 0x9999, 0x101, 0x105 ...). `push {r5, lr}` with no `sub sp` proves there is
 * no spill slot and nothing else to name.
 *
 * FLAGS ARE THE TREE DEFAULT -O2. No Makefile rule or `%` pattern names this
 * object, so `asm/%.o: src/%.c` with `GCC296_CFLAGS` applies. Note three OTHER
 * rom_7d768c objects (ovl_30_c_a_a_c_c_c_c_c_a_a, ovl_30_c_a_c_a_b,
 * ovl_30_c_a_a_c_a_b) carry explicit `-fno-rerun-cse-after-loop` rules; none
 * of them is a wildcard, so none reaches this stem.
 *
 * LANDING NEEDS A TEXT/TEXT SPLIT. The .s holds TWO functions and this is the
 * FIRST; OvlFunc_952_200bd40 (71 instructions, ref line 2870) stays as asm.
 * datacheck.py is SILENT on the file -- no `.section .data`, zero data
 * exports -- so tools/split_s.py's ordinary path applies. Exactly ONE linker
 * row names the object: overlays/rom_7d768c/overlay.ld:33.
 */
extern int __GetFlag(int id);
extern void __PlaySound(int id);
extern void __PlayMapMusic(void);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __SetCameraTarget(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_SetExtra(int slot, int v);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __ActorMessage(int a, int b);
extern void __Func_8019908(int a, int b);
extern void __Func_80118a8(int n);
extern void __Func_80118c0(int n);
extern void __Func_808f1c0(int a, int b);
extern void __Func_8091a58(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8091e9c(int n);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092208(int a, int b, int c);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_80922c4(int a, int b, int c);
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

extern unsigned char *iwram_3001ebc;

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_952_200a014(void)
{
    unsigned char *p;
    unsigned char one = 1;
    short *b;

    __PlaySound(0x1e);
    __CutsceneStart();
    __MessageID(0x22c4);
    __MapTransitionIn();
    __WaitMapTransition();
    { PIN4; q0 = 0xd8 << 16; q1 = -1; q2 = 0xb8 << 18; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x14);
    __CutsceneWait(0xa);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0xe, 0, 0x10);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __SetCameraTarget(0, 1);
    __Func_8093530();
    __CutsceneWait(0x28);
    { PIN3; q0 = 0; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xd0; q2 = 0xbe << 2;
      __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN4; q0 = 0xd8 << 16; q1 = -1; q2 = 0xb8 << 18; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    { PIN4; q0 = 1; q1 = -0x10; q2 = 0x10; q3 = 0xc0 << 8;
      __Func_809233c(q0, q1, q2, q3); }
    { PIN4; q0 = 3; q1 = 0; q2 = 0x18; q3 = 0xc0 << 8;
      __Func_809233c(q0, q1, q2, q3); }
    { PIN4; q0 = 2; q1 = 0x10; q2 = 0x10; q3 = 0xc0 << 8;
      __Func_809233c(q0, q1, q2, q3); }
    __MapActor_WaitMovement(1);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    if (__GetFlag(0x951))
    {
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0x14, 3);
        __CutsceneWait(0x14);
        __ActorMessage(0x14, 0);
        __CutsceneWait(0xa);
        __Func_8092adc(0xe, 0xa0 << 8, 0);
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(0xe, 3);
        __CutsceneWait(0x14);
        __ActorMessage(0xe, 0);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0x14, 3);
        __CutsceneWait(0x14);
        __ActorMessage(0x14, 0);
        { PIN3; q0 = 0x14; q1 = 0xc0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x14);
        { PIN3; q0 = 0x14; q1 = 0x80 << 9; q2 = 0x80 << 8;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 0x14; q1 = 0; q2 = -0x10;
          __Func_8092304(q0, q1, q2); }
        __CutsceneWait(0x28);
        __MapActor_DoAnim(0x14, 3);
        __CutsceneWait(0x28);
        { PIN3; q0 = 0x14; q1 = 0x80 << 7; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x14);
        __Func_8092304(0x14, 0, 0x20);
        __Func_8092adc(0xe, 0x80 << 8, 0);
        __CutsceneWait(0xa);
        __Func_8092adc(0x14, 0, 0);
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(0x14, 3);
        __CutsceneWait(0x1e);
        { PIN3; q0 = 0x14; q1 = 0x80 << 7; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x14);
        { PIN3; q0 = 0x14; q1 = 0xcccc; q2 = 0x6666;
          __MapActor_SetSpeed(q0, q1, q2); }
        __MapActor_GetActor(0x14)[0x5a] &= 0xfe;
        { PIN3; q0 = 0x14; q1 = 0; q2 = -0x10;
          __Func_8092304(q0, q1, q2); }
    p = __MapActor_GetActor(0x14);
    p[0x5a] = one | p[0x5a];
        { PIN3; q0 = 0xe; q1 = 0x80 << 7; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x28);
        { PIN3; q0 = 0xe; q1 = 0xcccc; q2 = 0x6666;
          __MapActor_SetSpeed(q0, q1, q2); }
        __Func_8092304(0xe, 0, 0x10);
        __CutsceneWait(0x28);
        __Func_8019908(0xa4, 2);
        __ActorMessage(-1, 0);
        __Func_808f1c0(0xa4, 3);
        { PIN2; q1 = 0; q0 = 0xa4;
          __Func_8091a58(q0, q1); }
        __Func_8092adc(0, 0xc0 << 8, 0);
        __CutsceneWait(0x1e);
        { PIN3; q0 = 0xe; q1 = 0; q2 = -0x10;
          __Func_8092304(q0, q1, q2); }
        __Func_8092adc(0xe, 0x80 << 7, 0);
        __CutsceneWait(0x1e);
        __Func_80925cc(0xe, 2);
        __CutsceneWait(0x14);
        __Func_8092c40(0xe, 0);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 2;
    }
    else
    {
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 5;
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0x14, 3);
        __CutsceneWait(0x14);
        __ActorMessage(0x14, 0);
        __Func_8092adc(0xe, 0xa0 << 8, 0);
        __CutsceneWait(0x28);
        __MapActor_DoAnim(0xe, 3);
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0x14, 3);
        __CutsceneWait(0x1e);
        __Func_8092adc(0xe, 0x80 << 7, 0);
        __CutsceneWait(0x1e);
        __Func_80925cc(0xe, 2);
        __CutsceneWait(0x14);
        __Func_8092c40(0xe, 0);
    }
    if (__Func_8091c7c(0, 0) == 0)
    {
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0x14, 3);
        __CutsceneWait(0x14);
        __ActorMessage(0x14, 0);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    }
    else
    {
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0x14, 3);
        __CutsceneWait(0x14);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __ActorMessage(0x14, 0);
    }
    __CutsceneWait(0xa);
    __Func_8092adc(0xe, 0xa0 << 8, 0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x14, 3);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __MapActor_Jump(1, 4, 0xd);
    __MapActor_Jump(1, 4, 0x1e);
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xe; q1 = 0x19999; q2 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0x14, 0, 0x10);
    __WaitFrames(2);
    __Func_8092adc(0x14, 0x80 << 6, 0);
    __CutsceneWait(0xa);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 4);
    __CutsceneWait(0x1e);
    __Func_809280c(0xe, 0x14, 0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x14);
    __Func_8092848(0xe, 1, 0);
    __CutsceneWait(0x28);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x1e);
    __Func_809280c(1, 2, 0x1e);
    __CutsceneWait(0xa);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x14);
    __Func_809280c(2, 1, 0x1e);
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    __MapActor_Emote(1, 0x83 << 1, 0x32);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __Func_8092848(1, 0, 0);
    __Func_8092848(3, 2, 0x32);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(2, 0xc0 << 8, 0);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x14, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 2; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 0xe;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0)
    {
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0xe, 3);
        __CutsceneWait(0x1e);
        __ActorMessage(0xe, 0);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    }
    else
    {
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0xe, 4);
        __CutsceneWait(0x14);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __ActorMessage(0xe, 0);
    }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x14; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x14, 0);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(3, 3);
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0x32);
    __Func_809280c(0xe, 0x14, 0x3c);
    __Func_8092adc(0xe, 0x80 << 7, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x14; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x14, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_8092848(1, 0, 0x32);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0, 3);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 3; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(3, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0x81 << 1; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    __Func_809280c(0x14, 0xe, 0x32);
    __CutsceneWait(0xa);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0x14);
    __Func_8092848(1, 0, 0);
    __Func_8092848(3, 2, 0x32);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x32);
    __Func_8092adc(0x14, 0x80 << 6, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    __ActorMessage(1, 0);
    __CutsceneWait(0x14);
    __Func_8092adc(0xe, 0x80 << 8, 0);
    __CutsceneWait(0x28);
    { PIN3; q0 = 0xe; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 2; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    __Func_8092848(0xe, 2, 0x28);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_8092848(1, 0, 0);
    __Func_8092848(3, 2, 0x3c);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(2, 0xc0 << 8, 0);
    __CutsceneWait(0x32);
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __Func_8092848(1, 0, 0x28);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 1;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0)
    {
        __CutsceneWait(0x14);
        __ActorMessage(1, 0);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 3;
    }
    else
    {
        __CutsceneWait(0xa);
        { PIN2; q1 = 0;
          *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
          __asm__ volatile ("");
          q0 = 1;
          __Func_8092c40(q0, q1); }
        if (__Func_8091c7c(0, 0) == 0)
        {
            __CutsceneWait(0x14);
            __ActorMessage(1, 0);
            *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        }
        else
        {
            *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
            __ActorMessage(1, 0);
        }
    }
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x28);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 3; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(3, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x14);
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    __Func_809280c(0x14, 0xe, 0x28);
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xe; q1 = 0x105; q2 = 0x46;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 0x101; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x101; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(1, 0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    { PIN3; q0 = 2; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x28);
    __Func_809280c(0, 2, 0);
    __Func_809280c(1, 2, 0);
    __Func_809280c(3, 2, 0);
    __Func_809280c(0x14, 2, 0);
    __CutsceneWait(0x32);
    __CutsceneWait(0xa);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x14);
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(2, 4);
    __CutsceneWait(0x14);
    __ActorMessage(2, 0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x14, 3);
    __CutsceneWait(0x1e);
    __Func_809280c(0x14, 0xe, 0x1e);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_809259c(0, 2);
    __Func_809259c(1, 2);
    __Func_809259c(3, 2);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0xe, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_8092848(1, 0, 0);
    __Func_8092848(3, 2, 0x32);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x28);
    { PIN3; q0 = 0xe; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0xe; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x14; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x14, 0);
    __CutsceneWait(0x14);
    __Func_8092848(0xe, 0x14, 0x28);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN2; q0 = 0x14; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    { PIN3; q0 = 0x14; q1 = 0x19999; q2 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0x14, 0, 0x18);
    __Func_809280c(0, 0x14, 0);
    __Func_809280c(1, 0x14, 0);
    __Func_809280c(3, 0x14, 0);
    __Func_809280c(2, 0x14, 0);
    { PIN3; q0 = 0xe; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x14; q1 = 0x80 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_809280c(0x14, 0xe, 0);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0xe; q1 = 0x105; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x14; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x14; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x14; q1 = 0; q2 = -0x18;
      __Func_8092304(q0, q1, q2); }
    __Func_8092adc(0x14, 0, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x14; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 3; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(3, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_8092848(1, 0, 0);
    __Func_8092848(3, 2, 0x32);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0; q1 = 0x101; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x101; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x101; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x14; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __Func_8092848(0xe, 0x14, 0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xe; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x14; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __Func_8092adc(0xe, 0x80 << 8, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 2; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x14, 0x80 << 6, 0);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __Func_80925cc(3, 2);
    __CutsceneWait(0x14);
    __ActorMessage(3, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xe, 0);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(3, 3);
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0x1e);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xe; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_8092848(1, 0, 0x32);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(1, 0xc0 << 8, 0);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 3; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(3, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 0x101; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x101; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_Emote(3, 0x101, 0x28);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 2; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 2;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0xe, 0) == 0)
    {
        __CutsceneWait(0xa);
        __ActorMessage(2, 0);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    }
    else
    {
        __CutsceneWait(0xa);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __ActorMessage(2, 0);
    }
    __CutsceneWait(0xa);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x14);
    __Func_8092adc(0x14, 0, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __Func_809280c(0xe, 0x14, 0x28);
    __MapActor_DoAnim(0xe, 4);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_8092848(1, 0, 0x28);
    __ActorMessage(1, 0);
    { PIN3; q0 = 0xe; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x14; q1 = 0x80 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __Func_8092848(3, 2, 0x3c);
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(2, 0xc0 << 8, 0);
    __CutsceneWait(0x1e);
    __ActorMessage(3, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(2, 4);
    __CutsceneWait(0x14);
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    __Func_8092848(0x14, 0xe, 0x3c);
    { PIN3; q0 = 0x14; q1 = 0x80 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __Func_8092adc(0xe, 0x80 << 7, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 0x14;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0)
    {
        __CutsceneWait(0x14);
        { PIN3; q0 = 1; q1 = 0x81 << 1; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        __ActorMessage(1, 0);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    }
    else
    {
        __CutsceneWait(0xa);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __ActorMessage(1, 0);
    }
    __CutsceneWait(0xa);
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x14; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x14, 0);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(3, 3);
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_809280c(0x14, 0xe, 0x28);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x14; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __Func_809280c(0xe, 0x14, 0x28);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __Func_8092adc(0xe, 0x80 << 7, 0);
    __CutsceneWait(0x14);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 0x101; q2 = 0x50;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x14; q1 = 0x81 << 1; q2 = 0x46;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_DoAnim(0xe, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_8092848(1, 0, 0);
    __Func_8092848(3, 2, 0x32);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(2, 0xc0 << 8, 0);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x14; q1 = 0x83 << 1; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0x14; q1 = 0x80 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0x84 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 0xe;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0)
    {
        __CutsceneWait(0x14);
        { PIN3; q0 = 0xe; q1 = 0x80 << 1; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        __ActorMessage(0xe, 0);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    }
    else
    {
        __CutsceneWait(0xa);
        { PIN3; q0 = 0xe; q1 = 0x80 << 1; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __ActorMessage(0xe, 0);
    }
    __CutsceneWait(0xa);
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __Func_809280c(0xe, 0x14, 0x28);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __Func_809280c(0x14, 0xe, 0x28);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x14; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x14, 3);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __Func_8092adc(0x14, 0x80 << 6, 0);
    __CutsceneWait(0x14);
    __MapActor_SetExtra(0, 0x14);
    __MapActor_SetExtra(1, 0x14);
    __MapActor_SetExtra(3, 0x14);
    __MapActor_SetExtra(2, 0x14);
    { PIN3; q0 = 0x14; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0x14, 0, 0x20);
    __Func_8092adc(0x14, 0, 0);
    { PIN3; q0 = 0xe; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x14, 3);
    __CutsceneWait(0x1e);
    __Func_8092304(1, 0x10, 0);
    __Func_80922c4(0x14, 0, 0x50);
    __CutsceneWait(0x28);
    { PIN3; q0 = 1; q1 = -0x10; q2 = 0;
      __Func_8092304(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_WaitMovement(0x14);
    __CutsceneWait(0x50);
    __MapActor_SetPos(0x14, 0, 0);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(2, 0xc0 << 8, 0);
    __CutsceneWait(0x1e);
    __MapActor_SetExtra(0, 0xe);
    __MapActor_SetExtra(1, 0xe);
    __MapActor_SetExtra(3, 0xe);
    __MapActor_SetExtra(2, 0xe);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0xe; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0xe, 0, 0x18);
    __Func_8092304(0xe, -0x50, 0);
    __CutsceneWait(0xa);
    __Func_8092adc(0xe, 0, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0x14);
    __Func_8092adc(0xe, 0x80 << 7, 0);
    __CutsceneWait(0x14);
    __Func_8092304(0xe, 0, 0x30);
    __Func_8092304(0xe, -0x40, 0);
    __MapActor_SetIdle(0);
    __MapActor_SetIdle(1);
    __MapActor_SetIdle(3);
    __MapActor_SetIdle(2);
    __MapActor_SetPos(0xe, 0, 0);
    __CutsceneWait(0x14);
    __Func_8092848(0, 3, 0);
    __Func_8092848(1, 2, 0);
    __CutsceneWait(0x1e);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(3, 3);
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0x1e);
    __PlaySound(0x11);
    { PIN3; q0 = 1; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(1, 2);
    b = (short *)__MapActor_GetActor(0);
    if (b != 0)
        __MapActor_TravelTo(1, b[5], b[9]);
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetAnim(2, 2);
    b = (short *)__MapActor_GetActor(0);
    if (b != 0)
        __MapActor_TravelTo(2, b[5], b[9]);
    __MapActor_WaitMovement(2);
    __MapActor_SetPos(2, 0, 0);
    __MapActor_SetAnim(3, 2);
    b = (short *)__MapActor_GetActor(0);
    if (b != 0)
        __MapActor_TravelTo(3, b[5], b[9]);
    __MapActor_WaitMovement(3);
    __MapActor_SetPos(3, 0, 0);
    __CutsceneWait(0xa);
    __PlayMapMusic();
    __CutsceneEnd();
}

