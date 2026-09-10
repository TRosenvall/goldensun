// fakematch
/* ovl_314_c_c_a_a_c_c_c.c  --  OvlFunc_926_2008658 + OvlFunc_926_200871c
 *   [the WHOLE of asm/overlays/rom_7b2078/ovl_314_c_c_a_a_c_c_c.s -- both
 *    `.thumb_func_start`s, so NO SPLIT is required and
 *    overlays/rom_7b2078/overlay.ld:31 stays VERBATIM]
 *
 *   OK WHOLE OBJECT -- 1124 bytes, 424 encodings and 126 relocations identical
 *   (per function, objcmp --func: 2008658 196 bytes / 75 encodings / 21 relocs;
 *    200871c 928 bytes / 349 encodings / 105 relocs.  Each measured 3x.)
 *
 * Two straight-line cutscene scripts, 73 + 337 instructions, over 32 shared
 * callees.  FAKEMATCH: matched with the register-pin idiom, so both names go
 * in fakematch.txt.
 *
 * THE SELECTION LINE SAID `hi=0 hiv=0` AND THAT WAS RIGHT TO READ AS WORK, but
 * the work here was NOT high-register eviction: the ROM's own push mask
 * (`push {lr}` / `push {r5, lr}`) came out of the bare transcription unchanged,
 * and no pin was ever needed to shed r8-r11.  What the 13 surviving pins buy is
 * ARGUMENT FILL ORDER, and one of them (the only PIN2) is a genuine eviction.
 *
 * ASCENDING FILL IS RIGHT AND THE ROM'S OWN EMITTED ORDER IS WRONG.  This is
 * the sharpest negative in the file and it inverts the obvious move.  SEVEN of
 * the nine pinned blocks in the second function build a shifted constant with
 * `mov rX, #k / lsl rX, #n`, and at each of them the ROM interleaves another
 * argument between the two halves:
 *
 *     rom    mov r2, #0xfc / mov r1, #0xa8 / lsl r2, #1 / mov r0, #0
 *     bare   mov r2, #0xfc / lsl r2, #1 / mov r1, #0xa8 / mov r0, #0
 *
 * Transcribing the ROM's register birth order into the pin (`q2 = ...; q1 =
 * ...; q0 = ...;`) is INERT at all SEVEN -- 15 differing, and the seven sites'
 * instruction streams are byte-for-byte what the BARE call emitted.  Plain
 * ASCENDING (`q0; q1; q2;`) is exact at all seven.  The other two blocks reach
 * their constants through the literal pool rather than a shift
 * (`__MapActor_Emote(8, 0x105, 0x3c)` and `__MapActor_SetSpeed(8, 0x4ccc,
 * 0x2666)`), and those two are exact under ROM-order AND under ascending --
 * measured both ways, equal.  So the file is written uniformly ascending.
 *
 * The pin is not placing the instructions; it is fixing the pre-sched2 order
 * that sched2 then rearranges INTO the ROM's interleave.  This confirms and
 * sharpens the sibling ovl_30_c_c_c_c_a_a_a_c_c_c_a.c note that ROM-order
 * transcription "measured EXACTLY EQUAL to the plain ascending fill": that
 * holds for POOLED operands, but at a SHIFT-BUILT operand ROM-order is equal
 * to NO PIN AT ALL, i.e. strictly worse than ascending.
 *
 * MECHANISM CONTROL: the finished file compiled -fno-schedule-insns2 is 100
 * differing.  sched2 is doing the ROM's work and the pins only feed it.
 *
 * WHAT THE PIN SET IS WORTH: all 13 pins removed (every call written plainly)
 * is 41 differing WITH RELOCATIONS DIFFERING -- the recorded "14+ and relocs
 * differ is CSE" band, which is the one PIN2 below.
 *
 * THE ONE EVICTION PIN.  0x10000 (`0x80 << 9`) is passed to __Func_8091220 and
 * then, six calls later, to __Func_8091200.  gcc commons it into r5 -- which is
 * ALREADY in the push mask, so there is no added push to warn you -- and reads
 * it back with `adds r0, r5, #0` where the ROM rebuilds `mov r0, #0x80 / lsl
 * r0, #9`.  Pinning only the FIRST of the two sites is enough; the pin on the
 * second is INERT and was dropped by minimisation.  A useful shape: the
 * eviction pin goes on the DEFINITION site, not on every use.
 *
 * TWO NAMED int LOCALS FOR THE TWO STACK ARGUMENTS.  __Func_8010704 takes six
 * arguments; the ROM sets the two stack slots from TWO registers,
 * `mov r3, #0xa / mov r2, #0x18 / str r3, [sp] / str r2, [sp, #4]`, and a bare
 * `__Func_8010704(0xa, 0x1a, 1, 1, 0xa, 0x18)` reuses r3 for both and issues
 * `mov / str / mov / str`.  Writing `int s0 = 0xa; int s1 = 0x18;` in a block
 * around the call gives the two values overlapping live ranges, the allocator
 * hands out r3 and r2, and sched2 produces the ROM's pairing -- worth 3 of the
 * function's 9.  cprop does NOT fold these two locals back into the stores.
 * A `register`-pinned pair (r3, r2) is EXACTLY EQUAL; the plain locals are
 * preferred because they cost no extra pin.
 *
 * MEASURED WORSE, worth recording: `do { } while (0)` in front of the three
 * fill-order sites in the first function instead of the pins is 8 differing
 * against the pins' 0 and the bare call's 6 -- the ordering barrier is not a
 * substitute for the fill-order pin, and here it actively costs 2.
 *
 * A CSE-SUBSTITUTED ZERO IS NOT A SOURCE-LEVEL DIFFERENCE.  In the flag==0 arm
 * the ROM writes the actor byte with `strb r5, [r0]` (r5 = the __GetFlag
 * result, known 0 on that edge) where the flag!=0 arm writes `mov r3, #0 /
 * strb r3, [r0]`.  Both arms are plain `= 0` in the source; cse.c's
 * record_jump_equiv puts r5 in the constant's class on the taken edge and
 * picks the register.  No lever needed -- this fell out of the bare
 * transcription.
 *
 * PIN MINIMISATION ran to a fixpoint FROM BOTH ENDS and the two directions
 * produce BYTE-IDENTICAL sources: 13 of 14 required, the single drop being the
 * second 0x10000 eviction pin described above.
 *
 * Harness: scratch_elev/b256/zero/ -- dump.py (side-by-side disassembly),
 * minimise.py (drop-one fixpoint), ctrl.py (mechanism controls), a*.c/b*.c the
 * per-function chains, m*.c the merged TU.
 */
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8093054(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int a);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Surprise(int slot, int a);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern int __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_8093500(int a, int b);
extern void __Func_8093530(void);
extern void __Func_8091200(int a, int b);
extern void __Func_8091220(int a, int b);
extern void __Func_8091254(int a);
extern void __WaitMapTransition(void);
extern void OvlFunc_926_200c0dc(int a, int b);
extern void OvlFunc_926_200c128(void);
extern void OvlFunc_926_200c140(void);
extern unsigned int iwram_3001ebc;

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_926_2008658(void)
{
    __CutsceneStart();
    __SetFlag(0x894);
    __Func_809280c(0x9, 0x0, 0x0);
    __CutsceneWait(0xa);
    __MessageID(0x17b7);
    __Func_80925cc(0x9, 0x2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x0; q1 = 0x80 << 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __Func_8093054(0x9, 0x0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x9; q1 = 0x80 << 1; q2 = 0x50; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0xd0 << 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x9, 0x2);
    __CutsceneWait(0x14);
    __Func_8093040(0x9, 0x0, 0x14);
    __Func_8092adc(0x9, 0x0, 0x14);
    __MapActor_DoAnim(0x9, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x9, 0x0, 0x14);
    { int s0 = 0xa; int s1 = 0x18;
      __Func_8010704(0xa, 0x1a, 0x1, 0x1, s0, s1); }
    __CutsceneEnd();
}

void OvlFunc_926_200871c(void)
{
    int f;

    __CutsceneStart();
    f = __GetFlag(0xc0 << 2);
    if (f != 0) {
        { PIN3; q0 = 0x0; q1 = 0xa8; q2 = 0xfc << 1; __Func_80921c4(q0, q1, q2); }
        __CutsceneWait(0x5);
        __Func_8092adc(0x0, 0xc0 << 8, 0x14);
        __MapActor_GetActor(0x8)[0x5b] = 0;
        __PlaySound(0x98);
        *(int *)(__MapActor_GetActor(0x8) + 0x28) = 0x80 << 12;
        __MapActor_SetAnim(0x8, 0x1);
        __CutsceneWait(0x1e);
        __MessageID(0x17ac);
    } else {
        __MessageID(0x179f);
        OvlFunc_926_200c0dc(0x0, 0x8);
        __CutsceneWait(0x1e);
        __ActorMessage(0x8, 0x0);
        OvlFunc_926_200c128();
        __CutsceneWait(0x14);
        { PIN3; q0 = 0x0; q1 = 0xa8; q2 = 0xfc << 1; __Func_80921c4(q0, q1, q2); }
        __CutsceneWait(0x5);
        __Func_8092adc(0x0, 0xc0 << 8, 0x14);
        __PlaySound(0x98);
        __MapActor_GetActor(0x8)[0x5b] = 0;
        *(int *)(__MapActor_GetActor(0x8) + 0x28) = 0x80 << 12;
        __MapActor_SetAnim(0x8, 0x1);
        __CutsceneWait(0x1e);
        __Func_8092c40(0x8, 0x0);
        if (__Func_8091c7c(0x0, 0x0) == 1) {
            __Func_80925cc(0x8, 0x2);
            __CutsceneWait(0x14);
            __ActorMessage(0x8, 0x0);
            __CutsceneWait(0x14);
            OvlFunc_926_200c0dc(0x8, 0x0);
            __CutsceneWait(0x1e);
            __Func_80925cc(0x0, 0x2);
            __CutsceneWait(0x32);
            OvlFunc_926_200c128();
            __CutsceneWait(0x1e);
            __MapActor_DoAnim(0x8, 0x3);
            __ActorMessage(0x8, 0x0);
        } else {
            *(short *)((unsigned char *)iwram_3001ebc + 0x1d8) += 2;
            __ActorMessage(0x8, 0x0);
        }
        __MapActor_DoAnim(0x8, 0x3);
        __CutsceneWait(0x1e);
        __MapActor_Emote(0x8, 0x80 << 1, 0x3c);
        __MessageID(0x17a4);
        __Func_8092c40(0x8, 0x0);
        if (__Func_8091c7c(0x0, 0x0) == 1) {
            { PIN3; q0 = 0x8; q1 = 0x105; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
            OvlFunc_926_200c0dc(0x8, 0x0);
            __CutsceneWait(0x1e);
            __Func_80925cc(0x0, 0x2);
            __CutsceneWait(0x32);
            OvlFunc_926_200c128();
            __CutsceneWait(0x1e);
            __ActorMessage(0x8, 0x0);
            *(short *)((unsigned char *)iwram_3001ebc + 0x1d8) += 1;
        } else {
            *(short *)((unsigned char *)iwram_3001ebc + 0x1d8) += 1;
            __CutsceneWait(0x14);
            __MapActor_DoAnim(0x8, 0x3);
            __CutsceneWait(0x14);
            __ActorMessage(0x8, 0x0);
        }
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0x8, 0x4);
        __CutsceneWait(0x14);
        __ActorMessage(0x8, 0x0);
        __Func_80925cc(0x0, 0x2);
        __CutsceneWait(0x14);
        __ActorMessage(0x8, 0x0);
        __CutsceneWait(0x14);
        { PIN3; q0 = 0x8; q1 = 0xc0 << 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 0x8; q1 = 0x4ccc; q2 = 0x2666; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 0x8; q1 = 0xa8; q2 = 0xe8 << 1; __Func_80921c4(q0, q1, q2); }
        __CutsceneWait(0x3c);
        { PIN3; q0 = 0x8; q1 = 0x80 << 7; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
        __Func_8093040(0x8, 0x0, 0xa);
        { PIN3; q0 = 0x0; q1 = 0x81 << 1; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 0x8; q1 = 0xa8; q2 = 0xec << 1; __Func_80921c4(q0, q1, q2); }
    }
    __Func_8092c40(0x8, 0x0);
    if (__Func_8091c7c(0x0, 0x0) == 1) {
        __MessageID(0x17ab);
        __ActorMessage(0x8, 0x0);
        __SetFlag(0xc0 << 2);
    } else {
        __MessageID(0x17ad);
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(0x8, 0x3);
        __CutsceneWait(0x14);
        __Func_80933d4(0x80 << 8, 0x80 << 5);
        __Func_8093500(0x8, 0x1);
        { PIN2; q0 = 0x80 << 9; q1 = 0x0; __Func_8091220(q0, q1); }
        __Func_8091200(0x10003, 0x1);
        __Func_8091254(0x1e);
        __WaitMapTransition();
        __Func_8093530();
        OvlFunc_926_200c140();
        __Func_8091200(0x80 << 9, 0x0);
        __Func_8091254(0x1e);
        __ActorMessage(0x8, 0x0);
        __Func_80925cc(0x8, 0x2);
        __CutsceneWait(0x14);
        __ActorMessage(0x8, 0x0);
        __Func_809259c(0x0, 0x1);
        __MapActor_Surprise(0x0, 0x81 << 1);
        __CutsceneWait(0x3c);
        __Func_8093040(0x8, 0x0, 0xa);
        __SetFlag(0x891);
    }
    __MapActor_SetAnim(0x8, 0x5);
    __CutsceneEnd();
}
