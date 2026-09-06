/* OvlFunc_928_2008f30  --  0x02008f30
 *   [asm/overlays/rom_7b6668/ovl_314_c_c_a_c_c_c_c_c_a.s, 1st of 1]
 *
 * 114 instructions of straight-line cutscene: 21 call sites, one save-flag
 * guard around a counter bump, and a three-call camera/effect block that
 * shares one coordinate triple.  Message id 0x17fd; reads and sets save bit
 * 0x203.  Drives map actor 0x13 with gScript_928__020096a0 under a
 * task (OvlFunc_928_2008358) started at priority 0xc80 and stopped again.
 *
 * =====================================================================
 * THE EXPERIMENT: FOUR DISTINCT HIGH REGISTERS ARE FREE.  THE RESIDUE IS
 * NOT ABOUT THEM AT ALL.
 * =====================================================================
 *
 * This function was picked as the CONTROL for the "four distinct high
 * registers" screen: 25 high-register references across four distinct values,
 * small enough to characterise completely.  The answer is unambiguous and it
 * is the whole point of the specimen.
 *
 * WHAT THE ROM HOLDS IN r8-r11 -- all four, completely:
 *
 *     r11 = OvlFunc_928_2008358    the task function's address, born FIRST
 *                                  (`ldr r3,=... / mov r11,r3` before
 *                                  __StartTask), live across 8 calls to its
 *                                  reload at __StopTask; 2 references
 *     r9  = 0xa8 << 16 = 0x00a80000   the X coordinate, 4 references
 *     r8  = 0x80 << 12 = 0x00080000   the Y coordinate, 3 references
 *     r10 = 0x9c << 17 = 0x01380000   the Z coordinate, 4 references
 *
 * The three coordinates are ONE argument triple shared by three consecutive
 * OvlFunc_common0_10c calls (which differ only in their 4th argument: 0,
 * 0x3333, 0xffffcccd -- 0, +0.2, -0.2 in 16.16), and r9/r10 are reused a
 * fourth time as the two coordinates of the following __MapActor_SetPos.
 *
 * DOES gcc REACH THE SAME ASSIGNMENT UNAIDED?  YES -- EXACTLY, ON THE FIRST
 * PLAIN TRANSCRIPTION, WITH NO LEVER OF ANY KIND.  The first candidate
 * (scratch_elev/b246/f2008f30/v1.c: literal arguments, no pins, no locals, no
 * reordering) already emits, byte for byte:
 *
 *   - the 8-instruction wide prologue
 *     `push {r5,r6,lr} / mov r6,r11 / mov r5,r10 / push {r5,r6} /
 *      mov r6,r9 / mov r5,r8 / push {r5,r6} / sub sp,#0x10`
 *   - all four `mov rN, r3` definitions IN THE ROM'S ORDER (r11, then r9,
 *     r8, r10 -- note r8 second, r10 third)
 *   - all thirteen `mov rLow, rN` reloads, including the ROM's per-site
 *     argument fill order, which is r0/r1/r2 at the first two
 *     OvlFunc_common0_10c sites and r1/r2/r0 at the third
 *   - the 7-instruction wide epilogue
 *     `pop {r3,r5,r6} / mov r8,r3 / mov r9,r5 / mov r10,r6 / pop {r3} /
 *      mov r11,r3 / pop {r5,r6} / pop {r0} / bx r0`
 *
 * ALL 25 high-register references were correct before anything was tried, and
 * NOT ONE of the three levers this function finally needed touches r8-r11:
 * they are an argument pin on r0, an `int` local for an `strh` constant, and
 * a pointer local's statement position.  THE FINAL RESIDUE WAS ENTIRELY
 * UNRELATED TO THE HIGH REGISTERS.
 *
 * NEW, and grepped first as "high register", "r8-r11", "four distinct",
 * "REG_ALLOC_ORDER" -- no hit states this: the corpus's recorded unaided cases
 * are SINGLE high registers ("r8-r11 is NOT a blocker", batch: `Func_800d924`
 * and `Func_800d98c` matched with "nothing", each on ONE value: "a zero live
 * across two calls, a pointer argument live across a loop's call").  This is
 * FOUR DISTINCT VALUES, in a NON-MONOTONE assignment -- the first-born value
 * takes r11, the LAST slot of REG_ALLOC_ORDER -- and it is still free.  The
 * corrected reference-count metric scores this function 25/4; on the evidence
 * here, four distinct high-register VALUES is not pressure either.  What the
 * metric appears to measure is nothing.
 *
 * A SECOND OBSERVATION, MECHANISM NOT ESTABLISHED: r7 IS NEVER ALLOCATED.
 * Six values are live across calls here and the ROM spends r5, r6, r8, r9,
 * r10, r11 on them -- r7 is free and simply not used, in the ROM and in our
 * matching output alike.  Two obvious explanations were TESTED AND BOTH FAIL:
 *
 *   - "operand class": the two values in LOW callee-saved registers (r5 = 0,
 *     r6 = 0x20001) are exactly the two whose only uses are `str rN,[sp,#k]`,
 *     which needs LO_REGS, while the four in r8-r11 are used only as
 *     `mov rLow, rN`.  Tempting, but gcc's own `.17.lreg` dump refutes it:
 *     EVERY ONE of the 21 pseudos in this function prefers LO_REGS at cost 0,
 *     and each carries a NONZERO HI_REGS cost.  Nothing prefers a high
 *     register; local-alloc simply runs out.
 *   - "r7 is fixed as the Thumb frame pointer": refuted by the template
 *     src/overlays/rom_7b6668/ovl_314_c_c_a_c_c_a_a.c, which under IDENTICAL
 *     flags compiles to `push {r5, r6, r7, lr}` and keeps a real variable
 *     there.
 *
 * So the r7 skip is real, reproducible and unexplained.  It costs nothing --
 * gcc reproduces it unaided -- but it means "a fourth long-lived value reaches
 * r11 only after r8, r10 and r9 are taken" (docs/elevation.md, REG_ALLOC_ORDER
 * read from the compiler source) does not describe every function: here the
 * order taken is r5, r6, then r11, r9, r8, r10, with r7 skipped entirely.
 *
 * =====================================================================
 * WHAT IT ACTUALLY TOOK: THREE LEVERS, NONE OF THEM ABOUT r8-r11
 * =====================================================================
 *
 * NO FLAG GROUP.  `tryc.makefile_flags` returns the EMPTY set for both
 * src/overlays/rom_7b6668/ovl_314_c_c_a_c_c_c_c_c_a.c and the scratch path
 * screened from, so the two paths compile under identical flags and the
 * "flags depend on the path you screen from" trap does not apply.  Plain -O2.
 *
 * 1. ONE PIN ON r0 AT THE FIRST `__GetFlag(0x203)`.  0x203 is used at two
 *    sites (`__GetFlag` and, four instructions later past a branch join,
 *    `__SetFlag`).  Plain C lets gcse's cprop hoist it into r5 --
 *    `ldr r5,=0x203` emitted early, then `mov r0,r5` at both sites -- which
 *    is one instruction too many and shifts the whole tail.  This is the
 *    recorded "straight-line call scripts: a constant used at two call sites
 *    is hoisted" class and the recorded cure, `register int q0 __asm__("r0")`,
 *    is exact.  It is the ORDINARY direction of "ONE PIN AT THE FIRST USE
 *    COVERS THE LATER ONES": the pin at the LATER site alone measures 40,
 *    identical to no pin at all, and adding a second pin at `__SetFlag` on
 *    top of the first is an exact TIE.
 *
 *    THE PIN ALSO FIXES A CLUSTER 200 BYTES EARLIER, and this is why the
 *    residue looked like two problems.  Without it, all three
 *    OvlFunc_common0_10c sites store their outgoing stack words in the order
 *    [sp], [sp,#4], [sp,#0xc], [sp,#8] where the ROM stores them ascending.
 *    Six differing lines at three sites, none of them near 0x203, and every
 *    one of them disappears when the hoist does: the extra pseudo was moving
 *    sched2's tie-break at the earlier sites.  Confirms "ONE SCRATCH-REGISTER
 *    PIN CAN SETTLE TWO DISTANT CLUSTERS" and "A HOMOGENEOUS RESIDUE IS ONE
 *    LEVER, NOT N PROBLEMS", in the same direction as the recorded specimen
 *    (the pin is LATER than the cluster it repairs).  Nothing spelled at the
 *    earlier sites moves those six lines.
 *
 * 2. AN `int` LOCAL FOR THE `strh` CONSTANT 0x8000.  The ROM builds it
 *    `mov r3,#0x80 / lsl r3,#8`; written as a literal in the store, gcc emits
 *    `ldr r3,=0x8000` -- 63 differing, 312 bytes, EIGHT LONG, and a mid-
 *    function pool dump with a branch over it.  This is exactly
 *    docs/elevation.md's "gcc-2.96 has no immediate alternative for an HImode
 *    constant": the right-hand side has to be int-typed so the `strh`
 *    truncates it.  Confirmed in the negative too -- `unsigned short h` is
 *    59 differing at 122 instructions, i.e. the narrow local is ACTIVELY
 *    WRONG here, not merely inert.  The value's spelling is free
 *    (`h = 0x8000` is an exact tie with `h = 0x80 << 8`); only the TYPE
 *    matters.
 *
 * 3. THE LOADED POINTER NEEDS ITS OWN LOCAL, BORN BEFORE THE CONSTANT'S.
 *    With lever 2 in place the residue is four lines: the ROM has
 *    `mov r3,#0x80 / ldr r2,[r0,#0x50] / lsl r3,#8 / strh r3,[r2,#0x1e]` and
 *    we had r2 and r3 transposed.  Every instruction right, two scratch
 *    registers swapped -- the "register birth order" signature, and the cure
 *    is birth order: name the loaded pointer `q` and assign it BEFORE `h`.
 *    Both the collapsed form (no `q`, nested deref) and the swapped form
 *    (`h` before `q`) measure 10; the shipped order is exact.
 *
 *    NOTE THE TRAP, which is the recorded "DO NOT TRANSCRIBE THE ROM'S SHIFT
 *    ORDER" wearing different clothes.  The ROM EMITS the constant first and
 *    the pointer load second; the source that matches assigns them the other
 *    way round.  sched2 hoists the `mov r3,#0x80` above the `ldr` afterwards.
 *    Transcribing the emitted order is what produced the 10.  The recorded
 *    entry "Source order of two loads decides which gets r8 and which gets
 *    r10" is the same mechanism at high-register width; this specimen shows
 *    it in LOW scratch registers r2/r3, and with the two racers of DIFFERENT
 *    kinds -- a loaded pointer against an HImode-store constant.
 *
 * =====================================================================
 * TEARDOWN, from the shipped file, one lever removed at a time
 * =====================================================================
 *
 *     drop the PIN1 at the first __GetFlag       40 of 123, relocations differ
 *     move that pin to the __SetFlag site        40, relocations differ
 *     the pin present but not applied to a call  40, relocations differ
 *     drop the `int h` local (literal in store)  63, 312 bytes (8 LONG),
 *                                                relocations differ, pool dump
 *     `unsigned short h` instead of `int h`      59, 122 instructions,
 *                                                relocations differ
 *     drop the `q` pointer local                 10
 *     assign `h` before `q`                      10
 *     no levers at all (v1.c, plain)             66, 312 bytes
 *     `int h` alone, no pin                      44, size and count right
 *
 * EXACT TIES -- the set is minimal but not unique:
 *
 *     a second pin at __SetFlag, in addition           tie
 *     `int q` instead of `unsigned char *q`            tie
 *     `h = 0x8000` instead of `h = 0x80 << 8`          tie
 *     naming the __MapActor_GetActor result in `obj`   tie
 *     `int __StartTask(...)` instead of void           tie
 *     `int __GetFlag()` (empty parameter list)         tie
 *     `void __CutsceneEnd()` (empty parameter list)    tie
 *     `int p` for OvlFunc_common0_10c's 8th argument   tie
 *
 * MINIMISED TO A FIXPOINT.  Two locals present in the first matching
 * candidate were inert scaffolding and were removed before shipping: `obj`
 * holding the __MapActor_GetActor result, and `t` holding the __GetFlag
 * result (the pin's block simply encloses the `if` instead).  A second full
 * pass over the three survivors finds every one still load-bearing.
 *
 * =====================================================================
 * LANDING NEEDS NOTHING BUT THE FILE, AND NO LINKER EDIT
 * =====================================================================
 *
 * The .s holds exactly ONE function.  It emits no `.section`, `.data`,
 * `.rodata`, `.bss`, `.word`, `.byte`, `.align` or `.global` line, so there is
 * no section to remap.  Grepped on FULL PATH, exactly one linker line names
 * the object -- overlays/rom_7b6668/overlay.ld:42,
 * `asm/overlays/rom_7b6668/ovl_314_c_c_a_c_c_c_c_c_a.o(.text)` -- and the
 * tree default rule `asm/%.o: src/%.c` (Makefile:146) builds `asm/.../X.o`
 * from `src/.../X.c`, so the path does not move.  Landing is: add this .c,
 * delete the .s.  (A file of the SAME BASENAME exists under
 * asm/overlays/rom_7b2078/ and src/overlays/rom_7b2078/; it is a different
 * overlay and a different object, and nothing here touches it.)
 *
 * VERDICT:
 *   OK OvlFunc_928_2008f30 -- 304 bytes, 123 encodings and 24 relocations
 *   identical
 *
 * -- worked in scratch_elev/b246/f2008f30; sweep.sh there re-measures any set
 *    of candidates in one container invocation.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetBehavior(int slot, void *script);
extern void __MapActor_WaitScript(int slot);
extern void __ActorMessage(int slot, int a);
extern void __PlaySound(int id);
extern void __StartTask(void (*fn)(void), int n);
extern void __StopTask(void (*fn)(void));
extern void __Func_809202c(void);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void OvlFunc_common0_10c(int x, int y, int z, int w,
                                int a, int b, int c, void *p);
extern unsigned char *iwram_3001ebc;
extern unsigned char gScript_928__020096a0[];
extern void OvlFunc_928_2008358(void);

#define PIN1 register int q0 __asm__("r0")

void OvlFunc_928_2008f30(void)
{
    int h;
    unsigned char *q;

    __CutsceneStart();
    __MapActor_SetBehavior(0x13, gScript_928__020096a0);
    __StartTask(OvlFunc_928_2008358, 0xc8 << 4);
    __MapActor_WaitScript(0x13);
    __PlaySound(0x7c);
    OvlFunc_common0_10c(0xa8 << 16, 0x80 << 12, 0x9c << 17, 0,
                        0, 0, 0x20001, 0);
    OvlFunc_common0_10c(0xa8 << 16, 0x80 << 12, 0x9c << 17, 0x3333,
                        0, 0, 0x20001, 0);
    OvlFunc_common0_10c(0xa8 << 16, 0x80 << 12, 0x9c << 17, 0xffffcccd,
                        0, 0, 0x20001, 0);
    __StopTask(OvlFunc_928_2008358);
    q = *(unsigned char **)(__MapActor_GetActor(0x13) + 0x50);
    h = 0x80 << 8;
    *(unsigned short *)(q + 0x1e) = h;
    __MapActor_SetPos(0x15, 0xa8 << 16, 0x9c << 17);
    __CutsceneWait(0x14);
    __Func_809280c(0xe, 0x13, 0);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0xa);
    __MessageID(0x17fd);
    { PIN1; q0 = 0x203;
      if (__GetFlag(q0) != 0) {
          *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
      } }
    __ActorMessage(0xe, 0);
    __SetFlag(0x203);
    __Func_809202c();
    __CutsceneEnd();
}
