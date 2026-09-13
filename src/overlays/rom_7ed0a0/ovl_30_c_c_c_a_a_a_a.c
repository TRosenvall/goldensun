/* Cluster OvlFunc_964_2009fdc..OvlFunc_964_200a040 -- the WHOLE of
 * asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_a_a.s (both functions, 43
 * instructions each).  No split; the .s becomes this one .c.
 *
 * VERDICT -- whole object, forced -O2 (scratch_elev/b262/OvlFunc_964_2009fdc/
 * wholeo_2009fdc.py, = wholecmp.py with the -O level overridden):
 *
 *   OK WHOLE OBJECT -- 200 bytes, 88 encodings and 12 relocations identical
 *
 * and per function (olevel.py, forced -O2):
 *
 *   OvlFunc_964_2009fdc  -O2  EXACT  ref 44 insn/100 bytes, ours 44 insn/100 bytes
 *   OvlFunc_964_200a040  -O2  EXACT  ref 44 insn/100 bytes, ours 44 insn/100 bytes
 *
 * Three runs, same lines.  overlay.map:503 gives this .o's .text as 0xc8 =
 * 200 bytes at 0x2009fdc, which is the whole-object figure above.
 *
 * NEEDS AN EXPLICIT -O2 RULE.  Makefile's
 * `asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o` wildcard (O1_CFLAGS) catches
 * this object with `%` = `_a_a`.  At the inherited -O1 the whole object is 38
 * encodings differing, first at index 3.  $(GCC296_CFLAGS) is exact; so is
 * $(CSE_CFLAGS), but nothing here needs -fno-rerun-cse-after-loop, so the plain
 * -O2 group is the honest statement.  Do NOT narrow the wildcard -- the
 * explicit rule for ovl_30_c_c_c_a_a_a_b.o sitting beside it is the precedent.
 *
 * THE LEVER: A BARE REGISTER PIN, AND NO BRANCH NEEDED.
 *
 * Both functions were parked (src/non_matching/overlays/2009fdc.c) as
 * straight-line repeated-constant blockers, root-caused as requiring a
 * DOMINATING BRANCH that neither function has.  That root cause is wrong.  The
 * bare hard-register pin -- `register int qN __asm__("rN")`, no
 * `__asm__ volatile` barrier -- removes the pseudo the constant would flow
 * through, so cse1 has nothing to common, and the ASSIGNMENT ORDER of the
 * pinned variables then fixes the argument-fill order at the same time.  One
 * lever, both blocker classes, no control flow invented.
 *
 *   2009fdc: 0xc6 << 18 is passed to __MapActor_SetPos at BOTH sites; -O2
 *     hoists it into r5 (`mov r5,#0xc6 / lsl r5,#18`, then `mov r2,r5` /
 *     `mov r1,r5`).  14 of 44 differing.  Under the pin it is rebuilt fresh at
 *     each site, exactly as the ROM does.
 *   200a040: the shared -1 was being DERIVED from the live 0x31 as
 *     `sub r5, r5, #0x32`, which also dragged the second __Func_8010704's two
 *     stack stores out of ROM order.  32 of 44 differing.  Pinning the two
 *     __Func_808edac sites rebuilds `mov r1,#1 / neg r1,r1` per site and the
 *     stack stores fall back into place with no other edit.
 *
 * ASSIGNMENT ORDER, NOT DECLARATION ORDER, IS WHAT SETS INSN_LUID.  The tree's
 * note that "declaration order is argument-setup order" holds for the
 * initialised form `register int q __asm__("rN") = K;`.  A BARE declaration
 * emits no insn, so with PIN3's fixed q0/q1/q2 declarations the whole result is
 * decided by the order the assignments are written.  Measured on 2009fdc:
 *
 *     assign q0, q1, q2   EXACT      <- this file
 *     assign q1, q2, q0   6 differing
 *     assign q2, q1, q0   8 differing
 *     declarations reversed (q2,q1,q0) but assigned q0,q1,q2   EXACT
 *
 * EVERY SITE OF THE REPEATED CONSTANT MUST BE PINNED, which is the per-call
 * form of the recorded "anchor every argument of a call you anchor any argument
 * of".  Measured:
 *
 *     2009fdc  both SetPos sites pinned   EXACT
 *              site 1 only                 3 differing
 *              site 2 only                12 differing
 *     200a040  both edac sites pinned     EXACT
 *              site 1 only                19 differing (and 2 insns short)
 *              site 2 only                32 differing (no change from plain)
 *
 * ELIMINATED, with numbers (all -O2, on the target alone):
 *     plain literals, full prototypes            14 / 32
 *     __MapActor_SetPos prototype dropped        14
 *     __MapActor_SetPos prototype emptied        14
 *     pin, assignment order q1,q2,q0              6
 *     pin, assignment order q2,q1,q0              8
 *     -O1, any of the above                      >= 19
 * The prototype lever ("r0 at the end -> drop the prototype") is inert here
 * because r0 wants to be in the MIDDLE, not at the end.
 *
 * The stack-arg pairs t1/t2 and the inner-scope t3 are kept from the park: they
 * are what produce the ROM's `str [sp]` / `str [sp,#4]` order at both
 * __Func_8010704 sites, and both sites are instruction-exact with them.
 *
 * LINKER: this is a whole-file conversion, so overlays/rom_7ed0a0/overlay.ld:78
 *
 *         asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_a_a.o(.text)
 *
 * stays VERBATIM, prefix and all.  It is the only .ld line naming this object;
 * there is no .data, .rodata or .bss line for it in any linker script.
 */
extern void __Func_8010704(int a, int b, int c, int d, unsigned int e, unsigned int f);
extern void __Func_808edac(int a, int b, int c);
extern void __MapActor_SetPos(int slot, int x, int y);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_964_2009fdc(void)
{
    unsigned int t1 = 8;
    unsigned int t2 = 0x31;
    __Func_8010704(0x48, 0x31, 1, 1, t1, t2);
    {
        unsigned int t3 = 0x2b;
        __Func_8010704(0x71, 0x2b, 1, 1, t2, t3);
    }
    __Func_808edac(0x64, 0, 0);
    __Func_808edac(0x65, 0, 0);
    { PIN3; q0 = 0xf; q1 = 0x88 << 16; q2 = 0xc6 << 18; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x10; q1 = 0xc6 << 18; q2 = 0xae << 18; __MapActor_SetPos(q0, q1, q2); }
}

void OvlFunc_964_200a040(void)
{
    unsigned int t1 = 8;
    unsigned int t2 = 0x31;
    __Func_8010704(8, 0x71, 1, 1, t1, t2);
    {
        unsigned int t3 = 0x2b;
        __Func_8010704(0x31, 0x6b, 1, 1, t2, t3);
    }
    { PIN3; q0 = 0x64; q1 = -1; q2 = -1; __Func_808edac(q0, q1, q2); }
    { PIN3; q0 = 0x65; q1 = -1; q2 = -1; __Func_808edac(q0, q1, q2); }
    __MapActor_SetPos(0xf, 0, 0);
    __MapActor_SetPos(0x10, 0, 0);
}
