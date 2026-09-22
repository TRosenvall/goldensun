/* OvlFunc_881_2009ca4 -- NON-MATCHING, 212 encodings of 582, size 1492 against the
 * ROM's 1488 (+4), 584 instructions against 582.
 *
 * BOTH NUMBERS, per the measurement rule: 212 of 582 under objcmp, 69 after
 * normalising pool offsets.  THE GAP IS THE 4-BYTE POOL DISPLACEMENT FROM TWO EXTRA
 * INSTRUCTIONS -- not an extra pool WORD: the reference needs 19 slots across its
 * three segments and this candidate emits 19.  That distinction is worth making
 * explicitly, because "the pool moved" usually means a word was added and here it
 * does not.
 *
 * Blocker class: several, none dominant -- which is why it is parked rather than
 * pushed.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_77a7c8/2009ca4.c \
 *     asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_c_a.s
 * ONE function in the reference, so no split is needed.
 *
 * RESIDUE CLASSES:
 *   - an r2/r3 scratch permutation at FOUR sites, INVERTED AT EVERY SITE.  By the
 *     OvlFunc_921_2009fa4 result from the same agent, a uniformly-inverted
 *     signature means a cause is still UPSTREAM -- do not chase the permutation.
 *   - two extra instructions in the __ActorMessage(8,0) fills.
 *   - the shared-zero strb/str group (PINNING IT IS WORSE, 82 against 69).
 *   - the __Func_8091c7c while-loop rotated the wrong way -- and the goto rewrite
 *     that was worth 27 on OvlFunc_921_2009fa4 COSTS 33 here, so that lever is
 *     per-function and not transferable within an overlay.
 *   - `ldr r5,=0xc64` placement, and the head's r1/r2 choice.
 *
 * A _MSG_* TELL THAT MEASUREMENT RETIRED.  The ROM's `ldr r5,=0xc64 ... add r5,#1`
 * looks like the classic message-base shape, but it REPRODUCES FROM PLAIN C:
 * `v5 = 0xc64; f(v5, 3); v5++; g(v5);`.  NO _MSG_c64 IS NEEDED.  Recorded because
 * the shape is one this project has added symbols for before, and here the literal
 * suffices -- measure before reporting a tell.
 *
 * Flag probes, none adopted: -fno-gcse gives 92 against 101 (the only flag that
 * moves at all), -fmove-all-movables 128, -fno-rerun-cse-after-loop 120.
 *
 * No per-file Makefile flag override applies to this stem.
 *
 * NEXT: find the upstream cause of the uniform r2/r3 inversion.  Everything else in
 * the list is plausibly downstream of it.
 */
/* OvlFunc_881_2009ca4 (0x02009ca4) -- NON-MATCHING / PARK.  561 instructions.
 *
 * MEASURED, BOTH NUMBERS, BECAUSE THE POOL HAS MOVED:
 *
 *   python3 tools/objcmp.py scratch_elev/b281/E/OvlFunc_881_2009ca4.c \
 *     asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_c_a.s --func OvlFunc_881_2009ca4
 *   XX SIZE  ref 1488 bytes, ours 1492
 *   XX ENCODINGS differ in 212 place(s) (ref 582, ours 584)
 *      first at index 12: ref 495a  ours 4a5a
 *
 * and 69 REAL differences once `ldr [pc, #N]` offsets are normalised.  The gap
 * between 212 and 69 is entirely the 4-byte pool displacement caused by the TWO
 * extra instructions (584 against 582); the pool WORD COUNT is right -- the
 * reference needs 19 slots across its three segments and we emit 19 -- so this
 * is not batch 280's extra-pool-word cascade, it is two real instructions
 * moving every pc-relative load.  Do not read the 69 as a distance without
 * saying so.
 *
 * RESIDUE, seven classes, all small and all named:
 *   - r2/r3 SCRATCH PERMUTATION at four sites (the `mov rX, r8 / ldrb r0,[rX]`
 *     pair around __Func_8012038 in 2009ca4's sibling shape, the strb address
 *     copy, the `ldr rX,[sp,#0]` reload).  Inverted at every site, which by the
 *     2009fa4 result in this same batch is the signature of a SYMPTOM, not a
 *     cause -- something upstream is still wrong.
 *   - the two extra instructions, both in the __ActorMessage(8, 0) argument
 *     fills where the ROM writes r1 before r0.
 *   - the `mov r2,#0 / mov r3,#2 / mov r1,r8 / strb / str r2 / str r2` group:
 *     the ROM shares ONE zero in r2 with the two word stores; pinning it
 *     (`register int z2 __asm__("r2")`) is WORSE, 82 against 69.
 *   - the while-loop over __Func_8091c7c: the ROM keeps its test at the TOP of
 *     the block and ours rotates it to the bottom.  `for(;;) { if (...) break; }`
 *     is worse (122 against 90 at the time it was tried) -- unlike 2009fa4,
 *     where the same rewrite was worth 27.  Untangling that is the next thing
 *     to try, with the if/else at `i == 6` held fixed.
 *   - `ldr r5, =0xc64` placement (the message base is otherwise CORRECT: the
 *     ROM's `add r5, #1` reproduces from `v5 = 0xc64; f(v5,3); v5++; g(v5);`,
 *     so NO _MSG_c64 SYMBOL IS NEEDED -- a `_MSG_*` tell that measurement
 *     retired).
 *   - the head: which of r1/r2 carries 0xea300000 / 0xfad00000, and where the
 *     second load lands.  Inert against a `register int t1 __asm__("r1")` pin.
 *
 * LEVERS THAT CLOSED THE OTHER ~490 INSTRUCTIONS:
 *
 * 1. A LOOP-INVARIANT CONSTANT ADDEND IS NEVER A MOVABLE, AND A NARROW register
 *    PIN IS THE ONLY WAY TO HOIST IT.  This is the finding of the function and
 *    it is general.  The ROM hoists `mov r7,#0x80 / lsl r7,#4` out of its
 *    frame loop; we rematerialised it INSIDE, every iteration, in all three
 *    loops.  Reading `.08.loop` says why: at loop.c time the value is not a
 *    `set` at all, it is `(plus (reg) (const_int 2048))` -- an immediate operand
 *    of the add -- so scan_loop never builds a movable and move_movables' own
 *    `threshold * savings * lifetime >= insn_count` test is never reached.
 *    Reload then materialises it at each use.  A block-scoped
 *    `register int d7 __asm__("r7")` assigned before the loop forces the
 *    separate hard-register def the ROM has: 101 -> 90 over three loops.
 *    A function-scoped pin on the same register is WORSE (144), because the
 *    ROM reuses r7 for four different roles and a whole-function pin blocks
 *    three of them.
 * 2. `register short`, NOT `register int`, IS WHAT POOLS A HALFWORD ADDEND.
 *    The ROM loads its facing increment with `ldr r7, =0x1000` out of an
 *    EXPLICIT mid-body pool (`.align 2,0 / .L1f6c: .word 0x1000 / .pool`, the
 *    64-byte HImode pool_range signature from batch 280).  `d7 = 0x1000` as an
 *    int gives `mov r7,#0x80 / lsl r7,#5`; declaring the pin `register short`
 *    gives the ROM's pooled load.  90 -> 74 -- and it is the same
 *    *thumb_movhi_insn mechanism as batch 280's pooled zero, seen from the
 *    other side: there the lever was to STOP pooling, here to START.
 * 3. ARGUMENT PINS FOR THE REPEATED POOL CONSTANTS, as ever: 0x16e (three
 *    sites, `q0 = 0xb7; q0 <<= 1`), 0x101 (three), 0x102/0x100 (four).
 *    CONTROL: a pin block that encloses the whole function body is ACTIVELY
 *    WRONG -- it keeps r0 live across the call and forces `mov r5, r0 / cmp r5`
 *    where the ROM has `cmp r0, #0`.  Pin the SITE, not the region.
 * 4. NAMING THE BASE CONSTANTS IS WRONG HERE.  `bx = 0x15d00000; x = .../2 + bx;`
 *    forces the definition above the division; the ROM materialises it in the
 *    MIDDLE (`ldr r2,=0x15d00000 / asr r3,#1 / mov r11,r2 / add r3,r11`), which
 *    is what plain repeated literals plus CSE produce.  Worth 6.
 *
 * FLAG PROBES, all rejected: -fmove-all-movables 128, -fno-rerun-cse-after-loop
 * 120, -fno-gcse 92 (better than the 101 baseline it was measured against, and
 * the only one that moves -- recorded, NOT adopted, because it does not close
 * the function and a per-file flag row is a claim about the original build).
 */
extern unsigned char iwram_3001e40[];

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int n);
extern void __MapActor_DoAnim(int slot, int n);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __Actor_SetColorswap(unsigned char *a, int n);
extern void __GiveDjinni(int a, int b, int c);
extern void __Func_807808c(int a);
extern void __Func_807a458(int a, int b, int c);
extern void __Func_808c44c(void);
extern void __Func_808c4c0(void);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_80936a0(int a, int b);
extern void __Func_8093710(void);
extern void __Func_8096fb0(int a, int b);
extern void __Func_8097194(void);
extern void __Func_80925cc(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8019908(int a, int b);
extern void __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __Func_801776c(int a, int b);
extern void __Func_802899c(int a, int b);
extern void __Func_80aa56c(void);
extern void __Func_80955b0(int a, int b, int c);
extern void OvlFunc_881_200c058(unsigned char *a);
extern void OvlFunc_881_2009c08(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_881_2009ca4(void)
{
    unsigned char *a8;
    unsigned char *p;
    unsigned char *s;
    int x;
    int z;
    int zero;
    int i;
    int d;
    short dh;
    int m;
    int h;

    a8 = __MapActor_GetActor(8);
    p = __MapActor_GetActor(0);
    x = (*(int *)(p + 8) + (int)0xea300000) / 2 + 0x15d00000;
    z = (*(int *)(p + 0x10) + (int)0xfad00000) / 2 + (0xa6 << 19);
    { register int z5 __asm__("r5"); z5 = 0; zero = z5; }
    if (__GetFlag(0xb7 << 1) == 0) {
        __Func_807808c(1);
        { PIN1; q0 = 0xb7; q0 <<= 1; __SetFlag(q0); }
        __CutsceneStart();
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_SetPos(8, *(int *)(p + 8), *(int *)(p + 0x10));
        { PIN3; q1 = 0; q2 = 0; q0 = 0; __GiveDjinni(q0, q1, q2); }
        { PIN3; q1 = 0; q2 = 0; q0 = 0; __Func_807a458(q0, q1, q2); }
        __Func_808c44c();
        __Func_809280c(0, 8, 0);
        __CutsceneWait(0xa);
        { PIN3; q0 = 0; q1 = 0x101; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
        h = 1;
        *(short *)(a8 + 0x66) = h;
        __Func_809280c(8, 0, 0);
        __WaitFrames(0x10);
        __MessageID(0xc4f);
        __ActorMessage(8, 0);
        __Func_808c4c0();
        __Func_80936a0(0x13333, 6);
        __Func_8093710();
        __Func_808c44c();
        s = a8 + 0x55;
        *s = 2;
        *(int *)(a8 + 0x48) = 0x80 << 7;
        *(int *)(a8 + 0x30) = 0x80 << 9;
        *(int *)(a8 + 0x34) = 0x80 << 9;
        *(int *)(a8 + 0x28) = zero;
        *(int *)(a8 + 0x14) = zero;
        __Actor_TravelTo(a8, 0x15d00000, 0, 0xa6 << 19);
        { register int d7 __asm__("r7");
          d7 = 0x80 << 4;
          for (i = 0xf; i >= 0; i--) {
              *(int *)(a8 + 0x18) += d7;
              *(int *)(a8 + 0x1c) += d7;
              __WaitFrames(1);
          } }
        __Func_809280c(8, 0, 0);
        __Func_809280c(0, 8, 0);
        __WaitFrames(0x10);
        *(int *)(a8 + 0x6c) = 0;
        __Actor_SetColorswap(a8, 0);
        *(int *)(a8 + 0x48) = 0x80 << 9;
        __ActorMessage(8, 0);
        __PlaySound(0x83);
        __Func_8096fb0(0x8c, 0);
        for (i = 0x3b; i >= 0; i--) {
            if (*(int *)iwram_3001e40 & 2)
                __Actor_SetColorswap(a8, 7);
            else
                __Actor_SetColorswap(a8, 0);
            if ((*(int *)iwram_3001e40 & 0xf) == 0)
                OvlFunc_881_200c058(a8);
            __WaitFrames(1);
        }
        __Func_8097194();
        __Actor_SetColorswap(a8, 0);
        __Func_80925cc(8, 2);
        __ActorMessage(8, 0);
        { PIN3; q1 = 0x81; q2 = 0x1e; q0 = 0; q1 <<= 1; __MapActor_Emote(q0, q1, q2); }
        __ActorMessage(8, 0);
        { PIN3; q0 = 0; q1 = 0x101; q2 = 0x1e; __MapActor_Emote(q0, q1, q2); }
        __Func_80921c4(8, x >> 16, z >> 16);
        __MapActor_SetAnim(0, 0x16);
        __ActorMessage(8, 0);
        { PIN3; q0 = 0; q1 = 0x101; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
        __MapActor_Jump(8, 4, 0x1e);
        __Func_8019908(0x96 << 1, 4);
        __ActorMessage(8, 0);
        { PIN3; q1 = 0x80; q2 = 0x1e; q0 = 0; q1 <<= 1; __MapActor_Emote(q0, q1, q2); }
        __ActorMessage(8, 0);
        __Func_80925cc(0, 2);
        __ActorMessage(8, 0);
        __MapActor_Jump(8, 2, 0x1e);
        __ActorMessage(8, 0);
        *s = 0;
        __Actor_TravelTo(a8, x, 0x80 << 13, z);
        { register short d7 __asm__("r7");
          d7 = 0x1000;
          for (i = 0xf; i >= 0; i--) {
              *(short *)(a8 + 6) += d7;
              __WaitFrames(1);
          } }
        __MapActor_SetAnim(0, 1);
        __ActorMessage(8, 0);
        *s = 2;
        *(int *)(a8 + 0x28) = 0;
        *(int *)(a8 + 0x14) = 0;
        { register short d7 __asm__("r7");
          d7 = 0x1000;
          for (i = 7; i >= 0; i--) {
              *(short *)(a8 + 6) += d7;
              __WaitFrames(1);
          } }
        __MapActor_SetAnim(0, 0x16);
        __ActorMessage(8, 0);
        { PIN3; q1 = 0x81; q0 = 8; q1 <<= 1; q2 = 0x1e; __MapActor_Emote(q0, q1, q2); }
        __Func_809280c(8, 0, 0);
        __Func_80925cc(8, 2);
        __ActorMessage(8, 0);
        __MapActor_Jump(8, 2, 0x1e);
        __Func_8092c40(8, 0);
        i = 0;
        while (__Func_8091c7c(0, 0) == 1) {
            __MapActor_Jump(8, 2, 0x14);
            __MapActor_Jump(8, 2, 0x14);
            if (i == 6) {
                __MessageID(0xc62);
                __ActorMessage(8, 0);
                goto done;
            }
            __MessageID(0xc5c + i);
            __Func_8092c40(8, 0);
            i++;
        }
        __MapActor_SetAnim(0, 0x16);
        __MapActor_Jump(8, 2, 0x14);
        __MapActor_Jump(8, 4, 0x14);
        __MessageID(0xc63);
        __ActorMessage(8, 0);
    done:
        __Func_8019908(0x96 << 1, 4);
        __PlaySound(0x51);
        m = 0xc64;
        __Func_801776c(m, 3);
        m++;
        __MessageID(m);
        __MapActor_Jump(8, 2, 0x14);
        __ActorMessage(8, 0);
        __PlaySound(9);
        OvlFunc_881_2009c08();
        __Func_808c4c0();
    } else {
        __CutsceneStart();
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_SetPos(8, *(int *)(p + 8), *(int *)(p + 0x10));
        *(int *)(a8 + 0x28) = 0xa0 << 12;
        __Actor_TravelTo(a8, x, 0, z);
        __CutsceneWait(0x1e);
        __Func_808c44c();
        __Func_809280c(8, 0, 0);
        __Func_809280c(0, 8, 0);
        __MapActor_SetAnim(0, 0x16);
        __MessageID(0xc68);
        __MapActor_Jump(8, 2, 0x14);
        __MapActor_Jump(8, 2, 0x14);
        __ActorMessage(8, 0);
        __Func_80925cc(8, 2);
        __ActorMessage(8, 0);
        __PlaySound(0x6f);
        __Func_802899c(0, 2);
        __SetFlag(0x16f);
        __ClearFlag(0x171);
        __Func_80aa56c();
        __MessageID(0xc6a);
        __Actor_TravelTo(a8, 0x15d00000, 0, 0xa6 << 19);
        __CutsceneWait(0x1e);
        __ActorMessage(8, 0);
        __Func_809280c(8, 0, 0);
        __ActorMessage(8, 0);
        __Func_8092c40(8, 0);
        if (__Func_8091c7c(0, 0) == 1) {
            __MapActor_SetAnim(0, 0x16);
            __Func_80925cc(8, 2);
            __MessageID(0xc6d);
            __Func_8092c40(8, 0);
            if (__Func_8091c7c(0, 0) != 1) {
                __ActorMessage(8, 0);
                __Func_80921c4(8, x >> 16, z >> 16);
                OvlFunc_881_2009c08();
                __Func_808c4c0();
                return;
            }
        }
        __MapActor_SetAnim(0, 0x16);
        __MessageID(0xc6f);
        __MapActor_Jump(8, 2, 0x14);
        __MapActor_Jump(8, 2, 0x14);
        __MapActor_DoAnim(0, 3);
        { PIN3; q1 = 0x80; q2 = 0x1e; q0 = 8; q1 <<= 1; __MapActor_Emote(q0, q1, q2); }
        __ActorMessage(8, 0);
        __SetFlag(0x16f);
        __SetFlag(0x171);
        __Func_80aa56c();
        __MapActor_Jump(8, 2, 0x14);
        __ActorMessage(8, 0);
        __Func_808c4c0();
        __Func_80955b0(8, 0, 0);
        __PlaySound(0x2a);
        __CutsceneEnd();
        { PIN1; q0 = 0xb7; q0 <<= 1; __ClearFlag(q0); }
        __ClearFlag(0x16f);
        __ClearFlag(0x171);
    }
}
