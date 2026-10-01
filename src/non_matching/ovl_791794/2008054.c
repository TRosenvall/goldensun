/* OvlFunc_897_2008054  --  0x02008054  --  NON-MATCHING, 320 encodings of 1378
 *
 * *** CLAIM CORRECTED ON INSTALL: 1058 -> 320. *** parkcheck re-measures this line with objcmp
 * at production flags and read 320; the 1058 was an intermediate candidate's figure left in the
 * claim position.  The body here is the 320 candidate -- size exact-minus-4, COUNT EXACT
 * (1378/1378), 96.4% aligned in 37 hunks, with all 340 relocations carrying the same
 * (type, symbol) pairs in the same order and only their POSITIONS differing, first at entry 256.
 * Fifth occurrence in this tree of a claim line disagreeing with its own body, so it is worth
 * repeating: the claim line is objcmp's figure at PRODUCTION FLAGS, for THIS body, and nothing
 * else belongs in that position
 * (320 differ).  SIZE EXACT and COUNT EXACT, which is what the ranking axis
 * asks for and what makes this a near-closed function rather than a parked one.
 *
 * objcmp.py under the PRODUCTION flags:
 *   ENCODINGS differ in 320 place(s) (ref 1378, ours 1378)
 *   first at index 83: ref 22e4  ours 24e4
 *   RELOCATIONS differ
 * SIZE: 3548 against 3548 -- EXACT (objcmp prints no SIZE row).
 * COUNT: 1378 against 1378 -- EXACT.
 * RELOCATIONS: 340 against 340, and the DIFFERENCE IS POSITIONAL ONLY.  The
 * two lists carry the same 340 (type, symbol) pairs in the same order with no
 * entry present on one side and absent on the other; the first offset that
 * differs is entry 256, 0x00000a26 against 0x00000a24, a two-byte shift.  So
 * the relocation row is a CASCADE of the one real size-neutral defect below,
 * not a symbol-set defect.  It is reported here unfiltered because filtering it
 * out would manufacture a false claim.
 * aligncmp.py, separately: ref 1378 encodings, ours 1378, aligned-equal 1329
 * (96.4% of ref), differing/ins/del 53 in 37 hunks.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_791794/2008054.c \
 *     asm/overlays/rom_791794/ovl_30_a_c_c_a.s --func OvlFunc_897_2008054
 *
 * LANDING IS THE EASIEST SHAPE THERE IS: NO SPLIT AT ALL.
 * asm/overlays/rom_791794/ovl_30_a_c_c_a.s holds EXACTLY ONE function and no
 * `.section .data`, no `.incbin`, no `.global`; datacheck.py is SILENT on it.
 * Note the standing caveat that silence is not proof of a single linker row --
 * 110 objects in this tree carry a `.rodata` linker line with no data section
 * -- but here it checks out: ONE row names the object,
 * overlays/rom_791794/overlay.ld:20, and it is `(.text)`.  split_s.py is not
 * needed; this is a whole-file conversion.  Confirm with --whole before
 * landing, since two green --func runs say nothing about the shipped TU.
 *
 * shimcount.py: 461 register pins, and it flags `has a fakematch-class shim and
 * NO fakematch.txt row` -- a fakematch.txt row is part of landing.
 *
 * THE THREE EDITS THAT GOT IT TO SIZE-AND-COUNT EXACT, IN ORDER OF VALUE.
 *
 *   1. THE SYMBOL ADDRESS IS A THIRD LONG-LIVED QUANTITY AND IT IS WORTH THE
 *      WHOLE PROLOGUE.  The ROM is `push {r5, r6, r7, lr}` -- THREE callee-saved
 *      registers: r5 holds `&iwram_3001ec4`, r7 the pointer loaded from it, r6
 *      the derived `base + 0x40c`.  r5 exists only so that a later
 *      `sub r5, #8` can reach 0x3001ebc, eight bytes below.  Reading the two
 *      WRAM cells as two separate externs gives `push {r5, r6, lr}` and +28/+9:
 *      two unrelated SYMBOL_REFs give the allocator nothing to keep.  Declaring
 *      `unsigned char **ptr; ptr = &iwram_3001ec4; base = *ptr;` and spelling
 *      the second cell `ptr[-2]` puts the ADDRESS in a variable, reproduces the
 *      `sub` exactly, pools only `iwram_3001ec4` -- so the relocation SYMBOL is
 *      the ROM's, not a symbol-plus-addend -- and moves the first differing
 *      encoding off index 0.
 *
 *   2. THE POINTER LOAD HAPPENS BEFORE THE FIRST CALL.  `ptr`/`base` are read
 *      ahead of `__CutsceneStart()`, matching the ROM's two `ldr` at the top of
 *      the body.  Placing them after the call costs the prologue.
 *
 *   3. A BARE CALL WHOSE RESULT THE NEXT STATEMENT CONSUMES IS AN EXTRA CALL.
 *      The transcription emitted both `__MapActor_GetActor(0xf);` and
 *      `a = __MapActor_GetActor(0xf);` at the four sites where the ROM's result
 *      register feeds the following instruction.  With edits 1 and 2 in place
 *      the four stray calls stood at +24 size / +8 count and 1103 differing;
 *      deleting them took that to size EXACT, count EXACT and 326 differing
 *      in one edit.
 *
 *   4. `__Func_8092c40` AND `__StartTask` WANT DESCENDING FILLS.  The single
 *      __Func_8092c40 site is again the one whose call is followed by the
 *      `__Func_8091c7c` test and again wants `q1 = 0; q0 = 0x1001;`; both
 *      __StartTask sites want the shifted count before the function pointer.
 *      Four encodings, 324 -> 320.
 *
 * WHAT IS LEFT IS FIVE REGISTER-CHOICE SITES, NOT A WRONG PROGRAM.  Reading the
 * immediates in every differing hunk: no immediate, no symbol and no call
 * differs anywhere.  The five are
 *   a. the `(0xe4 << 1)` offset at the `ptr[-2]` store -- ROM r2, ours r4;
 *   b. the second `base + 0x40c` re-derive -- ROM `ldr r3 / add r5, r7, r3`
 *      then `mov r3, #0`, ours the same three with r2 and the `mov` early;
 *   c. the `__StartTask` `lsl` sitting after the pool load instead of before;
 *   d. the `__MapActor_GetActor(5)` `ldrsh` pair -- ROM pointer in r2 with the
 *      0xa/0x12 offsets in r3, ours r3 with offsets in r4;
 *   e. the `+ 0x5a` address, where the ROM spends an extra copy
 *      (`mov r2, r0 / mov r5, r2 / add r5, #0x5a`) that ours coalesces away.
 * (e) is the one that shifts the relocation offsets by two from entry 256, and
 * it is the one to attack next.
 *
 * MEASURED AND REJECTED, so nobody re-spends it:
 *   - a SECOND local for the second `base + 0x40c` range: 326 -> 324, KEPT.
 *   - a per-site local for the `0xb` GetActor result (`c` instead of reusing
 *     `a`): -4 size / -2 count, 1100 differing.  REJECTED, and note it looks
 *     locally right while being globally much worse -- a figure improving for
 *     the wrong reason, in reverse.
 *   - `n = (int)__MapActor_GetActor(0xb); q = (unsigned char *)(n + 0x5a);`
 *     -4/-2, 1101.  REJECTED.
 *   - `q = __MapActor_GetActor(0xb); q = q + 0x5a;`  -4/-2, 1101.  REJECTED.
 *   - `q = &a[0x5a]` instead of `q = a + 0x5a`: BYTE-IDENTICAL to the kept
 *     spelling.  Inert.
 *   - `off = 0xe4 << 1;` as a named local at the `ptr[-2]` store: -4/-2, 1245.
 *     REJECTED, and it is the documented both-directions case -- an int temp
 *     STOPPING a pool, exactly as the lever's caveat warns.
 *   - `0x1c8` written out instead of `(0xe4 << 1)`: BYTE-IDENTICAL.  Confirms
 *     again that constant SPELLING is not a lever; both are one CONST_INT.
 * Every one of the three -4/-2 results is the same two encodings, and all three
 * come from adding an `int` local.  THE DECLARATION OF AN EXTRA INT SCALAR IS
 * ITSELF WORTH -2 ENCODINGS HERE; that is the trap to avoid in this function.
 *
 * CONTROL FLOW AND THE TWO FALSE BRANCHES.  Four branch targets, and only ONE
 * is control flow: `__Func_8091c7c(0, 0)` with `bne .L418`, i.e.
 * `if (... == 0)`, selecting between two `__MessageID` constants and joining at
 * .L41e.  `.L860` and `.Ld38` are POOL SKIPS -- each is preceded by
 * `.pool_aligned` and the `b` jumps over the pool -- and reading them as
 * control flow was worth four wrong argument fills, because register state
 * carries straight across a pool skip (the `mov r1, #2` before `b .L860`
 * supplies the second argument of the `__Func_80925cc` AFTER it).  Three- and
 * four-character `.L` labels here all contain a hex letter, so none of them
 * can collide with a gcc label; no `.L` extern is needed in this function at
 * all, unlike its 889 sibling.
 */
extern void __PlaySound(int id);
extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __SetFlag(int id);
extern void __StartTask(int fn, int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __ActorMessage(int a, int b);
extern void __Actor_SetSpriteFlags(int a, int b);
extern void __Func_800fe9c(void);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void __Func_8091890(int a);
extern int __Func_8091c7c(int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_897_2008e30(int a);
extern void OvlFunc_897_2008f64(void);
extern void OvlFunc_897_2009084(void);
extern void OvlFunc_897_2009410(void);
extern void OvlFunc_897_200ad48(int a, int b, int c);
extern void OvlFunc_897_20090c4(void);
extern void OvlFunc_897_200935c(void);

extern unsigned char *iwram_3001ebc;
extern unsigned char *iwram_3001ec4;

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_897_2008054(void)
{
    unsigned char **ptr;
    unsigned char *base;
    unsigned char *a;
    unsigned char *q;
    short *b;
    int *g;
    int *g2;

    ptr = &iwram_3001ec4;
    base = *ptr;
    __CutsceneStart();
    g = (int *)(base + 0x40c);
    *g = 0;
    __PlaySound(0x8d);
    { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 9; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xe8 << 16; q2 = 0x9c << 16;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xda << 16; q2 = 0xac << 16;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xd0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0x1db0000; q2 = 0xa6 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0x1eb0000; q2 = 0xa6 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x1cb0000; q2 = 0xae << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x1fb0000; q2 = 0xae << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x1d70000; q2 = 0x99 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x1df0000; q2 = 0xb5 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    __Func_8092b08(0xf, 1);
    { PIN4; q0 = 0xe8 << 16; q1 = -1; q2 = 0x9c << 16; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    *(int *)(ptr[-2] + (0xe4 << 1)) = 8;
    __MapTransitionIn();
    __WaitMapTransition();
    { PIN2; q0 = 0x7fff; q1 = 0;
      __Func_8091200(q0, q1); }
    __Func_8091254(4);
    __CutsceneWait(4);
    { PIN2; q0 = 0x80 << 9; q1 = 0;
      __Func_8091200(q0, q1); }
    __Func_8091254(4);
    __CutsceneWait(4);
    OvlFunc_897_2008f64();
    { PIN2; q0 = 0x7fff; q1 = 0;
      __Func_8091200(q0, q1); }
    __Func_8091254(4);
    __CutsceneWait(0x10);
    __PlaySound(0x90);
    { PIN2; q0 = 0x80 << 9; q1 = 0;
      __Func_8091200(q0, q1); }
    __Func_8091254(4);
    __CutsceneWait(4);
    { PIN2; q0 = 0x7fff; q1 = 0;
      __Func_8091200(q0, q1); }
    __Func_8091254(4);
    __CutsceneWait(4);
    __PlaySound(0x90);
    { PIN2; q0 = 0x80 << 9; q1 = 0;
      __Func_8091200(q0, q1); }
    __Func_8091254(0x30);
    __CutsceneWait(0x30);
    __MapActor_Jump(0, 6, 0);
    __MapActor_Jump(1, 6, 0x14);
    OvlFunc_897_200ad48(1, 0x14, 0x14);
    OvlFunc_897_200ad48(0, 0x14, 0x28);
    { PIN1; q0 = 0x10cd;
      __MessageID(q0); }
    __Func_8093040(0xb, 0, 0x14);
    __ActorMessage(0xa, 0);
    OvlFunc_897_200ad48(1, 0x14, 0);
    __Func_8093040(5, 0, 0x14);
    OvlFunc_897_200ad48(0, 0x14, 0);
    __Func_8093040(0xe, 0, 0x14);
    __Func_8093040(9, 0, 0x14);
    __Func_8092848(0, 1, 0);
    __CutsceneWait(0x28);
    __Func_809259c(0, 2);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xd0 << 8; q2 = 0x1e;
      __Func_8092adc(q0, q1, q2); }
    OvlFunc_897_200ad48(1, 0x14, 0);
    __Func_809259c(0, 2);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    OvlFunc_897_200ad48(0, 0x14, 0x14);
    __Actor_SetSpriteFlags((int)__MapActor_GetActor(0xf), 0);
    { PIN3; q0 = 0xf; q1 = 0x1450000; q2 = 0x97 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    a = __MapActor_GetActor(0xf);
    a[0x55] = 5;
    *g = 1;
    { PIN1; q0 = 0x121;
      __PlaySound(q0); }
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __Func_8012350();
    __CutsceneWait(0x96);
    __Func_8093040(0xb, 0, 0x14);
    __ActorMessage(5, 0);
    __Func_8093040(0xa, 0, 0xa);
    __Func_8092848(0, 1, 0x14);
    { PIN3; q0 = 0; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 6; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0x1db0000; q2 = 0xa6 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0x1eb0000; q2 = 0xa6 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x1cb0000; q2 = 0xae << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x1fb0000; q2 = 0xae << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x1d70000; q2 = 0x99 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x1df0000; q2 = 0xb5 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN2; q0 = 0x66666; q1 = 0xcccc;
      __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0xa4 << 17; q1 = -1; q2 = 0x12b0000; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __PlaySound(0xa7);
    { PIN2; q0 = 0x205294; q1 = 2;
      __Func_8091200(q0, q1); }
    __Func_8091254(0x14);
    __WaitFrames(0x14);
    { PIN2; q0 = 0x80 << 9; q1 = 2;
      __Func_8091200(q0, q1); }
    __Func_8091254(0x14);
    __CutsceneWait(0xc8);
    { PIN2; q1 = 0; q0 = 0x1001;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
    { PIN1; q0 = 0x10d6;
      __MessageID(q0); }
    } else {
    { PIN1; q0 = 0x10d7;
      __MessageID(q0); }
    }
    { PIN3; q0 = 0x1001; q1 = 0; q2 = 0x50;
      __Func_8093040(q0, q1, q2); }
    { PIN1; q0 = 0x10d8;
      __MessageID(q0); }
    __Func_8093040(9, 0, 0x14);
    OvlFunc_897_200ad48(1, 0x14, 0);
    g2 = (int *)(base + 0x40c);
    *g2 = 0;
    __PlaySound(0x8d);
    { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x50);
    *g2 = 1;
    { PIN1; q0 = 0x121;
      __PlaySound(q0); }
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __Func_8012350();
    OvlFunc_897_200ad48(0, 0x14, 0x3c);
    __Func_8093040(0xe, 0, 0x1e);
    { PIN3; q0 = 0xf; q1 = 0xa0 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    OvlFunc_897_200ad48(1, 0x14, 0x14);
    OvlFunc_897_200ad48(0, 0x14, 0x14);
    { PIN3; q0 = 0x1001; q1 = 0; q2 = 0x1e;
      __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 0xf; q1 = 0x80 << 5; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    OvlFunc_897_200ad48(1, 0x14, 0x14);
    OvlFunc_897_200ad48(0, 0x14, 0x14);
    __Func_8093040(5, 0, 0x1e);
    OvlFunc_897_2009084();
    { PIN2; q1 = 0xc8 << 4; q0 = (int)OvlFunc_897_20090c4;
      __StartTask(q0, q1); }
    { PIN2; q1 = 0xc8 << 4; q0 = (int)OvlFunc_897_200935c;
      __StartTask(q0, q1); }
    __CutsceneWait(0xf0);
    __Func_8093040(0xa, 0, 0x1e);
    { PIN3; q0 = 5; q1 = 0x1db0000; q2 = 0xa6 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0x1eb0000; q2 = 0xa6 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x1cb0000; q2 = 0xae << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x1fb0000; q2 = 0xae << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x1d70000; q2 = 0x99 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x1df0000; q2 = 0xb5 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    b = (short *)__MapActor_GetActor(5);
    { PIN4; q0 = b[5] << 16; q1 = -1; q2 = b[9] << 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x28);
    __Func_80925cc(0xd, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xd, 0);
    __Func_80925cc(0xb, 2);
    __Func_8093040(0xb, 0, 0xa);
    __MapActor_SetAnim(0xe, 4);
    __Func_8093040(0xe, 0, 0x14);
    __Func_8092848(0xa, 0xb, 0x28);
    __MapActor_SetAnim(0xa, 4);
    __Func_8093040(0xa, 0, 0xa);
    { PIN3; q0 = 5; q1 = 0x80 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0xc0 << 6; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x4005; q1 = 0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xd0 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092848(0xa, 0xb, 0x28);
    { PIN3; q0 = 0xa; q1 = 0x105; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xb0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0xa, 0, 0xa);
    { PIN3; q0 = 0xa; q1 = 0x80 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0xb, 2);
    __Func_8093040(0xb, 0, 0xa);
    __MapActor_SetAnim(0xa, 3);
    __Func_8093040(0xa, 0, 0x28);
    { PIN3; q0 = 0xe; q1 = 0xb0 << 8; q2 = 0x3c;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xe, 3);
    __Func_8093040(0xe, 0, 0xa);
    { PIN3; q0 = 0xd; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0xa0 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xa0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0xd0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xe, 4);
    __Func_8093040(0xe, 0, 0xa);
    { PIN3; q0 = 0xd; q1 = 0x103; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x1d7; q2 = 0x9d << 1;
      __Func_80921c4(q0, q1, q2); }
    __Func_809259c(0xd, 3);
    __Func_8093040(0xd, 0, 0xa);
    { PIN3; q0 = 0xe; q1 = 0xb0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0xe, 4);
    __Func_8093040(0xe, 0, 0xa);
    __Func_809259c(0xd, 3);
    __Func_8093040(0xd, 0, 0xa);
    __Func_80925cc(9, 2);
    { PIN3; q0 = 0x4009; q1 = 0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xb0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0xa, 0, 0x1e);
    __Func_8092adc(0xb, 0, 0x14);
    __Func_80925cc(0xb, 2);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0xb, 3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0xb; q1 = 0xd0 << 8; q2 = 0x1e;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xb, 4);
    __CutsceneWait(0xa);
    __Func_8093040(0xb, 0, 0xa);
    { PIN3; q0 = 0xd; q1 = 0xa0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0xd, 2);
    __CutsceneWait(0x14);
    __Func_8092848(0xa, 0xb, 0);
    __CutsceneWait(0x1e);
    __MapActor_SetAnim(0xa, 3);
    __MapActor_DoAnim(0xb, 3);
    { PIN3; q0 = 0xb; q1 = 0xd0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x28);
    __Func_80925cc(0xd, 1);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0xd; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x28);
    { PIN3; q0 = 0xd; q1 = 0x81 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0xe; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0xe, 4);
    { PIN3; q0 = 0x200e; q1 = 0; q2 = 0x1e;
      __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0xc0 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x2005; q1 = 0; q2 = 0x28;
      __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_DoAnim(0xd, 3);
    { PIN3; q0 = 0xa; q1 = 0x101; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(0xe, 2);
    __Func_8093040(0xe, 0, 0x3c);
    __MapActor_DoAnim(0xd, 3);
    __CutsceneWait(0x28);
    { PIN3; q0 = 5; q1 = 0x81 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8092848(0xa, 0xb, 0x14);
    __MapActor_SetAnim(0xa, 3);
    __MapActor_DoAnim(0xb, 3);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xa; q1 = 0x9999; q2 = 0x4ccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x9999; q2 = 0x4ccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x1db; q2 = 0xae << 1;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x1eb; q2 = 0xae << 1;
      __Func_809218c(q0, q1, q2); }
    __MapActor_WaitMovement(0xb);
    __MapActor_WaitMovement(0xa);
    __MapActor_SetAnim(0xb, 1);
    __MapActor_SetAnim(0xa, 1);
    { PIN3; q0 = 0xb; q1 = 0xd0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0xc0 << 6; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x103; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(0xb, 2);
    { PIN2; q0 = 0x200b; q1 = 0;
      __ActorMessage(q0, q1); }
    { PIN3; q0 = 0xb; q1 = 0x19999; q2 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x1db; q2 = 0xa9 << 1;
      __Func_80921c4(q0, q1, q2); }
    a = __MapActor_GetActor(0xb);
    q = a + 0x5a;
    *q = 0xfe & *q;
    { PIN3; q0 = 0xb; q1 = 0x1db; q2 = 0xae << 1;
      __Func_80921c4(q0, q1, q2); }
    __Func_80925cc(5, 1);
    __MapActor_Jump(5, 4, 0);
    { PIN3; q0 = 5; q1 = 0x1cb; q2 = 0x9e << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0x80 << 8; q2 = 0x80 << 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x103; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    __Func_809259c(0xd, 3);
    __Func_8093040(0xd, 0, 0x1e);
    __Func_80925cc(0xb, 1);
    __CutsceneWait(0x14);
    *q = 1 | *q;
    { PIN3; q0 = 0xb; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x1db; q2 = 0xa6 << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xb0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 0x200b; q1 = 0;
      __ActorMessage(q0, q1); }
    { PIN3; q0 = 0xa; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_809259c(0xa, 2);
    __Func_8093040(0xa, 0, 0x14);
    { PIN3; q0 = 0xb; q1 = 0x81 << 1; q2 = 0x14;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xc0 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xa, 4);
    __MapActor_DoAnim(0xb, 3);
    { PIN3; q0 = 0xb; q1 = 0xb0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xb, 3);
    __MapActor_DoAnim(0xd, 3);
    __CutsceneWait(0x14);
    { PIN3; q0 = 5; q1 = 0xd8 << 1; q2 = 0x9e << 1;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0xd3 << 1; q2 = 0x137;
      __Func_80921c4(q0, q1, q2); }
    __MapActor_SetAnim(5, 1);
    { PIN3; q0 = 5; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0xc0 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0xa, 2);
    { PIN3; q0 = 0xa; q1 = 0xd0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x100a; q1 = 0; q2 = 0x28;
      __Func_8093040(q0, q1, q2); }
    __Func_80925cc(9, 2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 9; q1 = 0xb0 << 8; q2 = 0x1e;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0x80 << 8; q2 = 0x80 << 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0x1eb; q2 = 0x94 << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0xa0 << 7; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x1d7; q2 = 0x9a << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xa0 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x1c7; q2 = 0x9a << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x1e7; q2 = 0x9a << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xd0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0xd0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0xd0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 0x80 << 8; q1 = 0x80 << 5;
      __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0xec << 17; q1 = -1; q2 = 0x96 << 17; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x28);
    { PIN1; q0 = 0x246;
      __SetFlag(q0); }
    { PIN3; q0 = 0xa; q1 = 0x80 << 8; q2 = 0x80 << 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x80 << 8; q2 = 0x80 << 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x80 << 8; q2 = 0x80 << 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_DoAnim(0xa, 3);
    OvlFunc_897_2008e30(0xa);
    __Func_80925cc(9, 2);
    OvlFunc_897_2008e30(9);
    __MapActor_DoAnim(0xb, 3);
    OvlFunc_897_2008e30(0xb);
    { PIN3; q0 = 5; q1 = 0x90 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __Func_809259c(5, 2);
    { PIN3; q0 = 0x2005; q1 = 0; q2 = 0x28;
      __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0xb0 << 8; q2 = 0x1e;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0xe0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    OvlFunc_897_2008e30(5);
    OvlFunc_897_2008e30(0xd);
    { PIN3; q0 = 0xe; q1 = 0xe0 << 7; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0xe, 0, 0x1e);
    __MapActor_DoAnim(0xe, 4);
    __CutsceneWait(0x14);
    __Func_8093040(0xe, 0, 0x1e);
    OvlFunc_897_2008e30(0xe);
    OvlFunc_897_2009410();
    __Func_8091890(5);
}
